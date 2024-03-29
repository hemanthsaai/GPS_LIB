#ifndef ALERTS_H_
#define ALERTS_H_

typedef struct _nmea_data_str
{
	char 	gps_vldty[2];			/* 0 = Invalid; 1 = Fix; 2 = Differential, 3 = Sensitive  					*/
	char	date[9];			/* DDMMYYYY																	*/
	char	time[7];			/* HHMMSS																	*/
	char 	lat[13];			/* Latitude in decimal degrees -  dd.mmmmmm format 							*/
	char 	lat_dir;			/* N � North, S � South														*/
	char 	lon[13];			/* Longitude in decimal degrees - dd.mmmmmm format 							*/
	char 	lon_dir;			/* E � East,  W � West														*/
	char	speed[7];			/* Speed of Vehicle as Calculated by GPS module in VLT. (in KM/HR)			*/
	char	heading[5];			/* Course over ground in degrees 											*/
	char	num_sat_fix[3];		/* Number of satellites available for fix 									*/
	char	altitude[13];		/* Altitude in meters (above sea level) 									*/
	char	pdop[4];			/* Positional dilution of precision 										*/
	char	hdop[4];			/* Horizontal dilution of precision 										*/
}NMEA_DATA_STR;

typedef struct _gsm_data_STR
{
	char	nw_operator[7];		/* Name of Network Operator 												*/
	char	gsm_strength[3];	/* Value Ranging from 0 � 31 												*/
	char	mcc[4];				/* Mobile Country Code														*/
	char	mnc[3];				/* Mobile Network Code														*/
	char	lac[5];				/* Location Area Code in HEX												*/
	char	cell_id[5];			/* GSM Cell ID																*/
	char	nmr[5];				/* Neighboring 4 cell ID along with their LAC and signal strength			*/
}GSM_DATA_STR;

typedef struct _veh_data_str
{
	char	veh_reg_no[17];		/* Registration Number of the Vehicle 										*/
	char	ignition;			/* 1 = IGN On , 0 = IGN Off 												*/
	char	veh_bat_sts;		/* 0 = Vehicle Battery Disconnected 1= Vehicle Battery Reconnected			*/
	char	main_ip_volt[6];	/* Indicator showing source voltage in Volts.								*/
	char	int_bat_volt[6];	/* Indicator for Level of battery charge remaining							*/
	char	emer_btn_sts;		/* 1= On , 0 = Off															*/
	char	dig_ip_sts[5];		/* 4 external digital inputs (Status of Input 1 to Input 4 (0=Off; 1=On))	*/
	char	dig_op_sts[3];		/* 2 external digital output status (0=Off; 1=On)							*/
}VEH_DATA_STR;

typedef struct _hw_data_str
{
	char 	imei[16];			/* Unique ID of the Vehicle 												*/
	char 	pkt_hdr[4];			/* The unique identifier for all messages from VLT 							*/
	char	firm_ver[6];		/* Version details of the Firmware used in EX.1.0.0 						*/
	char	tmpr_alert;			/* C = Cover Closed , O = Cover Open										*/
}HW_DATA_STR;



#endif
