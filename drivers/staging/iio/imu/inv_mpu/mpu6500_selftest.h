#ifndef _MPU6050_SELFTEST_H_
#define _MPU6500_SELFTEST_H_

enum {
	MPUREG_SELF_TEST_X_GYRO = 0,	/* 0x00,   0 */
	MPUREG_SELF_TEST_Y_GYRO,	/* 0x01,   1 */
	MPUREG_SELF_TEST_Z_GYRO,	/* 0x02,   2 */
	MPUREG_SELF_TEST_X_ACCEL = 0xD,	/* 0x0D,   13 */
	MPUREG_SELF_TEST_Y_ACCEL ,	/* 0x0E,   14 */
	MPUREG_SELF_TEST_Z_ACCEL,	/* 0x0F,   15 */
	MPUREG_XG_OFFS_USRH = 0x13,	/* 0x13,  19 */
	MPUREG_XG_OFFS_USRL,	/* 0x14,  20 */
	MPUREG_YG_OFFS_USRH,	/* 0x15,  21 */
	MPUREG_YG_OFFS_USRL,	/* 0x16,  22 */
	MPUREG_ZG_OFFS_USRH,	/* 0x17,  23 */
	MPUREG_ZG_OFFS_USRL,	/* 0x18,  24 */
	MPUREG_SMPLRT_DIV,	/* 0x19,  25 */
	MPUREG_CONFIG,		/* 0x1A,  26 */
	MPUREG_GYRO_CONFIG,	/* 0x1b,  27 */
	MPUREG_ACCEL_CONFIG,	/* 0x1c,  28 */
	MPUREG_ACCEL_CONFIG2,	/* 0x1d,  29 */
	MPUREG_LP_ACCEL_ODR,	/* 0x1e,  30 */
	MPUREG_WOM_THR,	/* 0x1f,  31 */
	MPUREG_ACCEL_INTEL_ENABLE,	/* 0x20,  32 */
	MPUREG_FIFO_EN = 0x23,		/* 0x23,  35 */
	MPUREG_I2C_MST_CTRL,	/* 0x24,  36 */
	MPUREG_I2C_SLV0_ADDR,	/* 0x25,  37 */
	MPUREG_I2C_SLV0_REG,	/* 0x26,  38 */
	MPUREG_I2C_SLV0_CTRL,	/* 0x27,  39 */
	MPUREG_I2C_SLV1_ADDR,	/* 0x28,  40 */
	MPUREG_I2C_SLV1_REG,	/* 0x29,  41 */
	MPUREG_I2C_SLV1_CTRL,	/* 0x2a,  42 */
	MPUREG_I2C_SLV2_ADDR,	/* 0x2B,  43 */
	MPUREG_I2C_SLV2_REG,	/* 0x2c,  44 */
	MPUREG_I2C_SLV2_CTRL,	/* 0x2d,  45 */
	MPUREG_I2C_SLV3_ADDR,	/* 0x2E,  46 */
	MPUREG_I2C_SLV3_REG,	/* 0x2f,  47 */
	MPUREG_I2C_SLV3_CTRL,	/* 0x30,  48 */
	MPUREG_I2C_SLV4_ADDR,	/* 0x31,  49 */
	MPUREG_I2C_SLV4_REG,	/* 0x32,  50 */
	MPUREG_I2C_SLV4_DO,	/* 0x33,  51 */
	MPUREG_I2C_SLV4_CTRL,	/* 0x34,  52 */
	MPUREG_I2C_SLV4_DI,	/* 0x35,  53 */
	MPUREG_I2C_MST_STATUS,	/* 0x36,  54 */
	MPUREG_INT_PIN_CFG,	/* 0x37,  55 */
	MPUREG_INT_ENABLE,	/* 0x38,  56 */
	MPUREG_DMP_INT_STATUS,	/* 0x39,  57 */
	MPUREG_INT_STATUS,	/* 0x3A,  58 */
	MPUREG_ACCEL_XOUT_H,	/* 0x3B,  59 */
	MPUREG_ACCEL_XOUT_L,	/* 0x3c,  60 */
	MPUREG_ACCEL_YOUT_H,	/* 0x3d,  61 */
	MPUREG_ACCEL_YOUT_L,	/* 0x3e,  62 */
	MPUREG_ACCEL_ZOUT_H,	/* 0x3f,  63 */
	MPUREG_ACCEL_ZOUT_L,	/* 0x40,  64 */
	MPUREG_TEMP_OUT_H,	/* 0x41,  65 */
	MPUREG_TEMP_OUT_L,	/* 0x42,  66 */
	MPUREG_GYRO_XOUT_H,	/* 0x43,  67 */
	MPUREG_GYRO_XOUT_L,	/* 0x44,  68 */
	MPUREG_GYRO_YOUT_H,	/* 0x45,  69 */
	MPUREG_GYRO_YOUT_L,	/* 0x46,  70 */
	MPUREG_GYRO_ZOUT_H,	/* 0x47,  71 */
	MPUREG_GYRO_ZOUT_L,	/* 0x48,  72 */
	MPUREG_EXT_SLV_SENS_DATA_00,	/* 0x49,  73 */
	MPUREG_EXT_SLV_SENS_DATA_01,	/* 0x4a,  74 */
	MPUREG_EXT_SLV_SENS_DATA_02,	/* 0x4b,  75 */
	MPUREG_EXT_SLV_SENS_DATA_03,	/* 0x4c,  76 */
	MPUREG_EXT_SLV_SENS_DATA_04,	/* 0x4d,  77 */
	MPUREG_EXT_SLV_SENS_DATA_05,	/* 0x4e,  78 */
	MPUREG_EXT_SLV_SENS_DATA_06,	/* 0x4F,  79 */
	MPUREG_EXT_SLV_SENS_DATA_07,	/* 0x50,  80 */
	MPUREG_EXT_SLV_SENS_DATA_08,	/* 0x51,  81 */
	MPUREG_EXT_SLV_SENS_DATA_09,	/* 0x52,  82 */
	MPUREG_EXT_SLV_SENS_DATA_10,	/* 0x53,  83 */
	MPUREG_EXT_SLV_SENS_DATA_11,	/* 0x54,  84 */
	MPUREG_EXT_SLV_SENS_DATA_12,	/* 0x55,  85 */
	MPUREG_EXT_SLV_SENS_DATA_13,	/* 0x56,  86 */
	MPUREG_EXT_SLV_SENS_DATA_14,	/* 0x57,  87 */
	MPUREG_EXT_SLV_SENS_DATA_15,	/* 0x58,  88 */
	MPUREG_EXT_SLV_SENS_DATA_16,	/* 0x59,  89 */
	MPUREG_EXT_SLV_SENS_DATA_17,	/* 0x5a,  90 */
	MPUREG_EXT_SLV_SENS_DATA_18,	/* 0x5B,  91 */
	MPUREG_EXT_SLV_SENS_DATA_19,	/* 0x5c,  92 */
	MPUREG_EXT_SLV_SENS_DATA_20,	/* 0x5d,  93 */
	MPUREG_EXT_SLV_SENS_DATA_21,	/* 0x5e,  94 */
	MPUREG_EXT_SLV_SENS_DATA_22,	/* 0x5f,  95 */
	MPUREG_EXT_SLV_SENS_DATA_23,	/* 0x60,  96 */
	MPUREG_ACCEL_INTEL_STATUS,	/* 0x61,  97 */
	MPUREG_62_RSVD,		/* 0x62,  98 */
	MPUREG_I2C_SLV0_DO,	/* 0x63,  99 */
	MPUREG_I2C_SLV1_DO,	/* 0x64, 100 */
	MPUREG_I2C_SLV2_DO,	/* 0x65, 101 */
	MPUREG_I2C_SLV3_DO,	/* 0x66, 102 */
	MPUREG_I2C_MST_DELAY_CTRL,	/* 0x67, 103 */
	MPUREG_SIGNAL_PATH_RESET,	/* 0x68, 104 */
	MPUREG_ACCEL_INTEL_CTRL,	/* 0x69, 105 */
	MPUREG_USER_CTRL,	/* 0x6A, 106 */
	MPUREG_PWR_MGMT_1,	/* 0x6B, 107 */
	MPUREG_PWR_MGMT_2,	/* 0x6C, 108 */
	MPUREG_BANK_SEL,	/* 0x6D, 109 */
	MPUREG_MEM_START_ADDR,	/* 0x6E, 100 */
	MPUREG_MEM_R_W,		/* 0x6F, 111 */
	MPUREG_DMP_CFG_1,	/* 0x70, 112 */
	MPUREG_DMP_CFG_2,	/* 0x71, 113 */
	MPUREG_FIFO_COUNTH,	/* 0x72, 114 */
	MPUREG_FIFO_COUNTL,	/* 0x73, 115 */
	MPUREG_FIFO_R_W,	/* 0x74, 116 */
	MPUREG_WHOAMI,		/* 0x75, 117 */
	MPUREG_XA_OFFSET_H = 0x77, /*0x77, 119*/
	MPUREG_XA_OFFSET_L, /*0x78, 119*/
	MPUREG_YA_OFFSET_H = 0x7A, /*0x7A, 119*/
	MPUREG_YA_OFFSET_L, /*0x7B, 119*/
	MPUREG_ZA_OFFSET_H = 0x7D, /*0x7D, 119*/
	MPUREG_ZA_OFFSET_L, /*0x7E, 119*/

	NUM_OF_MPU_REGISTERS	/* = 0x76, 118 */
};

#define	BIT_STBY_XG				0x04
#define	BIT_STBY_YG				0x02
#define	BIT_STBY_ZG				0x01

enum mpu_filter {
	MPU_FILTER_250HZ_NOLPF2 = 0,
	MPU_FILTER_184HZ,
	MPU_FILTER_92HZ,
	MPU_FILTER_41HZ,
	MPU_FILTER_20HZ,
	MPU_FILTER_10HZ,
	MPU_FILTER_5HZ,
	MPU_FILTER_3600HZ_NOLPF,
	NUM_MPU_FILTER
};

enum mpu_fullscale {
	MPU_FS_250DPS = 0,
	MPU_FS_500DPS,
	MPU_FS_1000DPS,
	MPU_FS_2000DPS,
	NUM_MPU_FS
};

int mpu6500_selftest_run(struct inv_mpu_state *st,
			int packet_cnt[3],
			int gyro_bias[3],
			int gyro_rms[3],
			int gyro_lsb_bias[3]);
int mpu6500_gyro_hw_self_check(struct inv_mpu_state *st, int ratio[3]);

#endif