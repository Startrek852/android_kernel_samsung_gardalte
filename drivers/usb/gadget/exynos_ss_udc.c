/* linux/drivers/usb/gadget/exynos_ss_udc.c
 *
 * Copyright 2011 Samsung Electronics Co., Ltd.
 *	http://www.samsung.com/
 *
 * EXYNOS SuperSpeed USB 3.0 Device Controlle driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/slab.h>

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/otg.h>
#include <linux/usb/composite.h>
#include <linux/usb/exynos_usb3_drd.h>

#include <linux/platform_data/dwc3-exynos.h>

#include <asm/byteorder.h>

#include "exynos_ss_udc.h"

static void exynos_ss_udc_kill_all_requests(struct exynos_ss_udc *udc,
					    struct exynos_ss_udc_ep *udc_ep,
					    int result);
static void exynos_ss_udc_ep0_restart(struct exynos_ss_udc *udc);
static void exynos_ss_udc_complete_setup(struct usb_ep *ep,
					 struct usb_request *req);
static void exynos_ss_udc_complete_request(struct exynos_ss_udc *udc,
					   struct exynos_ss_udc_ep *udc_ep,
					   struct exynos_ss_udc_req *udc_req,
					   int result);
static void exynos_ss_udc_start_req(struct exynos_ss_udc *udc,
				    struct exynos_ss_udc_ep *udc_ep,
				    struct exynos_ss_udc_req *udc_req,
				    bool send_zlp);
static void exynos_ss_udc_ep_activate(struct exynos_ss_udc *udc,
				      struct exynos_ss_udc_ep *udc_ep);
static void exynos_ss_udc_ep_deactivate(struct exynos_ss_udc *udc,
					struct exynos_ss_udc_ep *udc_ep);
static int exynos_ss_udc_start(struct usb_gadget *gadget,
			       struct usb_gadget_driver *driver);
static int exynos_ss_udc_stop(struct usb_gadget *gadget,
			      struct usb_gadget_driver *driver);

static const char driver_name[] = "exynos-ss-udc";
static const char *ep_types[] = {"iso", "bulk", "int"};

static int poll_bit_set(void __iomem *ptr, u32 val, int timeout)
{
	u32 reg;

	do {
		reg = readl(ptr);
		if (reg & val)
			return 0;

		udelay(1);
	} while (timeout-- > 0);

	return -ETIME;
}

static int poll_bit_clear(void __iomem *ptr, u32 val, int timeout)
{
	u32 reg;

	do {
		reg = readl(ptr);
		if (!(reg & val))
			return 0;

		udelay(1);
	} while (timeout-- > 0);

	return -ETIME;
}

static struct exynos_ss_udc_ep *get_udc_ep(struct exynos_ss_udc *udc,
					   int phys_epnum)
{
	int dir_in, epnum, epindex;

	dir_in = phys_epnum % 2;
	epnum = phys_to_epnum(phys_epnum);
	epindex = epnum_to_epindex(epnum, dir_in);

	return &udc->eps[epindex];
}

/**
 * ep_from_windex - convert control wIndex value to endpoint
 * @udc: The device state.
 * @windex: The control request wIndex field (in host order).
 *
 * Convert the given wIndex into a pointer to an driver endpoint
 * structure, or return NULL if it is not a valid endpoint.
 */
static struct exynos_ss_udc_ep *ep_from_windex(struct exynos_ss_udc *udc,
					       u32 windex)
{
	int dir = (windex & USB_DIR_IN) ? 1 : 0;
	int epnum = windex & 0x7F;
	int epindex = epnum_to_epindex(epnum, dir);
	struct exynos_ss_udc_ep *udc_ep = &udc->eps[epindex];

	if (windex >= 0x100)
		return NULL;

	if (epindex >= EXYNOS_USB3_EPS)
		return NULL;

	if (epnum && udc_ep->dir_in != dir)
		return NULL;

	return udc_ep;
}

/**
 * get_ep_head - return the first request on the endpoint
 * @udc_ep: The endpoint to get request from.
 *
 * Get the first request on the endpoint.
 */
static struct exynos_ss_udc_req *get_ep_head(struct exynos_ss_udc_ep *udc_ep)
{
	if (list_empty(&udc_ep->req_queue))
		return NULL;

	return list_first_entry(&udc_ep->req_queue,
				struct exynos_ss_udc_req,
				queue);
}

/**
 * on_list - check request is on the given endpoint
 * @ep: The endpoint to check.
 * @test: The request to test if it is on the endpoint.
 */
static bool on_list(struct exynos_ss_udc_ep *udc_ep,
		    struct exynos_ss_udc_req *test)
{
	struct exynos_ss_udc_req *udc_req, *treq;

	list_for_each_entry_safe(udc_req, treq, &udc_ep->req_queue, queue) {
		if (udc_req == test)
			return true;
	}

	return false;
}

/**
 * exynos_ss_udc_map_dma - map the DMA memory being used for the request
 * @udc: The device state.
 * @udc_req: The request being processed.
 * @udc_ep: The endpoint the request is on.
 *
 * We've been asked to queue a request, so ensure that the memory buffer
 * is correctly setup for DMA.
 *
 * The UDC requires all OUT transfer request lengths to be multiple of
 * max packet size. Bounce buffer is used to meet this requirement.
 */
static int exynos_ss_udc_map_dma(struct exynos_ss_udc *udc,
				 struct exynos_ss_udc_req *udc_req,
				 struct exynos_ss_udc_ep *udc_ep)
{
	struct usb_request *req = &udc_req->req;
	int rem;

	/* Bounce buffer for OUT transfers */
	if (!udc_ep->dir_in) {
		rem = req->length % udc_ep->ep.maxpacket;
		if (rem > 0) {
			dev_dbg(udc->dev,
				"%s: buffer length is not multiple of max packet size\n",
				__func__);

			/* Backup buffer address and its length */
			udc_req->buf = req->buf;
			udc_req->length = req->length;

			/*
			 * Create buffer with the length divisible
			 * by the maxpacket size
			 */
			req->length += udc_ep->ep.maxpacket - rem;
			req->buf = kzalloc(req->length, GFP_ATOMIC);
			if (!req->buf) {
				dev_err(udc->dev,
					"%s: cannot allocate bounce buffer\n",
					__func__);
				req->length = udc_req->length;
				req->buf = udc_req->buf;
				return -ENOMEM;
			}

			udc_req->bounced = true;
		}
	}

	return usb_gadget_map_request(&udc->gadget, req, udc_ep->dir_in);
}

/**
 * exynos_ss_udc_unmap_dma - unmap the DMA memory being used for the request
 * @udc: The device state.
 * @udc_req: The request being processed.
 * @udc_ep: The endpoint for the request.
 *
 * This is the reverse of exynos_ss_udc_map_dma(), called for the completion
 * of a request to ensure the buffer is ready for access by the caller.
 */
static void exynos_ss_udc_unmap_dma(struct exynos_ss_udc *udc,
				    struct exynos_ss_udc_req *udc_req,
				    struct exynos_ss_udc_ep *udc_ep)
{
	struct usb_request *req = &udc_req->req;

	usb_gadget_unmap_request(&udc->gadget, req, udc_ep->dir_in);

	/* Bounce buffer for OUT transfers */
	if (!udc_ep->dir_in && udc_req->bounced) {
		/* Copy data that we received */
		memcpy(udc_req->buf, req->buf, udc_req->length);
		kfree(req->buf);

		/* Restore original buffer and its length */
		req->length = udc_req->length;
		req->buf = udc_req->buf;

		udc_req->bounced = false;
	}
}

/**
 * exynos_ss_udc_issue_epcmd - issue physical endpoint-specific command
 * @udc: The device state.
 * @epcmd: The command to issue.
 */
static int exynos_ss_udc_issue_epcmd(struct exynos_ss_udc *udc,
				     struct exynos_ss_udc_ep_command *epcmd)
{
	int res;
	u32 depcmd;

	/* If some of parameters are not in use, we will write it anyway
	   for simplification */
	writel(epcmd->param0, udc->regs + EXYNOS_USB3_DEPCMDPAR0(epcmd->ep));
	writel(epcmd->param1, udc->regs + EXYNOS_USB3_DEPCMDPAR1(epcmd->ep));
	writel(epcmd->param2, udc->regs + EXYNOS_USB3_DEPCMDPAR2(epcmd->ep));

	depcmd = epcmd->cmdtyp | epcmd->cmdflags;
	writel(depcmd, udc->regs + EXYNOS_USB3_DEPCMD(epcmd->ep));

	res = poll_bit_clear(udc->regs + EXYNOS_USB3_DEPCMD(epcmd->ep),
			     EXYNOS_USB3_DEPCMDx_CmdAct,
			     1000);
	return res;
}

/**
 * exynos_ss_udc_end_xfer - end active transfer for endpoint
 * @udc: The device state.
 * @udc_ep: The endpoint for which transfer is stopped.
 */
static void exynos_ss_udc_end_xfer(struct exynos_ss_udc *udc,
				   struct exynos_ss_udc_ep *udc_ep)
{
	struct exynos_ss_udc_ep_command epcmd = {{0}, };
	int phys_epnum = get_phys_epnum(udc_ep);
	int res;

	if (!udc_ep->tri)
		return;

	epcmd.ep = phys_epnum;
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPENDXFER;
	epcmd.cmdflags = (udc_ep->tri <<
		EXYNOS_USB3_DEPCMDx_CommandParam_SHIFT) |
		EXYNOS_USB3_DEPCMDx_HiPri_ForceRM |
		EXYNOS_USB3_DEPCMDx_CmdIOC |
		EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0) {
		dev_err(udc->dev, "Failed to end transfer\n");
		udc_ep->not_ready = 1;
	}

	udc_ep->tri = 0;
}

#ifdef CONFIG_BATTERY_SAMSUNG
void exynos_ss_udc_cable_connect(struct exynos_ss_udc *udc, bool connect)
{
	static int last_connect;

	if (last_connect != connect) {
		samsung_cable_check_status(connect);
		last_connect = connect;
	}
}
#define EXYNOS_SS_UDC_CABLE_CONNECT(udc, connect)	\
	exynos_ss_udc_cable_connect(udc, connect)
#else
#define EXYNOS_SS_UDC_CABLE_CONNECT(udc, conect)
#endif

/**
 * exynos_ss_udc_run_stop - start/stop the device controller operation
 * @udc: The device state.
 * @is_on: The action to take (1 - start, 0 - stop).
 */
static void exynos_ss_udc_run_stop(struct exynos_ss_udc *udc, int is_on)
{
	int res;

	if (is_on) {
		__orr32(udc->regs + EXYNOS_USB3_DCTL,
			EXYNOS_USB3_DCTL_Run_Stop);
		res = poll_bit_clear(udc->regs + EXYNOS_USB3_DSTS,
				     EXYNOS_USB3_DSTS_DevCtrlHlt,
				     1000);
	} else {
		__bic32(udc->regs + EXYNOS_USB3_DCTL,
			EXYNOS_USB3_DCTL_Run_Stop);
		res = poll_bit_set(udc->regs + EXYNOS_USB3_DSTS,
				   EXYNOS_USB3_DSTS_DevCtrlHlt,
				   1000);
	}

	if (res < 0)
		dev_dbg(udc->dev, "Failed to %sconnect by software\n",
				  is_on ? "" : "dis");
}

/**
 * exynos_ss_udc_ep_enable - configure endpoint, making it usable
 * @ep: The endpoint being configured.
 * @desc: The descriptor for desired behavior.
 */
