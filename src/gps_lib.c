/*
 ============================================================================
 Name        : gps_lib.c
 Author      : HemanthJsai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Ansi-style
 ============================================================================
 */
#include "he_std.h"
#include "gps_lib_sentence.h"
#include <stdio.h>


/**
 * \brief Check the received packet type
 * @return Received packet type
 * @see nmeaPACKTYPE
 */

int nmeatype_check(char *nmea_databuf)
{
	int nmeatype_id, itr, status;
	char * nmea_packname[] = {
			"GPGGA",
			"GPGSA",
			"GPGSV",
			"GPRMC",
			"GPVTG"
	};

	if(nmea_databuf[0] == '$')
	{
		for(itr = 0; itr < GPSIZE-1; itr++)
		{
			status = he_strcmp(&nmea_databuf[1],nmea_packname[itr],5);
			if(status)
			{
				nmeatype_id = itr+1;
				return nmeatype_id;
			}

		}
	}
	nmeatype_id = GPNON;
	return nmeatype_id;
}


/**
 * \brief Check the checksum of received packet
 * @return CHECKSUM_VALID if checksum is valid else CHECKSUM_INVALID
 * @see nmeaPACKTYPE
 */
int nmea_checksum(char *nmea_databuf)
{
	int itr = 0;
	int checksum_rec = 0;
	int checksum_calc = 0;
	char checksum_ascii[3] = {0};
	nmea_databuf++;
	while(nmea_databuf[itr] != '*')
	{
		checksum_calc ^= nmea_databuf[itr];
		itr = itr + 1;
	}
	checksum_ascii[0] = nmea_databuf[itr+1];
	checksum_ascii[1] = nmea_databuf[itr+2];
	printf("rec = %s\n",checksum_ascii);
	checksum_rec  = he_a2hex(checksum_ascii);
	if(checksum_rec == checksum_calc)
	{
#ifdef DEBUG
		printf("CHECKSUM Match\n");
#endif
		return CHECKSUM_VALID;
	}
	else
	{
#ifdef DEBUG
		printf("received  %x    calculated  %x\n",checksum_rec,checksum_calc);
		printf("CHECKSUM MISMATCH\n");
#endif
		return CHECKSUM_INVALID;
	}

}


/**
 * \brief Extracts nmea_databuf data and fills info_gpgga structure
 */
