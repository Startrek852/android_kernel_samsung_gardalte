# Makefile of modem_if

EXTRA_CFLAGS += -Idrivers/misc/modem_v1

obj-y += sipc5_modem.o sipc5_io_device.o sipc5_common.o
obj-y += sipc4_modem.o sipc4_io_device.o
obj-y += modem_net_flowcontrol_device.o modem_utils.o

obj-$(CONFIG_CDMA_MODEM_CBP72) += modem_modemctl_device_cbp72.o
obj-$(CONFIG_LTE_MODEM_CMC221) += modem_modemctl_device_cmc221.o modem_boot_device_spi.o
obj-$(CONFIG_UMTS_MODEM_SS222) += modem_modemctl_device_ss222.o modem_boot_device_spi.o
obj-$(CONFIG_UMTS_MODEM_SH222AP) += modem_modemctl_device_sh222ap.o

obj-$(CONFIG_LINK_DEVICE_DPRAM) += modem_link_device_dpram.o modem_link_device_dpram_ext_op.o
obj-$(CONFIG_LINK_DEVICE_C2C) += modem_link_device_c2c.o
obj-$(CONFIG_LINK_DEVICE_SHMEM)	+= modem_link_device_shmem.o

# Check whether or not memory-type interface
ifeq ($(CONFIG_LINK_DEVICE_DPRAM),y)
LINK_DEVICE_MEMORY_INTERFACE=y
endif

ifeq ($(CONFIG_LINK_DEVICE_C2C),y)
LINK_DEVICE_MEMORY_INTERFACE=y
endif

ifeq ($(CONFIG_LINK_DEVICE_SHMEM),y)
LINK_DEVICE_MEMORY_INTERFACE=y
endif

ifdef LINK_DEVICE_MEMORY_INTERFACE
obj-y += modem_link_device_memory.o
endif
