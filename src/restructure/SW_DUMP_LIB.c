/*
 ============================================================================
 Name        : SW_DUMP_LIB.c
 Author      : HemanthJSai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include "SW_DUMP_LIB.h"

/*  BRIEF\
 *  This Function updates the io_sts_frmHW structure
 *	from the Hardware.
 *
 *	CALL FREQUENCY\
 *	This function is called every time when application
 *	starts running. Based on the PIN status application
 *	decides in which mode to run
 *
 *	TODO
 *	Currently this function reads data from
 *	a file HW_data.log for simulation. Once
 *	the Hardware is available read from pins
 *
 */
void read_hwio_status(HW_IO_STS *io_sts_frmHW)
{
	FILE *hw_file_ptr = 0;
	hw_file_ptr = fopen("Simulation_files\\HW_data.log","r");
	if(hw_file_ptr <= 0 )
	{
		printf("error in opening file\n");
	}
	else
	{
		printf("FILE OPENED HW_data.log\n");
		fscanf(hw_file_ptr, "%d%d%d%d%d%d%d%d%d%d",&io_sts_frmHW->veh_ign_sts,
				&io_sts_frmHW->veh_bat_sts,
				&io_sts_frmHW->eme_btn_sts,
				&io_sts_frmHW->dev_tmp_alt,
				&io_sts_frmHW->dig_ip_sts_0,
				&io_sts_frmHW->dig_ip_sts_1,
				&io_sts_frmHW->dig_ip_sts_2,
				&io_sts_frmHW->dig_ip_sts_3,
				&io_sts_frmHW->dig_op_sts_0,
				&io_sts_frmHW->dig_op_sts_1);
		printf("%d %d %d %d %d %d %d %d %d %d\n",io_sts_frmHW->veh_ign_sts,
				io_sts_frmHW->veh_bat_sts,
				io_sts_frmHW->eme_btn_sts,
				io_sts_frmHW->dev_tmp_alt,
				io_sts_frmHW->dig_ip_sts_0,
				io_sts_frmHW->dig_ip_sts_1,
				io_sts_frmHW->dig_ip_sts_2,
				io_sts_frmHW->dig_ip_sts_3,
				io_sts_frmHW->dig_op_sts_0,
				io_sts_frmHW->dig_op_sts_1);
		fflush(stdout);
		fclose(hw_file_ptr);
	}
}


/*	BRIEF\
 *  This Function updates the sdcard_consts structure
 *	from the SDCARD or EEPROM.
 *
 *	CALL FREQUENCY\
 *	This is called once before the application starts running
 *	and called again only after a configuration update request
 *
 *	TODO
 *	Currently this function reads data from
 *	a file SDcard_CONSTANTS.log for simulation.
 *	Once the Hardware is available read from MEMORY
 */
void read_SD_CONSTS(SDCARD_RAW *sdcard_consts)
{
	FILE *sdcard_file_ptr = 0;
	sdcard_file_ptr = fopen("Simulation_files\\SDcard_CONSTANTS.log","r");
	if(sdcard_file_ptr <= 0 )
	{
		printf("error in opening file\n");
	}
	else
	{
		printf("FILE OPENED SDcard_CONSTANTS.log\n");
		fscanf(sdcard_file_ptr, "%s %s %s %s",sdcard_consts->veh_reg_no,
				sdcard_consts->imei,
				sdcard_consts->pkt_hdr,
				sdcard_consts->firm_ver);
		printf("%s %s %s %s\n",sdcard_consts->veh_reg_no,
				sdcard_consts->imei,
				sdcard_consts->pkt_hdr,
				sdcard_consts->firm_ver);
		fflush(stdout);
		fclose(sdcard_file_ptr);
	}
}


/*	BRIEF\
 *  This Function is a INTERRUPT simulator via FILE
 *  and not needed when using hardware
 *
 *	CALL FREQUENCY\
 *	This is called once before the application starts running
 *
 *	TODO
 *	Configure interrupts for all the gbl_intr fields
 *	and update gbl_intr in the interrupt handler
 *
 */
void read_GBL_INTR(GBL_INTR_HNDLR *gbl_intr)
{
	FILE *gblintr_file_ptr = 0;
	gblintr_file_ptr = fopen("Simulation_files\\GLOBAL_INTERRUPTS.log","r");
	if(gblintr_file_ptr <= 0 )
	{
		printf("error in opening file\n");
	}
	else
	{
		printf("FILE OPENED GLOBAL_INTERRUPTS.log\n");
		fscanf(gblintr_file_ptr, "%d %d %d %d %d",&gbl_intr->eme_intr,
				&gbl_intr->ign_intr,
				&gbl_intr->on_bat_intr,
				&gbl_intr->tmpr_intr,
				&gbl_intr->low_bat);
		printf("%d %d %d %d %d\n",gbl_intr->eme_intr,
				gbl_intr->ign_intr,
				gbl_intr->on_bat_intr,
				gbl_intr->tmpr_intr,
				gbl_intr->low_bat);
		fflush(stdout);
		fclose(gblintr_file_ptr);
	}
}


/*	BRIEF\
 *  This Function decides which mode the application
 *  should start.
 *
 *  on simulator this function should be called
 *  only after read_GBL_INTR()
 *
 *	CALL FREQUENCY\
 *	This is always called before the application starts running
 *
 *	TODO
 *	REFACTOR the priorities
 */
APPLICATION_MODE prioritize_modes(void)
{
	APPLICATION_MODE mode;
	if(gbl_intr.eme_intr == 1)
	{
		mode = EMERGENCY_MODE;
	}
	else if(gbl_intr.low_bat == 1)
	{
		mode = LOW_BATTERY_MODE;
	}
	else if(gbl_intr.on_bat_intr == 1)
	{
		mode = ON_BATTERY_MODE;
	}
	else if(gbl_intr.ign_intr == 1)
	{
		mode = IGN_ON_MODE;
	}
	else if(gbl_intr.ign_intr == 0)
	{
		mode = IGN_OFF_MODE;
	}
	else
	{
		mode = DEFAULT_MODE;
	}

	return mode;
}