void extract_nmeaGPGGA(nmeaGPGGA * info_gpgga, char *nmea_databuf)
{
	char buf_extract[15] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;
	int intbuf;
	double doublebuf;
	char charbuf[15];

	nmea_databuf = nmea_databuf + 7;
#ifdef DEBUG
	printf("%s\n",nmea_databuf);
#endif
	while(nmea_databuf[itr0] != '*') // Loop till end of the buffer
	{
		for(itr1=0; ((nmea_databuf[itr0]  != ','  ) &&
					(nmea_databuf[itr0] != '*'  ) )
		; itr0++,itr1++)  // loop till next ',' as each element separated by ','
		{
			buf_extract[itr1] = nmea_databuf[itr0];  // extract the data between two commas
		}
		buf_extract[itr1] = 0;	// end of string is written to buffer
		if( nmea_databuf[itr0] == '*' )
		{
			/* Last element in nmea packet do not increment itr0 , so while loop will fail */
		}
		else
		{
			itr0++;			/* Increment itr0 so loop continues to next element */
		}

		switch(swt_case_idx++)			//Each case for each element in the buffer
		{
		case 0:
			he_strcpy(buf_extract,charbuf,2);
			info_gpgga->utc.hour = he_a2i(charbuf);
			he_strcpy(&buf_extract[2],charbuf,2);
			info_gpgga->utc.min = he_a2i(charbuf);
			he_strcpy(&buf_extract[4],charbuf,2);
			info_gpgga->utc.sec = he_a2i(charbuf);
#ifdef DEBUG
			printf("UTC    hh  %d  mm %d  ss %d\n",info_gpgga->utc.hour, info_gpgga->utc.min, info_gpgga->utc.sec);
#endif
			break;
		case 1:
			/*   Calculation is as below
			 *  Lat : 3137.36664 becomes 31 degrees and 37.26664 seconds = 31 + 37.36664/60 = 31.6227773
			 *	Lon : 00212.21149 becomes 2 degrees and 12.21149 seconds = 2 + 12.21149/60 = 2.20352483
			 *	So as latitude is in format DDSS.SSSSS
			 *	DD = int(float(Lat)/100) = int(3137.36664/100) = int(31.3736664) = 31
			 *	SS = float(lat) - DD * 100 = 3137.36664 - 31 * 100 = 3137.36664 - 3100 = 37.36664
			 *	LatDec = DD + SS/60 = 31 + 37.36664/60 = 31 + 0.6227773333333333 = 31.6227773333333333
			 */
			doublebuf = he_a2f(buf_extract);
			intbuf = (int) ( ((float)(doublebuf)/100) );
			doublebuf = (float)(doublebuf - intbuf *100);
			doublebuf = intbuf + (doublebuf/60);
			info_gpgga->lat = doublebuf;
#ifdef DEBUG
			printf("LAT  %f\n",info_gpgga->lat);
#endif
			break;
		case 2:
			info_gpgga->ns = buf_extract[0] ;
#ifdef DEBUG
			printf("NS   %c\n",info_gpgga->ns);
#endif
			break;
		case 3:
			/*
			 *  Calculation is similar to Latitude
			 */
			doublebuf = he_a2f(buf_extract);
			intbuf = (int) ( ((float)(doublebuf)/100) );
			doublebuf = (float)(doublebuf - intbuf *100);
			doublebuf = intbuf + (doublebuf/60);
			info_gpgga->lon = doublebuf;
#ifdef DEBUG
			printf("LON  %f\n",info_gpgga->lon);
#endif
			break;
		case 4:
			info_gpgga->ew = buf_extract[0] ;
#ifdef DEBUG
			printf("EW   %c\n",info_gpgga->ew);
#endif
			break;
		case 5:
			info_gpgga->sig = he_a2i(buf_extract);
#ifdef DEBUG
			printf("SIG  %d\n",info_gpgga->sig);
#endif
			break;
		case 6:
			info_gpgga->satinuse = he_a2i(buf_extract);
#ifdef DEBUG
			printf("satinuse  %d\n",info_gpgga->satinuse);
#endif
			break;
		case 7:
			info_gpgga->HDOP = he_a2f(buf_extract);
#ifdef DEBUG
			printf("HDOP   %f\n",info_gpgga->HDOP);
#endif
			break;
		case 8:
			info_gpgga->elv = he_a2f(buf_extract);
#ifdef DEBUG
			printf("elv   %f\n",info_gpgga->elv);
#endif
			break;
		case 9:
			info_gpgga->elv_units = buf_extract[0] ;
#ifdef DEBUG
			printf("elv_units   %c\n",info_gpgga->elv_units);
#endif
			break;
		case 10:
			info_gpgga->diff = he_a2f(buf_extract);
#ifdef DEBUG
			printf("diff   %f\n",info_gpgga->diff);
#endif
			break;
		case 11:
			info_gpgga->diff_units = buf_extract[0] ;
#ifdef DEBUG
			printf("diff_units   %c\n",info_gpgga->diff_units);
#endif
			break;
		case 12:
			/*
			 *  EMPTY FIELD BY DEFAULT AS PER NMEA FORMAT
			 *  https://www.gpsinformation.org/dale/nmea.htm
			 *  HENCE NO OPERATION NEEDED
			 */
			break;
		case 13:
			/*
			 *  EMPTY FIELD BY DEFAULT AS PER NMEA FORMAT
			 *  https://www.gpsinformation.org/dale/nmea.htm
			 *  HENCE NO OPERATION NEEDED
			 */
			break;
		default:
#ifdef DEBUG
			printf("default");
#endif
			break;
		}
#ifdef DEBUG
		printf(" \n");
#endif
	}
}

/**
 * \brief Extracts nmea_databuf data and fills info_gpgsa structure
 */
void extract_nmeaGPGSA(nmeaGPGSA *info_gpgsa, char *nmea_databuf)
{
	char buf_extract[5] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;

	nmea_databuf = nmea_databuf + 7;
#ifdef DEBUG
	printf("%s\n",nmea_databuf);
#endif
	while(nmea_databuf[itr0] != '*') // Loop till end of the buffer
	{
		for(itr1=0; ( (nmea_databuf[itr0]  != ','  ) &&
					  (nmea_databuf[itr0]  != '*'  ) ) ; itr0++,itr1++)  // loop till next ',' as each element separated by ','
		{
			buf_extract[itr1] = nmea_databuf[itr0];  // extract the data between two commas
		}
		buf_extract[itr1] = 0;	// end of string is written to buffer
		if( nmea_databuf[itr0] == '*' )
		{
			/* Last element in nmea packet do not increment itr0 , so while loop will fail */
		}
		else
		{
			itr0++;			/* Increment itr0 so loop continues to next element */
		}
		switch(swt_case_idx++)			//Each case for each element in the buffer
		{
		case 0:
			info_gpgsa->fix_mode = buf_extract[0] ;
#ifdef DEBUG
			printf("fix_mode   %c\n",info_gpgsa->fix_mode);
#endif
			break;
		case 1:
			info_gpgsa->fix_type = he_a2i(buf_extract);
#ifdef DEBUG
			printf("fix_type  %d\n",info_gpgsa->fix_type);
#endif
			break;
		case 2:
			info_gpgsa->sat_prn[0] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[0]);
#endif
			break;
		case 3:
			info_gpgsa->sat_prn[1] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[1]);
#endif
			break;
		case 4:
			info_gpgsa->sat_prn[2] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[2]);