static int exynos_ss_udc_ep_enable(struct usb_ep *ep,
				   const struct usb_endpoint_descriptor *desc)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	unsigned long flags;
	int dir_in;
	int epnum;

	dev_dbg(udc->dev,
		"%s: ep %s: a 0x%02x, attr 0x%02x, mps 0x%04x, intr %d\n",
		__func__, ep->name, desc->bEndpointAddress, desc->bmAttributes,
		desc->wMaxPacketSize, desc->bInterval);

	if (!ep || !desc || desc->bDescriptorType != USB_DT_ENDPOINT) {
		dev_err(udc->dev, "%s: invalid parameters\n", __func__);
		return -EINVAL;
	}

	/* not to be called for EP0 */
	WARN_ON(udc_ep->epnum == 0);

	if (udc_ep->enabled)
		return -EINVAL;

	epnum = (desc->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK);
	if (epnum != udc_ep->epnum) {
		dev_err(udc->dev, "%s: EP number mismatch!\n", __func__);
		return -EINVAL;
	}

	dir_in = (desc->bEndpointAddress & USB_ENDPOINT_DIR_MASK) ? 1 : 0;
	if (dir_in != udc_ep->dir_in) {
		dev_err(udc->dev, "%s: EP direction mismatch!\n", __func__);
		return -EINVAL;
	}

	spin_lock_irqsave(&udc_ep->lock, flags);

	/* update the endpoint state */
	udc_ep->ep.maxpacket = usb_endpoint_maxp(desc);
	udc_ep->type = usb_endpoint_type(desc);

	switch (udc_ep->type) {
	case USB_ENDPOINT_XFER_ISOC:
		dev_err(udc->dev, "no current ISOC support\n");
		spin_unlock_irqrestore(&udc_ep->lock, flags);
		return -EINVAL;

	case USB_ENDPOINT_XFER_BULK:
		dev_dbg(udc->dev, "Bulk endpoint\n");
		break;

	case USB_ENDPOINT_XFER_INT:
		dev_dbg(udc->dev, "Interrupt endpoint\n");
		break;

	case USB_ENDPOINT_XFER_CONTROL:
		dev_dbg(udc->dev, "Control endpoint\n");
		break;
	}

	exynos_ss_udc_ep_activate(udc, udc_ep);
	udc_ep->enabled = 1;
	udc_ep->halted = udc_ep->wedged = 0;
	spin_unlock_irqrestore(&udc_ep->lock, flags);

	return 0;
}

/**
 * exynos_ss_udc_ep_disable - endpoint is no longer usable
 * @ep: The endpoint being unconfigured.
 */
static int exynos_ss_udc_ep_disable(struct usb_ep *ep)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	unsigned long flags;

	dev_dbg(udc->dev, "%s: ep%d%s\n", __func__,
			  udc_ep->epnum, udc_ep->dir_in ? "in" : "out");

	if (!ep) {
		dev_err(udc->dev, "%s: invalid parameters\n", __func__);
		return -EINVAL;
	}

	if (!udc_ep->enabled)
		return 0;

	spin_lock_irqsave(&udc_ep->lock, flags);
	exynos_ss_udc_ep_deactivate(udc, udc_ep);
	udc_ep->enabled = 0;
	spin_unlock_irqrestore(&udc_ep->lock, flags);

	/* terminate all requests with shutdown */
	exynos_ss_udc_kill_all_requests(udc, udc_ep, -ESHUTDOWN);

	return 0;
}

/**
 * exynos_ss_udc_ep_alloc_request - allocate a request object
 * @ep: The endpoint to be used with the request.
 * @flags: Allocation flags.
 *
 * Allocate a new USB request structure appropriate for the specified endpoint.
 */
static struct usb_request *exynos_ss_udc_ep_alloc_request(struct usb_ep *ep,
							  gfp_t flags)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	struct exynos_ss_udc_req *udc_req;

	dev_dbg(udc->dev, "%s: ep%d\n", __func__, udc_ep->epnum);

	udc_req = kzalloc(sizeof(struct exynos_ss_udc_req), flags);
	if (!udc_req)
		return NULL;

	INIT_LIST_HEAD(&udc_req->queue);

	udc_req->req.dma = DMA_ADDR_INVALID;
	return &udc_req->req;
}

/**
 * exynos_ss_udc_ep_free_request - free a request object
 * @ep: The endpoint associated with the request.
 * @req: The request being freed.
 *
 * Reverse the effect of exynos_ss_udc_ep_alloc_request().
 */
static void exynos_ss_udc_ep_free_request(struct usb_ep *ep,
					  struct usb_request *req)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	struct exynos_ss_udc_req *udc_req = our_req(req);

	dev_dbg(udc->dev, "%s: ep%d, req %p\n", __func__, udc_ep->epnum, req);

	kfree(udc_req);
}

/**
 * exynos_ss_udc_ep_queue - queue (submit) an I/O request to an endpoint
 * @ep: The endpoint associated with the request.
 * @req: The request being submitted.
 * @gfp_flags: Not used.
 *
 * Queue a request and start it if the first.
 */
static int exynos_ss_udc_ep_queue(struct usb_ep *ep,
				  struct usb_request *req,
				  gfp_t gfp_flags)
{
	struct exynos_ss_udc_req *udc_req = our_req(req);
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	unsigned long irqflags;
	bool first;
	int ret;

	dev_dbg(udc->dev, "%s: ep%d%s (%p): %d@%p, noi=%d, zero=%d, snok=%d\n",
			  __func__, udc_ep->epnum,
			  udc_ep->dir_in ? "in" : "out", req,
			  req->length, req->buf, req->no_interrupt,
			  req->zero, req->short_not_ok);

	/* initialise status of the request */
	INIT_LIST_HEAD(&udc_req->queue);

	req->actual = 0;
	req->status = -EINPROGRESS;

	/* Sync the buffers as necessary */
	if (req->buf == udc->ctrl_buff)
		req->dma = udc->ctrl_buff_dma;
	else if (req->buf == udc->ep0_buff)
		req->dma = udc->ep0_buff_dma;
	else {
		ret = exynos_ss_udc_map_dma(udc, udc_req, udc_ep);
		if (ret)
			return ret;
	}

	spin_lock_irqsave(&udc_ep->lock, irqflags);

	first = list_empty(&udc_ep->req_queue);
	list_add_tail(&udc_req->queue, &udc_ep->req_queue);

	if (first && !udc_ep->not_ready)
		exynos_ss_udc_start_req(udc, udc_ep, udc_req, false);

	spin_unlock_irqrestore(&udc_ep->lock, irqflags);

	return 0;
}

/**
 * exynos_ss_udc_ep_dequeue - dequeue an I/O request from an endpoint
 * @ep: The endpoint associated with the request.
 * @req: The request being canceled.
 *
 * Dequeue a request and call its completion routine.
 */
static int exynos_ss_udc_ep_dequeue(struct usb_ep *ep, struct usb_request *req)
{
	struct exynos_ss_udc_req *udc_req = our_req(req);
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	unsigned long flags;

	dev_dbg(udc->dev, "%s: ep%d%s (%p)\n", __func__,
			  udc_ep->epnum, udc_ep->dir_in ? "in" : "out", req);

	spin_lock_irqsave(&udc_ep->lock, flags);

	if (!on_list(udc_ep, udc_req)) {
		spin_unlock_irqrestore(&udc_ep->lock, flags);
		return -EINVAL;
	}

	exynos_ss_udc_complete_request(udc, udc_ep, udc_req, -ECONNRESET);
	spin_unlock_irqrestore(&udc_ep->lock, flags);

	return 0;
}

/**
 * exynos_ss_udc_ep_sethalt - set/clear the endpoint halt feature
 * @ep: The endpoint being stalled/reset.
 * @value: The action to take (1 - set stall, 0 - clear stall).
 */
static int exynos_ss_udc_ep_sethalt(struct usb_ep *ep, int value)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	struct exynos_ss_udc_ep_command epcmd = {{0}, };
	int epnum = udc_ep->epnum;
	int phys_epnum = get_phys_epnum(udc_ep);
	unsigned long irqflags;
	int res;

	dev_dbg(udc->dev, "%s(ep %p %s, %d)\n", __func__, ep, ep->name, value);

	spin_lock_irqsave(&udc_ep->lock, irqflags);

	if (value && epnum != 0 && udc_ep->dir_in && udc_ep->req) {
		dev_dbg(udc->dev, "%s: transfer in progress!\n", __func__);
		spin_unlock_irqrestore(&udc_ep->lock, irqflags);
		return -EAGAIN;
	}

	if (epnum == 0)
		/* Only OUT direction can be stalled */
		epcmd.ep = 0;
	else
		epcmd.ep = phys_epnum;

	if (value)
		epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPSSTALL;
	else
		epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPCSTALL;

	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0) {
		dev_err(udc->dev, "Failed to set/clear stall\n");
		spin_unlock_irqrestore(&udc_ep->lock, irqflags);
		return res;
	}

	if (epnum == 0) {
		if (value)
			udc->ep0_state = EP0_STALL;
	} else {
		udc_ep->halted = !!value;

		if (!value)
			udc_ep->wedged = 0;
	}

	spin_unlock_irqrestore(&udc_ep->lock, irqflags);

	return 0;
}

/**
 * exynos_ss_udc_ep_setwedge - set the halt feature and ignore clear requests
 * @ep: The endpoint being wedged.
 */
static int exynos_ss_udc_ep_setwedge(struct usb_ep *ep)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	unsigned long irqflags;

	spin_lock_irqsave(&udc_ep->lock, irqflags);
	udc_ep->wedged = 1;
	spin_unlock_irqrestore(&udc_ep->lock, irqflags);

	return exynos_ss_udc_ep_sethalt(ep, 1);
}

static struct usb_ep_ops exynos_ss_udc_ep_ops = {
	.enable		= exynos_ss_udc_ep_enable,
	.disable	= exynos_ss_udc_ep_disable,
	.alloc_request	= exynos_ss_udc_ep_alloc_request,
	.free_request	= exynos_ss_udc_ep_free_request,
	.queue		= exynos_ss_udc_ep_queue,
	.dequeue	= exynos_ss_udc_ep_dequeue,
	.set_halt	= exynos_ss_udc_ep_sethalt,
	.set_wedge	= exynos_ss_udc_ep_setwedge,
};

/**
 * exynos_ss_udc_start_req - start a USB request from an endpoint's queue
 * @udc: The device state.
 * @udc_ep: The endpoint to process a request for.
 * @udc_req: The request being started.
 * @send_zlp: True if we are sending ZLP.
 *
 * Start the given request running by setting the TRB appropriately,
 * and issuing Start Transfer endpoint command.
 */
static void exynos_ss_udc_start_req(struct exynos_ss_udc *udc,
				    struct exynos_ss_udc_ep *udc_ep,
				    struct exynos_ss_udc_req *udc_req,
				    bool send_zlp)
{
	struct exynos_ss_udc_ep_command epcmd = {{0}, };
	struct usb_request *ureq = &udc_req->req;
	enum trb_control trb_type = NORMAL;
	int epnum = udc_ep->epnum;
	int xfer_length;
	int res;

	dev_dbg(udc->dev, "%s: ep%d%s, req %p\n", __func__, epnum,
			   udc_ep->dir_in ? "in" : "out", ureq);

	/* If endpoint is stalled, we will restart request later */
	if (udc_ep->halted) {
		dev_dbg(udc->dev, "%s: ep%d is stalled\n", __func__, epnum);
		return;
	}

	/* Get type of TRB */
	if (epnum == 0)
		switch (udc->ep0_state) {
		case EP0_SETUP_PHASE:
			trb_type = CONTROL_SETUP;
			break;

		case EP0_DATA_PHASE:
			trb_type = CONTROL_DATA;
			break;

		case EP0_STATUS_PHASE:
			if (udc->ep0_three_stage)
				trb_type = CONTROL_STATUS_3;
			else
				trb_type = CONTROL_STATUS_2;
			break;

		case EP0_WAIT_NRDY:
			dev_dbg(udc->dev, "%s: wait XferNotReady for EP0\n",
					   __func__);
			return;

		default:
			dev_err(udc->dev, "%s: Erroneous EP0 state (%d)",
					   __func__, udc->ep0_state);
			return;
		}
	else
		trb_type = NORMAL;

	udc_ep->req = udc_req;

	/* Get transfer length */
	if (send_zlp)
		xfer_length = 0;
	else
		xfer_length = ureq->length & EXYNOS_USB3_TRB_BUFSIZ_MASK;

	/* Fill TRB */
	udc_ep->trb->buff_ptr_low = (u32) ureq->dma;
	udc_ep->trb->buff_ptr_high = 0;
	udc_ep->trb->param1 = EXYNOS_USB3_TRB_BUFSIZ(xfer_length);
	udc_ep->trb->param2 = EXYNOS_USB3_TRB_LST |
			      EXYNOS_USB3_TRB_HWO |
			      EXYNOS_USB3_TRB_TRBCTL(trb_type);

	/* Start Transfer */
	epcmd.ep = get_phys_epnum(udc_ep);
	epcmd.param0 = 0;
	epcmd.param1 = udc_ep->trb_dma;
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPSTRTXFER;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0) {
		dev_err(udc->dev, "Failed to start transfer\n");
	} else {
		u32 depcmd;

		depcmd = readl(udc->regs + EXYNOS_USB3_DEPCMD(epcmd.ep));
		udc_ep->tri = (depcmd >> EXYNOS_USB3_DEPCMDx_EventParam_SHIFT) &
				EXYNOS_USB3_DEPCMDx_XferRscIdx_LIMIT;

		udc_ep->sent_zlp = send_zlp;
	}
}

