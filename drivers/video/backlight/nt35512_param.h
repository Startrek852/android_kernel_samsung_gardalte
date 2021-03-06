#ifndef __NT35512_PARAM_H__
#define __NT35512_PARAM_H__

#define BOE_WVGA_GAMMA_CTR1 \
0x00, 0x37, 0x00, 0x51, 0x00, \
0x71, 0x00, 0x96, 0x00, 0xAA, \
0x00, 0xD3, 0x00, 0xF0, 0x01, \
0x1D, 0x01, 0x45, 0x01, 0x84, \
0x01, 0xB5, 0x02, 0x02, 0x02, \
0x46, 0x02, 0x48, 0x02, 0x80, \
0x02, 0xC0, 0x02, 0xE8, 0x03, \
0x14, 0x03, 0x32, 0x03, 0x5D, \
0x03, 0x73, 0x03, 0x91, 0x03, \
0xA0, 0x03, 0xBF, 0x03, 0xCF, \
0x03, 0xEF

#define BOE_WVGA_GAMMA_CTR2 \
0x00, 0x37, 0x00, 0x51, 0x00, \
0x71, 0x00, 0x96, 0x00, 0xAA, \
0x00, 0xD3, 0x00, 0xF0, 0x01, \
0x1D, 0x01, 0x45, 0x01, 0x84, \
0x01, 0xB5, 0x02, 0x02, 0x02, \
0x46, 0x02, 0x48, 0x02, 0x80, \
0x02, 0xC0, 0x02, 0xE8, 0x03, \
0x14, 0x03, 0x32, 0x03, 0x5D, \
0x03, 0x73, 0x03, 0x91, 0x03, \
0xA0, 0x03, 0xBF, 0x03, 0xCF, \
0x03, 0xEF

/* Power Sequence */
static unsigned char SEQ_POWER_INIT0[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x01
};

static unsigned char SEQ_POWER_INIT1[] = {
	0xB0,
	0x09, 0x09, 0x09
};

static unsigned char SEQ_POWER_INIT2[] = {
	0xB6,
	0x34, 0x34, 0x34
};

static unsigned char SEQ_POWER_INIT3[] = {
	0xB1,
	0x09, 0x09, 0x09
};

static unsigned char SEQ_POWER_INIT4[] = {
	0xB7,
	0x24, 0x24, 0x24
};

static unsigned char SEQ_POWER_INIT5[] = {
	0xB3,
	0x05, 0x05, 0x05
};

static unsigned char SEQ_POWER_INIT6[] = {
	0xB9,
	0x24, 0x24, 0x24
};

static unsigned char SEQ_POWER_INIT7[] = {
	0xBF,
	0x01, 0x00
};

static unsigned char SEQ_POWER_INIT8[] = {
	0xB5,
	0x0b, 0x0b, 0x0b
};

static unsigned char SEQ_POWER_INIT9[] = {
	0xBA,
	0x24, 0x24, 0x24
};

static unsigned char SEQ_POWER_INIT10[] = {
	0xC2,
	0x01, 0x00
};

static unsigned char SEQ_POWER_INIT11[] = {
	0xBC,
	0x00, 0x90, 0x00
};

static unsigned char SEQ_POWER_INIT12[] = {
	0xBD,
	0x00, 0x90, 0x00
};

/* Display Parameter */
static unsigned char SEQ_DISPLAY_INIT0[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x00
};

static unsigned char SEQ_DISPLAY_INIT1[] = {
	0xB6,
	0x06, 0x00
};

static unsigned char SEQ_DISPLAY_INIT2[] = {
	0xB7,
	0x00, 0x00
};

static unsigned char SEQ_DISPLAY_INIT3[] = {
	0xB8,
	0x01, 0x05, 0x05, 0x05
};

static unsigned char SEQ_DISPLAY_INIT4[] = {
	0xBA,
	0x01, 0x00
};

static unsigned char SEQ_DISPLAY_INIT5[] = {
	0xBC,
	0x00, 0x00, 0x00
};

static unsigned char SEQ_DISPLAY_INIT6[] = {
	0xBD,
	0x01, 0x84, 0x07, 0x32, 0x00
};

static unsigned char SEQ_DISPLAY_INIT7[] = {
	0xBE,
	0x01, 0x84, 0x07, 0x31, 0x00
};

static unsigned char SEQ_DISPLAY_INIT8[] = {
	0xBF,
	0x01, 0x84, 0x07, 0x31, 0x00
};

static unsigned char SEQ_DISPLAY_INIT9[] = {
	0xCC,
	0x03, 0x00, 0x00
};

static unsigned char SEQ_DISPLAY_INIT10[] = {
	0xB1,
	0xF8, 0x06
};

/* seq disable cmd */
static unsigned char SEQ_DISABLE_CMD[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x00, 0x00
};

/* Gamma Setting Seguece */
static unsigned char SEQ_GAMMA_INIT1[] = {
	0xD1,
	BOE_WVGA_GAMMA_CTR1
};

static unsigned char SEQ_GAMMA_INIT2[] = {
	0xD2,
	BOE_WVGA_GAMMA_CTR1
};

static unsigned char SEQ_GAMMA_INIT3[] = {
	0xD3,
	BOE_WVGA_GAMMA_CTR1
};

static unsigned char SEQ_GAMMA_INIT4[] = {
	0xD4,
	BOE_WVGA_GAMMA_CTR2
};

static unsigned char SEQ_GAMMA_INIT5[] = {
	0xD5,
	BOE_WVGA_GAMMA_CTR2
};

static unsigned char SEQ_GAMMA_INIT6[] = {
	0xD6,
	BOE_WVGA_GAMMA_CTR2
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

#endif /* __NT35512_PARAM_H__ */
