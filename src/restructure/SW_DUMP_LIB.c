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

/*
 *  This Function updates the io_sts_frmHW structure
 *	from the Hardware.
 *	TODO Currently this function reads data from
 *		 a file HW_data.log for simulation. Once
 *		 the Hardware is available read from pins
 *
 */
void read_hwio_status(HW_IO_STS *io_sts_frmHW)
{
	FILE *hw_file_ptr = 0;
	hw_file_ptr = fopen("HW_data.log","r");
	if(hw_file_ptr == 0 )
	{
		printf("error in opening file\n");
	}
	else
	{
		printf("FILE OPENED\n");
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