/**
 * exynos_ss_udc_enqueue_status - start a request for EP0 status stage
 * @udc: The device state.
 */
static void exynos_ss_udc_enqueue_status(struct exynos_ss_udc *udc)
{
	struct usb_request *req = udc->ctrl_req;
	struct exynos_ss_udc_req *udc_req = our_req(req);
	int ret;

	dev_dbg(udc->dev, "%s: queueing status request\n", __func__);

	req->zero = 0;
	req->length = 0;
	req->buf = udc->ctrl_buff;
	req->complete = NULL;

	if (!list_empty(&udc_req->queue)) {
		dev_info(udc->dev, "%s already queued???\n", __func__);
		return;
	}

	ret = exynos_ss_udc_ep_queue(&udc->eps[EP0INDEX].ep, req, GFP_ATOMIC);
	if (ret < 0)
		dev_err(udc->dev, "%s: failed queue (%d)\n", __func__, ret);
}

/**
 * exynos_ss_udc_enqueue_data - start a request for EP0 data stage
 * @udc: The device state.
 * @buff: The buffer used for data.
 * @length: The length of data.
 * @complete: The function called when request completes.
 */
static int exynos_ss_udc_enqueue_data(struct exynos_ss_udc *udc,
				      void *buff, int length,
				      void (*complete) (struct usb_ep *ep,
						struct usb_request *req))
{
	struct usb_request *req = udc->ctrl_req;
	struct exynos_ss_udc_req *udc_req = our_req(req);
	int ret;

	dev_dbg(udc->dev, "%s: queueing data request\n", __func__);

	req->zero = 0;
	req->length = length;

	if (buff == NULL)
		req->buf = udc->ep0_buff;
	else
		req->buf = buff;

	req->complete = complete;

	if (!list_empty(&udc_req->queue)) {
		dev_info(udc->dev, "%s: already queued???\n", __func__);
		return -EAGAIN;
	}

	ret = exynos_ss_udc_ep_queue(&udc->eps[EP0INDEX].ep, req, GFP_ATOMIC);
	if (ret < 0) {
		dev_err(udc->dev, "%s: failed to enqueue data request (%d)\n",
				   __func__, ret);
		return ret;
	}

	return 0;
}

/**
 * exynos_ss_udc_enqueue_setup - start a request for EP0 setup stage
 * @udc: The device state.
 *
 * Enqueue a request on EP0 if necessary to receive any SETUP packets
 * from the host.
 */
static void exynos_ss_udc_enqueue_setup(struct exynos_ss_udc *udc)
{
	struct usb_request *req = udc->ctrl_req;
	struct exynos_ss_udc_req *udc_req = our_req(req);
	int ep0index = EP0INDEX;
	int ret;

	dev_dbg(udc->dev, "%s: queueing setup request\n", __func__);

	req->zero = 0;
	req->length = EXYNOS_USB3_CTRL_BUFF_SIZE;
	req->buf = udc->ctrl_buff;
	req->complete = exynos_ss_udc_complete_setup;

	if (!list_empty(&udc_req->queue)) {
		dev_dbg(udc->dev, "%s already queued???\n", __func__);
		return;
	}

	udc->eps[ep0index].dir_in = 0;

	ret = exynos_ss_udc_ep_queue(&udc->eps[ep0index].ep, req, GFP_ATOMIC);
	if (ret < 0)
		dev_err(udc->dev, "%s: failed queue (%d)\n", __func__, ret);
}

/**
 * exynos_ss_udc_process_set_config - process request SET_CONFIGURATION
 * @udc: The device state.
 * @ctrl: The USB control request.
 */
static int exynos_ss_udc_process_set_config(struct exynos_ss_udc *udc,
					    struct usb_ctrlrequest *ctrl)
{
	int ret;
	u16 config = le16_to_cpu(ctrl->wValue);

	dev_dbg(udc->dev, "%s\n", __func__);

	udc->our_status = false;

	switch (udc->state) {
	case USB_STATE_ADDRESS:
		ret = udc->driver->setup(&udc->gadget, ctrl);
		if (!ret || ret == USB_GADGET_DELAYED_STATUS) {
			ret = 1;
			if (config)
				udc->state = USB_STATE_CONFIGURED;
		}
		break;

	case USB_STATE_CONFIGURED:
		ret = udc->driver->setup(&udc->gadget, ctrl);
		if (!ret || ret == USB_GADGET_DELAYED_STATUS) {
			ret = 1;
			if (!config)
				udc->state = USB_STATE_ADDRESS;
		}
		break;

	case USB_STATE_DEFAULT:
	/* FALLTHROUGH */
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

/**
 * exynos_ss_udc_complete_set_sel - completion of SET_SEL request data stage
 * @ep: The endpoint the request was on.
 * @req: The request completed.
 */
static void exynos_ss_udc_complete_set_sel(struct usb_ep *ep,
					   struct usb_request *req)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;
	u8 *sel = req->buf;
	u32 param;
	u32 dgcmd;
	int res;

	/* Our device is U1/U2 enabled, so we will use U2PEL */
	param = sel[5] << 8 | sel[4];
	/* Documentation says "If the value is greater than 125us, then
	 * software must program a value of zero into this register */
	if (param > 125)
		param = 0;

	dev_dbg(udc->dev, "%s: dgcmd_param = 0x%08x\n", __func__, param);

	dgcmd = EXYNOS_USB3_DGCMD_CmdAct |
		EXYNOS_USB3_DGCMD_CmdTyp_SetPerParams;

	writel(param, udc->regs + EXYNOS_USB3_DGCMDPAR);
	writel(dgcmd, udc->regs + EXYNOS_USB3_DGCMD);
	res = poll_bit_clear(udc->regs + EXYNOS_USB3_DGCMD,
			     EXYNOS_USB3_DGCMD_CmdAct,
			     1000);
	if (res < 0)
		dev_err(udc->dev, "Failed to set periodic parameters\n");
}

/**
 * exynos_ss_udc_process_set_sel - process request SET_SEL
 * @udc: The device state.
 */
static int exynos_ss_udc_process_set_sel(struct exynos_ss_udc *udc)
{
	int ret;

	dev_dbg(udc->dev, "%s\n", __func__);

	if (udc->state != USB_STATE_ADDRESS &&
	    udc->state != USB_STATE_CONFIGURED)
		return -EINVAL;

	ret = exynos_ss_udc_enqueue_data(udc, udc->ep0_buff,
					 EXYNOS_USB3_EP0_BUFF_SIZE,
					 exynos_ss_udc_complete_set_sel);
	if (ret < 0) {
		dev_err(udc->dev, "%s: failed to become ready for SEL data\n",
				   __func__);
		return ret;
	}

	return 1;
}

/**
 * exynos_ss_udc_process_set_isoch_delay - process request SET_ISOCH_DELAY
 * @udc: The device state.
 * @ctrl: The USB control request.
 */
static int exynos_ss_udc_process_set_isoch_delay(struct exynos_ss_udc *udc,
						 struct usb_ctrlrequest *ctrl)
{
	u16 isoch_delay;
	int ret = 1;

	if (ctrl->wIndex || ctrl->wLength) {
		ret = -EINVAL;
		goto err;
	}

	switch (udc->state) {
	case USB_STATE_DEFAULT:
	/* FALLTHROUGH */
	case USB_STATE_ADDRESS:
	/* FALLTHROUGH */
	case USB_STATE_CONFIGURED:
		isoch_delay = le16_to_cpu(ctrl->wValue);
		/* REVISIT don't know what to do with this value */
		break;
	default:
		ret = -EINVAL;
		break;
	}

err:
	return ret;
}


/**
 * exynos_ss_udc_set_test_mode - set UDC to specific test mode
 * @udc: The device state.
 * @selector: The test mode selector.
 */
static void exynos_ss_udc_set_test_mode(struct exynos_ss_udc *udc,
					u8 selector)
{
	u32 reg;

	reg = readl(udc->regs + EXYNOS_USB3_DCTL);
	reg &= ~EXYNOS_USB3_DCTL_TstCtl_MASK;
	reg |= EXYNOS_USB3_DCTL_TstCtl(selector);
	writel(reg, udc->regs + EXYNOS_USB3_DCTL);
}

/**
 * exynos_ss_udc_handle_test_mode - handle TEST_MODE feature
 * @udc: The device state.
 * @wIndex: The request wIndex field.
 */
static int exynos_ss_udc_handle_test_mode(struct exynos_ss_udc *udc,
					  u16 wIndex)
{
	u8 selector = wIndex >> 8;
	char *mode;
	int ret = 0;

	switch (selector) {
	case TEST_J:
		mode = "TEST J";
		break;
	case TEST_K:
		mode = "TEST K";
		break;
	case TEST_SE0_NAK:
		mode = "TEST SE0 NAK";
		break;
	case TEST_PACKET:
		mode = "TEST PACKET";
		break;
	case TEST_FORCE_EN:
		mode = "TEST FORCE EN";
		break;
	default:
		mode = "unknown";
		ret = -EINVAL;
		break;
	}

	dev_info(udc->dev, "Test mode selector is %s\n", mode);

	if (ret == 0) {
		udc->test_mode = true;
		udc->test_selector = selector;
	}

	return ret;
}

/**
 * exynos_ss_udc_process_clr_feature - process request CLEAR_FEATURE
 * @udc: The device state.
 * @ctrl: The USB control request.
 */
static int exynos_ss_udc_process_clr_feature(struct exynos_ss_udc *udc,
					     struct usb_ctrlrequest *ctrl)
{
	struct exynos_ss_udc_ep *udc_ep;
	struct exynos_ss_udc_req *udc_req;
	int ret;
	bool restart;
	u16 wValue;
	u16 wIndex;
	u8 recip;

	dev_dbg(udc->dev, "%s\n", __func__);

	if (udc->state != USB_STATE_ADDRESS &&
	    udc->state != USB_STATE_CONFIGURED)
		return -EINVAL;

	wValue = le16_to_cpu(ctrl->wValue);
	wIndex = le16_to_cpu(ctrl->wIndex);
	recip = ctrl->bRequestType & USB_RECIP_MASK;

	switch (recip) {
	case USB_RECIP_DEVICE:
		switch (wValue) {
		case USB_DEVICE_U1_ENABLE:
			if (udc->gadget.speed != USB_SPEED_SUPER ||
			    udc->state == USB_STATE_ADDRESS)
				return -EINVAL;

			__bic32(udc->regs + EXYNOS_USB3_DCTL,
				EXYNOS_USB3_DCTL_InitU1Ena);
			break;

		case USB_DEVICE_U2_ENABLE:
			if (udc->gadget.speed != USB_SPEED_SUPER ||
			    udc->state == USB_STATE_ADDRESS)
				return -EINVAL;

			__bic32(udc->regs + EXYNOS_USB3_DCTL,
				EXYNOS_USB3_DCTL_InitU2Ena);
			break;

		default:
			return -ENOENT;
		}
		break;

	case USB_RECIP_ENDPOINT:
		udc_ep = ep_from_windex(udc, wIndex);
		if (!udc_ep) {
			dev_dbg(udc->dev, "%s: no endpoint for 0x%04x\n",
					  __func__, wIndex);
			return -ENOENT;
		}

		if (udc->state == USB_STATE_ADDRESS &&
		    udc_ep->epnum != 0)
			return -EINVAL;

		switch (wValue) {
		case USB_ENDPOINT_HALT:
			if (!udc_ep->wedged) {
				ret = exynos_ss_udc_ep_sethalt(&udc_ep->ep, 0);
				if (ret < 0)
					return ret;

				/* If we have pending request, then start it */
				restart = !list_empty(&udc_ep->req_queue);
				if (restart) {
					udc_req = get_ep_head(udc_ep);
					exynos_ss_udc_start_req(udc, udc_ep,
								udc_req, false);
				}
			}
			break;

		default:
			return -ENOENT;
		}

		break;

	default:
		return -ENOENT;
	}

	return 1;
}

/**
 * exynos_ss_udc_process_set_feature - process request SET_FEATURE
 * @udc: The device state.
 * @ctrl: The USB control request.
 */
static int exynos_ss_udc_process_set_feature(struct exynos_ss_udc *udc,
					     struct usb_ctrlrequest *ctrl)
{
	struct exynos_ss_udc_ep *udc_ep;
	int ret;
	u16 wValue;
	u16 wIndex;
	u8 recip;

