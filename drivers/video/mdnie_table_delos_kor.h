#ifndef __MDNIE_TABLE_H__
#define __MDNIE_TABLE_H__

#include "mdnie.h"

#if defined(CONFIG_FB_MDNIE_PWM)
#define MDNIE_PWM_REG_CNT		10
#define MDNIE_PWM_CABC_REG_CNT	26

static unsigned short mdnie_pwm[MDNIE_PWM_REG_CNT] = {
	0x0000, 0x0001,
	0x00f8, 0x0011,
	0x00f9, 0x0000,
	0x00ff, 0x0000,
	END_SEQ, 0x0000,
};

static unsigned short mdnie_pwm_cabc[MDNIE_PWM_CABC_REG_CNT] = {
	0x0000, 0x0001,
	0x004b, 0x0000,
	0x004c, 0x0000,
	0x004d, 0x0000,
	0x004e, 0x0000,
	0x004f, 000000,
	0x0050, 0x0000,
	0x0051, 0x0000,
	0x0052, 0x0000,
	0x0053, 0x0000,
	0x00f8, 0x0010,
	0x00ff, 0x0000,
	END_SEQ, 0x0000,
};

static unsigned short dft_cabc_plut[9] = {
	0x651,
	0x6b7,
	0x5ff,
	0x799,
	0x651,
	0x614,
	0x5ad,
	0x570,
	0x614,
};
#endif

