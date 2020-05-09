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
#include "../nmea/include/he_nmea_lib.h"
#include "../stdlib/include/he_std.h"

char *command[] = {	"AT+QGNSSRD=GGA",
					"AT+QGNSSRD=GSA",
					"AT+QGNSSRD=GSV",
					"AT+QGNSSRD=RMC",
					"AT+QGNSSRD=VTG"} ;

char *Response[] = {"$GPGGA,185849.80,5001.2652,N,3613.0586,E,3,08,0.0,0.0,M,0.0,M,0.0,0000*42",
					"$GPGSA,A,3,01,02,03,04,05,06,07,08,00,00,00,00,0.0,0.0,0.0*3a",
					"$GPGSV,2,1,08,01,05,005,80,02,05,050,80,03,05,095,80,04,05,140,80*7f",
					"$GPRMC,185849.80,A,5001.2652,N,3613.0586,E,11.2,349.0,240420,349.0,E,A*3d",
					"$GPVTG,349.0,T,349.0,M,11.2,N,20.8,K*46"};

//char * nmea_databuf = "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71";
char *nmea_databuf;
#ifdef NMEA_AS_MAIN
int main(void) {


	int nmea_sts = NMEA_INVALID;
	nmea_info_grp_type nmea_info_grp;
	char Input[20];
	scanf("%s",Input);
	nmea_databuf = 0;


	for(int iter = 0; iter <5; iter ++)
	{
		if(he_strcmp(command[iter],Input))
		{
			nmea_databuf = Response[iter];
		}
	}
	printf("%s\n",nmea_databuf);
	if(nmea_databuf)
	{
		nmea_sts = he_nmea_process(nmea_databuf,&nmea_info_grp);
		if(nmea_sts == NMEA_VALID)
		{
			printf("SUCCESS\n");
		}
		else
		{
			printf("FAIL\n");
		}
	}
	else
	{
		printf("INVALID\n");
	}

	return EXIT_SUCCESS;
}
#else

int nmea_main(void){
	int nmea_sts = NMEA_INVALID;
	nmea_info_grp_type nmea_data;

	for (int i = 0; i<=4; i++)
	{
		nmea_databuf = Response[i];
		nmea_sts = he_nmea_process(nmea_databuf,&nmea_data);
	}
	return nmea_sts;
}
#endif