#endif
			break;
		case 5:
			info_gpgsa->sat_prn[3] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[3]);
#endif
			break;
		case 6:
			info_gpgsa->sat_prn[4] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[4]);
#endif
			break;
		case 7:
			info_gpgsa->sat_prn[5] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[5]);
#endif
			break;
		case 8:
			info_gpgsa->sat_prn[6] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[6]);
#endif
			break;
		case 9:
			info_gpgsa->sat_prn[7] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[7]);
#endif
			break;
		case 10:
			info_gpgsa->sat_prn[8] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[8]);
#endif
			break;
		case 11:
			info_gpgsa->sat_prn[9] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[9]);
#endif
			break;
		case 12:
			info_gpgsa->sat_prn[10] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[10]);
#endif
			break;
		case 13:
			info_gpgsa->sat_prn[11] = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_prn  %d\n",info_gpgsa->sat_prn[11]);
#endif
			break;
		case 14:
			info_gpgsa->PDOP = he_a2f(buf_extract);
#ifdef DEBUG
			printf("PDOP   %f\n",info_gpgsa->PDOP);
#endif
			printf("PDOP");
			break;
		case 15:
			info_gpgsa->HDOP = he_a2f(buf_extract);
#ifdef DEBUG
			printf("HDOP   %f\n",info_gpgsa->HDOP);
#endif
			break;
		case 16:
			printf("%s\n",buf_extract);
			info_gpgsa->VDOP = he_a2f(buf_extract);
#ifdef DEBUG
			printf("VDOP   %f\n",info_gpgsa->VDOP);
#endif
			break;
		default:
			printf("default");
			printf("%c\n",nmea_databuf[itr0+1]);
			printf("-----%s\n",buf_extract);
			break;
		}
		fflush(stdout);
	}

}


/**
 * \brief Extracts nmea_databuf data and fills info_gpgsv structure
 */
void extract_nmeaGPGSV(nmeaGPGSV *info_gpgsv, char *nmea_databuf)
{
	char buf_extract[5] = {0};
	int itr0 = 0;
	int itr1 = 0;
	int sat_data_idx = 0;
	char swt_case_idx = 0;

	nmea_databuf = nmea_databuf + 7;

	while(nmea_databuf[itr0] != '*') // Loop till end of the buffer
	{
		for(itr1=0; ((nmea_databuf[itr0]  != ','  ) &&
					(nmea_databuf[itr0] != '*'  ) )
		; itr0++,itr1++)  // loop till next ',' as each element separated by ','
		{
			buf_extract[itr1] = nmea_databuf[itr0];  // extract the data between two commas
		}
		buf_extract[itr1] = 0;	// end of string is written to buffer
		if( nmea_databuf[itr0] == '*' )
		{
			/* Last element in nmea packet do not increment itr0 , so while loop will fail */
		}
		else
		{
			itr0++;				/* Increment itr0 so loop continues to next element */
		}

		if(swt_case_idx >= 7)			// each nmea packet has 4 satellite information
		{								// to reuse the same cases in the switch once we reach the end then we
			swt_case_idx -= 4;          // reduce the swt_case_idx to 4 so, we have same cases to get data
			sat_data_idx ++;
		}

		switch(++swt_case_idx)			//Each case for each element in the buffer
		{
		case 1:
			info_gpgsv->pack_count = he_a2i(buf_extract);
#ifdef DEBUG
			printf("pack_count  %d\n",info_gpgsv->pack_count);
#endif
			break;
		case 2:
			info_gpgsv->pack_index = he_a2i(buf_extract);
#ifdef DEBUG
			printf("pack_index  %d\n",info_gpgsv->pack_index);
#endif
			break;
		case 3:
			info_gpgsv->sat_count = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_count  %d\n",info_gpgsv->sat_count);
#endif
			break;
		case 4:
			info_gpgsv->sat_data[sat_data_idx].id = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_data[%d].id  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].id);
#endif
			break;
		case 5:
			info_gpgsv->sat_data[sat_data_idx].elv = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_data[%d].elv  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].elv);
