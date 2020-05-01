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


#include "he_nmea_lib.h"
#include "he_std.h"

char *command[] = {	"AT+QGNSSRD=GGA",
					"AT+QGNSSRD=GSA",
					"AT+QGNSSRD=GSV",
					"AT+QGNSSRD=RMC",
					"AT+QGNSSRD=VTG"} ;

char *Response[] = {//"$GPGGA,S,KDfh,klasdhas,dbfgiad,fhvoiadf,hoahg,fgo;arerong yergfaoweyefewp;f *3D",
					//"$GPGGA,,,,,,,,,,sdgsdgfhtrshrsthrthththaerggetghgher,,,,",
					//"$GPGGA,SKDfhklasdhasd32161863oiadfhoahgfgo;arhg aerong yergfaoweyefewp;f*2E",
					"$GPGGA,aldskfhksdjihdv,5001.2652,N,3613.0586,E,3,08,0.0,0.1,M,0.0,M,0.0,0000*16",
					"$GPGSA,A,3,1.0,02,03,04,05,06,0A,08,00,00,00,00,0.0,0.0,0.0*62",
					"$GPGSV,2,1,08,01,05,005,80,02,05,050,80,03,05,095,80,04,05,140,80*7f",
					"$GPRMC,185849.80,A,5001.2652,N,3613.0586,E,11.2,349.0,240420,349.0,E,A*3d",
					"$GPVTG,,T,349.0,M,11.2,N,20.8,K*66"};

//char * nmea_databuf = "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71";
char *nmea_databuf;


int main(void) {

	int nmea_sts = NMEA_INVALID;
	nmea_grp_type nmea_data;
	char Input[20];
	scanf("%s",Input);
	nmea_databuf = 0;


	for(int iter = 0; iter <5; iter ++)
	{
		if(he_strcmp(command[iter],Input))
		{
			nmea_databuf = Response[iter];
			//printf("%s\n",Response[iter]);
		}
	}
	printf("%s\n",nmea_databuf);
	if(nmea_databuf)
	{
		nmea_sts = he_nmea_main(nmea_databuf,&nmea_data);
		if(nmea_sts == NMEA_VALID)
		{
			printf("SUCCESS\n");
			printf("%d\n",nmea_data.nmea_pkt_type);
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
