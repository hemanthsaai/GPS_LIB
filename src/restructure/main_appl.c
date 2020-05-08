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

#ifdef APPL_AS_MAIN
int main(void) {
#else
int appl_main(void) {
#endif

	HW_IO_STS io_sts_frmHW;

	read_hwio_status(&io_sts_frmHW);

	puts("!!!PERFECT Hemanth!!!"); /* prints !!!Hello Hemanth!!! */
	fflush(stdout);
	return EXIT_SUCCESS;
}
