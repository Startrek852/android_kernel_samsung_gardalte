/*
 * Register interface file for Samsung Camera Interface (FIMC-Lite) driver
 *
 * Copyright (c) 2011 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#ifndef FLITE_CORE_H_
#define FLITE_CORE_H_

/* #define DEBUG */
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/videodev2_exynos_media.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/pm_runtime.h>
#include <linux/pm_qos.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-mediabus.h>
#include <media/exynos_flite.h>
#include <media/v4l2-ioctl.h>
#include <media/exynos_mc.h>

#if defined(CONFIG_VIDEOBUF2_CMA_PHYS)
#include <media/videobuf2-cma-phys.h>
#elif defined(CONFIG_VIDEOBUF2_ION)
#include <media/videobuf2-ion.h>
#endif
#include <plat/cpu.h>

#include "fimc-lite-reg.h"

#define flite_info(fmt, args...) \
	printk(KERN_INFO "[INFO]%s:%d: "fmt "\n", __func__, __LINE__, ##args)
#define flite_err(fmt, args...) \
	printk(KERN_ERR "[ERROR]%s:%d: "fmt "\n", __func__, __LINE__, ##args)
#define flite_warn(fmt, args...) \
	printk(KERN_WARNING "[WARNNING]%s:%d: "fmt "\n", __func__, __LINE__, ##args)

#ifdef DEBUG
#define flite_dbg(fmt, args...) \
	printk(KERN_DEBUG "[DEBUG]%s:%d: " fmt "\n", __func__, __LINE__, ##args)
#else
#define flite_dbg(fmt, args...)
#endif

#define FLITE_MAX_RESET_READY_TIME	20 /* 100ms */
#ifdef CONFIG_VIDEO_SAMSUNG_EXT_CAMERA
#define FLITE_MAX_CTRL_NUM_EXT		9
#define FLITE_MAX_CTRL_NUM		(25 + FLITE_MAX_CTRL_NUM_EXT)
#else
#define FLITE_MAX_CTRL_NUM		27
#endif
#define FLITE_CLK_NAME_SIZE		20
#define FLITE_MAX_OUT_BUFS 		flite->reqbufs_cnt
#ifdef CONFIG_ARCH_EXYNOS4
#define FLITE_MAX_MBUS_NUM		1
#endif
enum flite_input_entity {
	FLITE_INPUT_NONE,
	FLITE_INPUT_SENSOR,
	FLITE_INPUT_CSIS,
};

enum flite_output_entity {
	FLITE_OUTPUT_NONE = (1 << 0),
	FLITE_OUTPUT_GSC = (1 << 1),
	FLITE_OUTPUT_MEM = (1 << 2),
};

enum flite_out_path {
	FLITE_ISP,
	FLITE_DMA,
};

enum flite_state {
	FLITE_ST_OPEN,
	FLITE_ST_SUBDEV_OPEN,
	FLITE_ST_POWER,
	FLITE_ST_STREAM,
	FLITE_ST_SUSPEND,
	FLITE_ST_RUN,
	FLITE_ST_PIPE_STREAM,
	FLITE_ST_PEND,
};

#define flite_active(dev) test_bit(FLITE_ST_RUN, &(dev)->state)
#define ctrl_to_dev(__ctrl) \
	container_of((__ctrl)->handler, struct flite_dev, ctrl_handler)
#define flite_get_frame(flite, pad)\
	((pad == FLITE_PAD_SINK) ? &flite->s_frame : &flite->d_frame)

struct flite_variant {
	u16 max_w;
	u16 max_h;
	u16 align_win_offs_w;
	u16 align_out_w;
	u16 align_out_offs_w;
};

/**
  * struct flite_fmt - driver's color format data
  * @name :	format description
  * @code :	Media Bus pixel code
  * @fmt_reg :	H/W bit for setting format
  */
struct flite_fmt {
	char				*name;
	u32				pixelformat;
	enum v4l2_mbus_pixelcode	code;
	u32				fmt_reg;
	u32				is_yuv;
	u8				depth[VIDEO_MAX_PLANES];
};

struct flite_addr {
	dma_addr_t	y;
};

/**
 * struct flite_frame - source/target frame properties
 * @o_width:	buffer width as set by S_FMT
 * @o_height:	buffer height as set by S_FMT
 * @width:	image pixel width
 * @height:	image pixel weight
 * @offs_h:	image horizontal pixel offset
 * @offs_v:	image vertical pixel offset
 */

/*
		o_width
	---------------------
	|    width(cropped) |
	|    	----- 	    |
	|offs_h |   | 	    |
	|    	-----	    |
	|		    |
	---------------------
 */
struct flite_frame {
	u32 o_width;
	u32 o_height;
	u32 width;
	u32 height;
	u32 offs_h;
	u32 offs_v;
	unsigned long payload;
	struct flite_addr addr;
	struct flite_fmt *fmt;
	bool cacheable;
};

struct flite_pipeline {
	struct media_pipeline *pipe;
	struct v4l2_subdev *flite;
	struct v4l2_subdev *csis;
	struct v4l2_subdev *sensor;
};

struct flite_sensor_info {
	struct exynos_isp_info *pdata;
	struct v4l2_subdev *sd;
	struct clk *camclk;
	void (*priv_ops)(void *data, int arg);
};

struct flite_ctrls {
	struct v4l2_ctrl	*cacheable;
	struct v4l2_ctrl	*scenemode;
	struct v4l2_ctrl	*focusmode;
	struct v4l2_ctrl	*whitebalance;
	struct v4l2_ctrl	*effect;
	struct v4l2_ctrl	*iso;
	struct v4l2_ctrl	*contrast;
	struct v4l2_ctrl	*saturation;
	struct v4l2_ctrl	*sharpness;
	struct v4l2_ctrl	*brightness;
	struct v4l2_ctrl	*metering;
	struct v4l2_ctrl	*start_capture;
	struct v4l2_ctrl	*framerate;
	struct v4l2_ctrl	*autofocus;
	struct v4l2_ctrl	*obj_position_x;
	struct v4l2_ctrl	*obj_position_y;
	struct v4l2_ctrl	*face_detection;
	struct v4l2_ctrl	*wdr;
	struct v4l2_ctrl	*autofocus_result;
	struct v4l2_ctrl	*jpeg_quality;
	struct v4l2_ctrl	*exif_iso;
	struct v4l2_ctrl	*exif_shutterspeed;
	struct v4l2_ctrl	*aeawb_lockunlock;
	struct v4l2_ctrl	*caf_startstop;
	struct v4l2_ctrl	*digital_zoom;
#ifdef CONFIG_VIDEO_SAMSUNG_EXT_CAMERA
	struct v4l2_ctrl	*vt_mode;
	struct v4l2_ctrl	*sensor_mode;
	struct v4l2_ctrl	*touch_af_startstop;
	struct v4l2_ctrl	*flash_mode;
	struct v4l2_ctrl	*antishake;
	struct v4l2_ctrl	*check_esd;
	struct v4l2_ctrl	*antibanding;
	struct v4l2_ctrl	*exif_exptime;
	struct v4l2_ctrl	*exif_flash;
	struct v4l2_ctrl	*reset;
#else
	struct v4l2_ctrl	*flash_mode;
	struct v4l2_ctrl	*single_af;
#endif
};
/**
  * struct flite_dev - top structure of FIMC-Lite device
  * @pdev :	pointer to the FIMC-Lite platform device
  * @lock :	the mutex protecting this data structure
  * @sd :	subdevice pointer of FIMC-Lite
  * @fmt :	Media bus format of FIMC-Lite
  * @regs_res :	ioremapped regs of FIMC-Lite
  * @regs :	SFR of FIMC-Lite
  */
struct flite_dev {
	struct platform_device		*pdev;
	struct exynos_platform_flite	*pdata; /* depended on isp */
	spinlock_t			slock;
	struct v4l2_subdev		*sd_flite;
#if defined(CONFIG_MEDIA_CONTROLLER)
	struct exynos_md		*mdev;
	struct v4l2_subdev		*sd_csis;
	struct flite_sensor_info	sensor[SENSOR_MAX_ENTITIES];
	struct media_pad		pads[FLITE_PADS_NUM];
	struct media_pad		vd_pad;
	struct flite_frame		d_frame;
	struct mutex			lock;
	struct video_device		*vfd;
	int				refcnt;
	u32				reqbufs_cnt;
	struct vb2_queue		vbq;
	struct vb2_alloc_ctx		*alloc_ctx;
	const struct flite_vb2		*vb2;
	struct flite_pipeline		pipeline;
	struct v4l2_ctrl_handler	ctrl_handler;
	struct flite_ctrls		flite_ctrls;
	bool				ctrls_rdy;
	struct list_head		pending_buf_q;
	struct list_head		active_buf_q;
	int				active_buf_cnt;
	int				pending_buf_cnt;
	int				buf_index;
	struct clk			*flite_clk;
	struct clk			*camif_clk;
	bool				sysmmu_attached;
#endif
	struct v4l2_mbus_framefmt	mbus_fmt;
	struct flite_frame		s_frame;
	struct resource			*regs_res;
	void __iomem			*regs;
	int				irq;
	unsigned long			state;
	u32				out_path;
	wait_queue_head_t		irq_queue;
	u32				id;
	enum flite_input_entity		input;
	enum flite_output_entity	output;

	struct pm_qos_request	 	mif_qos;
	int				qos_lock_value;
	int				frame_rate;
	u32				ovf_cnt;

#ifdef CONFIG_VIDEO_SAMSUNG_EXT_CAMERA
	int				dbg_irq_cnt;
	int				cap_mode;
#endif
};

struct flite_vb2 {
	const struct vb2_mem_ops *ops;
	void *(*init)(struct flite_dev *flite);
	void (*cleanup)(void *alloc_ctx);

	unsigned long (*plane_addr)(struct vb2_buffer *vb, u32 plane_no);

	int (*resume)(void *alloc_ctx);
	void (*suspend)(void *alloc_ctx);

	int (*cache_flush)(struct vb2_buffer *vb, u32 num_planes);
	void (*set_cacheable)(void *alloc_ctx, bool cacheable);
	void (*set_sharable)(void *alloc_ctx, bool sharable);
};

struct flite_buffer {
	struct vb2_buffer	vb;
	struct list_head	list;
	struct flite_addr	paddr;
	int			index;
};

/* fimc-reg.c */
void flite_hw_clr_ovf_indication(struct flite_dev *dev);
void flite_hw_set_ovf_interrupt_source(struct flite_dev *dev);
void flite_hw_clr_ovf_interrupt_source(struct flite_dev *dev);
int flite_hw_check_ovf_interrupt_source(struct flite_dev *dev);
void flite_hw_set_cam_source_size(struct flite_dev *dev);
void flite_hw_set_cam_channel(struct flite_dev *dev);
void flite_hw_set_camera_type(struct flite_dev *dev, struct s3c_platform_camera *cam);
int flite_hw_set_source_format(struct flite_dev *dev);
void flite_hw_set_output_dma(struct flite_dev *dev, bool enable);
void flite_hw_set_output_gscaler(struct flite_dev *dev, bool enable);
void flite_hw_set_interrupt_source(struct flite_dev *dev, u32 source);
void flite_hw_set_config_irq(struct flite_dev *dev, struct s3c_platform_camera *cam);
void flite_hw_set_window_offset(struct flite_dev *dev);
void flite_hw_set_capture_start(struct flite_dev *dev);
void flite_hw_set_capture_stop(struct flite_dev *dev);
void flite_hw_reset(struct flite_dev *dev);
void flite_hw_set_last_capture_end_clear(struct flite_dev *dev);
void flite_hw_set_inverse_polarity(struct flite_dev *dev);
void flite_hw_set_sensor_type(struct flite_dev *dev);
void flite_hw_set_out_order(struct flite_dev *dev);
void flite_hw_set_output_size(struct flite_dev *dev);
void flite_hw_set_dma_offset(struct flite_dev *dev);
void flite_hw_set_output_addr(struct flite_dev *dev, struct flite_addr *addr,
							int index);
void flite_hw_set_output_frame_count_seq(struct flite_dev *dev, int cnt);
void flite_hw_set_output_index(struct flite_dev *dev, bool enable, int index);

#if defined(CONFIG_VIDEOBUF2_CMA_PHYS)
extern const struct flite_vb2 flite_vb2_cma;
#elif defined(CONFIG_VIDEOBUF2_ION)
extern const struct flite_vb2 flite_vb2_ion;
#endif

/* inline function for performance-sensitive region */
static inline void flite_hw_clear_irq(struct flite_dev *dev)
{
	u32 cfg = readl(dev->regs + FLITE_REG_CISTATUS);
	cfg &= ~FLITE_REG_CISTATUS_IRQ_CAM;
	writel(cfg, dev->regs + FLITE_REG_CISTATUS);
}

static inline void flite_hw_get_int_src(struct flite_dev *dev, u32 *src)
{
	*src = readl(dev->regs + FLITE_REG_CISTATUS);
	*src &= FLITE_REG_CISTATUS_IRQ_MASK;
}

static inline void user_to_drv(struct v4l2_ctrl *ctrl, s32 value)
{
	ctrl->cur.val = ctrl->val = value;
}

inline struct flite_fmt const *find_flite_format(struct v4l2_mbus_framefmt *mf);

/*
 * Add buf to the capture active buffers queue.
 * Locking: Need to be called with fimc_dev::slock held.
 */

#if defined(CONFIG_MEDIA_CONTROLLER)
static inline void active_queue_add(struct flite_dev *flite,
				    struct flite_buffer *buf)
{
	list_add_tail(&buf->list, &flite->active_buf_q);
	flite->active_buf_cnt++;
}

/*
 * Pop a video buffer from the capture active buffers queue
 * Locking: Need to be called with fimc_dev::slock held.
 */
static inline struct flite_buffer *active_queue_pop(struct flite_dev *flite)
{
	struct flite_buffer *buf;

	buf = list_entry(flite->active_buf_q.next, struct flite_buffer, list);
	list_del(&buf->list);
	flite->active_buf_cnt--;

	return buf;
}

/* Add video buffer to the capture pending buffers queue */
static inline void pending_queue_add(struct flite_dev *flite,
					  struct flite_buffer *buf)
{
	list_add_tail(&buf->list, &flite->pending_buf_q);
	flite->pending_buf_cnt++;
}

/* Add video buffer to the capture pending buffers queue */
static inline struct flite_buffer *pending_queue_pop(struct flite_dev *flite)
{
	struct flite_buffer *buf;

	buf = list_entry(flite->pending_buf_q.next, struct flite_buffer, list);
	list_del(&buf->list);
	flite->pending_buf_cnt--;

	return buf;
}

static inline void flite_lock(struct vb2_queue *vq)
{
	struct flite_dev *flite = vb2_get_drv_priv(vq);
	mutex_lock(&flite->lock);
}

static inline void flite_unlock(struct vb2_queue *vq)
{
	struct flite_dev *flite = vb2_get_drv_priv(vq);
	mutex_unlock(&flite->lock);
}

static inline bool is_qos_needed(struct flite_dev *flite)
{
	struct flite_frame *frame = &flite->d_frame;

#ifdef CONFIG_VIDEO_SAMSUNG_EXT_CAMERA
	return true;
#else
	if (((frame->width >= 1280) || (frame->height >= 720))
	    && (flite->frame_rate == 30))
		return true;
	else
		return false;
#endif
}
#endif
#endif /* FLITE_CORE_H */