static unsigned short tune_camera[] = {
/*start Delos camera*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1c04,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_auto_camera[] = {
/*start Delos auto camera*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1804,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_ebook[] = {
/*start Delos ebook*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1c04,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00f2,	/*SCR KgWg*/
0x009c,0x00e7,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_email[] = {
/*start Delos email*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1c04,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00eb,	/*SCR KgWg*/
0x009c,0x00eb,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_auto_ui[] = {
/*start Delos auto ui*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1804,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_auto_video[] = {
/*start Delos auto video*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0060,	/*DE pe*/
0x0043,0x0060,	/*DE pf*/
0x0044,0x0060,	/*DE pb*/
0x0045,0x0060,	/*DE ne*/
0x0046,0x0060,	/*DE nf*/
0x0047,0x0060,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1804,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_auto_gallery[] = {
/*start Delos auto gallery*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0030,	/*DE pe*/
0x0043,0x0060,	/*DE pf*/
0x0044,0x0060,	/*DE pb*/
0x0045,0x0060,	/*DE ne*/
0x0046,0x0060,	/*DE nf*/
0x0047,0x0060,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1804,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_auto_vt[] = {
/*start Delos auto vtcall*/
0x0007,0x0007,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0030,0x0fff,	/*DNR dirTh*/
0x0031,0x19ff,	/*DNR dirnumTh decon7Th*/
0x0032,0xff04,	/*DNR decon5Th maskTh*/
0x0033,0x0000,	/*DNR blTh*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x00a0,	/*DE pe*/
0x0043,0x00a0,	/*DE pf*/
0x0044,0x00a0,	/*DE pb*/
0x0045,0x00a0,	/*DE ne*/
0x0046,0x00a0,	/*DE nf*/
0x0047,0x00a0,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x0a0a,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x2004,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_auto_browser[] = {
/*start Delos auto browser*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1804,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_dynamic_ui[] = {
/*start Delos dynamic ui*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0007,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1a04,	/*CS weight grayTH*/
0x005f,0x0080,	/*CC chsel strength*/
0x0060,0x0000,	/*CC lut r  0*/
0x0061,0x0893,	/*CC lut r  16 144 */
0x0062,0x1aa5,	/*CC lut r  32 160*/
0x0063,0x29b7,	/*CC lut r  48 176*/
0x0064,0x39c8,	/*CC lut r  64 192*/
0x0065,0x4bd8,	/*CC lut r  80 208*/
0x0066,0x5de6,	/*CC lut r  96 224*/
0x0067,0x6ff4,	/*CC lut r 112 240*/
0x0068,0x81ff,	/*CC lut r 128 255*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_dynamic_video[] = {
/*start Delos dynamic video*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0007,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0070,	/*DE pe*/
0x0043,0x0070,	/*DE pf*/
0x0044,0x0070,	/*DE pb*/
0x0045,0x0070,	/*DE ne*/
0x0046,0x0070,	/*DE nf*/
0x0047,0x0070,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1a04,	/*CS weight grayTH*/
0x005f,0x0080,	/*CC chsel strength*/
0x0060,0x0000,	/*CC lut r  0*/
0x0061,0x0893,	/*CC lut r  16 144 */
0x0062,0x1aa5,	/*CC lut r  32 160*/
0x0063,0x29b7,	/*CC lut r  48 176*/
0x0064,0x39c8,	/*CC lut r  64 192*/
0x0065,0x4bd8,	/*CC lut r  80 208*/
0x0066,0x5de6,	/*CC lut r  96 224*/
0x0067,0x6ff4,	/*CC lut r 112 240*/
0x0068,0x81ff,	/*CC lut r 128 255*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_dynamic_gallery[] = {
/*start Delos dynamic gallery*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0007,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0040,	/*DE pe*/
0x0043,0x0070,	/*DE pf*/
0x0044,0x0070,	/*DE pb*/
0x0045,0x0070,	/*DE ne*/
0x0046,0x0070,	/*DE nf*/
0x0047,0x0070,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1a04,	/*CS weight grayTH*/
0x005f,0x0080,	/*CC chsel strength*/
0x0060,0x0000,	/*CC lut r  0*/
0x0061,0x0893,	/*CC lut r  16 144 */
0x0062,0x1aa5,	/*CC lut r  32 160*/
0x0063,0x29b7,	/*CC lut r  48 176*/
0x0064,0x39c8,	/*CC lut r  64 192*/
0x0065,0x4bd8,	/*CC lut r  80 208*/
0x0066,0x5de6,	/*CC lut r  96 224*/
0x0067,0x6ff4,	/*CC lut r 112 240*/
0x0068,0x81ff,	/*CC lut r 128 255*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_dynamic_vt[] = {
/*start Delos dynamic vtcall*/
0x0007,0x0007,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0007,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0030,0x0fff,	/*DNR dirTh*/
0x0031,0x19ff,	/*DNR dirnumTh decon7Th*/
0x0032,0xff04,	/*DNR decon5Th maskTh*/
0x0033,0x0000,	/*DNR blTh*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x00b0,	/*DE pe*/
0x0043,0x00b0,	/*DE pf*/
0x0044,0x00b0,	/*DE pb*/
0x0045,0x00b0,	/*DE ne*/
0x0046,0x00b0,	/*DE nf*/
0x0047,0x00b0,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x0a0a,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x2204,	/*CS weight grayTH*/
0x005f,0x0080,	/*CC chsel strength*/
0x0060,0x0000,	/*CC lut r  0*/
0x0061,0x0893,	/*CC lut r  16 144 */
0x0062,0x1aa5,	/*CC lut r  32 160*/
0x0063,0x29b7,	/*CC lut r  48 176*/
0x0064,0x39c8,	/*CC lut r  64 192*/
0x0065,0x4bd8,	/*CC lut r  80 208*/
0x0066,0x5de6,	/*CC lut r  96 224*/
0x0067,0x6ff4,	/*CC lut r 112 240*/
0x0068,0x81ff,	/*CC lut r 128 255*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_dynamic_browser[] = {
/*start Delos dynamic browser*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0007,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1a04,	/*CS weight grayTH*/
0x005f,0x0080,	/*CC chsel strength*/
0x0060,0x0000,	/*CC lut r  0*/
0x0061,0x0893,	/*CC lut r  16 144 */
0x0062,0x1aa5,	/*CC lut r  32 160*/
0x0063,0x29b7,	/*CC lut r  48 176*/
0x0064,0x39c8,	/*CC lut r  64 192*/
0x0065,0x4bd8,	/*CC lut r  80 208*/
0x0066,0x5de6,	/*CC lut r  96 224*/
0x0067,0x6ff4,	/*CC lut r 112 240*/
0x0068,0x81ff,	/*CC lut r 128 255*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_standard_ui[] = {
/*start Delos standard ui*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1c04,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_standard_video[] = {
/*start Delos standard video*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0030,	/*DE pe*/
0x0043,0x0030,	/*DE pf*/
0x0044,0x0030,	/*DE pb*/
0x0045,0x0030,	/*DE ne*/
0x0046,0x0030,	/*DE nf*/
0x0047,0x0030,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1c04,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_standard_gallery[] = {
/*start Delos standard gallery*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0030,	/*DE pe*/
0x0043,0x0030,	/*DE pf*/
0x0044,0x0030,	/*DE pb*/
0x0045,0x0030,	/*DE ne*/
0x0046,0x0030,	/*DE nf*/
0x0047,0x0030,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1c04,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_standard_vt[] = {
/*start Delos standard vtcall*/
0x0007,0x0007,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0030,0x0fff,	/*DNR dirTh*/
0x0031,0x19ff,	/*DNR dirnumTh decon7Th*/
0x0032,0xff00,	/*DNR decon5Th maskTh*/
0x0033,0x0000,	/*DNR blTh*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0100,	/*DE pe*/
0x0043,0x0100,	/*DE pf*/
0x0044,0x0100,	/*DE pb*/
0x0045,0x0100,	/*DE ne*/
0x0046,0x0100,	/*DE nf*/
0x0047,0x0100,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x0a0a,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x3004,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_standard_browser[] = {
/*start Delos standard browser*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1c04,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00f0,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00ff,	/*SCR KgWg*/
0x009c,0x00ff,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_natural_gallery[] = {
	/*start JA natural gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0020, /*DE pe*/
	0x0043, 0x0100, /*DE pf*/
	0x0044, 0x0100, /*DE pb*/
	0x0045, 0x0020, /*DE ne*/
	0x0046, 0x0100, /*DE nf*/
	0x0047, 0x0100, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_natural_ui[] = {
	/*start JA natural ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_natural_video[] = {
	/*start JA natural video*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0042, 0x0100, /*DE pe*/
	0x0043, 0x0100, /*DE pf*/
	0x0044, 0x0100, /*DE pb*/
	0x0045, 0x0100, /*DE ne*/
	0x0046, 0x0100, /*DE nf*/
	0x0047, 0x0100, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_natural_vt[] = {
	/*start JA natural vtcall*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_natural_browser[] = {
	/*start JA natural ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_movie_ui[] = {
/*start Delos movie ui*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00ff,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00e8,	/*SCR KgWg*/
0x009c,0x00eb,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_movie_video[] = {
/*start Delos movie video*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00ff,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00e8,	/*SCR KgWg*/
0x009c,0x00eb,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_movie_gallery[] = {
/*start Delos movie gallery*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00ff,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00e8,	/*SCR KgWg*/
0x009c,0x00eb,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_movie_vt[] = {
/*start Delos movie vtcall*/
0x0007,0x0007,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0007,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0007,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0030,0x0fff,	/*DNR dirTH*/
0x0031,0x19ff,	/*DNR dirnumTh decon7Th*/
0x0032,0xff04,	/*DNR decon5Th maskTh*/
0x0033,0x0000,	/*DNR blTh*/
0x0040,0x0080,	/*DE egth*/
0x0042,0x0040,	/*DE pe*/
0x0043,0x0040,	/*DE pf*/
0x0044,0x0040,	/*DE pb*/
0x0045,0x0040,	/*DE ne*/
0x0046,0x0040,	/*DE nf*/
0x0047,0x0040,	/*DE nb*/
0x0048,0x1000,	/*DE max ratio*/
0x0049,0x0100,	/*DE min ratio*/
0x0050,0x1010,	/*CS hg ry*/
0x0051,0x1010,	/*CS hg gc*/
0x0052,0x1010,	/*CS hg bm*/
0x0053,0x1204,	/*CS weight grayTH*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00ff,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00e8,	/*SCR KgWg*/
0x009c,0x00eb,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_movie_browser[] = {
/*start Delos movie browser*/
0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
0x0091,0xff00,	/*SCR RrCr*/
0x0092,0x00ff,	/*SCR RgCg*/
0x0093,0x00ff,	/*SCR RbCb*/
0x0094,0x00ff,	/*SCR GrMr*/
0x0095,0xff00,	/*SCR GgMg*/
0x0096,0x00ff,	/*SCR GbMb*/
0x0097,0x00ff,	/*SCR BrYr*/
0x0098,0x00ff,	/*SCR BgYg*/
0x0099,0xff00,	/*SCR BbYb*/
0x009a,0x00ff,	/*SCR KrWr*/
0x009b,0x00e8,	/*SCR KgWg*/
0x009c,0x00eb,	/*SCR KbWb*/
0x00ff,0x0000,	/*Mask Release*/
/*end*/
	END_SEQ, 0x0000,
};

#if defined(CONFIG_FB_MDNIE_PWM)
static unsigned short tune_cabc_ebook[] = {
	/*start JA ebook*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0000, 0x0001, /*BANK 1*/
	0x005f, 0x0080, /*CC chsel strength*/
	0x0060, 0x0000, /*CC lut r	 0*/
	0x0061, 0x1090, /*CC lut r	16 144*/
	0x0062, 0x20a0, /*CC lut r	32 160*/
	0x0063, 0x30b0, /*CC lut r	48 176*/
	0x0064, 0x40c0, /*CC lut r	64 192*/
	0x0065, 0x50d0, /*CC lut r	80 208*/
	0x0066, 0x60e0, /*CC lut r	96 224*/
	0x0067, 0x70f0, /*CC lut r 112 240*/
	0x0068, 0x80ff, /*CC lut r 128 255*/
	0x0091, 0xff00, /*SCR RrCr*/
	0x0092, 0x00ff, /*SCR RgCg*/
	0x0093, 0x00ff, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xff00, /*SCR GgMg*/
	0x0096, 0x00ff, /*SCR GbMb*/
	0x0097, 0x00ff, /*SCR BrYr*/
	0x0098, 0x00ff, /*SCR BgYg*/
	0x0099, 0xff00, /*SCR BbYb*/
	0x009a, 0x20ff, /*SCR KrWr*/
	0x009b, 0x20f4, /*SCR KgWg*/
	0x009c, 0x00e8, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_auto_ui[] = {
	/*start Vienna ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /* CS hg ry */
	0x0051, 0x1010, /* CS hg gc */
	0x0052, 0x1010, /* CS hg bm */
	0x0053, 0x1804, /* CS weight grayTH */
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xff00, /*SCR RrCr*/
	0x0092, 0x00ff, /*SCR RgCg*/
	0x0093, 0x00ff, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xff00, /*SCR GgMg*/
	0x0096, 0x00ff, /*SCR GbMb*/
	0x0097, 0x00ff, /*SCR BrYr*/
	0x0098, 0x00ff, /*SCR BgYg*/
	0x0099, 0xff00, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00ff, /*SCR KgWg*/
	0x009c, 0x00ff, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_auto_video[] = {
	/*start Vienna gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0050, /*DE pe*/
	0x0043, 0x0050, /*DE pf*/
	0x0044, 0x0050, /*DE pb*/
	0x0045, 0x0050, /*DE ne*/
	0x0046, 0x0050, /*DE nf*/
	0x0047, 0x0050, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /* CS hg ry */
	0x0051, 0x1010, /* CS hg gc */
	0x0052, 0x1010, /* CS hg bm */
	0x0053, 0x1804, /* CS weight grayTH */
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xff00, /*SCR RrCr*/
	0x0092, 0x00ff, /*SCR RgCg*/
	0x0093, 0x00ff, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xff00, /*SCR GgMg*/
	0x0096, 0x00ff, /*SCR GbMb*/
	0x0097, 0x00ff, /*SCR BrYr*/
	0x0098, 0x00ff, /*SCR BgYg*/
	0x0099, 0xff00, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00ff, /*SCR KgWg*/
	0x009c, 0x00ff, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_auto_gallery[] = {
	/*start Vienna gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0000, /*DE pe*/
	0x0043, 0x0050, /*DE pf*/
	0x0044, 0x0050, /*DE pb*/
	0x0045, 0x0050, /*DE ne*/
	0x0046, 0x0050, /*DE nf*/
	0x0047, 0x0050, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /* CS hg ry */
	0x0051, 0x1010, /* CS hg gc */
	0x0052, 0x1010, /* CS hg bm */
	0x0053, 0x1804, /* CS weight grayTH */
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xff00, /*SCR RrCr*/
	0x0092, 0x00ff, /*SCR RgCg*/
	0x0093, 0x00ff, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xff00, /*SCR GgMg*/
	0x0096, 0x00ff, /*SCR GbMb*/
	0x0097, 0x00ff, /*SCR BrYr*/
	0x0098, 0x00ff, /*SCR BgYg*/
	0x0099, 0xff00, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00ff, /*SCR KgWg*/
	0x009c, 0x00ff, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_auto_vt[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_auto_browser[] = {
	0x0000, 0x0000,	/*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x00ff, 0x0000,	/*Mask Release*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_dynamic_ui[] = {
	/*start JA dynamic ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1604, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x005f, 0x0080, /*CC chsel strength*/
	0x0060, 0x0000, /*CC lut r	0*/
	0x0061, 0x0d93, /*CC lut r	16 144 */
	0x0062, 0x1aa5, /*CC lut r	32 160*/
	0x0063, 0x29b7, /*CC lut r	48 176*/
	0x0064, 0x39c8, /*CC lut r	64 192*/
	0x0065, 0x4bd8, /*CC lut r	80 208*/
	0x0066, 0x5de6, /*CC lut r	96 224*/
	0x0067, 0x6ff4, /*CC lut r 112 240*/
	0x0068, 0x81ff, /*CC lut r 128 255*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_dynamic_video[] = {
	/*start JA dynamic video*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0042, 0x0100, /*DE pe*/
	0x0043, 0x0100, /*DE pf*/
	0x0044, 0x0100, /*DE pb*/
	0x0045, 0x0100, /*DE ne*/
	0x0046, 0x0100, /*DE nf*/
	0x0047, 0x0100, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1204, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x005f, 0x0080, /*CC chsel strength*/
	0x0060, 0x0000, /*CC lut r	0*/
	0x0061, 0x0d93, /*CC lut r	16 144 */
	0x0062, 0x1aa5, /*CC lut r	32 160*/
	0x0063, 0x29b7, /*CC lut r	48 176*/
	0x0064, 0x39c8, /*CC lut r	64 192*/
	0x0065, 0x4bd8, /*CC lut r	80 208*/
	0x0066, 0x5de6, /*CC lut r	96 224*/
	0x0067, 0x6ff4, /*CC lut r 112 240*/
	0x0068, 0x81ff, /*CC lut r 128 255*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_dynamic_gallery[] = {
	/*start JA dynamic gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0020, /*DE pe*/
	0x0043, 0x0100, /*DE pf*/
	0x0044, 0x0100, /*DE pb*/
	0x0045, 0x0020, /*DE ne*/
	0x0046, 0x0100, /*DE nf*/
	0x0047, 0x0100, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1204, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x005f, 0x0080, /*CC chsel strength*/
	0x0060, 0x0000, /*CC lut r	0*/
	0x0061, 0x0d93, /*CC lut r	16 144 */
	0x0062, 0x1aa5, /*CC lut r	32 160*/
	0x0063, 0x29b7, /*CC lut r	48 176*/
	0x0064, 0x39c8, /*CC lut r	64 192*/
	0x0065, 0x4bd8, /*CC lut r	80 208*/
	0x0066, 0x5de6, /*CC lut r	96 224*/
	0x0067, 0x6ff4, /*CC lut r 112 240*/
	0x0068, 0x81ff, /*CC lut r 128 255*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_dynamic_vt[] = {
	/*start JA dynamic vtcall*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1604, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x005f, 0x0080, /*CC chsel strength*/
	0x0060, 0x0000, /*CC lut r	0*/
	0x0061, 0x0d93, /*CC lut r	16 144 */
	0x0062, 0x1aa5, /*CC lut r	32 160*/
	0x0063, 0x29b7, /*CC lut r	48 176*/
	0x0064, 0x39c8, /*CC lut r	64 192*/
	0x0065, 0x4bd8, /*CC lut r	80 208*/
	0x0066, 0x5de6, /*CC lut r	96 224*/
	0x0067, 0x6ff4, /*CC lut r 112 240*/
	0x0068, 0x81ff, /*CC lut r 128 255*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_dynamic_browser[] = {
	/*start JA dynamic ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1604, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x005f, 0x0080, /*CC chsel strength*/
	0x0060, 0x0000, /*CC lut r	0*/
	0x0061, 0x0d93, /*CC lut r	16 144 */
	0x0062, 0x1aa5, /*CC lut r	32 160*/
	0x0063, 0x29b7, /*CC lut r	48 176*/
	0x0064, 0x39c8, /*CC lut r	64 192*/
	0x0065, 0x4bd8, /*CC lut r	80 208*/
	0x0066, 0x5de6, /*CC lut r	96 224*/
	0x0067, 0x6ff4, /*CC lut r 112 240*/
	0x0068, 0x81ff, /*CC lut r 128 255*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_standard_ui[] = {
	/*start Vienna ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0020, /*DE pe*/
	0x0043, 0x0060, /*DE pf*/
	0x0044, 0x0060, /*DE pb*/
	0x0045, 0x0020, /*DE ne*/
	0x0046, 0x0060, /*DE nf*/
	0x0047, 0x0060, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /* CS hg ry */
	0x0051, 0x1010, /* CS hg gc */
	0x0052, 0x1010, /* CS hg bm */
	0x0053, 0x1804, /* CS weight grayTH */
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xff00, /*SCR RrCr*/
	0x0092, 0x00ff, /*SCR RgCg*/
	0x0093, 0x00ff, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xff00, /*SCR GgMg*/
	0x0096, 0x00ff, /*SCR GbMb*/
	0x0097, 0x00ff, /*SCR BrYr*/
	0x0098, 0x00ff, /*SCR BgYg*/
	0x0099, 0xff00, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00ff, /*SCR KgWg*/
	0x009c, 0x00ff, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_standard_video[] = {
	/*start Vienna gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0000, /*DE pe*/
	0x0043, 0x0060, /*DE pf*/
	0x0044, 0x0060, /*DE pb*/
	0x0045, 0x0060, /*DE ne*/
	0x0046, 0x0060, /*DE nf*/
	0x0047, 0x0060, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x0d0d, /* CS hg ry */
	0x0051, 0x1010, /* CS hg gc */
	0x0052, 0x1010, /* CS hg bm */
	0x0053, 0x1404, /* CS weight grayTH */
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xff00, /*SCR RrCr*/
	0x0092, 0x00ff, /*SCR RgCg*/
	0x0093, 0x00ff, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xff00, /*SCR GgMg*/
	0x0096, 0x00ff, /*SCR GbMb*/
	0x0097, 0x00ff, /*SCR BrYr*/
	0x0098, 0x00ff, /*SCR BgYg*/
	0x0099, 0xff00, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00ff, /*SCR KgWg*/
	0x009c, 0x00ff, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_standard_gallery[] = {
	/*start Vienna gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0000, /*DE pe*/
	0x0043, 0x0060, /*DE pf*/
	0x0044, 0x0060, /*DE pb*/
	0x0045, 0x0060, /*DE ne*/
	0x0046, 0x0060, /*DE nf*/
	0x0047, 0x0060, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x0d0d, /* CS hg ry */
	0x0051, 0x1010, /* CS hg gc */
	0x0052, 0x1010, /* CS hg bm */
	0x0053, 0x1404, /* CS weight grayTH */
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xff00, /*SCR RrCr*/
	0x0092, 0x00ff, /*SCR RgCg*/
	0x0093, 0x00ff, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xff00, /*SCR GgMg*/
	0x0096, 0x00ff, /*SCR GbMb*/
	0x0097, 0x00ff, /*SCR BrYr*/
	0x0098, 0x00ff, /*SCR BgYg*/
	0x0099, 0xff00, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00ff, /*SCR KgWg*/
	0x009c, 0x00ff, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_standard_vt[] = {
	/*start JA standard vtcall*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1404, /*CS weight grayTH*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_standard_browser[] = {
	/*start JA standard ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_natural_gallery[] = {
	/*start JA natural gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0040, 0x0080, /*DE egth*/
	0x0042, 0x0020, /*DE pe*/
	0x0043, 0x0100, /*DE pf*/
	0x0044, 0x0100, /*DE pb*/
	0x0045, 0x0020, /*DE ne*/
	0x0046, 0x0100, /*DE nf*/
	0x0047, 0x0100, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_natural_ui[] = {
	/*start JA natural ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_natural_video[] = {
	/*start JA natural video*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0042, 0x0100, /*DE pe*/
	0x0043, 0x0100, /*DE pf*/
	0x0044, 0x0100, /*DE pb*/
	0x0045, 0x0100, /*DE ne*/
	0x0046, 0x0100, /*DE nf*/
	0x0047, 0x0100, /*DE nb*/
	0x0048, 0x1000, /*DE max ratio*/
	0x0049, 0x0100, /*DE min ratio*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_natural_vt[] = {
	/*start JA natural vtcall*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_natural_browser[] = {
	/*start JA natural ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x1010, /*CS hg ry*/
	0x0051, 0x1010, /*CS hg gc*/
	0x0052, 0x1010, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xfb40, /*SCR RrCr*/
	0x0092, 0x32e5, /*SCR RgCg*/
	0x0093, 0x28e2, /*SCR RbCb*/
	0x0094, 0x00ff, /*SCR GrMr*/
	0x0095, 0xd72b, /*SCR GgMg*/
	0x0096, 0x2dd6, /*SCR GbMb*/
	0x0097, 0x3af3, /*SCR BrYr*/
	0x0098, 0x00eb, /*SCR BgYg*/
	0x0099, 0xd73b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_movie_ui[] = {
	/*start JA movie ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x120e, /*CS hg ry*/
	0x0051, 0x1214, /*CS hg gc*/
	0x0052, 0x0e0d, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xd87e, /*SCR RrCr*/
	0x0092, 0x2be9, /*SCR RgCg*/
	0x0093, 0x21e1, /*SCR RbCb*/
	0x0094, 0x64e6, /*SCR GrMr*/
	0x0095, 0xde10, /*SCR GgMg*/
	0x0096, 0x41d7, /*SCR GbMb*/
	0x0097, 0x20f0, /*SCR BrYr*/
	0x0098, 0x00e8, /*SCR BgYg*/
	0x0099, 0xd04b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_movie_video[] = {
	/*start JA movie video*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x120e, /*CS hg ry*/
	0x0051, 0x1214, /*CS hg gc*/
	0x0052, 0x0e0d, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xd87e, /*SCR RrCr*/
	0x0092, 0x2be9, /*SCR RgCg*/
	0x0093, 0x21e1, /*SCR RbCb*/
	0x0094, 0x64e6, /*SCR GrMr*/
	0x0095, 0xde10, /*SCR GgMg*/
	0x0096, 0x41d7, /*SCR GbMb*/
	0x0097, 0x20f0, /*SCR BrYr*/
	0x0098, 0x00e8, /*SCR BgYg*/
	0x0099, 0xd04b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_movie_gallery[] = {
	/*start JA movie gallery*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x120e, /*CS hg ry*/
	0x0051, 0x1214, /*CS hg gc*/
	0x0052, 0x0e0d, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xd87e, /*SCR RrCr*/
	0x0092, 0x2be9, /*SCR RgCg*/
	0x0093, 0x21e1, /*SCR RbCb*/
	0x0094, 0x64e6, /*SCR GrMr*/
	0x0095, 0xde10, /*SCR GgMg*/
	0x0096, 0x41d7, /*SCR GbMb*/
	0x0097, 0x20f0, /*SCR BrYr*/
	0x0098, 0x00e8, /*SCR BgYg*/
	0x0099, 0xd04b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_movie_vt[] = {
	/*start JA movie vtcall*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x120e, /*CS hg ry*/
	0x0051, 0x1214, /*CS hg gc*/
	0x0052, 0x0e0d, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xd87e, /*SCR RrCr*/
	0x0092, 0x2be9, /*SCR RgCg*/
	0x0093, 0x21e1, /*SCR RbCb*/
	0x0094, 0x64e6, /*SCR GrMr*/
	0x0095, 0xde10, /*SCR GgMg*/
	0x0096, 0x41d7, /*SCR GbMb*/
	0x0097, 0x20f0, /*SCR BrYr*/
	0x0098, 0x00e8, /*SCR BgYg*/
	0x0099, 0xd04b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};

static unsigned short tune_cabc_movie_browser[] = {
	/*start JA movie ui*/
	0x0000, 0x0000, /*BANK 0*/
	0x0007,0x0006,	/*DNR roi latency clk on 00 01 1 0*/
	0x0009,0x0006,	/*DE roi latency clk on 00 01 1 0*/
	0x000a,0x0006,	/*CS roi latency clk on 00 01 1 0*/
	0x000b,0x0006,	/*CC roi latency clk on 00 01 1 0*/
	0x000c,0x0007,	/*SCR roi latency clk on 00 01 1 0*/
	0x0050, 0x120e, /*CS hg ry*/
	0x0051, 0x1214, /*CS hg gc*/
	0x0052, 0x0e0d, /*CS hg bm*/
	0x0053, 0x1804, /*CS weight grayTH*/
	0x0000, 0x0001, /*BANK 1*/
	0x0091, 0xd87e, /*SCR RrCr*/
	0x0092, 0x2be9, /*SCR RgCg*/
	0x0093, 0x21e1, /*SCR RbCb*/
	0x0094, 0x64e6, /*SCR GrMr*/
	0x0095, 0xde10, /*SCR GgMg*/
	0x0096, 0x41d7, /*SCR GbMb*/
	0x0097, 0x20f0, /*SCR BrYr*/
	0x0098, 0x00e8, /*SCR BgYg*/
	0x0099, 0xd04b, /*SCR BbYb*/
	0x009a, 0x00ff, /*SCR KrWr*/
	0x009b, 0x00f7, /*SCR KgWg*/
	0x009c, 0x00ef, /*SCR KbWb*/
	0x00ff, 0x0000, /*Mask Release*/
	/*end*/
	END_SEQ, 0x0000,
};
#endif

struct mdnie_tuning_info tuning_table[CABC_MAX][MODE_MAX][SCENARIO_MAX] = {
	{
		{
			{"dyn_ui",		tune_dynamic_ui},
			{"dyn_video",	tune_dynamic_video},
			{"dyn_video",	tune_dynamic_video},
			{"dyn_video",	tune_dynamic_video},
			{"camera",		tune_camera},
			{"dyn_ui",		tune_dynamic_ui},
			{"dyn_gallery",	tune_dynamic_gallery},
			{"dyn_vt",		tune_dynamic_vt},
			{"dyn_browser",	tune_dynamic_browser},
			{"dyn_ebook",		tune_ebook},
			{"email",		tune_email},
		}, {
			{"std_ui",		tune_standard_ui},
			{"std_video",	tune_standard_video},
			{"std_video",	tune_standard_video},
			{"std_video",	tune_standard_video},
			{"camera",		tune_camera},
			{"std_ui",		tune_standard_ui},
			{"std_gallery",	tune_standard_gallery},
			{"std_vt",		tune_standard_vt},
			{"std_browser",	tune_standard_browser},
			{"std_ebook",		tune_ebook},
			{"email",		tune_email},
		},
#if !defined(CONFIG_FB_MDNIE_PWM)
		{
			{"nat_ui",		tune_natural_ui},
			{"nat_video",	tune_natural_video},
			{"nat_video",	tune_natural_video},
			{"nat_video",	tune_natural_video},
			{"camera",		tune_camera},
			{"nat_ui",		tune_natural_ui},
			{"nat_gallery",	tune_natural_gallery},
			{"nat_vt",		tune_natural_vt},
			{"nat_browser",	tune_natural_browser},
			{"nat_ebook",		tune_ebook},
			{"email",		tune_email},
		},
#endif
	{
			{"mov_ui",		tune_movie_ui},
			{"mov_video",		tune_movie_video},
			{"mov_video",		tune_movie_video},
			{"mov_video",		tune_movie_video},
			{"camera",		tune_camera},
			{"mov_ui",		tune_movie_ui},
			{"mov_gallery",	tune_movie_gallery},
			{"mov_vt",		tune_movie_vt},
			{"mov_browser",	tune_movie_browser},
			{"mov_ebook",		tune_ebook},
			{"email",		tune_email},
		}, {
			{"auto_ui",		tune_auto_ui},
			{"auto_video",		tune_auto_video},
			{"auto_video",		tune_auto_video},
			{"auto_video",		tune_auto_video},
			{"auto_camera",		tune_auto_camera},
			{"auto_ui",		tune_auto_ui},
			{"auto_gallery",	tune_auto_gallery},
			{"auto_vt",		tune_auto_vt},
			{"auto_browser",	tune_auto_browser},
			{"auto_ebook",		tune_ebook},
			{"email",		tune_email},
		},
	},
#if defined(CONFIG_FB_MDNIE_PWM)
	{
		{
			{"cabc_dyn_ui",		tune_cabc_dynamic_ui},
			{"cabc_dyn_video",	tune_cabc_dynamic_video},
			{"cabc_dyn_video",	tune_cabc_dynamic_video},
			{"cabc_dyn_video",	tune_cabc_dynamic_video},
			{"camera",		tune_camera},
			{"cabc_dyn_ui",		tune_cabc_dynamic_ui},
			{"cabc_dyn_gallery",	tune_cabc_dynamic_gallery},
			{"cabc_dyn_vt",		tune_cabc_dynamic_vt},
			{"cabc_dyn_browser",	tune_cabc_dynamic_browser},
			{"cabc_dyn_ebook",		tune_cabc_ebook},
			{"email",		tune_camera},
		}, {
			{"cabc_std_ui",		tune_cabc_standard_ui},
			{"cabc_std_video",	tune_cabc_standard_video},
			{"cabc_std_video",	tune_cabc_standard_video},
			{"cabc_std_video",	tune_cabc_standard_video},
			{"camera",		tune_camera},
			{"cabc_std_ui",		tune_cabc_standard_ui},
			{"cabc_std_gallery",	tune_cabc_standard_gallery},
			{"cabc_std_vt",		tune_cabc_standard_vt},
			{"cabc_std_browser",	tune_cabc_standard_browser},
			{"cabc_std_ebook",		tune_cabc_ebook},
			{"email",		tune_camera},
		},
#if !defined(CONFIG_FB_MDNIE_PWM)
		{
			{"cabc_nat_ui",		tune_cabc_natural_ui},
			{"cabc_nat_video",	tune_cabc_natural_video},
			{"cabc_nat_video",	tune_cabc_natural_video},
			{"cabc_nat_video",	tune_cabc_natural_video},
			{"camera",		tune_camera},
			{"cabc_nat_ui",		tune_cabc_natural_ui},
			{"cabc_nat_gallery",	tune_cabc_natural_gallery},
			{"cabc_nat_vt",		tune_cabc_natural_vt},
			{"cabc_nat_browser",	tune_cabc_natural_browser},
			{"cabc_nat_ebook",		tune_cabc_ebook},
			{"email",		tune_camera},
		},
#endif
	{
			{"cabc_mov_ui",		tune_cabc_movie_ui},
			{"cabc_mov_video",		tune_cabc_movie_video},
			{"cabc_mov_video",		tune_cabc_movie_video},
			{"cabc_mov_video",		tune_cabc_movie_video},
			{"camera",		tune_camera},
			{"cabc_mov_ui",		tune_cabc_movie_ui},
			{"cabc_mov_gallery",	tune_cabc_movie_gallery},
			{"cabc_mov_vt",		tune_cabc_movie_vt},
			{"cabc_mov_browser",	tune_cabc_movie_browser},
			{"cabc_mov_ebook",		tune_cabc_ebook},
			{"email",		tune_camera},
		}, {
			{"cabc_auto_ui",		tune_cabc_auto_ui},
			{"cabc_auto_video",		tune_cabc_auto_video},
			{"cabc_auto_video",		tune_cabc_auto_video},
			{"cabc_auto_video",		tune_cabc_auto_video},
			{"camera",		tune_auto_camera},
			{"cabc_auto_ui",		tune_cabc_auto_ui},
			{"cabc_auto_gallery",	tune_cabc_auto_gallery},
			{"cabc_auto_vt",		tune_cabc_auto_vt},
			{"cabc_auto_browser",	tune_cabc_auto_browser},
			{"cabc_auto_ebook",		tune_cabc_ebook},
			{"email",		tune_camera},
		},
	}
#endif
};


#endif /* __MDNIE_TABLE_H__ */