	dev_dbg(udc->dev, "%s\n", __func__);

	if (udc->state != USB_STATE_ADDRESS &&
	    udc->state != USB_STATE_CONFIGURED)
		return -EINVAL;

	wValue = le16_to_cpu(ctrl->wValue);
	wIndex = le16_to_cpu(ctrl->wIndex);
	recip = ctrl->bRequestType & USB_RECIP_MASK;

	switch (recip) {
	case USB_RECIP_DEVICE:
		switch (wValue) {
		case USB_DEVICE_TEST_MODE:
			if (wIndex & 0xff)
				return -EINVAL;

			ret = exynos_ss_udc_handle_test_mode(udc, wIndex);
			if (ret < 0)
				return ret;
			break;
		case USB_DEVICE_U1_ENABLE:
			if (udc->gadget.speed != USB_SPEED_SUPER ||
			    udc->state == USB_STATE_ADDRESS)
				return -EINVAL;

			/*
			 * Enable U1 entry only for DWC3 revisions > 1.85a,
			 * since earlier revisions have a bug
			 */
			if (udc->core->release > 0x185a)
				__orr32(udc->regs + EXYNOS_USB3_DCTL,
					EXYNOS_USB3_DCTL_InitU1Ena);
			break;

		case USB_DEVICE_U2_ENABLE:
			if (udc->gadget.speed != USB_SPEED_SUPER ||
			    udc->state == USB_STATE_ADDRESS)
				return -EINVAL;

			/*
			 * Enable U2 entry only for DWC3 revisions > 1.85a,
			 * since earlier revisions have a bug
			 */
			if (udc->core->release > 0x185a)
				__orr32(udc->regs + EXYNOS_USB3_DCTL,
					EXYNOS_USB3_DCTL_InitU2Ena);
			break;

		default:
			return -ENOENT;
		}
		break;

	case USB_RECIP_INTERFACE:
		switch (wValue) {
		case USB_INTRF_FUNC_SUSPEND:
			if (udc->gadget.speed != USB_SPEED_SUPER ||
			    udc->state == USB_STATE_ADDRESS)
				return -EINVAL;

			/*
			 * Currently, there is no functions supporting
			 * FUNCTION_SUSPEND feature. Moreover, if a function
			 * doesn't support the feature (true for all), composite
			 * driver returns error on 'setup' call. This causes
			 * Command Verifier test to fail. To fix it, will use
			 * dummy handler instead.
			 */

			break;

		default:
			return -ENOENT;
		}
		break;

	case USB_RECIP_ENDPOINT:
		udc_ep = ep_from_windex(udc, wIndex);
		if (!udc_ep) {
			dev_dbg(udc->dev, "%s: no endpoint for 0x%04x\n",
					  __func__, wIndex);
			return -ENOENT;
		}

		if (udc->state == USB_STATE_ADDRESS &&
		    udc_ep->epnum != 0)
			return -EINVAL;

		switch (wValue) {
		case USB_ENDPOINT_HALT:
			ret = exynos_ss_udc_ep_sethalt(&udc_ep->ep, 1);
			if (ret < 0)
				return ret;
			break;

		default:
			return -ENOENT;
		}

		break;

	default:
		return -ENOENT;
	}

	return 1;
}

/**
 * exynos_ss_udc_process_get_status - process request GET_STATUS
 * @udc: The device state.
 * @ctrl: The USB control request.
 */
static int exynos_ss_udc_process_get_status(struct exynos_ss_udc *udc,
					    struct usb_ctrlrequest *ctrl)
{
	struct exynos_ss_udc_ep *udc_ep0 = &udc->eps[EP0INDEX];
	struct exynos_ss_udc_ep *udc_ep;
	u8 *reply = udc->ep0_buff;
	u32 reg;
	int ret;

	dev_dbg(udc->dev, "%s: USB_REQ_GET_STATUS\n", __func__);

	if (udc->state != USB_STATE_ADDRESS &&
	    udc->state != USB_STATE_CONFIGURED)
		return -EINVAL;

	if (!udc_ep0->dir_in) {
		dev_warn(udc->dev, "%s: direction out?\n", __func__);
		return -EINVAL;
	}

	if (le16_to_cpu(ctrl->wLength) != 2)
		return -EINVAL;

	switch (ctrl->bRequestType & USB_RECIP_MASK) {
	case USB_RECIP_DEVICE:
		*reply = 1;
		if (udc->gadget.speed == USB_SPEED_SUPER) {
			reg = readl(udc->regs + EXYNOS_USB3_DCTL);

			if (reg & EXYNOS_USB3_DCTL_InitU1Ena)
				*reply |= 1 << 2;

			if (reg & EXYNOS_USB3_DCTL_InitU2Ena)
				*reply |= 1 << 3;
		}
		*(reply + 1) = 0;
		break;

	case USB_RECIP_INTERFACE:
		if (udc->state == USB_STATE_ADDRESS)
			return -EINVAL;

		/* currently, the data result should be zero */
		*reply = 0;
		*(reply + 1) = 0;
		break;

	case USB_RECIP_ENDPOINT:
		udc_ep = ep_from_windex(udc, le16_to_cpu(ctrl->wIndex));
		if (!udc_ep)
			return -ENOENT;

		if (udc->state == USB_STATE_ADDRESS &&
		    udc_ep->epnum != 0)
			return -EINVAL;

		*reply = udc_ep->halted ? 1 : 0;
		*(reply + 1) = 0;
		break;

	default:
		return 0;
	}

	ret = exynos_ss_udc_enqueue_data(udc, reply, 2, NULL);
	if (ret) {
		dev_err(udc->dev, "%s: failed to send reply\n", __func__);
		return ret;
	}

	return 1;
}

/**
 * exynos_ss_udc_process_set_address - process request SET_CONFIGURATION
 * @udc: The device state.
 * @ctrl: The USB control request.
 */
static int exynos_ss_udc_process_set_address(struct exynos_ss_udc *udc,
					     struct usb_ctrlrequest *ctrl)
{
	int ret = 1;
	u16 address = le16_to_cpu(ctrl->wValue);

	dev_dbg(udc->dev, "%s\n", __func__);

	switch (udc->state) {
	case USB_STATE_DEFAULT:
		if (address)
			udc->state = USB_STATE_ADDRESS;
		break;

	case USB_STATE_ADDRESS:
		if (!address)
			udc->state = USB_STATE_DEFAULT;
		break;

	case USB_STATE_CONFIGURED:
	/* FALLTHROUGH */
	default:
		ret = -EINVAL;
		break;
	}

	if (ret == 1) {
		__bic32(udc->regs + EXYNOS_USB3_DCFG,
			EXYNOS_USB3_DCFG_DevAddr_MASK);
		__orr32(udc->regs + EXYNOS_USB3_DCFG,
			EXYNOS_USB3_DCFG_DevAddr(address));
		dev_dbg(udc->dev, "new address %d\n", address);
	}

	return ret;
}

/**
 * exynos_ss_udc_process_control - process a control request
 * @udc: The device state.
 * @ctrl: The control request received.
 *
 * The controller has received the SETUP phase of a control request, and
 * needs to work out what to do next (and whether to pass it on to the
 * gadget driver).
 */
static void exynos_ss_udc_process_control(struct exynos_ss_udc *udc,
					  struct usb_ctrlrequest *ctrl)
{
	struct exynos_ss_udc_ep *udc_ep0 = &udc->eps[EP0INDEX];
	int ret = 0;

	dev_dbg(udc->dev, "ctrl Req=%02x, Type=%02x, V=%04x, L=%04x\n",
		ctrl->bRequest, ctrl->bRequestType,
		ctrl->wValue, ctrl->wLength);

	/* record the direction of the request, for later use when enquing
	 * packets onto EP0. */

	udc_ep0->dir_in = (ctrl->bRequestType & USB_DIR_IN) ? 1 : 0;
	dev_dbg(udc->dev, "ctrl: dir_in=%d\n", udc_ep0->dir_in);

	/* if we've no data with this request, then the last part of the
	 * transaction is going to implicitly be IN. */
	if (ctrl->wLength == 0) {
		udc_ep0->dir_in = 1;
		udc->ep0_three_stage = 0;
		udc->ep0_state = EP0_WAIT_NRDY;
	} else
		udc->ep0_three_stage = 1;

	udc->our_status = true;

	if ((ctrl->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		switch (ctrl->bRequest) {
		case USB_REQ_SET_ADDRESS:
			ret = exynos_ss_udc_process_set_address(udc, ctrl);
			break;

		case USB_REQ_GET_STATUS:
			ret = exynos_ss_udc_process_get_status(udc, ctrl);
			break;

		case USB_REQ_CLEAR_FEATURE:
			ret = exynos_ss_udc_process_clr_feature(udc, ctrl);
			break;

		case USB_REQ_SET_FEATURE:
			ret = exynos_ss_udc_process_set_feature(udc, ctrl);
			break;

		case USB_REQ_SET_SEL:
			ret = exynos_ss_udc_process_set_sel(udc);
			break;
		case USB_REQ_SET_ISOCH_DELAY:
			ret = exynos_ss_udc_process_set_isoch_delay(udc, ctrl);
			break;
		case USB_REQ_SET_CONFIGURATION:
			ret = exynos_ss_udc_process_set_config(udc, ctrl);
			break;
		}
	}

	/* as a fallback, try delivering it to the driver to deal with */

	if (ret == 0 && udc->driver) {
		/*
		 * Gadget is responsible for Status stage handling only
		 * in case of two stage control transfer.
		 */
		if (!udc->ep0_three_stage)
			udc->our_status = false;

		ret = udc->driver->setup(&udc->gadget, ctrl);
		if (ret < 0)
			dev_dbg(udc->dev, "driver->setup() ret %d\n", ret);
	}

	/* the request is either unhandlable, or is not formatted correctly
	 * so respond with a STALL for the status stage to indicate failure.
	 */

	if (ret < 0) {
		dev_dbg(udc->dev, "ep0 stall (dir=%d)\n", udc_ep0->dir_in);
		exynos_ss_udc_ep0_restart(udc);
	}
}

/**
 * exynos_ss_udc_complete_setup - completion of a setup transfer
 * @ep: The endpoint the request was on.
 * @req: The request completed.
 *
 * Called on completion of any requests the driver itself submitted for
 * EP0 setup packets.
 */
static void exynos_ss_udc_complete_setup(struct usb_ep *ep,
					 struct usb_request *req)
{
	struct exynos_ss_udc_ep *udc_ep = our_ep(ep);
	struct exynos_ss_udc *udc = udc_ep->parent;

	if (req->status < 0) {
		dev_dbg(udc->dev, "%s: failed %d\n", __func__, req->status);
		return;
	}

