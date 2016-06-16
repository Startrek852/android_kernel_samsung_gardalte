/*
 * ir_remote_con_mc96_fw.h - ABOV IRDA control unit Firmware.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/* Total FRAME COUNT=69, Last FRAME is finishing info frame for full code
   checksum caculation */
#define FRAME_COUNT		69

/* ONE_PACKET_SIZE:(1B)+length(1B)+address(2B)+00h(1B)+code(64B)
			+checksum(1B) */
#define ONE_PACKET_SIZE		70

const u8 VALID_CODE_CHECKSUM[] = {
	0x6F, 0xF4, 0x10, 0x00, 0x20, 0xFF, 0x02, 0x92,
};

const u8 INITIAL_USER_IR_MODE_DATA[8] = {
	0x00, 0x06, 0x01, 0x01, 0x01, 0x06, 0x00, 0x0F,
};

const u8 IRDA_binary[] = {
	/* Model name       : ESPRESSO_LTE */
	/* Module revision  : 0x0302 */
	/* Firmware version : 0x0001 */
	0x3A, 0x40, 0x10, 0x00, 0x00, 0x02, 0x1D, 0x58, 0x02, 0x1F,
	0xED, 0xD2, 0x0D, 0x22, 0x00, 0x00, 0x02, 0x1F, 0xF4, 0xD2,
	0x0F, 0x22, 0x00, 0x00, 0x02, 0x1F, 0xFB, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x20, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x20,
	0x10, 0x7E, 0x08, 0x7F, 0x00, 0x90, 0x00, 0x00, 0xE4, 0xF0,
	0xA3, 0xDF, 0xFC, 0xDE, 0xFA, 0xE4, 0xF5, 0x42, 0xF5, 0xA6,

	0x3A, 0x40, 0x10, 0x40, 0x00, 0x43, 0xF5, 0x40, 0xF5, 0x41,
	0xF5, 0x3E, 0x75, 0x3F, 0x08, 0x75, 0x9C, 0x5D, 0x75, 0xA5,
	0xA1, 0x22, 0x00, 0x00, 0x02, 0x10, 0x56, 0xC0, 0xE0, 0xC0,
	0x83, 0xC0, 0x82, 0xC0, 0xD0, 0x75, 0xD0, 0x08, 0x43, 0x80,
	0x20, 0x85, 0x9D, 0x5B, 0xE5, 0x9D, 0x70, 0x13, 0x12, 0x1E,
	0x53, 0x00, 0x53, 0x80, 0xDF, 0x43, 0x80, 0x20, 0x53, 0x80,
	0xDF, 0x43, 0x80, 0x20, 0x02, 0x14, 0xDC, 0xE5, 0x9C, 0x99,

	0x3A, 0x40, 0x10, 0x80, 0x00, 0x30, 0xE2, 0x03, 0x02, 0x12,
	0xBE, 0x75, 0x1C, 0x00, 0xE5, 0x5B, 0x30, 0xE4, 0x60, 0x30,
	0xE1, 0x38, 0x75, 0x42, 0x00, 0x75, 0x43, 0x00, 0x75, 0x3C,
	0x00, 0x75, 0x3D, 0x08, 0xAB, 0x30, 0xAA, 0x31, 0xA9, 0x32,
	0x85, 0x43, 0x82, 0x85, 0x42, 0x83, 0x12, 0x17, 0x68, 0xFF,
	0xF5, 0xA5, 0x74, 0x00, 0x25, 0x43, 0xF5, 0x82, 0x74, 0x00,
	0x35, 0x42, 0xF5, 0x83, 0xEF, 0xF0, 0x05, 0x43, 0xE5, 0xD0,

	0x3A, 0x40, 0x10, 0xC0, 0x00, 0x43, 0x70, 0x02, 0x05, 0x42,
	0xF5, 0x82, 0x80, 0x6E, 0x75, 0x40, 0x00, 0x75, 0x41, 0x00,
	0x85, 0xA5, 0x48, 0xAF, 0x9D, 0x75, 0x5D, 0x00, 0x8F, 0x5E,
	0x75, 0x3C, 0x08, 0x75, 0x3D, 0x04, 0xE5, 0x48, 0x64, 0xA0,
	0x70, 0x03, 0x02, 0x14, 0x0C, 0x12, 0x1E, 0x53, 0x02, 0x14,
	0x0C, 0xE5, 0x5B, 0x20, 0xE1, 0x03, 0x02, 0x11, 0x77, 0xC3,
	0xE5, 0x43, 0x95, 0x3D, 0xE5, 0x42, 0x95, 0x3C, 0x50, 0xFA,

	0x3A, 0x40, 0x11, 0x00, 0x00, 0x6E, 0x85, 0x49, 0xA5, 0x74,
	0x00, 0x25, 0x43, 0xF5, 0x82, 0x74, 0x00, 0x35, 0x42, 0xF5,
	0x83, 0xE5, 0x49, 0xF0, 0x05, 0x43, 0xE5, 0x43, 0x70, 0x02,
	0x05, 0x42, 0xE5, 0x3D, 0x24, 0xFE, 0xFF, 0xE5, 0x3C, 0x34,
	0xFF, 0xFE, 0xC3, 0xE5, 0x43, 0x9F, 0xE5, 0x42, 0x9E, 0x50,
	0x14, 0xAB, 0x30, 0xAA, 0x31, 0xA9, 0x32, 0x85, 0x43, 0x82,
	0x85, 0x42, 0x83, 0x12, 0x17, 0x68, 0xF5, 0x49, 0x02, 0x93,

	0x3A, 0x40, 0x11, 0x40, 0x00, 0x14, 0x0C, 0xEF, 0xB5, 0x43,
	0x0B, 0xEE, 0xB5, 0x42, 0x07, 0xE5, 0x38, 0xF5, 0x49, 0x02,
	0x14, 0x0C, 0xE5, 0x3D, 0x24, 0xFF, 0xFF, 0xE5, 0x3C, 0x34,
	0xFF, 0xFE, 0xEF, 0x65, 0x43, 0x70, 0x03, 0xEE, 0x65, 0x42,
	0x60, 0x03, 0x02, 0x14, 0x0C, 0xE5, 0x39, 0xF5, 0x49, 0x02,
	0x14, 0x0C, 0x75, 0x49, 0x00, 0xD2, 0x06, 0x02, 0x14, 0x0C,
	0x85, 0xA5, 0x5C, 0xC3, 0xE5, 0x41, 0x95, 0x3D, 0xE5, 0xDE,

	0x3A, 0x40, 0x11, 0x80, 0x00, 0x40, 0x95, 0x3C, 0x40, 0x03,
	0x02, 0x12, 0xB4, 0xE5, 0x41, 0x45, 0x40, 0x70, 0x1C, 0xE5,
	0x5C, 0xFD, 0x7C, 0x00, 0x8C, 0x44, 0xF5, 0x45, 0xE5, 0x45,
	0x75, 0x45, 0x00, 0xF5, 0x44, 0x75, 0x33, 0x00, 0x75, 0x34,
	0x00, 0x8C, 0x46, 0x8D, 0x47, 0x80, 0x23, 0xE5, 0x41, 0x64,
	0x01, 0x45, 0x40, 0x70, 0x26, 0xAF, 0x5C, 0xFE, 0xEF, 0x42,
	0x45, 0xE5, 0x45, 0x24, 0x02, 0xF5, 0x3D, 0xE4, 0x35, 0xA1,

	0x3A, 0x40, 0x11, 0xC0, 0x00, 0x44, 0xF5, 0x3C, 0xEF, 0x25,
	0x47, 0xF5, 0x47, 0xEE, 0x35, 0x46, 0xF5, 0x46, 0x75, 0x3A,
	0x00, 0x75, 0x3B, 0x00, 0xC2, 0x01, 0x02, 0x12, 0xA9, 0xC3,
	0xE5, 0x41, 0x94, 0x05, 0xE5, 0x40, 0x94, 0x00, 0x50, 0x27,
	0xE5, 0x5C, 0x25, 0x47, 0xF5, 0x47, 0xE4, 0x35, 0x46, 0xF5,
	0x46, 0x74, 0xFE, 0x25, 0x41, 0xF5, 0x82, 0x74, 0xFF, 0x35,
	0x40, 0xF5, 0x83, 0xE5, 0x5C, 0xF0, 0x75, 0x3A, 0x00, 0x9C,

	0x3A, 0x40, 0x12, 0x00, 0x00, 0x75, 0x3B, 0x03, 0xC2, 0x01,
	0xC2, 0x00, 0x02, 0x12, 0xA9, 0xE5, 0x3D, 0x24, 0xFE, 0xFF,
	0xE5, 0x3C, 0x34, 0xFF, 0xFE, 0xC3, 0xE5, 0x41, 0x9F, 0xE5,
	0x40, 0x9E, 0x50, 0x5A, 0xE5, 0x5C, 0x25, 0x47, 0xF5, 0x47,
	0xE4, 0x35, 0x46, 0xF5, 0x46, 0xC3, 0xE5, 0x3A, 0x94, 0x08,
	0x50, 0x17, 0x05, 0x3B, 0xE5, 0x3B, 0xAC, 0x3A, 0x70, 0x02,
	0x05, 0x3A, 0x14, 0x24, 0x00, 0xF5, 0x82, 0x74, 0x00, 0xAD,

	0x3A, 0x40, 0x12, 0x40, 0x00, 0x3C, 0xF5, 0x83, 0xE5, 0x5C,
	0xF0, 0xC3, 0xE5, 0x3A, 0x94, 0x08, 0x50, 0x0E, 0x74, 0x00,
	0x25, 0x3B, 0xF5, 0x82, 0x74, 0x00, 0x35, 0x3A, 0xF5, 0x83,
	0xE4, 0xF0, 0xE5, 0x3B, 0x24, 0x01, 0xE4, 0x35, 0x3A, 0xC3,
	0x94, 0x08, 0x50, 0x42, 0x74, 0x01, 0x25, 0x3B, 0xF5, 0x82,
	0x74, 0x00, 0x35, 0x3A, 0xF5, 0x83, 0xE4, 0xF0, 0x80, 0x32,
	0xEF, 0xB5, 0x41, 0x13, 0xEE, 0xB5, 0x40, 0x0F, 0x75, 0x1E,

	0x3A, 0x40, 0x12, 0x80, 0x00, 0x33, 0x00, 0x85, 0x5C, 0x34,
	0xE5, 0x34, 0x75, 0x34, 0x00, 0xF5, 0x33, 0x80, 0x1B, 0xE5,
	0x3D, 0x24, 0xFF, 0xFF, 0xE5, 0x3C, 0x34, 0xFF, 0xFE, 0xEF,
	0xB5, 0x41, 0x0D, 0xEE, 0xB5, 0x40, 0x09, 0xE5, 0x5C, 0x42,
	0x34, 0x75, 0x9C, 0x70, 0xD2, 0x04, 0x05, 0x41, 0xE5, 0x41,
	0x70, 0x0C, 0x05, 0x40, 0x02, 0x14, 0x0C, 0xAF, 0x9D, 0x75,
	0x5D, 0x00, 0x8F, 0x5E, 0x02, 0x14, 0x0C, 0xE5, 0x5B, 0x23,

	0x3A, 0x40, 0x12, 0xC0, 0x00, 0x20, 0xE1, 0x03, 0x02, 0x14,
	0x24, 0x20, 0xE0, 0x03, 0x02, 0x14, 0x1A, 0x75, 0x1C, 0x00,
	0xC3, 0xE5, 0x43, 0x95, 0x3F, 0xE5, 0x42, 0x95, 0x3E, 0x40,
	0x03, 0x02, 0x14, 0x12, 0xE5, 0x43, 0x45, 0x42, 0x70, 0x0C,
	0xE5, 0x44, 0xFF, 0xF5, 0xA5, 0x75, 0x46, 0x00, 0x8F, 0x47,
	0x80, 0x17, 0xE5, 0x43, 0x64, 0x01, 0x45, 0x42, 0x70, 0x1A,
	0xE5, 0x45, 0xF5, 0xA5, 0xE5, 0x45, 0x25, 0x47, 0xF5, 0xC6,

	0x3A, 0x40, 0x13, 0x00, 0x00, 0x47, 0xE4, 0x35, 0x46, 0xF5,
	0x46, 0x75, 0x3A, 0x00, 0x75, 0x3B, 0x00, 0xC2, 0x01, 0x02,
	0x14, 0x04, 0xC3, 0xE5, 0x43, 0x94, 0x05, 0xE5, 0x42, 0x94,
	0x00, 0x50, 0x3F, 0xAB, 0x35, 0xAA, 0x36, 0xA9, 0x37, 0xE5,
	0x43, 0x24, 0xFF, 0xFF, 0xE5, 0x42, 0x34, 0xFF, 0x8F, 0x82,
	0xF5, 0x83, 0x12, 0x17, 0x68, 0xF5, 0xA5, 0xF5, 0x5C, 0x25,
	0x47, 0xF5, 0x47, 0xE4, 0x35, 0x46, 0xF5, 0x46, 0x74, 0xC3,

	0x3A, 0x40, 0x13, 0x40, 0x00, 0xFE, 0x25, 0x43, 0xF5, 0x82,
	0x74, 0xFF, 0x35, 0x42, 0xF5, 0x83, 0xE5, 0x5C, 0xF0, 0x75,
	0x3A, 0x00, 0x75, 0x3B, 0x03, 0xC2, 0x01, 0xC2, 0x00, 0x02,
	0x14, 0x04, 0xE5, 0x3F, 0x24, 0xFE, 0xFF, 0xE5, 0x3E, 0x34,
	0xFF, 0xFE, 0xC3, 0xE5, 0x43, 0x9F, 0xE5, 0x42, 0x9E, 0x50,
	0x72, 0xAB, 0x35, 0xAA, 0x36, 0xA9, 0x37, 0xE5, 0x43, 0x24,
	0xFF, 0xFD, 0xE5, 0x42, 0x34, 0xFF, 0x8D, 0x82, 0xF5, 0x0C,

	0x3A, 0x40, 0x13, 0x80, 0x00, 0x83, 0x12, 0x17, 0x68, 0xF5,
	0xA5, 0xF5, 0x5C, 0x25, 0x47, 0xF5, 0x47, 0xE4, 0x35, 0x46,
	0xF5, 0x46, 0xC3, 0xE5, 0x3A, 0x94, 0x08, 0x50, 0x17, 0x05,
	0x3B, 0xE5, 0x3B, 0xAC, 0x3A, 0x70, 0x02, 0x05, 0x3A, 0x14,
	0x24, 0x00, 0xF5, 0x82, 0x74, 0x00, 0x3C, 0xF5, 0x83, 0xE5,
	0x5C, 0xF0, 0xC3, 0xE5, 0x3A, 0x94, 0x08, 0x50, 0x0E, 0x74,
	0x00, 0x25, 0x3B, 0xF5, 0x82, 0x74, 0x00, 0x35, 0x3A, 0xFE,

	0x3A, 0x40, 0x13, 0xC0, 0x00, 0xF5, 0x83, 0xE4, 0xF0, 0xE5,
	0x3B, 0x24, 0x01, 0xE4, 0x35, 0x3A, 0xC3, 0x94, 0x08, 0x50,
	0x34, 0x74, 0x01, 0x25, 0x3B, 0xF5, 0x82, 0x74, 0x00, 0x35,
	0x3A, 0xF5, 0x83, 0xE4, 0xF0, 0x80, 0x24, 0xEF, 0xB5, 0x43,
	0x0A, 0xEE, 0xB5, 0x42, 0x06, 0xE5, 0x33, 0xF5, 0xA5, 0x80,
	0x16, 0xE5, 0x3F, 0x24, 0xFF, 0xFF, 0xE5, 0x3E, 0x34, 0xFF,
	0xFE, 0xEF, 0xB5, 0x43, 0x08, 0xEE, 0xB5, 0x42, 0x04, 0x71,

	0x3A, 0x40, 0x14, 0x00, 0x00, 0xE5, 0x34, 0xF5, 0xA5, 0x05,
	0x43, 0xE5, 0x43, 0x70, 0x02, 0x05, 0x42, 0x75, 0x9D, 0xFF,
	0x02, 0x14, 0xDC, 0xD2, 0x05, 0x75, 0x9C, 0x7A, 0x02, 0x14,
	0xDC, 0x12, 0x1E, 0x53, 0xD2, 0x07, 0xD2, 0x05, 0x02, 0x14,
	0xDC, 0xE5, 0x5B, 0x20, 0xE0, 0x03, 0x02, 0x14, 0xD5, 0x75,
	0x1C, 0x00, 0x30, 0xE7, 0x1C, 0xAF, 0xA5, 0x75, 0x5D, 0x00,
	0x8F, 0x5E, 0xAF, 0x9D, 0x75, 0x5D, 0x00, 0x8F, 0x5E, 0x44,

	0x3A, 0x40, 0x14, 0x40, 0x00, 0x75, 0x40, 0x00, 0x75, 0x41,
	0x00, 0x75, 0x3E, 0x00, 0x75, 0x3F, 0x08, 0x80, 0x69, 0xC3,
	0xE5, 0x41, 0x95, 0x3F, 0xE5, 0x40, 0x95, 0x3E, 0x50, 0x63,
	0x74, 0x00, 0x25, 0x41, 0xF5, 0x82, 0x74, 0x00, 0x35, 0x40,
	0xF5, 0x83, 0xE5, 0xA5, 0xF0, 0x05, 0x41, 0xE5, 0x41, 0x70,
	0x02, 0x05, 0x40, 0x64, 0x02, 0x45, 0x40, 0x70, 0x32, 0x90,
	0x00, 0x00, 0xE0, 0x75, 0x5D, 0x00, 0xF5, 0x5E, 0xE5, 0xEB,

	0x3A, 0x40, 0x14, 0x80, 0x00, 0x5E, 0x75, 0x5E, 0x00, 0xF5,
	0x5D, 0xA3, 0xE0, 0x42, 0x5E, 0x74, 0x02, 0x25, 0x5E, 0xF5,
	0x5E, 0xE4, 0x35, 0x5D, 0xF5, 0x5D, 0xD3, 0xE5, 0x5E, 0x94,
	0x00, 0xE5, 0x5D, 0x94, 0x08, 0x50, 0x17, 0x85, 0x5D, 0x3E,
	0x85, 0x5E, 0x3F, 0x80, 0x0F, 0xE5, 0x41, 0xB5, 0x3F, 0x0A,
	0xE5, 0x40, 0xB5, 0x3E, 0x05, 0x75, 0x9C, 0x70, 0xD2, 0x02,
	0x75, 0x9D, 0xFF, 0x80, 0x03, 0x75, 0x9C, 0x7A, 0x75, 0x88,

	0x3A, 0x40, 0x14, 0xC0, 0x00, 0x5D, 0x00, 0x75, 0x5E, 0x00,
	0x05, 0x5E, 0xE5, 0x5E, 0x70, 0x02, 0x05, 0x5D, 0x64, 0x0A,
	0x45, 0x5D, 0x70, 0xF2, 0x80, 0x07, 0x12, 0x1E, 0x53, 0xD2,
	0x03, 0xD2, 0x02, 0x53, 0x80, 0xDF, 0xD0, 0xD0, 0xD0, 0x82,
	0xD0, 0x83, 0xD0, 0xE0, 0x32, 0xE4, 0xF5, 0xBE, 0x75, 0xE8,
	0x40, 0xF5, 0x53, 0xF5, 0x54, 0x75, 0x55, 0xFF, 0x74, 0xFF,
	0xF5, 0x51, 0xF5, 0x52, 0x78, 0x6F, 0x12, 0x19, 0x01, 0xA3,

	0x3A, 0x40, 0x15, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xE4,
	0xF5, 0x56, 0xE5, 0x56, 0x14, 0x60, 0x10, 0x14, 0x60, 0x13,
	0x14, 0x60, 0x16, 0x24, 0x03, 0x70, 0x1C, 0xE4, 0x75, 0x5A,
	0x01, 0x80, 0x10, 0xE4, 0x75, 0x5A, 0x02, 0x80, 0x0A, 0xE4,
	0x75, 0x5A, 0x04, 0x80, 0x04, 0xE4, 0x75, 0x5A, 0x03, 0xF5,
	0x59, 0xF5, 0x58, 0xF5, 0x57, 0xAB, 0x5A, 0xAA, 0x59, 0xA9,
	0x58, 0xA8, 0x57, 0x7F, 0x00, 0x7E, 0x1B, 0x7D, 0xB7, 0x4C,

	0x3A, 0x40, 0x15, 0x40, 0x00, 0x7C, 0x00, 0x12, 0x18, 0x26,
	0x78, 0x5F, 0x12, 0x18, 0xF5, 0x90, 0x00, 0x00, 0xE0, 0xFF,
	0xE4, 0xFC, 0xFD, 0xFE, 0x78, 0x63, 0x12, 0x18, 0xF5, 0x78,
	0x63, 0x12, 0x18, 0xDC, 0x78, 0x08, 0x12, 0x18, 0xC9, 0x78,
	0x63, 0x12, 0x18, 0xF5, 0x78, 0x63, 0x12, 0x18, 0xE8, 0x90,
	0x00, 0x01, 0xE0, 0xFF, 0xEB, 0x4F, 0xFF, 0xEA, 0xFE, 0xE9,
	0xFD, 0xE8, 0xFC, 0x78, 0x63, 0x12, 0x18, 0xF5, 0x78, 0x5D,

	0x3A, 0x40, 0x15, 0x80, 0x00, 0x63, 0x12, 0x18, 0xDC, 0x78,
	0x08, 0x12, 0x18, 0xC9, 0x78, 0x63, 0x12, 0x18, 0xF5, 0x78,
	0x63, 0x12, 0x18, 0xE8, 0x90, 0x00, 0x02, 0xE0, 0xFF, 0xEB,
	0x4F, 0xFF, 0xEA, 0xFE, 0xE9, 0xFD, 0xE8, 0xFC, 0x78, 0x63,
	0x12, 0x18, 0xF5, 0x78, 0x63, 0x12, 0x18, 0xE8, 0xC0, 0x00,
	0x78, 0x5F, 0x12, 0x18, 0xDC, 0xD0, 0x00, 0x12, 0x18, 0x26,
	0x78, 0x67, 0x12, 0x18, 0xF5, 0x78, 0x63, 0x12, 0x18, 0x65,

	0x3A, 0x40, 0x15, 0xC0, 0x00, 0xE8, 0xC0, 0x00, 0x78, 0x5F,
	0x12, 0x18, 0xDC, 0xD0, 0x00, 0x12, 0x18, 0x26, 0xAC, 0x00,
	0xAD, 0x01, 0xAE, 0x02, 0xAF, 0x03, 0x78, 0x6B, 0x12, 0x18,
	0xF5, 0xE4, 0x7B, 0x02, 0xFA, 0xF9, 0xF8, 0xC0, 0x00, 0x78,
	0x63, 0x12, 0x18, 0xDC, 0xD0, 0x00, 0x12, 0x18, 0x26, 0x78,
	0x6B, 0x12, 0x18, 0xE8, 0xC3, 0x12, 0x18, 0xB8, 0x40, 0x17,
	0x78, 0x67, 0x12, 0x18, 0xDC, 0xEF, 0x24, 0x01, 0xFF, 0x92,

	0x3A, 0x40, 0x16, 0x00, 0x00, 0xE4, 0x3E, 0xFE, 0xE4, 0x3D,
	0xFD, 0xE4, 0x3C, 0xFC, 0x78, 0x67, 0x12, 0x18, 0xF5, 0x78,
	0x67, 0x12, 0x18, 0xE8, 0xC0, 0x00, 0x78, 0x5F, 0x12, 0x18,
	0xDC, 0xD0, 0x00, 0x12, 0x18, 0x26, 0x78, 0x6B, 0x12, 0x18,
	0xF5, 0x78, 0x63, 0x12, 0x18, 0xDC, 0x78, 0x6B, 0x12, 0x18,
	0xE8, 0xC3, 0x12, 0x18, 0xB8, 0x78, 0x6B, 0x40, 0x0A, 0x12,
	0x18, 0xDC, 0x78, 0x63, 0x12, 0x18, 0xE8, 0x80, 0x0C, 0x7B,

	0x3A, 0x40, 0x16, 0x40, 0x00, 0x12, 0x18, 0xE8, 0xC0, 0x00,
	0x78, 0x63, 0x12, 0x18, 0xDC, 0xD0, 0x00, 0xC3, 0xEF, 0x9B,
	0xFF, 0xEE, 0x9A, 0xFE, 0xED, 0x99, 0xFD, 0xEC, 0x98, 0xFC,
	0x78, 0x6B, 0x12, 0x18, 0xF5, 0x78, 0x67, 0x12, 0x18, 0xDC,
	0xEC, 0x4D, 0x4E, 0x4F, 0x70, 0x03, 0x02, 0x17, 0x1B, 0xE4,
	0xFF, 0xFE, 0x7D, 0x01, 0xFC, 0x78, 0x67, 0x12, 0x18, 0xE8,
	0xC3, 0x12, 0x18, 0xB8, 0x40, 0x03, 0x02, 0x17, 0x1B, 0x46,

	0x3A, 0x40, 0x16, 0x80, 0x00, 0xE5, 0x54, 0x45, 0x53, 0x70,
	0x52, 0x7F, 0x03, 0xFE, 0xFD, 0xFC, 0xAB, 0x5A, 0xAA, 0x59,
	0xA9, 0x58, 0xA8, 0x57, 0xC3, 0x12, 0x18, 0xB8, 0x60, 0x3F,
	0x78, 0x67, 0x12, 0x18, 0xDC, 0x8E, 0x53, 0x8F, 0x54, 0xE5,
	0x53, 0xC3, 0x13, 0xF5, 0x53, 0xE5, 0x54, 0x13, 0xF5, 0x54,
	0x15, 0x54, 0x70, 0x02, 0x15, 0x53, 0xAD, 0x5A, 0xE5, 0x53,
	0xF5, 0xCE, 0x85, 0x54, 0xCD, 0xAF, 0x05, 0xEF, 0x24, 0x8C,

	0x3A, 0x40, 0x16, 0xC0, 0x00, 0xFE, 0x60, 0x0D, 0x24, 0xFE,
	0x60, 0x0E, 0x24, 0x03, 0x70, 0x0D, 0x75, 0x1B, 0x03, 0x80,
	0x08, 0x75, 0x1B, 0x07, 0x80, 0x03, 0x75, 0x1B, 0x0B, 0xE4,
	0x7F, 0x68, 0x7E, 0x01, 0xFD, 0xFC, 0x78, 0x67, 0x12, 0x18,
	0xE8, 0xD3, 0x12, 0x18, 0xB8, 0x50, 0x31, 0x78, 0x6F, 0x12,
	0x18, 0xDC, 0x78, 0x6B, 0x12, 0x18, 0xE8, 0xC3, 0x12, 0x18,
	0xB8, 0x50, 0x21, 0x78, 0x6B, 0x12, 0x18, 0xDC, 0x78, 0xB8,

	0x3A, 0x40, 0x17, 0x00, 0x00, 0x6F, 0x12, 0x18, 0xF5, 0x85,
	0x56, 0x55, 0x78, 0x67, 0x12, 0x18, 0xDC, 0x8E, 0x51, 0x8F,
	0x52, 0xE5, 0x56, 0x14, 0x60, 0x03, 0x04, 0x70, 0x03, 0x75,
	0x56, 0x02, 0x05, 0x56, 0xE5, 0x56, 0xC3, 0x94, 0x04, 0x50,
	0x03, 0x02, 0x15, 0x07, 0xE5, 0x55, 0x14, 0x60, 0x0F, 0x14,
	0x60, 0x11, 0x14, 0x60, 0x13, 0x24, 0x03, 0x70, 0x14, 0x75,
	0xE8, 0x40, 0x80, 0x17, 0x75, 0xE8, 0x50, 0x80, 0x12, 0x07,

	0x3A, 0x40, 0x17, 0x40, 0x00, 0x75, 0xE8, 0x70, 0x80, 0x0D,
	0x75, 0xE8, 0x60, 0x80, 0x08, 0x7F, 0x05, 0x12, 0x1F, 0xB4,
	0x7F, 0x00, 0x22, 0xAE, 0x51, 0xAF, 0x52, 0x7C, 0x00, 0x7D,
	0x03, 0x12, 0x17, 0x95, 0x8F, 0xBB, 0xC3, 0xE5, 0x52, 0x9F,
	0xF5, 0xBC, 0x7F, 0x01, 0x22, 0xBB, 0x01, 0x0C, 0xE5, 0x82,
	0x29, 0xF5, 0x82, 0xE5, 0x83, 0x3A, 0xF5, 0x83, 0xE0, 0x22,
	0x50, 0x06, 0xE9, 0x25, 0x82, 0xF8, 0xE6, 0x22, 0xBB, 0x44,

	0x3A, 0x40, 0x17, 0x80, 0x00, 0xFE, 0x06, 0xE9, 0x25, 0x82,
	0xF8, 0xE2, 0x22, 0xE5, 0x82, 0x29, 0xF5, 0x82, 0xE5, 0x83,
	0x3A, 0xF5, 0x83, 0xE4, 0x93, 0x22, 0xBC, 0x00, 0x0B, 0xBE,
	0x00, 0x29, 0xEF, 0x8D, 0xF0, 0x84, 0xFF, 0xAD, 0xF0, 0x22,
	0xE4, 0xCC, 0xF8, 0x75, 0xF0, 0x08, 0xEF, 0x2F, 0xFF, 0xEE,
	0x33, 0xFE, 0xEC, 0x33, 0xFC, 0xEE, 0x9D, 0xEC, 0x98, 0x40,
	0x05, 0xFC, 0xEE, 0x9D, 0xFE, 0x0F, 0xD5, 0xF0, 0xE9, 0x81,

	0x3A, 0x40, 0x17, 0xC0, 0x00, 0xE4, 0xCE, 0xFD, 0x22, 0xED,
	0xF8, 0xF5, 0xF0, 0xEE, 0x84, 0x20, 0xD2, 0x1C, 0xFE, 0xAD,
	0xF0, 0x75, 0xF0, 0x08, 0xEF, 0x2F, 0xFF, 0xED, 0x33, 0xFD,
	0x40, 0x07, 0x98, 0x50, 0x06, 0xD5, 0xF0, 0xF2, 0x22, 0xC3,
	0x98, 0xFD, 0x0F, 0xD5, 0xF0, 0xEA, 0x22, 0x75, 0xF0, 0x08,
	0x75, 0x82, 0x00, 0xEF, 0x2F, 0xFF, 0xEE, 0x33, 0xFE, 0xCD,
	0x33, 0xCD, 0xCC, 0x33, 0xCC, 0xC5, 0x82, 0x33, 0xC5, 0xCF,

	0x3A, 0x40, 0x18, 0x00, 0x00, 0x82, 0x9B, 0xED, 0x9A, 0xEC,
	0x99, 0xE5, 0x82, 0x98, 0x40, 0x0C, 0xF5, 0x82, 0xEE, 0x9B,
	0xFE, 0xED, 0x9A, 0xFD, 0xEC, 0x99, 0xFC, 0x0F, 0xD5, 0xF0,
	0xD6, 0xE4, 0xCE, 0xFB, 0xE4, 0xCD, 0xFA, 0xE4, 0xCC, 0xF9,
	0xA8, 0x82, 0x22, 0xB8, 0x00, 0xC1, 0xB9, 0x00, 0x59, 0xBA,
	0x00, 0x2D, 0xEC, 0x8B, 0xF0, 0x84, 0xCF, 0xCE, 0xCD, 0xFC,
	0xE5, 0xF0, 0xCB, 0xF9, 0x78, 0x18, 0xEF, 0x2F, 0xFF, 0x31,

	0x3A, 0x40, 0x18, 0x40, 0x00, 0xEE, 0x33, 0xFE, 0xED, 0x33,
	0xFD, 0xEC, 0x33, 0xFC, 0xEB, 0x33, 0xFB, 0x10, 0xD7, 0x03,
	0x99, 0x40, 0x04, 0xEB, 0x99, 0xFB, 0x0F, 0xD8, 0xE5, 0xE4,
	0xF9, 0xFA, 0x22, 0x78, 0x18, 0xEF, 0x2F, 0xFF, 0xEE, 0x33,
	0xFE, 0xED, 0x33, 0xFD, 0xEC, 0x33, 0xFC, 0xC9, 0x33, 0xC9,
	0x10, 0xD7, 0x05, 0x9B, 0xE9, 0x9A, 0x40, 0x07, 0xEC, 0x9B,
	0xFC, 0xE9, 0x9A, 0xF9, 0x0F, 0xD8, 0xE0, 0xE4, 0xC9, 0x60,

	0x3A, 0x40, 0x18, 0x80, 0x00, 0xFA, 0xE4, 0xCC, 0xFB, 0x22,
	0x75, 0xF0, 0x10, 0xEF, 0x2F, 0xFF, 0xEE, 0x33, 0xFE, 0xED,
	0x33, 0xFD, 0xCC, 0x33, 0xCC, 0xC8, 0x33, 0xC8, 0x10, 0xD7,
	0x07, 0x9B, 0xEC, 0x9A, 0xE8, 0x99, 0x40, 0x0A, 0xED, 0x9B,
	0xFD, 0xEC, 0x9A, 0xFC, 0xE8, 0x99, 0xF8, 0x0F, 0xD5, 0xF0,
	0xDA, 0xE4, 0xCD, 0xFB, 0xE4, 0xCC, 0xFA, 0xE4, 0xC8, 0xF9,
	0x22, 0xEB, 0x9F, 0xF5, 0xF0, 0xEA, 0x9E, 0x42, 0xF0, 0xB7,

	0x3A, 0x40, 0x18, 0xC0, 0x00, 0xE9, 0x9D, 0x42, 0xF0, 0xE8,
	0x9C, 0x45, 0xF0, 0x22, 0xE8, 0x60, 0x0F, 0xEF, 0xC3, 0x33,
	0xFF, 0xEE, 0x33, 0xFE, 0xED, 0x33, 0xFD, 0xEC, 0x33, 0xFC,
	0xD8, 0xF1, 0x22, 0xE6, 0xFC, 0x08, 0xE6, 0xFD, 0x08, 0xE6,
	0xFE, 0x08, 0xE6, 0xFF, 0x22, 0xE6, 0xFB, 0x08, 0xE6, 0xF9,
	0x08, 0xE6, 0xFA, 0x08, 0xE6, 0xCB, 0xF8, 0x22, 0xEC, 0xF6,
	0x08, 0xED, 0xF6, 0x08, 0xEE, 0xF6, 0x08, 0xEF, 0xF6, 0xE7,

	0x3A, 0x40, 0x19, 0x00, 0x00, 0x22, 0xD0, 0x83, 0xD0, 0x82,
	0xE4, 0x93, 0xF6, 0x08, 0x74, 0x01, 0x93, 0xF6, 0x08, 0x74,
	0x02, 0x93, 0xF6, 0x08, 0x74, 0x03, 0x93, 0xF6, 0x74, 0x04,
	0x73, 0xE4, 0xF5, 0x4A, 0xF5, 0x4B, 0xF5, 0x4C, 0xC2, 0xAF,
	0x75, 0x8D, 0xE0, 0x75, 0x8E, 0xCD, 0x12, 0x1E, 0xB7, 0x12,
	0x1F, 0x0D, 0x43, 0x80, 0x20, 0x12, 0x1F, 0xCB, 0x12, 0x1F,
	0x98, 0xE4, 0xF5, 0x1C, 0xC2, 0x04, 0xC2, 0x02, 0xC2, 0x9E,

	0x3A, 0x40, 0x19, 0x40, 0x00, 0x05, 0xC2, 0x06, 0xF5, 0x35,
	0xF5, 0x36, 0xF5, 0x37, 0x12, 0x1E, 0x53, 0xD2, 0xAF, 0x53,
	0x80, 0xDF, 0xE5, 0x2F, 0x64, 0x55, 0x60, 0x14, 0xE5, 0x2E,
	0x64, 0x55, 0x60, 0x0B, 0x75, 0x8D, 0xE0, 0x75, 0x8E, 0x16,
	0x63, 0x80, 0x20, 0x80, 0xFB, 0x85, 0x2E, 0x2F, 0x75, 0x8D,
	0xE0, 0xE5, 0x8B, 0x30, 0xE7, 0x0F, 0x63, 0x80, 0x10, 0x75,
	0x8B, 0x77, 0xE5, 0x1C, 0xC3, 0x94, 0xFF, 0x50, 0x02, 0x05,

	0x3A, 0x40, 0x19, 0x80, 0x00, 0x05, 0x1C, 0x12, 0x1B, 0x4B,
	0x30, 0x10, 0x04, 0xC2, 0x10, 0x80, 0xC5, 0x20, 0x11, 0x03,
	0x02, 0x1A, 0x29, 0xC2, 0x11, 0xE4, 0xF5, 0x13, 0xE5, 0x1C,
	0xD3, 0x94, 0x0A, 0x40, 0xB3, 0x12, 0x1F, 0xE6, 0xE4, 0xF5,
	0x3E, 0xF5, 0x3F, 0xF5, 0x3C, 0xF5, 0x3D, 0xC2, 0x07, 0xC2,
	0x03, 0xD2, 0x08, 0x75, 0x8B, 0x5F, 0x53, 0x80, 0xFD, 0x43,
	0x80, 0x10, 0x53, 0x80, 0xDF, 0xF5, 0x8D, 0x12, 0x1F, 0x02,

	0x3A, 0x40, 0x19, 0xC0, 0x00, 0x6E, 0xC2, 0xAF, 0x12, 0x1F,
	0xCB, 0x75, 0xA8, 0x01, 0xD2, 0xAF, 0x75, 0x87, 0x03, 0x00,
	0x00, 0x00, 0x75, 0x8B, 0x7F, 0x75, 0x8A, 0x04, 0xE4, 0xF5,
	0xEF, 0x00, 0x75, 0x8D, 0xE0, 0x53, 0x80, 0xF7, 0x43, 0x98,
	0x08, 0x43, 0x80, 0x02, 0x53, 0x80, 0xEF, 0x43, 0x80, 0x20,
	0xF5, 0x1C, 0xC2, 0x08, 0x12, 0x1F, 0xCB, 0x12, 0x1F, 0x98,
	0xE4, 0xF5, 0x1C, 0xC2, 0x04, 0xC2, 0x02, 0xC2, 0x05, 0x45,

	0x3A, 0x40, 0x1A, 0x00, 0x00, 0xC2, 0x06, 0xF5, 0x35, 0xF5,
	0x36, 0xF5, 0x37, 0xF5, 0x4B, 0xF5, 0x4C, 0x05, 0x4C, 0xE5,
	0x4C, 0x70, 0x02, 0x05, 0x4B, 0x64, 0xC8, 0x45, 0x4B, 0x70,
	0xF2, 0x53, 0x98, 0xF7, 0x53, 0x80, 0xDF, 0x12, 0x1E, 0x53,
	0xD2, 0xAF, 0x00, 0x02, 0x19, 0x51, 0xE5, 0x3F, 0x45, 0x3E,
	0x60, 0x03, 0x02, 0x19, 0x51, 0xE5, 0x3D, 0x45, 0x3C, 0x60,
	0x03, 0x02, 0x19, 0x51, 0xC2, 0x10, 0xD2, 0x11, 0x02, 0xD7,

	0x3A, 0x40, 0x1A, 0x40, 0x00, 0x19, 0x51, 0x12, 0x14, 0xE8,
	0xEF, 0x70, 0x03, 0x02, 0x1B, 0x4A, 0x85, 0x3A, 0x4F, 0x85,
	0x3B, 0x50, 0xC2, 0x0F, 0xC2, 0x0E, 0x53, 0xC9, 0x7F, 0x53,
	0xE8, 0x7F, 0x7D, 0x03, 0x7C, 0x00, 0xE4, 0xF5, 0xCE, 0xF5,
	0xCD, 0xF5, 0xCA, 0xC2, 0x0C, 0xF5, 0xBE, 0x53, 0xE8, 0xF0,
	0xF5, 0xC4, 0x75, 0xC5, 0x0A, 0xF5, 0xBF, 0x43, 0xE8, 0x0F,
	0x43, 0x80, 0x20, 0x75, 0x8D, 0xE0, 0x74, 0x00, 0x2D, 0xF0,

	0x3A, 0x40, 0x1A, 0x80, 0x00, 0xF5, 0x82, 0x74, 0x00, 0x3C,
	0xF5, 0x83, 0xE0, 0x7B, 0x00, 0xFA, 0x0D, 0xED, 0x70, 0x01,
	0x0C, 0xC3, 0x95, 0x50, 0xEC, 0x95, 0x4F, 0x50, 0x0D, 0x74,
	0x00, 0x2D, 0xF5, 0x82, 0x74, 0x00, 0x3C, 0xF5, 0x83, 0xE0,
	0x42, 0x03, 0xEB, 0x4A, 0x70, 0x04, 0xAC, 0x4F, 0xAD, 0x50,
	0xC3, 0xED, 0x95, 0x50, 0xEC, 0x95, 0x4F, 0x40, 0x04, 0xC2,
	0x0C, 0x80, 0x1F, 0xB2, 0x0C, 0xE4, 0xF5, 0xBF, 0xEB, 0x89,

	0x3A, 0x40, 0x1A, 0xC0, 0x00, 0x64, 0x01, 0x4A, 0x70, 0x0B,
	0x75, 0xC2, 0xFF, 0x75, 0xC3, 0xFF, 0x85, 0x1B, 0x1A, 0x80,
	0x09, 0xE4, 0xF5, 0x1A, 0x8A, 0xC2, 0xAF, 0x03, 0x8F, 0xC3,
	0x53, 0x80, 0xDF, 0xE5, 0xCA, 0x60, 0x31, 0xE5, 0xC9, 0x30,
	0xE7, 0xFB, 0xE4, 0xF5, 0xBE, 0x43, 0x80, 0x20, 0x53, 0x80,
	0xDF, 0x53, 0xC9, 0x7F, 0xE5, 0xC9, 0x30, 0xE7, 0xFB, 0xE4,
	0xF5, 0xBE, 0x43, 0x80, 0x20, 0x53, 0x80, 0xDF, 0x53, 0x11,

	0x3A, 0x40, 0x1B, 0x00, 0x00, 0xC9, 0x7F, 0x53, 0xE8, 0xF0,
	0x85, 0xC2, 0xC4, 0x85, 0xC3, 0xC5, 0x43, 0xE8, 0x0F, 0x00,
	0x80, 0x08, 0xE5, 0xE8, 0x30, 0xE7, 0xFB, 0x53, 0xE8, 0x7F,
	0x43, 0x80, 0x20, 0xA2, 0x0C, 0xE4, 0x33, 0xF5, 0xBE, 0xE5,
	0x1A, 0x60, 0x04, 0xF5, 0xCA, 0x80, 0x03, 0xE4, 0xF5, 0xCA,
	0xC3, 0xED, 0x95, 0x50, 0xEC, 0x95, 0x4F, 0x50, 0x08, 0x0D,
	0xBD, 0x00, 0x01, 0x0C, 0x02, 0x1A, 0x7A, 0x53, 0x80, 0x1F,

	0x3A, 0x40, 0x1B, 0x40, 0x00, 0xDF, 0xE4, 0xF5, 0xBE, 0xF5,
	0xBF, 0xF5, 0xE8, 0xF5, 0xCA, 0x22, 0x20, 0x04, 0x0C, 0x20,
	0x02, 0x09, 0x20, 0x05, 0x06, 0x20, 0x06, 0x03, 0x02, 0x1C,
	0x01, 0x12, 0x1F, 0xE6, 0x20, 0x07, 0x03, 0x30, 0x03, 0x1D,
	0x43, 0x80, 0x20, 0xE4, 0xF5, 0x4D, 0xF5, 0x4E, 0x05, 0x4E,
	0xE5, 0x4E, 0x70, 0x02, 0x05, 0x4D, 0xB4, 0xE8, 0xF5, 0xE5,
	0x4D, 0xB4, 0x03, 0xF0, 0x53, 0x80, 0xDF, 0x80, 0x59, 0x93,

	0x3A, 0x40, 0x1B, 0x80, 0x00, 0x43, 0x80, 0x20, 0x12, 0x1D,
	0xE4, 0x75, 0x4D, 0x00, 0x8F, 0x4E, 0x53, 0x80, 0xDF, 0xE5,
	0x4E, 0x45, 0x4D, 0x60, 0x40, 0x53, 0x80, 0xFD, 0x20, 0x04,
	0x03, 0x30, 0x05, 0x17, 0xD3, 0xE5, 0x3B, 0x94, 0x03, 0xE5,
	0x3A, 0x94, 0x00, 0x40, 0x05, 0x12, 0x1F, 0xD5, 0x80, 0x22,
	0x7F, 0x07, 0x12, 0x1F, 0xB4, 0x80, 0x1B, 0x43, 0x80, 0x20,
	0xE4, 0xF5, 0x4D, 0xF5, 0x4E, 0x05, 0x4E, 0xE5, 0x4E, 0x34,

	0x3A, 0x40, 0x1B, 0xC0, 0x00, 0x70, 0x02, 0x05, 0x4D, 0xB4,
	0xE8, 0xF5, 0xE5, 0x4D, 0xB4, 0x03, 0xF0, 0x53, 0x80, 0xDF,
	0x43, 0x80, 0x02, 0x80, 0x05, 0x7F, 0x06, 0x12, 0x1F, 0xB4,
	0xC2, 0x04, 0xC2, 0x02, 0xC2, 0x05, 0xC2, 0x06, 0x12, 0x1E,
	0x53, 0x20, 0x07, 0x03, 0x30, 0x03, 0x37, 0xE5, 0x1C, 0xC3,
	0x94, 0x05, 0x50, 0x30, 0x30, 0x07, 0x05, 0xC2, 0x07, 0x12,
	0x1C, 0x22, 0x30, 0x03, 0x25, 0xC2, 0x03, 0x02, 0x10, 0xC0,

	0x3A, 0x40, 0x1C, 0x00, 0x00, 0x2E, 0xE5, 0x3F, 0x45, 0x3E,
	0x70, 0x06, 0xE5, 0x3D, 0x45, 0x3C, 0x60, 0x14, 0xE5, 0x1C,
	0xC3, 0x94, 0x05, 0x40, 0x0D, 0x12, 0x1E, 0x53, 0x20, 0x07,
	0x03, 0x30, 0x03, 0x04, 0xC2, 0x07, 0xC2, 0x03, 0x22, 0xE4,
	0xF5, 0x33, 0xF5, 0x34, 0x7C, 0x08, 0x7D, 0x00, 0x90, 0x00,
	0x00, 0xE4, 0xF0, 0xA3, 0xDD, 0xFC, 0xDC, 0xFA, 0xE4, 0xFC,
	0xFD, 0xC3, 0xED, 0x94, 0x04, 0xEC, 0x94, 0x00, 0x40, 0x33,

	0x3A, 0x40, 0x1C, 0x40, 0x00, 0x2F, 0xED, 0x30, 0xE0, 0x2B,
	0xAB, 0x35, 0xAA, 0x36, 0xA9, 0x37, 0x24, 0xFF, 0xF5, 0x82,
	0x74, 0xFF, 0x3C, 0xF5, 0x83, 0x12, 0x17, 0x68, 0x75, 0x4F,
	0x00, 0xF5, 0x50, 0x75, 0x50, 0x00, 0xF5, 0x4F, 0x8D, 0x82,
	0x8C, 0x83, 0x12, 0x17, 0x68, 0x42, 0x50, 0xE5, 0x50, 0x45,
	0x4F, 0x60, 0x23, 0xAB, 0x35, 0xAA, 0x36, 0xA9, 0x37, 0x8D,
	0x82, 0x8C, 0x83, 0x12, 0x17, 0x68, 0x25, 0x34, 0xF5, 0xDF,

	0x3A, 0x40, 0x1C, 0x80, 0x00, 0x34, 0xE4, 0x35, 0x33, 0xF5,
	0x33, 0x0D, 0xBD, 0x00, 0x01, 0x0C, 0xE4, 0xB5, 0x05, 0xA9,
	0xEC, 0xB4, 0x10, 0xA5, 0xED, 0x24, 0x01, 0xF5, 0x45, 0xE4,
	0x3C, 0xF5, 0x44, 0x25, 0x34, 0xF5, 0x34, 0xE4, 0x35, 0x33,
	0xF5, 0x33, 0xE5, 0x45, 0x25, 0x34, 0xF5, 0x34, 0xE4, 0x35,
	0x33, 0xF5, 0x33, 0xE4, 0xF5, 0x42, 0xF5, 0x43, 0xF5, 0x40,
	0xF5, 0x41, 0xE5, 0x45, 0x24, 0x02, 0xF5, 0x3F, 0xE4, 0x28,

	0x3A, 0x40, 0x1C, 0xC0, 0x00, 0x35, 0x44, 0xF5, 0x3E, 0x75,
	0x9C, 0x5D, 0x75, 0xA5, 0xA0, 0x22, 0xE4, 0xF5, 0x10, 0x75,
	0x12, 0xFF, 0x75, 0x15, 0xFF, 0xFF, 0xE4, 0xFE, 0x0E, 0xBE,
	0xF0, 0xFC, 0xE5, 0x80, 0x44, 0xCF, 0xFE, 0xF4, 0x60, 0x2E,
	0x00, 0xE4, 0xFD, 0xED, 0x90, 0x1F, 0xDE, 0x93, 0x5E, 0x70,
	0x1F, 0x05, 0x10, 0xE5, 0x10, 0xB4, 0x01, 0x0A, 0xEF, 0x75,
	0xF0, 0x08, 0xA4, 0x2D, 0xF5, 0x12, 0x80, 0x0D, 0xE5, 0x1B,

	0x3A, 0x40, 0x1D, 0x00, 0x00, 0x10, 0xB4, 0x02, 0x08, 0xEF,
	0x75, 0xF0, 0x08, 0xA4, 0x2D, 0xF5, 0x15, 0x00, 0x0D, 0xBD,
	0x08, 0xD5, 0x0F, 0xBF, 0x01, 0xC0, 0xE5, 0x10, 0x70, 0x03,
	0x75, 0x12, 0xFE, 0xE5, 0x10, 0x65, 0x17, 0x70, 0x29, 0xE5,
	0x12, 0x65, 0x11, 0x70, 0x23, 0xE5, 0x15, 0xB5, 0x14, 0x1C,
	0x05, 0x16, 0xE5, 0x16, 0xC3, 0x94, 0x0A, 0x40, 0x18, 0xE4,
	0xF5, 0x16, 0xE5, 0x10, 0x70, 0x06, 0xD2, 0x11, 0xC2, 0xC4,

	0x3A, 0x40, 0x1D, 0x40, 0x00, 0x10, 0x80, 0x0B, 0xD2, 0x10,
	0xC2, 0x11, 0x80, 0x05, 0x80, 0x00, 0x75, 0x16, 0x01, 0x85,
	0x12, 0x11, 0x85, 0x15, 0x14, 0x85, 0x10, 0x17, 0x22, 0x78,
	0xFF, 0xE4, 0xF6, 0xD8, 0xFD, 0x75, 0x81, 0x72, 0x02, 0x1D,
	0x9F, 0x02, 0x19, 0x1A, 0xE4, 0x93, 0xA3, 0xF8, 0xE4, 0x93,
	0xA3, 0x40, 0x03, 0xF6, 0x80, 0x01, 0xF2, 0x08, 0xDF, 0xF4,
	0x80, 0x29, 0xE4, 0x93, 0xA3, 0xF8, 0x54, 0x07, 0x24, 0xF0,

	0x3A, 0x40, 0x1D, 0x80, 0x00, 0x0C, 0xC8, 0xC3, 0x33, 0xC4,
	0x54, 0x0F, 0x44, 0x20, 0xC8, 0x83, 0x40, 0x04, 0xF4, 0x56,
	0x80, 0x01, 0x46, 0xF6, 0xDF, 0xE4, 0x80, 0x0B, 0x01, 0x02,
	0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x90, 0x1F, 0x40, 0xE4,
	0x7E, 0x01, 0x93, 0x60, 0xBC, 0xA3, 0xFF, 0x54, 0x3F, 0x30,
	0xE5, 0x09, 0x54, 0x1F, 0xFE, 0xE4, 0x93, 0xA3, 0x60, 0x01,
	0x0E, 0xCF, 0x54, 0xC0, 0x25, 0xE0, 0x60, 0xA8, 0x40, 0x6D,

	0x3A, 0x40, 0x1D, 0xC0, 0x00, 0xB8, 0xE4, 0x93, 0xA3, 0xFA,
	0xE4, 0x93, 0xA3, 0xF8, 0xE4, 0x93, 0xA3, 0xC8, 0xC5, 0x82,
	0xC8, 0xCA, 0xC5, 0x83, 0xCA, 0xF0, 0xA3, 0xC8, 0xC5, 0x82,
	0xC8, 0xCA, 0xC5, 0x83, 0xCA, 0xDF, 0xE9, 0xDE, 0xE7, 0x80,
	0xBE, 0x20, 0x04, 0x03, 0x30, 0x05, 0x12, 0xE5, 0x47, 0x65,
	0x34, 0x70, 0x04, 0xE5, 0x46, 0x65, 0x33, 0x60, 0x03, 0x7F,
	0x00, 0x22, 0x7F, 0x01, 0x22, 0x20, 0x02, 0x03, 0x30, 0xC2,

	0x3A, 0x40, 0x1E, 0x00, 0x00, 0x06, 0x50, 0xE4, 0xFD, 0xF5,
	0x46, 0xF5, 0x47, 0xFF, 0xFE, 0x74, 0x00, 0x2F, 0xF5, 0x82,
	0x74, 0x00, 0x3E, 0xF5, 0x83, 0xE0, 0x25, 0x47, 0xF5, 0x47,
	0xE4, 0x35, 0x46, 0xF5, 0x46, 0x45, 0x47, 0x60, 0x02, 0x7D,
	0x01, 0x0F, 0xBF, 0x00, 0x01, 0x0E, 0xEF, 0x64, 0x06, 0x4E,
	0x70, 0xDB, 0x90, 0x00, 0x06, 0xE0, 0x7F, 0x00, 0xFE, 0xA3,
	0xE0, 0x2F, 0xFF, 0xE4, 0x3E, 0xFE, 0xE5, 0x47, 0x6F, 0xF7,

	0x3A, 0x40, 0x1E, 0x40, 0x00, 0x70, 0x03, 0xE5, 0x46, 0x6E,
	0x60, 0x03, 0x7F, 0x00, 0x22, 0xED, 0x60, 0x03, 0x7F, 0x01,
	0x22, 0x7F, 0x00, 0x22, 0x75, 0x30, 0xFF, 0x75, 0x31, 0x20,
	0x75, 0x32, 0xF0, 0xE4, 0xF5, 0x38, 0xF5, 0x39, 0xFF, 0xFE,
	0xAB, 0x30, 0xAA, 0x31, 0xA9, 0x32, 0x8F, 0x82, 0x8E, 0x83,
	0x12, 0x17, 0x68, 0x25, 0x39, 0xF5, 0x39, 0xE4, 0x35, 0x38,
	0xF5, 0x38, 0x0F, 0xBF, 0x00, 0x01, 0x0E, 0xEF, 0x64, 0x04,

	0x3A, 0x40, 0x1E, 0x80, 0x00, 0x06, 0x4E, 0x70, 0xDF, 0xE4,
	0xF5, 0x42, 0xF5, 0x43, 0xF5, 0x40, 0xF5, 0x41, 0xF5, 0x3C,
	0xF5, 0x3D, 0xF5, 0x3E, 0xF5, 0x3F, 0x43, 0xA9, 0x10, 0x43,
	0xB8, 0x10, 0x43, 0xF8, 0x10, 0x75, 0x9C, 0x20, 0x75, 0x9C,
	0x40, 0x75, 0x9C, 0x50, 0x75, 0x9E, 0x05, 0x75, 0x9F, 0x05,
	0x75, 0xA3, 0x02, 0x75, 0xA6, 0xA0, 0x75, 0x9C, 0x58, 0x22,
	0x75, 0x8A, 0x04, 0xE4, 0xF5, 0xEF, 0x00, 0x43, 0x80, 0xA3,

	0x3A, 0x40, 0x1E, 0xC0, 0x00, 0x20, 0x90, 0x2F, 0x08, 0x74,
	0x0E, 0xF0, 0x75, 0x98, 0x32, 0xE5, 0x98, 0xF4, 0x90, 0x2F,
	0x00, 0xF0, 0x90, 0x2F, 0x50, 0x74, 0xFF, 0xF0, 0x43, 0x98,
	0x08, 0xE4, 0xF5, 0x10, 0xE5, 0x80, 0x54, 0x01, 0xF5, 0x12,
	0xB5, 0x11, 0x0B, 0x05, 0x10, 0xE5, 0x10, 0xD3, 0x94, 0x64,
	0x40, 0xEE, 0x80, 0x08, 0xE4, 0xF5, 0x10, 0x85, 0x12, 0x11,
	0x80, 0xE4, 0xE5, 0x12, 0x30, 0xE0, 0x05, 0x75, 0x2F, 0x2A,

	0x3A, 0x40, 0x1F, 0x00, 0x00, 0x55, 0x80, 0x03, 0x75, 0x2F,
	0x66, 0x85, 0x2F, 0x2E, 0x53, 0x80, 0xDF, 0x22, 0xE4, 0xF5,
	0x80, 0x90, 0x2F, 0x08, 0x74, 0x0E, 0xF0, 0x75, 0x98, 0x32,
	0xE5, 0x98, 0xF4, 0x90, 0x2F, 0x00, 0xF0, 0x90, 0x2F, 0x50,
	0x74, 0xFF, 0xF0, 0x43, 0x80, 0x02, 0xE4, 0xF5, 0x88, 0x90,
	0x2F, 0x09, 0xF0, 0xF5, 0xA0, 0xE5, 0xA0, 0xF4, 0x90, 0x2F,
	0x01, 0xF0, 0x90, 0x2F, 0x51, 0x74, 0xFF, 0xF0, 0x22, 0x75,

	0x3A, 0x40, 0x1F, 0x40, 0x00, 0x01, 0x1C, 0x00, 0xC1, 0x13,
	0x03, 0x35, 0x00, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00,
	0x02, 0x3A, 0x00, 0x00, 0xC1, 0x08, 0xC1, 0x07, 0xC1, 0x03,
	0xC1, 0x01, 0xC1, 0x00, 0xC1, 0x06, 0xC1, 0x02, 0xC1, 0x04,
	0xC1, 0x05, 0xC1, 0x12, 0xC1, 0x0D, 0xC1, 0x0F, 0xC1, 0x0E,
	0x00, 0xE4, 0xF5, 0xB1, 0xF5, 0xAD, 0xF5, 0xAE, 0xF5, 0xAF,
	0xF5, 0xAC, 0xF5, 0x13, 0x75, 0x11, 0xFF, 0x75, 0x14, 0x72,

	0x3A, 0x40, 0x1F, 0x80, 0x00, 0xFF, 0xF5, 0x17, 0xF5, 0x16,
	0xC2, 0x11, 0xC2, 0x10, 0x75, 0xB1, 0x01, 0x75, 0xAD, 0xFF,
	0x75, 0xAE, 0xFF, 0x75, 0xAF, 0xFF, 0xF5, 0xAC, 0x22, 0xE4,
	0xF5, 0xB1, 0xF5, 0xAD, 0xF5, 0xAE, 0xF5, 0xAF, 0xF5, 0xAC,
	0xF5, 0x13, 0x75, 0x11, 0xFF, 0x75, 0x14, 0xFF, 0xF5, 0x17,
	0xF5, 0x16, 0xC2, 0x11, 0xC2, 0x10, 0x22, 0xE4, 0xFE, 0xEE,
	0xC3, 0x9F, 0x50, 0x0C, 0xE4, 0xFD, 0x63, 0x80, 0x20, 0x9D,

	0x3A, 0x40, 0x1F, 0xC0, 0x00, 0x0D, 0xBD, 0x64, 0xF9, 0x0E,
	0x80, 0xEF, 0x53, 0x80, 0xDF, 0x22, 0xE4, 0xF5, 0xA8, 0xF5,
	0xA9, 0xF5, 0xAA, 0xF5, 0xAB, 0x22, 0x90, 0x2F, 0x11, 0x74,
	0x03, 0xF0, 0x02, 0x1A, 0x42, 0x01, 0x02, 0x04, 0x08, 0x10,
	0x20, 0x40, 0x80, 0xE4, 0xF5, 0x9C, 0x53, 0xA9, 0xEF, 0x22,
	0x00, 0x53, 0xA8, 0x01, 0xD2, 0x12, 0x32, 0x00, 0x53, 0xA8,
	0x02, 0xD2, 0x12, 0x32, 0x00, 0x53, 0xA8, 0x04, 0xD2, 0xDD,

	0x3A, 0x40, 0x20, 0x00, 0x00, 0x12, 0x32, 0x00, 0x53, 0xA8,
	0x08, 0xD2, 0x12, 0x32, 0x00, 0x53, 0xA8, 0x10, 0xD2, 0x12,
	0x32, 0x00, 0x53, 0xA8, 0x20, 0xD2, 0x12, 0x32, 0xE4, 0xF5,
	0xBE, 0xD2, 0x0E, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58,

	0x3A, 0x40, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,

	0x3A, 0x40, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,

	0x3A, 0x40, 0x20, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x01, 0x01, 0x06, 0x00,
	0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC2,

	0x3A, 0x00, 0x00, 0x00, 0x01, 0xFF
};