#endif
			break;
		case 6:
			info_gpgsv->sat_data[sat_data_idx].azimuth = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_data[%d].azimuth  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].azimuth);
#endif
			break;
		case 7:
			info_gpgsv->sat_data[sat_data_idx].sig = he_a2i(buf_extract);
#ifdef DEBUG
			printf("sat_data[%d].sig  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].sig);
#endif
			break;
		default:
			printf("GPGSV:  ERROR  %s\n",buf_extract);
			break;
		}
	}
	printf("GPGSV ERROR:  IMPLEMENTATION PENDING \nGPGSV has multiple packets this function handles only one packet\n");
	fflush(stdout);
}


/**
 * \brief Extracts nmea_databuf data and fills info_gprmc structure
 */
void extract_nmeaGPRMC(nmeaGPRMC *info_gprmc, char *nmea_databuf)
{
	char buf_extract[15] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;
	int intbuf;
	double doublebuf;
	char charbuf[15];

	nmea_databuf = nmea_databuf + 7;

	while(nmea_databuf[itr0] != '*') // Loop till end of the buffer
	{
		for(itr1=0; ((nmea_databuf[itr0]  != ','  ) &&
					(nmea_databuf[itr0] != '*'  ) )
		; itr0++,itr1++)  // loop till next ',' as each element separated by ','
		{
			buf_extract[itr1] = nmea_databuf[itr0];  // extract the data between two commas
		}
		buf_extract[itr1] = 0;	// end of string is written to buffer
		if( nmea_databuf[itr0] == '*' )
		{
			/* Last element in nmea packet do not increment itr0 , so while loop will fail */
		}
		else
		{
			itr0++;			/* Increment itr0 so loop continues to next element */
		}
		switch(++swt_case_idx)			//Each case for each element in the buffer
		{
			case 1:
				he_strcpy(buf_extract,charbuf,2);
				info_gprmc->utc.hour = he_a2i(charbuf);
				he_strcpy(&buf_extract[2],charbuf,2);
				info_gprmc->utc.min = he_a2i(charbuf);
				he_strcpy(&buf_extract[4],charbuf,2);
				info_gprmc->utc.sec = he_a2i(charbuf);
#ifdef DEBUG
				printf("\nUTC    hh  %d  mm %d  ss %d\n",info_gprmc->utc.hour, info_gprmc->utc.min, info_gprmc->utc.sec);
#endif
				break;
			case 2:
				info_gprmc->status = buf_extract[0] ;
#ifdef DEBUG
				printf("status   %c\n",info_gprmc->status);
#endif
				break;
			case 3:
				/*   Calculation is as below
				 *  Lat : 3137.36664 becomes 31 degrees and 37.26664 seconds = 31 + 37.36664/60 = 31.6227773
				 *	Lon : 00212.21149 becomes 2 degrees and 12.21149 seconds = 2 + 12.21149/60 = 2.20352483
				 *	So as latitude is in format DDSS.SSSSS
				 *	DD = int(float(Lat)/100) = int(3137.36664/100) = int(31.3736664) = 31
				 *	SS = float(lat) - DD * 100 = 3137.36664 - 31 * 100 = 3137.36664 - 3100 = 37.36664
				 *	LatDec = DD + SS/60 = 31 + 37.36664/60 = 31 + 0.6227773333333333 = 31.6227773333333333
				 */
				doublebuf = he_a2f(buf_extract);
				intbuf = (int) ( ((float)(doublebuf)/100) );
				doublebuf = (float)(doublebuf - intbuf *100);
				doublebuf = intbuf + (doublebuf/60);
				info_gprmc->lat = doublebuf;
#ifdef DEBUG
				printf("LAT  %f\n",info_gprmc->lat);
#endif
				break;
			case 4:
				info_gprmc->ns = buf_extract[0] ;
#ifdef DEBUG
				printf("NS   %c\n",info_gprmc->ns);
#endif
				break;
			case 5:
				/*
				 *  Calculation is similar to Latitude
				 */
				doublebuf = he_a2f(buf_extract);
				intbuf = (int) ( ((float)(doublebuf)/100) );
				doublebuf = (float)(doublebuf - intbuf *100);
				doublebuf = intbuf + (doublebuf/60);
				info_gprmc->lon = doublebuf;
#ifdef DEBUG
				printf("LON  %f\n",info_gprmc->lon);
#endif
				break;
			case 6:
				info_gprmc->ew = buf_extract[0] ;
#ifdef DEBUG
				printf("EW   %c\n",info_gprmc->ew);
#endif
				break;
			case 7:
				info_gprmc->speed = he_a2f(buf_extract);
#ifdef DEBUG
				printf("speed   %f\n",info_gprmc->speed);
#endif
				break;
			case 8:
				info_gprmc->direction = he_a2f(buf_extract);
#ifdef DEBUG
				printf("direction   %f\n",info_gprmc->direction);
#endif
				break;
			case 9:
				he_strcpy(buf_extract,charbuf,2);
				info_gprmc->date.day = he_a2i(charbuf);
				he_strcpy(&buf_extract[2],charbuf,2);
				info_gprmc->date.mon = he_a2i(charbuf);
				he_strcpy(&buf_extract[4],charbuf,2);
				info_gprmc->date.year = he_a2i(charbuf);
				info_gprmc->date.year = info_gprmc->date.year + 2000; // Year to be represented in YYYY hence adding 2000
#ifdef DEBUG
				printf("DATE    dd  %d  mm %d  yy %d\n",info_gprmc->date.day, info_gprmc->date.mon, info_gprmc->date.year);
#endif
				break;
			case 10:
				info_gprmc->declination = he_a2f(buf_extract);
#ifdef DEBUG
				printf("declination   %f\n",info_gprmc->declination);
#endif
				break;
			case 11:
				info_gprmc->declin_ew = buf_extract[0] ;
#ifdef DEBUG
				printf("declin_ew   %c\n",info_gprmc->declin_ew);
#endif
				break;
			case 12:
				info_gprmc->mode = buf_extract[0] ;
#ifdef DEBUG
				printf("mode   %c\n",info_gprmc->mode);
#endif
				break;
			default:
#ifdef DEBUG
				printf("\ngprmc:  error\n");
				printf("buffer @ error is : %s\n",buf_extract);
#endif
				break;
		}
#ifdef DEBUG
		fflush(stdout);
#endif

	}
}