	exynos_ss_udc_process_control(udc, req->buf);
}

/**
 * exynos_ss_udc_kill_all_requests - remove all requests from the endpoint's queue
 * @udc: The device state.
 * @ep: The endpoint the requests may be on.
 * @result: The result code to use.
 *
 * Go through the requests on the given endpoint and mark them
 * completed with the given result code.
 */
static void exynos_ss_udc_kill_all_requests(struct exynos_ss_udc *udc,
					    struct exynos_ss_udc_ep *udc_ep,
					    int result)
{
	struct exynos_ss_udc_req *udc_req, *treq;
	unsigned long flags;

	dev_dbg(udc->dev, "%s: ep%d\n", __func__, udc_ep->epnum);

	spin_lock_irqsave(&udc_ep->lock, flags);

	list_for_each_entry_safe(udc_req, treq, &udc_ep->req_queue, queue) {

		exynos_ss_udc_complete_request(udc, udc_ep, udc_req, result);
	}

	spin_unlock_irqrestore(&udc_ep->lock, flags);
}

/**
 * exynos_ss_udc_complete_request - complete a request given to us
 * @udc: The device state.
 * @udc_ep: The endpoint the request was on.
 * @udc_req: The request being completed.
 * @result: The result code (0 => Ok, otherwise errno)
 *
 * The given request has finished, so call the necessary completion
 * if it has one and then look to see if we can start a new request
 * on the endpoint.
 *
 * Note, expects the ep to already be locked as appropriate.
 */
static void exynos_ss_udc_complete_request(struct exynos_ss_udc *udc,
					   struct exynos_ss_udc_ep *udc_ep,
					   struct exynos_ss_udc_req *udc_req,
					   int result)
{
	bool restart;

	if (!udc_req) {
		dev_dbg(udc->dev, "%s: nothing to complete\n", __func__);
		return;
	}

	dev_dbg(udc->dev, "complete: ep %p %s, req %p, %d => %p\n",
		udc_ep, udc_ep->ep.name, udc_req,
		result, udc_req->req.complete);

	/* only replace the status if we've not already set an error
	 * from a previous transaction */

	if (udc_req->req.status == -EINPROGRESS)
		udc_req->req.status = result;

	udc_ep->req = NULL;
	udc_ep->tri = 0;
	udc_ep->sent_zlp = 0;
	list_del_init(&udc_req->queue);

	if (udc_req->req.buf != udc->ctrl_buff &&
	    udc_req->req.buf != udc->ep0_buff)
		exynos_ss_udc_unmap_dma(udc, udc_req, udc_ep);

	/* call the complete request with the locks off, just in case the
	 * request tries to queue more work for this endpoint. */

	if (udc_req->req.complete) {
		spin_unlock(&udc_ep->lock);
		udc_req->req.complete(&udc_ep->ep, &udc_req->req);
		spin_lock(&udc_ep->lock);
	}

	/* Look to see if there is anything else to do. Note, the completion
	 * of the previous request may have caused a new request to be started
	 * so be careful when doing this. */

	if (!udc_ep->req && result >= 0) {
		restart = !list_empty(&udc_ep->req_queue);
		if (restart) {
			udc_req = get_ep_head(udc_ep);
			exynos_ss_udc_start_req(udc, udc_ep, udc_req, false);
		}
	}
}

/**
 * exynos_ss_udc_complete_request_lock - complete a request given to us (locked)
 * @udc: The device state.
 * @udc_ep: The endpoint the request was on.
 * @udc_req: The request being completed.
 * @result: The result code (0 => Ok, otherwise errno).
 *
 * See exynos_ss_udc_complete_request(), but called with the endpoint's
 * lock held.
 */
static void exynos_ss_udc_complete_request_lock(struct exynos_ss_udc *udc,
					struct exynos_ss_udc_ep *udc_ep,
					struct exynos_ss_udc_req *udc_req,
					int result)
{
	unsigned long flags;

	spin_lock_irqsave(&udc_ep->lock, flags);
	exynos_ss_udc_complete_request(udc, udc_ep, udc_req, result);
	spin_unlock_irqrestore(&udc_ep->lock, flags);
}

/**
 * exynos_ss_udc_ep0_restart - stall and restart EP0
 * @udc: The device state.
 *
 * Stall EP0 and restart control transfer state machine.
 */
static void exynos_ss_udc_ep0_restart(struct exynos_ss_udc *udc)
{
	struct exynos_ss_udc_ep *udc_ep0 = &udc->eps[EP0INDEX];

	exynos_ss_udc_ep_sethalt(&udc_ep0->ep, 1);
	exynos_ss_udc_kill_all_requests(udc, udc_ep0, -ECONNRESET);
	udc->ep0_state = EP0_SETUP_PHASE;
	exynos_ss_udc_enqueue_setup(udc);
}

/**
 * exynos_ss_udc_ep_cmd_complete - process event EP Command Complete
 * @udc: The device state.
 * @udc_ep: The endpoint this event is for.
 * @event: The event being handled.
 */
static void exynos_ss_udc_ep_cmd_complete(struct exynos_ss_udc *udc,
					  struct exynos_ss_udc_ep *udc_ep,
					  u32 event)
{
	struct exynos_ss_udc_ep_command *epcmd, *tepcmd;
	struct exynos_ss_udc_req *udc_req;
	int epnum;
	int res;
	bool restart;

	dev_dbg(udc->dev, "%s: ep%d%s\n", __func__, udc_ep->epnum,
			  udc_ep->dir_in ? "in" : "out");

	/* We use IOC _only_ for End Transfer command currently */

	udc_ep->not_ready = 0;

	/* Issue all pending commands for endpoint */
	list_for_each_entry_safe(epcmd, tepcmd,
				 &udc_ep->cmd_queue, queue) {

		dev_dbg(udc->dev, "Pending command %02xh for ep%d%s\n",
				  epcmd->cmdtyp, epnum,
				  udc_ep->dir_in ? "in" : "out");

		res = exynos_ss_udc_issue_epcmd(udc, epcmd);
		if (res < 0)
			dev_err(udc->dev, "Failed to issue command\n");

		list_del_init(&epcmd->queue);
		kfree(epcmd);
	}

	/* If we have pending request, then start it */
	restart = !list_empty(&udc_ep->req_queue);
	if (restart) {
		udc_req = get_ep_head(udc_ep);
		exynos_ss_udc_start_req(udc, udc_ep,
					udc_req, false);
	}
}

/**
 * exynos_ss_udc_xfer_complete - complete transfer
 * @udc: The device state.
 * @udc_ep: The endpoint that has just completed.
 * @event: The event being handled.
 *
 * Transfer has been completed, update the transfer's state and then
 * call the relevant completion routines.
 */
static void exynos_ss_udc_xfer_complete(struct exynos_ss_udc *udc,
					struct exynos_ss_udc_ep *udc_ep,
					u32 event)
{
	struct exynos_ss_udc_req *udc_req = udc_ep->req;
	struct usb_request *req = &udc_req->req;
	int size_left;
	int result = 0;

	dev_dbg(udc->dev, "%s: ep%d%s, req %p\n",
			  __func__, udc_ep->epnum,
			  udc_ep->dir_in ? "in" : "out", req);

	if (!udc_req) {
		dev_dbg(udc->dev, "XferCompl but no req\n");
		return;
	}

	if (event & EXYNOS_USB3_DEPEVT_EventStatus_BUSERR) {
		dev_err(udc->dev, "%s: Bus Error occured\n", __func__);
		result = -ECONNRESET;
	}

	if (udc_ep->trb->param2 & EXYNOS_USB3_TRB_HWO)
		dev_err(udc->dev, "%s: HWO bit set\n", __func__);

	/* Finish ZLP handling for IN tranzactions */
	if (udc_ep->dir_in && udc_ep->sent_zlp) {
		dev_dbg(udc->dev, "%s: ZLP completed\n", __func__);
		goto sent_zlp;
	}

	size_left = udc_ep->trb->param1 & EXYNOS_USB3_TRB_BUFSIZ_MASK;

	if (udc_ep->dir_in) {
		/* Incomplete IN transfer */
		if (size_left) {
			dev_dbg(udc->dev, "%s: BUFSIZ is not zero (%d)",
					  __func__, size_left);
			/* REVISIT shall we -ECONNRESET here? */
		}

	}

	req->actual += req->length - size_left;

	/*
	 * Check if dealing with Maximum Packet Size (MPS) IN transfer.
	 * When sent data is a multiple MPS size, after last MPS sized
	 * packet send IN ZLP packet to inform the host that no more data
	 * is available.
	 * The state of req->zero member is checked to be sure that the
	 * number of bytes to send is smaller than expected from host.
	 * Check req->length to NOT send another ZLP when the current one
	 * is under completion (the one for which this completion has been
	 * called).
	 */
	if (udc_ep->dir_in && req->zero) {
		if (req->length && req->length == req->actual &&
		    !(req->length % udc_ep->ep.maxpacket)) {
			dev_dbg(udc->dev, "%s: send ZLP to complete transfer\n",
					  __func__);
			exynos_ss_udc_start_req(udc, udc_ep, udc_req, true);
			return;
		}
	}

sent_zlp:

	if (udc_ep->epnum == 0) {
		switch (udc->ep0_state) {
		case EP0_SETUP_PHASE:
			udc->ep0_state = EP0_DATA_PHASE;
			break;
		case EP0_DATA_PHASE:
			udc->ep0_state = EP0_WAIT_NRDY;
			break;
		case EP0_STATUS_PHASE:
			udc->ep0_state = EP0_SETUP_PHASE;
			if (udc->test_mode)
				exynos_ss_udc_set_test_mode(udc,
						udc->test_selector);
			break;
		default:
			dev_err(udc->dev, "%s: Erroneous EP0 state (%d)",
					  __func__, udc->ep0_state);
			/* Will try to repair from it */
			udc->ep0_state = EP0_SETUP_PHASE;
			return;
			break;
		}
	}

	exynos_ss_udc_complete_request_lock(udc, udc_ep, udc_req, result);
}

/**
 * exynos_ss_udc_xfer_notready - process event Transfer Not Ready
 * @udc: The device state.
 * @udc_ep: The endpoint this event is for.
 * @event: The event being handled.
 */
static void exynos_ss_udc_xfer_notready(struct exynos_ss_udc *udc,
					struct exynos_ss_udc_ep *udc_ep,
					u32 event)
{
	int phys_epnum = (event & EXYNOS_USB3_DEPEVT_EPNUM_MASK) >> 1;
	int direction = phys_epnum & 1;
	u32 status = event & EXYNOS_USB3_DEPEVT_EventStatus_CTL_MASK;

	dev_dbg(udc->dev, "%s: ep%d%s\n", __func__, udc_ep->epnum,
			  direction ? "in" : "out");

	if (udc_ep->epnum == 0) {
		switch (udc->ep0_state) {
		case EP0_SETUP_PHASE:
			/*
			 * Check if host is attempting to move data or start
			 * the status stage for a previous control transfer.
			 */
			if (status !=
				EXYNOS_USB3_DEPEVT_EventStatus_CTL_SETUP) {

				dev_dbg(udc->dev, "Unexpected XferNotReady "
						  "during EP0 Setup phase\n");

				exynos_ss_udc_ep0_restart(udc);
				return;
			}

			break;

		case EP0_DATA_PHASE:
			/*
			 * Check if host is attempting to move data in the
			 * wrong direction.
			 */
			if (udc_ep->dir_in != direction &&
			    status == EXYNOS_USB3_DEPEVT_EventStatus_CTL_DATA) {

				dev_dbg(udc->dev, "Unexpected XferNotReady "
						  "during EP0 Data phase\n");

				exynos_ss_udc_ep0_restart(udc);
				return;
			}

			break;

		case EP0_WAIT_NRDY:
			/*
			 * Check if host is attempting to start the data stage
			 * when data stage is not present or move more data
			 * than specified in the wLength field.
			 */
			if (status == EXYNOS_USB3_DEPEVT_EventStatus_CTL_DATA) {

				dev_dbg(udc->dev, "Unexpected XferNotReady "
						  "during EP0 Wait NotReady\n");

				exynos_ss_udc_ep0_restart(udc);
				return;
			}

			udc_ep->dir_in = direction;
			udc->ep0_state = EP0_STATUS_PHASE;

			if (udc->our_status) {
				exynos_ss_udc_enqueue_status(udc);
			} else {
				struct exynos_ss_udc_req *udc_req;
				unsigned long irqflags;

				if (list_empty(&udc_ep->req_queue))
					/*
					 * Request for Status stage will be
					 * started as soon as gadget queues it.
					 */
					return;

				/* Gadget already queued request; start it! */

				spin_lock_irqsave(&udc_ep->lock, irqflags);

				udc_req = get_ep_head(udc_ep);
				exynos_ss_udc_start_req(udc, udc_ep,
							udc_req, false);

				spin_unlock_irqrestore(&udc_ep->lock, irqflags);
			}

			break;

		case EP0_STATUS_PHASE:
			/* FALLTHROUGH */
		default:
			dev_dbg(udc->dev, "Unexpected XferNotReady\n");
			break;
		}
	}
}

/**
 * exynos_ss_udc_irq_connectdone - process event Connection Done
 * @udc: The device state.
 *
 * Get the speed of connection and configure physical endpoints 0 & 1.
 */
static void exynos_ss_udc_irq_connectdone(struct exynos_ss_udc *udc)
{
	struct exynos_ss_udc_ep_command epcmd = {{0}, };
	u32 reg, speed;
	int mps0, mps;
	int epindex;
	int res;

	dev_dbg(udc->dev, "%s\n", __func__);

	reg = readl(udc->regs + EXYNOS_USB3_DSTS);
	speed = reg & EXYNOS_USB3_DSTS_ConnectSpd_MASK;

	switch (speed) {
	/* High-speed */
	case 0:
		udc->gadget.speed = USB_SPEED_HIGH;
		mps0 = EP0_HS_MPS;
		mps = EP_HS_MPS;
		break;
	/* Full-speed */
	case 1:
	case 3:
		udc->gadget.speed = USB_SPEED_FULL;
		mps0 = EP0_FS_MPS;
		mps = EP_FS_MPS;
		break;
	/* Low-speed */
	case 2:
		udc->gadget.speed = USB_SPEED_LOW;
		mps0 = EP0_LS_MPS;
		mps = EP_LS_MPS;
		break;
	/* SuperSpeed */
	case 4:
		udc->gadget.speed = USB_SPEED_SUPER;
		mps0 = EP0_SS_MPS;
		mps = EP_SS_MPS;
		break;

	default:
		dev_err(udc->dev, "Connection speed is unknown (%d)\n", speed);
		return;
	}

	/* Suspend the inactive Phy */
	if (udc->gadget.speed == USB_SPEED_SUPER) {
		if (udc->core->ops->phy20_suspend)
			udc->core->ops->phy20_suspend(udc->core, 1);

		/* Accept U1&U2 transition */
		__orr32(udc->regs + EXYNOS_USB3_DCTL,
			EXYNOS_USB3_DCTL_AcceptU2Ena |
			EXYNOS_USB3_DCTL_AcceptU1Ena);
	} else {
		if (udc->core->ops->phy30_suspend)
			udc->core->ops->phy30_suspend(udc->core, 1);
	}

	for (epindex = 0; epindex < EXYNOS_USB3_EPS; epindex++) {
		int epnum = epindex_to_epnum(epindex, NULL);

		if (unlikely(epnum == 0))
			udc->eps[epindex].ep.maxpacket = mps0;
		else
			udc->eps[epindex].ep.maxpacket = mps;
	}

	epcmd.ep = 0;
	epcmd.param0 = EXYNOS_USB3_DEPCMDPAR0x_MPS(mps0);
	epcmd.param1 = EXYNOS_USB3_DEPCMDPAR1x_XferNRdyEn |
		       EXYNOS_USB3_DEPCMDPAR1x_XferCmplEn;
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPCFG;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0)
		dev_err(udc->dev, "Failed to configure physical EP0\n");

