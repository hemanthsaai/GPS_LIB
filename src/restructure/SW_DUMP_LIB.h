/*
 * SW_DUMP_LIB.h
 *
 *  Created on: 08-May-2020
 *      Author: hemanthjsai
 */

#ifndef SW_DUMP_LIB_H_
#define SW_DUMP_LIB_H_

/*  */
typedef enum _appl_mode
{
	EMERGENCY_MODE 	= 0,		/*  */
	LOW_BATTERY_MODE,			/*  */
	ON_BATTERY_MODE,			/*  */
	IGN_ON_MODE,				/*  */
	IGN_OFF_MODE,				/*  */
	DEFAULT_MODE				/*  */
}APPLICATION_MODE;

/*  */
typedef struct _gbl_intr_hndlr
{
	int eme_intr;				/*  */
	int ign_intr;				/*  */
	int on_bat_intr;			/*  */
	int tmpr_intr;				/*  */
	int low_bat;				/*  */
}GBL_INTR_HNDLR;

/*  */
typedef struct _hw_io_sts
{
	int		veh_ign_sts;		/* 1 = IGN On , 0 = IGN Off 												*/
	int		veh_bat_sts;		/* 0 = Vehicle Battery Disconnected 1= Vehicle Battery Reconnected			*/
	int		eme_btn_sts;		/* 1= On , 0 = Off															*/
	int		dev_tmp_alt;		/* 0 = Cover Closed , 1 = Cover Open										*/
	int		dig_ip_sts_0;		/* 4 external digital inputs (Status of Input 1 to Input 4 (0=Off; 1=On))	*/
	int		dig_ip_sts_1;
	int		dig_ip_sts_2;
	int		dig_ip_sts_3;
	int		dig_op_sts_0;		/* 2 external digital output status (0=Off; 1=On)							*/
	int		dig_op_sts_1;
}HW_IO_STS;

/*  */
typedef struct _hw_data_raw
{
	float	int_bat_volt;		/* Indicator for Level of battery charge remaining							*/
	float	main_ip_volt;		/* Indicator showing source voltage in Volts.								*/
}HW_DATA_RAW;

/*  */
typedef struct _sdcard_raw
{
	char	veh_reg_no[17];		/* Registration Number of the Vehicle 										*/
	char 	imei[16];			/* Unique ID of the Vehicle 												*/
	char 	pkt_hdr[4];			/* The unique identifier for all messages from VLT 							*/
	char	firm_ver[6];		/* Version details of the Firmware used in EX.1.0.0 						*/
}SDCARD_RAW;

/*  */
extern void 				read_GBL_INTR		(GBL_INTR_HNDLR *gbl_intr		);
extern void					read_hwio_status	(HW_IO_STS 		*io_sts_frmHW	);
extern void 				read_SD_CONSTS		(SDCARD_RAW 	*sdcard_consts	);
extern APPLICATION_MODE 	prioritize_modes	(void							);
extern void 				application_run		(APPLICATION_MODE curr_app_mode	);

extern GBL_INTR_HNDLR gbl_intr;

#endif /* SW_DUMP_LIB_H_ */
