/*
 ============================================================================
 Name        : Library.c
 Author      : HemanthJsai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "he_std.h"
#include "gps_lib_sentence.h"

char * nmea_databuf = "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71";
int main(void) {
	int ptype;
	nmeaGPGGA info_gpgga;
	nmeaGPGSA info_gpgsa;
	nmeaGPGSV info_gpgsv;
	nmeaGPRMC info_gprmc;
	nmeaGPVTG info_gpvtg;
/*	FILE *fptr;
	char nmea_databuf[200];
	if ((fptr = fopen("gpslog.txt", "r")) == NULL) {
	        printf("Error! opening file");
	        exit(1);
	    }
	while(!feof(fptr))
	{
		fgets(nmea_databuf, 200, fptr);
		if( strlen(nmea_databuf) < 10)
		{
			continue;
		}
*/
		if(CHECKSUM_VALID == nmea_checksum(nmea_databuf))
		{
			ptype = nmeatype_check(nmea_databuf);
			switch (ptype)
			{
			case GPNON:
				printf("NO INPUT");
				break;
			case GPGGA:
				printf("GPGGA");
				extract_nmeaGPGGA(&info_gpgga,nmea_databuf);
				break;
			case GPGSA:
				printf("GPGSA");
				extract_nmeaGPGSA(&info_gpgsa,nmea_databuf);
				break;
			case GPGSV:
				printf("GPGSV");
				extract_nmeaGPGSV(&info_gpgsv,nmea_databuf);
				break;
			case GPRMC:
				printf("GPRMC");
				extract_nmeaGPRMC(&info_gprmc,nmea_databuf);
				break;
			case GPVTG:
				printf("GPVTG");
				extract_nmeaGPVTG(&info_gpvtg,nmea_databuf);
				break;
			default:
				printf("error");
				break;
			}
			fflush(stdout);
			nmea_databuf[0] = 0;
/*		}*/

	}
	return EXIT_SUCCESS;
}