	epcmd.ep = 1;
	epcmd.param1 = EXYNOS_USB3_DEPCMDPAR1x_EpDir |
		       EXYNOS_USB3_DEPCMDPAR1x_XferNRdyEn |
		       EXYNOS_USB3_DEPCMDPAR1x_XferCmplEn;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0)
		dev_err(udc->dev, "Failed to configure physical EP1\n");
}

/**
 * exynos_ss_udc_irq_usbrst - process event USB Reset
 * @udc: The device state.
 */
static void exynos_ss_udc_irq_usbrst(struct exynos_ss_udc *udc)
{
	struct exynos_ss_udc_ep *udc_ep;
	int epindex;

	dev_dbg(udc->dev, "%s\n", __func__);
#ifdef CONFIG_USB_G_ANDROID
	/*
	 * Android MTP should be configuration after disconnect uevet
	 * A reset USB device has the following characteristics:
	 * - Responds to the default USB address
	 * - Is not configured
	 * - Is not initially suspended
	 */
	if (udc->state == USB_STATE_CONFIGURED)
		call_gadget(udc, disconnect);
#endif
	/* Disable test mode */
	__bic32(udc->regs + EXYNOS_USB3_DCTL, EXYNOS_USB3_DCTL_TstCtl_MASK);
	udc->test_mode = false;

	/* Enable PHYs */
	if (udc->core->ops->phy20_suspend)
		udc->core->ops->phy20_suspend(udc->core, 0);
	if (udc->core->ops->phy30_suspend)
		udc->core->ops->phy30_suspend(udc->core, 0);

	/* End transfer, kill all requests and clear STALL on the
	   non-EP0 endpoints */
	for (epindex = 0; epindex < EXYNOS_USB3_EPS; epindex++) {
		int epnum = epindex_to_epnum(epindex, NULL);

		if (unlikely(epnum == 0))
			continue;

		udc_ep = &udc->eps[epindex];

		exynos_ss_udc_end_xfer(udc, udc_ep);
		exynos_ss_udc_kill_all_requests(udc, udc_ep, -ECONNRESET);

		if (udc_ep->halted)
			exynos_ss_udc_ep_sethalt(&udc_ep->ep, 0);
	}

	/* Set device address to 0 */
	__bic32(udc->regs + EXYNOS_USB3_DCFG, EXYNOS_USB3_DCFG_DevAddr_MASK);

	udc->state = USB_STATE_DEFAULT;
}

/**
 * exynos_ss_udc_irq_ulstchng - process event USB Link State Change
 * @udc: The device state.
 * @event: The event being handled.
 */
static void exynos_ss_udc_irq_ulstchng(struct exynos_ss_udc *udc, u32 event)
{
	u32 link_state;

	link_state = event & EXYNOS_USB3_DEVT_EvtInfo_MASK;

	if (event & EXYNOS_USB3_DEVT_EvtInfo_SS) {
		if (link_state == EXYNOS_USB3_DEVT_EvtInfo_U3) {
			if (udc->core->ops->phy30_suspend)
				udc->core->ops->phy30_suspend(udc->core, 1);
		} else {
			if (udc->core->ops->phy30_suspend)
				udc->core->ops->phy30_suspend(udc->core, 0);
		}
	} else {
		if (link_state == EXYNOS_USB3_DEVT_EvtInfo_Suspend) {
			if (udc->core->ops->phy20_suspend)
				udc->core->ops->phy20_suspend(udc->core, 1);
		} else {
			if (udc->core->ops->phy20_suspend)
				udc->core->ops->phy20_suspend(udc->core, 0);
		}
	}

	/* Disconnect event detection for SMDK5250 EVT0 */
#if defined(CONFIG_MACH_SMDK5250)
	if (udc->core->release == 0x185a) {
		if (link_state == EXYNOS_USB3_DEVT_EvtInfo_Suspend ||
		    link_state == EXYNOS_USB3_DEVT_EvtInfo_SS_DIS) {
			call_gadget(udc, disconnect);
			EXYNOS_SS_UDC_CABLE_CONNECT(udc, false);
			dev_dbg(udc->dev, "Disconnection (0x%x, %s)\n",
				link_state >> EXYNOS_USB3_DEVT_EvtInfo_SHIFT,
				event & EXYNOS_USB3_DEVT_EvtInfo_SS ?
				"SS" : "non-SS");
		}
	}
#endif
}

/**
 * exynos_ss_udc_handle_depevt - handle endpoint-specific event
 * @udc: The device state.
 * @event: The event being handled.
 */
static void exynos_ss_udc_handle_depevt(struct exynos_ss_udc *udc, u32 event)
{
	int phys_epnum = (event & EXYNOS_USB3_DEPEVT_EPNUM_MASK) >> 1;
	struct exynos_ss_udc_ep *udc_ep = get_udc_ep(udc, phys_epnum);
	int epnum = udc_ep->epnum;

	switch (event & EXYNOS_USB3_DEPEVT_EVENT_MASK) {
	case EXYNOS_USB3_DEPEVT_EVENT_XferNotReady:
		dev_dbg(udc->dev, "Xfer Not Ready\n");

		exynos_ss_udc_xfer_notready(udc, udc_ep, event);
		break;

	case EXYNOS_USB3_DEPEVT_EVENT_XferComplete:
		dev_dbg(udc->dev, "Xfer Complete\n");

		exynos_ss_udc_xfer_complete(udc, udc_ep, event);

		if (epnum == 0 && udc->ep0_state == EP0_SETUP_PHASE)
			exynos_ss_udc_enqueue_setup(udc);

		break;

	case EXYNOS_USB3_DEPEVT_EVENT_EPCmdCmplt:
		dev_dbg(udc->dev, "EP Cmd Complete\n");

		exynos_ss_udc_ep_cmd_complete(udc, udc_ep, event);
		break;
	}
}

/**
 * exynos_ss_udc_handle_devt - handle device-specific event
 * @udc: The driver state.
 * @event: The event being handled.
 */
static void exynos_ss_udc_handle_devt(struct exynos_ss_udc *udc, u32 event)
{
	switch (event & EXYNOS_USB3_DEVT_EVENT_MASK) {
	case EXYNOS_USB3_DEVT_EVENT_ULStChng:
		dev_dbg(udc->dev, "USB-Link State Change");
		exynos_ss_udc_irq_ulstchng(udc, event);
		break;

	case EXYNOS_USB3_DEVT_EVENT_ConnectDone:
		dev_dbg(udc->dev, "Connection Done");
		EXYNOS_SS_UDC_CABLE_CONNECT(udc, true);
		exynos_ss_udc_irq_connectdone(udc);
		break;

	case EXYNOS_USB3_DEVT_EVENT_USBRst:
		dev_dbg(udc->dev, "USB Reset");
		exynos_ss_udc_irq_usbrst(udc);
		break;

	case EXYNOS_USB3_DEVT_EVENT_DisconnEvt:
		dev_dbg(udc->dev, "Disconnection Detected");
		call_gadget(udc, disconnect);
		udc->gadget.speed = USB_SPEED_UNKNOWN;
		udc->state = USB_STATE_NOTATTACHED;
		EXYNOS_SS_UDC_CABLE_CONNECT(udc, false);
		break;

	default:
		break;
	}
}

static void exynos_ss_udc_handle_otgevt(struct exynos_ss_udc *udc, u32 event)
{}

static void exynos_ss_udc_handle_gevt(struct exynos_ss_udc *udc, u32 event)
{}

/**
 * exynos_ss_udc_irq - handle device interrupt
 * @irq: The IRQ number triggered.
 * @pw: The pw value when registered the handler.
 */
static irqreturn_t exynos_ss_udc_irq(int irq, void *pw)
{
	struct exynos_ss_udc *udc = pw;
	int indx = udc->event_indx;
	int gevntcount;
	u32 event;
	u32 ecode1, ecode2;

	/* Check for NULL was done in probe */
	gevntcount = udc->core->ops->get_evntcount(udc->core);

	dev_dbg(udc->dev, "INTERRUPT (%d)\n", gevntcount >> 2);

	while (gevntcount) {
		event = udc->event_buff[indx++];

		dev_dbg(udc->dev, "event[%x] = 0x%08x\n",
			(unsigned int) &udc->event_buff[indx - 1], event);

		ecode1 = event & 0x01;

		if (ecode1 == 0)
			/* Device Endpoint-Specific Event */
			exynos_ss_udc_handle_depevt(udc, event);
		else {
			ecode2 = (event & 0xfe) >> 1;

			switch (ecode2) {
			/* Device-Specific Event */
			case 0x00:
				exynos_ss_udc_handle_devt(udc, event);
			break;

			/* OTG Event */
			case 0x01:
				exynos_ss_udc_handle_otgevt(udc, event);
			break;

			/* Other Core Event */
			case 0x03:
			case 0x04:
				exynos_ss_udc_handle_gevt(udc, event);
			break;

			/* Unknown Event Type */
			default:
				dev_info(udc->dev, "Unknown event type\n");
			break;
			}
		}

		/* We processed 1 event (4 bytes) */

		/* Check for NULL was done in probe */
		udc->core->ops->ack_evntcount(udc->core, 4);

		if (indx > (EXYNOS_USB3_EVENT_BUFF_WSIZE - 1))
			indx = 0;

		gevntcount -= 4;
	}

	udc->event_indx = indx;
	/* Do we need to read GEVENTCOUNT here and retry? */

	return IRQ_HANDLED;
}

/**
 * exynos_ss_udc_initep - initialize a single endpoint
 * @udc: The device state.
 * @udc_ep: The endpoint being initialised.
 * @epindex: The endpoint structure index.
 *
 * Initialise the given endpoint (as part of the probe and device state
 * creation) to give to the gadget driver. Setup the endpoint name, any
 * direction information and other state that may be required.
 */
