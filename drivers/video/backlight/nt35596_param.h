#ifndef __NT35596_PARAM_H__
#define __NT35596_PARAM_H__

static unsigned char SEQ_INIT_FF[] = {
	0xFF,
	0x05, 0x00
};

static unsigned char SEQ_INIT_FB[] = {
	0xFB,
	0x01, 0x00
};

static unsigned char SEQ_INIT_C5[] = {
	0xC5,
	0x30, 0x00
};

static unsigned char SEQ_INIT_FF_1[] = {
	0xFF,
	0x00, 0x00
};

static unsigned char SEQ_BRIGHTNESS_SELECT[] = {
	0x51,
	0xFF, 0x00
};

static unsigned char SEQ_BACKLIGHT_CTL[] = {
	0x53,
	0xFF, 0x00
};

static unsigned char SEQ_PWR_SAVE[] = {
	0x55,
	0x00, 0x00
};

static unsigned char SEQ_SLEEP_OUT[] = {
	0x11,
	0x00, 0x00
};

static unsigned char SEQ_DISPLAY_ON[] = {
	0x29,
	0x00, 0x00
};

/* Display off */
static unsigned char SEQ_DISPLAY_OFF[] = {
	0x28,
	0x00, 0x00
};

/* Sleep in */
static unsigned char SEQ_SLEEP_IN[] = {
	0x10,
	0x00, 0x00
};

#endif /* __NT35596_PARAM_H__ */
