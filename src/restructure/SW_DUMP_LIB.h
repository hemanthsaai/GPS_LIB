/*
 * SW_DUMP_LIB.h
 *
 *  Created on: 08-May-2020
 *      Author: heman
 */

#ifndef SW_DUMP_LIB_H_
#define SW_DUMP_LIB_H_

typedef enum _appl_mode
{
	EMERGENCY_MODE 	= 0,		/*  */
	TAMPER_MODE,				/*  */
	BATTERY_MODE,				/*  */
	IGN_ON_MODE,				/*  */
	IGN_OFF_MODE,				/*  */
	ENGINEER_MODE				/*  */
}APPLICATION_MODE;


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

typedef struct _hw_data_raw
{
	float	int_bat_volt;		/* Indicator for Level of battery charge remaining							*/
	float	main_ip_volt;		/* Indicator showing source voltage in Volts.								*/
}HW_DATA_RAW;

extern void read_hwio_status(HW_IO_STS *io_sts_frmHW);

#endif /* SW_DUMP_LIB_H_ */