/**
 * \brief Extracts nmea_databuf data and fills info_gpvtg structure
 */
void extract_nmeaGPVTG(nmeaGPVTG *info_gpvtg, char *nmea_databuf)
{
	char buf_extract[5] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;

	while(nmea_databuf[itr0] != '*') // Loop till end of the buffer
	{
		for(itr1=0; ((nmea_databuf[itr0]  != ','  ) &&
					(nmea_databuf[itr0] != '*'  ) )
		; itr0++,itr1++)  // loop till next ',' as each element separated by ','
		{
			buf_extract[itr1] = nmea_databuf[itr0];  // extract the data between two commas
		}
		buf_extract[itr1] = 0;	// end of string is written to buffer
		if( nmea_databuf[itr0] == '*' )
		{
			/* Last element in nmea packet do not increment itr0 , so while loop will fail */
		}
		else
		{
			itr0++;				/* Increment itr0 so loop continues to next element */
		}

		switch(++swt_case_idx)			//Each case for each element in the buffer
		{
		case 1:
			info_gpvtg->dir = he_a2f(buf_extract);
#ifdef DEBUG
			printf("dir   %f\n",info_gpvtg->dir);
#endif
			break;
		case 2:
			info_gpvtg->dir_t = buf_extract[0] ;
#ifdef DEBUG
			printf("dir_t   %c\n",info_gpvtg->dir_t);
#endif
			break;
		case 3:
			info_gpvtg->dec = he_a2f(buf_extract);
#ifdef DEBUG
			printf("dec   %f\n",info_gpvtg->dec);
#endif
			break;
		case 4:
			info_gpvtg->dec_m = buf_extract[0] ;
#ifdef DEBUG
			printf("dec_m   %c\n",info_gpvtg->dec_m);
#endif
			break;
		case 5:
			info_gpvtg->spn = he_a2f(buf_extract);
#ifdef DEBUG
			printf("spn   %f\n",info_gpvtg->spn);
#endif
			break;
		case 6:
			info_gpvtg->spn_n = buf_extract[0] ;
#ifdef DEBUG
			printf("spn_n   %c\n",info_gpvtg->spn_n);
#endif
			break;
		case 7:
			info_gpvtg->spk = he_a2f(buf_extract);
#ifdef DEBUG
			printf("spk   %f\n",info_gpvtg->spk);
#endif
			break;
		case 8:
			info_gpvtg->spk_k = buf_extract[0] ;
#ifdef DEBUG
			printf("spk_k   %c\n",info_gpvtg->spk_k);
#endif
			break;
		default:
#ifdef DEBUG
			printf("\ngpvtg:  error\n");
#endif
			break;
		}
	}
}