static int __devinit exynos_ss_udc_initep(struct exynos_ss_udc *udc,
					  struct exynos_ss_udc_ep *udc_ep,
					  int epindex)
{
	int epnum, epdir;
	const char *dir, *type;

	epnum = epindex_to_epnum(epindex, &epdir);

	udc_ep->epnum = epnum;
	udc_ep->dir_in = !!epdir;

	/*
	 * Generate endpoint name using scheme:
	 * ep   ep   ep   ep   ep   ep   ep   ep   ep   ...
	 * 0    1    1    2    2    3    3    4    4    ...
	 *      in   out  in   out  in   out  in   out  ...
	 *      -    -    -    -    -    -    -    -    ...
	 *      bulk bulk int  int  iso  iso  bulk bulk ...
	 */

	if (epnum == 0) {
		dir = "";
		type = "";
	} else {
		dir = udc_ep->dir_in ? "in" : "out";
		type = ep_types[epnum % 3];
	}

	snprintf(udc_ep->name, sizeof(udc_ep->name), "ep%d%s%s%s",
			epnum, dir, epnum ? "-" : "", type);

	dev_dbg(udc->dev, "%s: %s\n", __func__, udc_ep->name);

	INIT_LIST_HEAD(&udc_ep->req_queue);
	INIT_LIST_HEAD(&udc_ep->cmd_queue);
	INIT_LIST_HEAD(&udc_ep->ep.ep_list);

	spin_lock_init(&udc_ep->lock);

	/* add to the list of endpoints known by the gadget driver */
	if (epnum)
		list_add_tail(&udc_ep->ep.ep_list, &udc->gadget.ep_list);

	udc_ep->parent = udc;
	udc_ep->ep.name = udc_ep->name;
#if defined(CONFIG_USB_EXYNOS_SS_UDC_SSMODE)
	udc_ep->ep.maxpacket = epnum ? EP_SS_MPS : EP0_SS_MPS;
#else
	udc_ep->ep.maxpacket = epnum ? EP_HS_MPS : EP0_HS_MPS;
#endif
	udc_ep->ep.ops = &exynos_ss_udc_ep_ops;
	udc_ep->trb = dmam_alloc_coherent(udc->dev,
					  sizeof(struct exynos_ss_udc_trb),
					  &udc_ep->trb_dma,
					  GFP_KERNEL);
	if (!udc_ep->trb)
		return -ENOMEM;

	memset(udc_ep->trb, 0, sizeof(struct exynos_ss_udc_trb));

	if (epnum == 0) {
		struct exynos_ss_udc_req *udc_req;

		/* allocate EP0 control request */
		udc_req = devm_kzalloc(udc->dev,
				       sizeof(struct exynos_ss_udc_req),
				       GFP_KERNEL);
		if (!udc_req)
			return -ENOMEM;

		INIT_LIST_HEAD(&udc_req->queue);
		udc_req->req.dma = DMA_ADDR_INVALID;

		udc->ctrl_req = &udc_req->req;
		udc->ep0_state = EP0_UNCONNECTED;
	}

	return 0;
}

/**
 * exynos_ss_udc_corereset - issue softreset to the core
 * @udc: The device state.
 *
 * Issue a soft reset to the core, and await the core finishing it.
 */
static int exynos_ss_udc_corereset(struct exynos_ss_udc *udc)
{
	int res;

	/* issue soft reset */
	__orr32(udc->regs + EXYNOS_USB3_DCTL, EXYNOS_USB3_DCTL_CSftRst);

	res = poll_bit_clear(udc->regs + EXYNOS_USB3_DCTL,
			     EXYNOS_USB3_DCTL_CSftRst,
			     1000);
	if (res < 0)
		dev_err(udc->dev, "Failed to get CSftRst asserted\n");

	return res;
}

/**
 * exynos_ss_udc_ep0_activate - activate USB endpoint 0
 * @udc: The device state.
 *
 * Configure physical endpoints 0 & 1.
 */
static void exynos_ss_udc_ep0_activate(struct exynos_ss_udc *udc)
{
	struct exynos_ss_udc_ep_command epcmd = {{0}, };
	int res;

	/* Start New Configuration */
	epcmd.ep = 0;
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPSTARTCFG;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0)
		dev_err(udc->dev, "Failed to start new configuration\n");

	/* Configure Physical Endpoint 0 */
	epcmd.ep = 0;
#if defined(CONFIG_USB_EXYNOS_SS_UDC_SSMODE)
	epcmd.param0 = EXYNOS_USB3_DEPCMDPAR0x_MPS(EP0_SS_MPS);
#else
	epcmd.param0 = EXYNOS_USB3_DEPCMDPAR0x_MPS(EP0_HS_MPS);
#endif
	epcmd.param1 = EXYNOS_USB3_DEPCMDPAR1x_XferNRdyEn |
		       EXYNOS_USB3_DEPCMDPAR1x_XferCmplEn;
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPCFG;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0)
		dev_err(udc->dev, "Failed to configure physical EP0\n");

	/* Configure Physical Endpoint 1 */
	epcmd.ep = 1;
#if defined(CONFIG_USB_EXYNOS_SS_UDC_SSMODE)
	epcmd.param0 = EXYNOS_USB3_DEPCMDPAR0x_MPS(EP0_SS_MPS);
#else
	epcmd.param0 = EXYNOS_USB3_DEPCMDPAR0x_MPS(EP0_HS_MPS);
#endif
	epcmd.param1 = EXYNOS_USB3_DEPCMDPAR1x_EpDir |
		       EXYNOS_USB3_DEPCMDPAR1x_XferNRdyEn |
		       EXYNOS_USB3_DEPCMDPAR1x_XferCmplEn;
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPCFG;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0)
		dev_err(udc->dev, "Failed to configure physical EP1\n");

	/* Configure Pysical Endpoint 0 Transfer Resource */
	epcmd.ep = 0;
	epcmd.param0 = EXYNOS_USB3_DEPCMDPAR0x_NumXferRes(1);
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPXFERCFG;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0)
		dev_err(udc->dev,
			"Failed to configure physical EP0 transfer resource\n");

	/* Configure Pysical Endpoint 1 Transfer Resource */
	epcmd.ep = 1;
	epcmd.param0 = EXYNOS_USB3_DEPCMDPAR0x_NumXferRes(1);
	epcmd.cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPXFERCFG;
	epcmd.cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	res = exynos_ss_udc_issue_epcmd(udc, &epcmd);
	if (res < 0)
		dev_err(udc->dev,
			"Failed to configure physical EP1 transfer resource\n");

	/* Enable Physical Endpoints 0 and 1 */
	writel(3, udc->regs + EXYNOS_USB3_DALEPENA);
}

/**
 * exynos_ss_udc_ep_activate - activate USB endpoint
 * @udc: The device state.
 * @udc_ep: The endpoint being activated.
 *
 * Configure physical endpoints > 1.
 */
static void exynos_ss_udc_ep_activate(struct exynos_ss_udc *udc,
				      struct exynos_ss_udc_ep *udc_ep)
{
	const struct usb_endpoint_descriptor *desc = udc_ep->ep.desc;
	struct exynos_ss_udc_ep_command ep_command;
	struct exynos_ss_udc_ep_command *epcmd = &ep_command;
	int epnum = udc_ep->epnum;
	int maxburst = udc_ep->ep.maxburst;
	int binterval_m1;
	int res;

	if (!udc->eps_enabled) {
		udc->eps_enabled = true;

		/* Start New Configuration */
		epcmd->ep = 0;
		epcmd->cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPSTARTCFG;
		epcmd->cmdflags =
			(2 << EXYNOS_USB3_DEPCMDx_CommandParam_SHIFT) |
			EXYNOS_USB3_DEPCMDx_CmdAct;

		res = exynos_ss_udc_issue_epcmd(udc, epcmd);
		if (res < 0)
			dev_err(udc->dev, "Failed to start new configuration\n");
	}

	if (udc_ep->not_ready) {
		epcmd = kzalloc(sizeof(struct exynos_ss_udc_ep_command),
				GFP_ATOMIC);
		if (!epcmd) {
			/* Will try to issue command immediately */
			epcmd = &ep_command;
			udc_ep->not_ready = 0;
		}
	}

	/*
	 * Databook says: when the core is operating in Full-Speed mode,
	 * bInterval_m1 field must be set to 0.
	 */
	if (udc->gadget.speed == USB_SPEED_FULL || !desc->bInterval)
		binterval_m1 = 0;
	else
		binterval_m1 = desc->bInterval - 1;

	epcmd->ep = get_phys_epnum(udc_ep);
	epcmd->param0 = EXYNOS_USB3_DEPCMDPAR0x_EPType(udc_ep->type) |
			EXYNOS_USB3_DEPCMDPAR0x_MPS(udc_ep->ep.maxpacket) |
			EXYNOS_USB3_DEPCMDPAR0x_BrstSiz(maxburst);
	if (udc_ep->dir_in)
		epcmd->param0 |= EXYNOS_USB3_DEPCMDPAR0x_FIFONum(epnum);
	epcmd->param1 = EXYNOS_USB3_DEPCMDPAR1x_EpNum(epnum) |
			(udc_ep->dir_in ? EXYNOS_USB3_DEPCMDPAR1x_EpDir : 0) |
			EXYNOS_USB3_DEPCMDPAR1x_bInterval_m1(binterval_m1) |
			EXYNOS_USB3_DEPCMDPAR1x_XferNRdyEn |
			EXYNOS_USB3_DEPCMDPAR1x_XferCmplEn;
	epcmd->cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPCFG;
	epcmd->cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	if (udc_ep->not_ready)
		list_add_tail(&epcmd->queue, &udc_ep->cmd_queue);
	else {
		res = exynos_ss_udc_issue_epcmd(udc, epcmd);
		if (res < 0)
			dev_err(udc->dev, "Failed to configure physical EP\n");
	}

	/* Configure Pysical Endpoint Transfer Resource */
	if (udc_ep->not_ready) {
		epcmd = kzalloc(sizeof(struct exynos_ss_udc_ep_command),
				GFP_ATOMIC);
		if (!epcmd) {
			epcmd = &ep_command;
			udc_ep->not_ready = 0;
		}
	}

	epcmd->ep = get_phys_epnum(udc_ep);
	epcmd->param0 = EXYNOS_USB3_DEPCMDPAR0x_NumXferRes(1);
	epcmd->cmdtyp = EXYNOS_USB3_DEPCMDx_CmdTyp_DEPXFERCFG;
	epcmd->cmdflags = EXYNOS_USB3_DEPCMDx_CmdAct;

	if (udc_ep->not_ready)
		list_add_tail(&epcmd->queue, &udc_ep->cmd_queue);
	else {
		res = exynos_ss_udc_issue_epcmd(udc, epcmd);
		if (res < 0)
			dev_err(udc->dev, "Failed to configure physical EP "
					  "transfer resource\n");
	}

	/* Enable Physical Endpoint */
	__orr32(udc->regs + EXYNOS_USB3_DALEPENA, 1 << epcmd->ep);
}

/**
 * exynos_ss_udc_ep_deactivate - deactivate USB endpoint
 * @udc: The device state.
 * @udc_ep: The endpoint being deactivated.
 *
 * End any active transfer and disable endpoint.
 */
static void exynos_ss_udc_ep_deactivate(struct exynos_ss_udc *udc,
					struct exynos_ss_udc_ep *udc_ep)
{
	int phys_epnum = get_phys_epnum(udc_ep);

	udc->eps_enabled = false;

	if (!udc_ep->not_ready)
		exynos_ss_udc_end_xfer(udc, udc_ep);

	__bic32(udc->regs + EXYNOS_USB3_DALEPENA, 1 << phys_epnum);
}

/**
 * exynos_ss_udc_init - initialize the controller
 * @udc: The device state.
 *
 * Initialize the event buffer, enable events, activate USB EP0,
 * and start the controller operation.
 */
static void exynos_ss_udc_init(struct exynos_ss_udc *udc)
{
	/* DRD configuration */
	if (udc->core->ops->config)
		udc->core->ops->config(udc->core);

	writel(EXYNOS_USB3_DCFG_NumP(1) |
	       EXYNOS_USB3_DCFG_PerFrInt(2) |
	       EXYNOS_USB3_DCFG_LPMCap |
#if defined(CONFIG_USB_EXYNOS_SS_UDC_SSMODE)
	       EXYNOS_USB3_DCFG_DevSpd(4),
#else
	       EXYNOS_USB3_DCFG_DevSpd(0),
#endif
	       udc->regs + EXYNOS_USB3_DCFG);

	/* Event buffer */
	udc->event_indx = 0;
	if (udc->core->ops->set_event_buff)
		udc->core->ops->set_event_buff(udc->core, udc->event_buff_dma,
					       EXYNOS_USB3_EVENT_BUFF_BSIZE);

	/* Enable events */
	writel(EXYNOS_USB3_DEVTEN_ULStCngEn | EXYNOS_USB3_DEVTEN_ConnectDoneEn |
	       EXYNOS_USB3_DEVTEN_USBRstEn | EXYNOS_USB3_DEVTEN_DisconnEvtEn,
	       udc->regs + EXYNOS_USB3_DEVTEN);

	exynos_ss_udc_ep0_activate(udc);
}

