/*
 ============================================================================
 Name        : main_appl.c
 Author      : HemanthJSai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "SW_DUMP_LIB.h"
#include "..\stdlib\include\he_std.h"


GBL_INTR_HNDLR gbl_intr;

#ifdef APPL_AS_MAIN
int main(void) {
#else
int appl_header(void) {
#endif

	HW_IO_STS 	io_sts_frmHW;
	SDCARD_RAW	consts_frm_sdcard;
	APPLICATION_MODE curr_app_mode;

	while(1)
	{
		sleep(10);
	/* THIS is a interrupt simulator & don't use this on HARDWARE  */
	read_GBL_INTR(&gbl_intr);

	/*read_SD_CONSTS is executed once before the application starts running*/
	read_SD_CONSTS(&consts_frm_sdcard);

	read_hwio_status(&io_sts_frmHW);

	curr_app_mode = prioritize_modes();
	switch(curr_app_mode)
	{
	case EMERGENCY_MODE:
		printf("Starting Application in EMERGENCY_MODE\n");
		break;
	case LOW_BATTERY_MODE:
		printf("Starting Application in LOW_BATTERY_MODE\n");
		break;
	case ON_BATTERY_MODE:
		printf("Starting Application in ON_BATTERY_MODE\n");
		break;
	case IGN_ON_MODE:
		printf("Starting Application in IGN_ON_MODE\n");
		break;
	case IGN_OFF_MODE:
		printf("Starting Application in IGN_OFF_MODE\n");
		break;
	default:
		printf("Starting Application in DEFAULT_MODE\n");
		break;
	}

	//puts("!!!PERFECT Hemanth!!!"); /* prints !!!Hello Hemanth!!! */
	fflush(stdout);
	}
	return EXIT_SUCCESS;
}