static int exynos_ss_udc_enable(struct exynos_ss_udc *udc)
{
	struct platform_device *pdev = to_platform_device(udc->dev);

	dev_dbg(udc->dev, "%s\n", __func__);

	if (exynos_drd_try_get(pdev)) {
		dev_dbg(udc->dev, "%s: cannot get DRD\n", __func__);
		return -EAGAIN;
	}

	pm_runtime_get_sync(udc->dev->parent);

	if (udc->core->ops->change_mode)
		udc->core->ops->change_mode(udc->core, false);
	if (udc->core->ops->config)
		udc->core->ops->config(udc->core);

	exynos_ss_udc_corereset(udc);
	exynos_ss_udc_init(udc);

	udc->eps[EP0INDEX].enabled = 1;
	udc->ep0_state = EP0_SETUP_PHASE;
	exynos_ss_udc_enqueue_setup(udc);

	/* Start controller if S/W connect was signalled */
	if (udc->pullup_state)
		exynos_ss_udc_run_stop(udc, 1);

	return 0;
}

static int exynos_ss_udc_disable(struct exynos_ss_udc *udc)
{
	struct platform_device *pdev = to_platform_device(udc->dev);
	int epindex;

	/* Stop controller if it wasn't already stopped on S/W disconnect */
	if (udc->pullup_state)
		exynos_ss_udc_run_stop(udc, 0);

	/* all endpoints should be shutdown */
	for (epindex = 0; epindex < EXYNOS_USB3_EPS; epindex++)
		exynos_ss_udc_ep_disable(&udc->eps[epindex].ep);

	call_gadget(udc, disconnect);
	udc->gadget.speed = USB_SPEED_UNKNOWN;

	pm_runtime_put_sync(udc->dev->parent);

	EXYNOS_SS_UDC_CABLE_CONNECT(udc, false);

	exynos_drd_put(pdev);

	return 0;
}

/**
 * exynos_ss_udc_vbus_session - software-controlled vbus active/in-active
 * @gadget: The peripheral being vbus active/in-active.
 * @is_active: The action to take (1 - vbus enable, 0 - vbus disable).
 */
static int exynos_ss_udc_vbus_session(struct usb_gadget *gadget, int is_active)
{
	struct exynos_ss_udc *udc = container_of(gadget,
					struct exynos_ss_udc, gadget);
	int ret;

	if (!is_active)
		ret = exynos_ss_udc_disable(udc);
	else
		ret = exynos_ss_udc_enable(udc);

	if (!ret)
		udc->vbus_state = !!is_active;

	return ret;
}

/**
 * exynos_ss_udc_pullup - software-controlled connect/disconnect to USB host
 * @gadget: The peripheral being connected/disconnected.
 * @is_on: The action to take (1 - connect, 0 - disconnect).
 */
static int exynos_ss_udc_pullup(struct usb_gadget *gadget, int is_on)
{
	struct exynos_ss_udc *udc = container_of(gadget,
					struct exynos_ss_udc, gadget);

	udc->pullup_state = !!is_on;

	/*
	 * Start controller if vbus is active.
	 * Stop controller if it wasn't already stopped on vbus deactivation.
	 */
	if (udc->vbus_state)
		exynos_ss_udc_run_stop(udc, is_on);

	return 0;
}

/**
 * exynos_ss_udc_get_config_params - get UDC configuration
 * @params: The controller parameters being returned to the caller.
 */
void exynos_ss_udc_get_config_params(struct usb_dcd_config_params *params)
{
	params->bU1devExitLat = EXYNOS_USB3_U1_DEV_EXIT_LAT;
	params->bU2DevExitLat = cpu_to_le16(EXYNOS_USB3_U2_DEV_EXIT_LAT);
}

static struct usb_gadget_ops exynos_ss_udc_gadget_ops = {
	.vbus_session		= exynos_ss_udc_vbus_session,
	.pullup			= exynos_ss_udc_pullup,
	.get_config_params	= exynos_ss_udc_get_config_params,
	.udc_start		= exynos_ss_udc_start,
	.udc_stop		= exynos_ss_udc_stop,
};

static int exynos_ss_udc_start(struct usb_gadget *gadget,
			       struct usb_gadget_driver *driver)
{
	struct exynos_ss_udc *udc;

	udc = container_of(gadget, struct exynos_ss_udc, gadget);

	if (udc->driver) {
		dev_err(udc->dev, "%s is already bound to %s\n",
				udc->gadget.name,
				udc->driver->driver.name);
		return -EBUSY;
	}

	if (driver->max_speed < USB_SPEED_FULL) {
		dev_err(udc->dev, "%s: bad speed\n", __func__);
		return -EINVAL;
	}

	driver->driver.bus = NULL;
	udc->driver = driver;
	udc->gadget.dev.driver = &driver->driver;
	udc->gadget.dev.dma_mask = udc->dev->dma_mask;
	udc->gadget.speed = USB_SPEED_UNKNOWN;

	/* report to the user, and return */
	dev_info(udc->dev, "bound driver %s\n", driver->driver.name);
	return 0;
}

static int exynos_ss_udc_stop(struct usb_gadget *gadget,
			      struct usb_gadget_driver *driver)
{
	struct exynos_ss_udc *udc;

	udc = container_of(gadget, struct exynos_ss_udc, gadget);

	udc->driver = NULL;

	dev_info(udc->dev, "unregistered gadget driver '%s'\n",
		 driver->driver.name);

	return 0;
}

static int __devinit exynos_ss_udc_probe(struct platform_device *pdev)
{
	struct dwc3_exynos_data *pdata = pdev->dev.platform_data;
	struct device *dev = &pdev->dev;
	struct exynos_ss_udc *udc;
	struct resource *res;
	int epindex;
	int ret;

	if (!pdata) {
		dev_err(dev, "cannot get platform data\n");
		return -ENODEV;
	}

	udc = devm_kzalloc(dev, sizeof(struct exynos_ss_udc), GFP_KERNEL);
	if (!udc) {
		dev_err(dev, "cannot get memory\n");
		return -ENOMEM;
	}

	udc->dev = dev;
	udc->pdata = pdata;

	udc->core = exynos_drd_bind(pdev);
	/* Check prerequisites */
	if (!udc->core->ops->get_evntcount ||
	    !udc->core->ops->ack_evntcount){
		dev_err(dev, "prerequisits are not fully met\n");
		return -ENOENT;
	}

	udc->event_buff = dmam_alloc_coherent(dev, EXYNOS_USB3_EVENT_BUFF_BSIZE,
					      &udc->event_buff_dma,
					      GFP_KERNEL);
	if (!udc->event_buff) {
		dev_err(dev, "cannot get memory for event buffer\n");
		return -ENOMEM;
	}
	memset(udc->event_buff, 0, EXYNOS_USB3_EVENT_BUFF_BSIZE);

	udc->ctrl_buff = dmam_alloc_coherent(dev, EXYNOS_USB3_CTRL_BUFF_SIZE,
					     &udc->ctrl_buff_dma,
					     GFP_KERNEL);
	if (!udc->ctrl_buff) {
		dev_err(dev, "cannot get memory for control buffer\n");
		return -ENOMEM;
	}
	memset(udc->ctrl_buff, 0, EXYNOS_USB3_CTRL_BUFF_SIZE);

	udc->ep0_buff = dmam_alloc_coherent(dev, EXYNOS_USB3_EP0_BUFF_SIZE,
					    &udc->ep0_buff_dma,
					    GFP_KERNEL);
	if (!udc->ep0_buff) {
		dev_err(dev, "cannot get memory for EP0 buffer\n");
		return -ENOMEM;
	}
	memset(udc->ep0_buff, 0, EXYNOS_USB3_EP0_BUFF_SIZE);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "cannot find register resource 0\n");
		return -ENXIO;
	}

	if (!devm_request_mem_region(dev, res->start, resource_size(res),
				     dev_name(dev))) {
		dev_err(dev, "cannot reserve registers\n");
		return -ENOENT;
	}

	udc->regs = devm_ioremap_nocache(dev, res->start, resource_size(res));
	if (!udc->regs) {
		dev_err(dev, "cannot map registers\n");
		return -ENXIO;
	}
	udc->regs -= EXYNOS_USB3_DEV_REG_START;

	ret = platform_get_irq(pdev, 0);
	if (ret < 0) {
		dev_err(dev, "cannot find irq\n");
		return ret;
	}

	udc->irq = ret;

	/*
	 * Here we request IRQ and ensure it will be disabled if neither host
	 * nor udc use DRD. It allows to avoid 'Unbalanced enable for IRQ' when
	 * the driver tries to resume DRD and enable not-disabled irq on
	 * vbus_connect event.
	 *
	 * Note: during probing the driver doesn't access any registers so we
	 * put_sync DRD immediately after requesting IRQ.
	 */
	pm_runtime_get_sync(dev->parent);
	ret = devm_request_irq(dev, udc->irq, exynos_ss_udc_irq,
			       IRQF_SHARED, dev_name(dev), udc);
	pm_runtime_put_sync(dev->parent);

	if (ret < 0) {
		dev_err(dev, "cannot claim IRQ\n");
		return ret;
	}

	dev_info(dev, "regs %p, irq %d\n", udc->regs, udc->irq);

	dev_set_name(&udc->gadget.dev, "gadget");

	udc->gadget.ops = &exynos_ss_udc_gadget_ops;
	udc->gadget.name = driver_name;

	udc->gadget.dev.parent = dev;
	udc->gadget.dev.dma_mask = dev->dma_mask;

	/* setup endpoint information */

	INIT_LIST_HEAD(&udc->gadget.ep_list);

	epindex = EP0INDEX;
	udc->gadget.ep0 = &udc->eps[epindex].ep;

	/* initialise the endpoints now the core has been initialised */
	for (epindex = 0; epindex < EXYNOS_USB3_EPS; epindex++) {
		ret = exynos_ss_udc_initep(udc, &udc->eps[epindex], epindex);
		if (ret < 0) {
			dev_err(dev, "cannot get memory for TRB\n");
			return ret;
		}
	}

	ret = device_register(&udc->gadget.dev);
	if (ret) {
		dev_err(udc->dev, "failed to register gadget device\n");
		put_device(&udc->gadget.dev);
		return ret;
	}

	ret = usb_add_gadget_udc(&pdev->dev, &udc->gadget);
	if (ret) {
		dev_err(dev, "failed to register udc\n");
		goto err_add_udc;
	}

	if (udc->core->otg) {
		ret = otg_set_peripheral(udc->core->otg, &udc->gadget);
		if (ret) {
			dev_err(dev, "failed to bind peripheral to DRD switch\n");
			goto err_otg;
		}
	}

	platform_set_drvdata(pdev, udc);

	return 0;

err_otg:
	usb_del_gadget_udc(&udc->gadget);
err_add_udc:
	device_unregister(&udc->gadget.dev);

	return ret;
}

static int __devexit exynos_ss_udc_remove(struct platform_device *pdev)
{
	struct exynos_ss_udc *udc = platform_get_drvdata(pdev);

	if (udc->core->otg)
		otg_set_peripheral(udc->core->otg, NULL);

	usb_del_gadget_udc(&udc->gadget);

	usb_gadget_unregister_driver(udc->driver);

	device_unregister(&udc->gadget.dev);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef CONFIG_PM
static int exynos_ss_udc_suspend(struct platform_device *pdev,
				 pm_message_t state)
{
	struct exynos_ss_udc *udc = platform_get_drvdata(pdev);

	if (udc->driver) {
		call_gadget(udc, suspend);
		if (udc->pdata->quirks & FORCE_PM_PERIPHERAL)
			exynos_ss_udc_disable(udc);
	}

	return 0;
}

static int exynos_ss_udc_resume(struct platform_device *pdev)
{
	struct exynos_ss_udc *udc = platform_get_drvdata(pdev);

	if (udc->driver)
		call_gadget(udc, resume);

	return 0;
}
#else
#define exynos_ss_udc_suspend NULL
#define exynos_ss_udc_resume NULL
#endif /* CONFIG_PM */

static struct platform_driver exynos_ss_udc_driver = {
	.driver		= {
		.name	= driver_name,
		.owner	= THIS_MODULE,
	},
	.probe		= exynos_ss_udc_probe,
	.remove		= __devexit_p(exynos_ss_udc_remove),
	.suspend	= exynos_ss_udc_suspend,
	.resume		= exynos_ss_udc_resume,
};

module_platform_driver(exynos_ss_udc_driver);

MODULE_DESCRIPTION("EXYNOS SuperSpeed USB 3.0 Device Controller");
MODULE_AUTHOR("Anton Tikhomirov <av.tikhomirov@samsung.com>");
MODULE_LICENSE("GPL");
