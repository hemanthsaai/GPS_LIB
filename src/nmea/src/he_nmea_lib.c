/*
 ============================================================================
 Name        : he_nmea_lib.c
 Author      : HemanthJsai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Ansi-style
 ============================================================================
 */
#include "../include/he_nmea_lib.h"
#include "../../stdlib/include/he_std.h"
#include <stdio.h>
/*
 * Check if the nmea has * and ,
 * because after this all the functions consider the line ending as *
 * so filter the data here to avoid garbage in further modules
 *
 * Returns NMEA_VALID if the buffer is valid input for further processing
 */
int nmea_canbeparsed(char *nmea_databuf)
{
	int ret_sts = NMEA_INVALID;
	int buf_len = 0;
	int comma_cnt = 0;
	int star_cnt = 0;
	int iter = 0;
	int star_pos = 0;

	buf_len = he_strlen(nmea_databuf);

	if( (buf_len >= NMEA_MIN_LEN) && (buf_len <= NMEA_MAX_LEN))
	{
		while(iter <= buf_len)
		{
			if(nmea_databuf[iter] == '*')
			{ /*Take the position of * */
				star_cnt++;
				star_pos = iter;
			}
			else if(nmea_databuf[iter] == ',')
			{
				comma_cnt++;
			}
			iter++;
		}

		if(comma_cnt>=5 && star_cnt == 1)
		{	/* check for star position in the buffer is valid   Used 5 considering 2 checksum char + \r\n at the end*/
			if((buf_len - star_pos) >= 2 && (buf_len - star_pos) <= 4)
			{
				ret_sts = NMEA_VALID;
			}
		}
	}

#ifdef DEBUG
	printf("FUNC--nmea_canbeparsed\nbuffer Length Received:  %d\n",buf_len);
	printf("Fields of , in Buffer: %d\n",comma_cnt);
	printf("Pos of * : %d\n",star_pos);
	printf("Star count in data: %d\n",star_cnt);
#endif
	return ret_sts;
}


int nmea_datatype_isfieldvalid(char *buf, nmea_datatype check)
{
	int ret_val = NMEA_INVALID;
	switch(check)
	{
	case CHAR_VAL:
		if(he_isacharA_Z(buf[0]))
		{
			ret_val = NMEA_VALID;
		}
		break;
	case DOUBLE_VAL:
		if(he_isadouble(buf))
		{
			ret_val = NMEA_VALID;
		}
		break;
	case INT_VAL:
		if(he_isanint0_9(buf))
		{
			ret_val = NMEA_VALID;
		}
		break;
	}
	return ret_val;
}

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
			status = he_strcmp_len(&nmea_databuf[1],nmea_packname[itr],5);
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
	checksum_rec  = he_a2hex(checksum_ascii);
	if(checksum_rec == checksum_calc)
	{
#ifdef DEBUG
		printf("FUNC--nmea_checksum\nCHECKSUM Match\n");
#endif
		return CHECKSUM_VALID;
	}
	else
	{
#ifdef DEBUG
		printf("FUNC--nmea_checksum\nreceived checksum %x    calculated checksum %x\n",checksum_rec,checksum_calc);
		printf("CHECKSUM MISMATCH\n");
#endif
		return CHECKSUM_INVALID;
	}

}


/**
 * \brief Extracts nmea_databuf data and fills info_gpgga structure
 */
int extract_nmeaGPGGA(nmeaGPGGA * info_gpgga, char *nmea_databuf)
{
	char buf_extract[15] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;
	int intbuf;
	double doublebuf;
	char charbuf[15];
	int nmea_vldty = NMEA_VALID;

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
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				he_strcpy(buf_extract,charbuf,2);
				info_gpgga->utc.hour = he_a2i(charbuf);
				he_strcpy(&buf_extract[2],charbuf,2);
				info_gpgga->utc.min = he_a2i(charbuf);
				he_strcpy(&buf_extract[4],charbuf,2);
				info_gpgga->utc.sec = he_a2i(charbuf);
#ifdef DEBUG
			printf("UTC    hh  %d  mm %d  ss %d\n",info_gpgga->utc.hour, info_gpgga->utc.min, info_gpgga->utc.sec);
#endif
			}
			else
			{
				info_gpgga->utc.hour 	= 0;
				info_gpgga->utc.min  	= 0;
				info_gpgga->utc.sec		= 0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("UTC   INVALID\n");
#endif
			}
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
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				doublebuf = he_a2f(buf_extract);
				intbuf = (int) ( ((float)(doublebuf)/100) );
				doublebuf = (float)(doublebuf - intbuf *100);
				doublebuf = intbuf + (doublebuf/60);
				info_gpgga->lat = doublebuf;
#ifdef DEBUG
				printf("LAT  %f\n",info_gpgga->lat);
#endif
			}
			else
			{
				info_gpgga->lat = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("LAT  INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpgga->ns = buf_extract[0] ;
#ifdef DEBUG
				printf("NS   %c\n",info_gpgga->ns);
#endif
			}
			else
			{
				info_gpgga->ns = '0';
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("NS  INVALID\n");
#endif
			}
			break;
		case 3:
			/*
			 *  Calculation is similar to Latitude
			 */
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				doublebuf = he_a2f(buf_extract);
				intbuf = (int) ( ((float)(doublebuf)/100) );
				doublebuf = (float)(doublebuf - intbuf *100);
				doublebuf = intbuf + (doublebuf/60);
				info_gpgga->lon = doublebuf;
#ifdef DEBUG
				printf("LON  %f\n",info_gpgga->lon);
#endif
			}
			else
			{
				info_gpgga->lon = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("LON  INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpgga->ew = buf_extract[0] ;
#ifdef DEBUG
				printf("EW   %c\n",info_gpgga->ew);
#endif
			}
			else
			{
				info_gpgga->ew = '0';
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("EW  INVALID\n");
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgga->sig = he_a2i(buf_extract);
#ifdef DEBUG
				printf("SIG  %d\n",info_gpgga->sig);
#endif
			}
			else
			{
				info_gpgga->sig = 0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("SIG  INVALID\n");
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgga->satinuse = he_a2i(buf_extract);
#ifdef DEBUG
				printf("satinuse  %d\n",info_gpgga->satinuse);
#endif
			}
			else
			{
				info_gpgga->satinuse = 0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("satinuse  INVALID\n");
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpgga->HDOP = he_a2f(buf_extract);
#ifdef DEBUG
				printf("HDOP   %f\n",info_gpgga->HDOP);
#endif
			}
			else
			{
				info_gpgga->HDOP = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("HDOP  INVALID\n");
#endif
			}
			break;
		case 8:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpgga->elv = he_a2f(buf_extract);
#ifdef DEBUG
				printf("elv   %f\n",info_gpgga->elv);
#endif
			}
			else
			{
				info_gpgga->elv = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("elv  INVALID\n");
#endif
			}
			break;
		case 9:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpgga->elv_units = buf_extract[0] ;
#ifdef DEBUG
			printf("elv_units   %c\n",info_gpgga->elv_units);
#endif
			}
			else
			{
				info_gpgga->elv_units = '0';
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("elv_units  INVALID\n");
#endif
			}
			break;
		case 10:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpgga->diff = he_a2f(buf_extract);
#ifdef DEBUG
				printf("diff   %f\n",info_gpgga->diff);
#endif
			}
			else
			{
				info_gpgga->diff = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("diff  INVALID\n");
#endif
			}
			break;
		case 11:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpgga->diff_units = buf_extract[0] ;
#ifdef DEBUG
				printf("diff_units   %c\n",info_gpgga->diff_units);
#endif
			}
			else
			{
				info_gpgga->diff_units = '0';
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("diff_units  INVALID\n");
#endif
			}
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
			nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
			printf("default");
#endif
			break;
		}
#ifdef DEBUG
		printf(" \n");
#endif
	}
	return nmea_vldty;
}

/**
 * \brief Extracts nmea_databuf data and fills info_gpgsa structure
 */
int extract_nmeaGPGSA(nmeaGPGSA *info_gpgsa, char *nmea_databuf)
{
	char buf_extract[5] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;
	int nmea_vldty = NMEA_VALID;

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
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpgsa->fix_mode = buf_extract[0] ;
#ifdef DEBUG
				printf("fix_mode   %c\n",info_gpgsa->fix_mode);
#endif
			}
			else
			{
				info_gpgsa->fix_mode = '0' ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("fix_mode   INVALID\n");
#endif
			}
			break;
		case 1:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->fix_type = he_a2i(buf_extract);
#ifdef DEBUG
				printf("fix_type  %d\n",info_gpgsa->fix_type);
#endif
			}
			else
			{
				info_gpgsa->fix_type = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("fix_type  INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[0] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[0]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[0] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 3:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[1] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[1]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[1] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[2] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[2]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[2] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[3] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[3]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[3] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[4] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[4]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[4] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[5] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[5]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[5] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 8:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[6] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[6]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[6] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 9:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[7] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[7]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[7] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 10:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[8] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[8]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[8] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 11:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[9] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[9]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[9] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 12:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[10] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[10]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[10] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 13:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsa->sat_prn[11] = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_prn  %d\n",info_gpgsa->sat_prn[11]);
#endif
			}
			else
			{
				info_gpgsa->sat_prn[11] = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 14:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpgsa->PDOP = he_a2f(buf_extract);
#ifdef DEBUG
				printf("PDOP   %f\n",info_gpgsa->PDOP);
#endif
			}
			else
			{
				info_gpgsa->PDOP = 0.0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("PDOP  INVALID\n");
#endif
			}
			break;
		case 15:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpgsa->HDOP = he_a2f(buf_extract);
#ifdef DEBUG
				printf("HDOP   %f\n",info_gpgsa->HDOP);
#endif
			}
			else
			{
				info_gpgsa->HDOP = 0.0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("HDOP  INVALID\n");
#endif
			}
			break;
		case 16:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpgsa->VDOP = he_a2f(buf_extract);
#ifdef DEBUG
				printf("VDOP   %f\n",info_gpgsa->VDOP);
#endif
			}
			else
			{
				info_gpgsa->VDOP = 0.0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("VDOP  INVALID\n");
#endif
			}
			break;
		default:
			nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
			printf("default");
			printf("%c\n",nmea_databuf[itr0+1]);
			printf("-----%s\n",buf_extract);
#endif
			break;
		}
		fflush(stdout);
	}

	return nmea_vldty;
}


/**
 * \brief Extracts nmea_databuf data and fills info_gpgsv structure
 */
int extract_nmeaGPGSV(nmeaGPGSV *info_gpgsv, char *nmea_databuf)
{
	char buf_extract[5] = {0};
	int itr0 = 0;
	int itr1 = 0;
	int sat_data_idx = 0;
	char swt_case_idx = 0;
	int nmea_vldty = NMEA_VALID;

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
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsv->pack_count = he_a2i(buf_extract);
#ifdef DEBUG
				printf("pack_count  %d\n",info_gpgsv->pack_count);
#endif
			}
			else
			{
				info_gpgsv->pack_count = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("pack_count INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsv->pack_index = he_a2i(buf_extract);
#ifdef DEBUG
				printf("pack_index  %d\n",info_gpgsv->pack_index);
#endif
			}
			else
			{
				info_gpgsv->pack_index = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("pack_index INVALID\n");
#endif
			}
			break;
		case 3:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsv->sat_count = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_count  %d\n",info_gpgsv->sat_count);
#endif
			}
			else
			{
				info_gpgsv->sat_count = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_count INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsv->sat_data[sat_data_idx].id = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_data[%d].id  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].id);
#endif
			}
			else
			{
				info_gpgsv->sat_data[sat_data_idx].id = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_data[%d].id  INVALID\n",sat_data_idx);
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsv->sat_data[sat_data_idx].elv = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_data[%d].elv  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].elv);
#endif
			}
			else
			{
				info_gpgsv->sat_data[sat_data_idx].elv = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_data[%d].elv  INVALID\n",sat_data_idx);
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsv->sat_data[sat_data_idx].azimuth = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_data[%d].azimuth  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].azimuth);
#endif
			}
			else
			{
				info_gpgsv->sat_data[sat_data_idx].azimuth = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_data[%d].azimuth  INVALID\n",sat_data_idx);
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				info_gpgsv->sat_data[sat_data_idx].sig = he_a2i(buf_extract);
#ifdef DEBUG
				printf("sat_data[%d].sig  %d\n",sat_data_idx,info_gpgsv->sat_data[sat_data_idx].sig);
#endif
			}
			else
			{
				info_gpgsv->sat_data[sat_data_idx].sig = 0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("sat_data[%d].sig  INVALID\n",sat_data_idx);
#endif
			}
			break;
		default:
			nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
			printf("GPGSV:  ERROR  %s\n",buf_extract);
#endif
			break;
		}
	}
#ifdef DEBUG
	printf("GPGSV ERROR:  IMPLEMENTATION PENDING \nGPGSV has multiple packets this function handles only one packet\n");
	fflush(stdout);
#endif
	return nmea_vldty;
}


/**
 * \brief Extracts nmea_databuf data and fills info_gprmc structure
 */
int extract_nmeaGPRMC(nmeaGPRMC *info_gprmc, char *nmea_databuf)
{
	char buf_extract[15] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;
	int intbuf;
	double doublebuf;
	char charbuf[15];
	int nmea_vldty = NMEA_VALID;

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
				if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
				{
					he_strcpy(buf_extract,charbuf,2);
					info_gprmc->utc.hour = he_a2i(charbuf);
					he_strcpy(&buf_extract[2],charbuf,2);
					info_gprmc->utc.min = he_a2i(charbuf);
					he_strcpy(&buf_extract[4],charbuf,2);
					info_gprmc->utc.sec = he_a2i(charbuf);
#ifdef DEBUG
					printf("\nUTC    hh  %d  mm %d  ss %d\n",info_gprmc->utc.hour, info_gprmc->utc.min, info_gprmc->utc.sec);
#endif
				}
				else
				{
					info_gprmc->utc.hour = 0;
					info_gprmc->utc.min  = 0;
					info_gprmc->utc.sec  = 0;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("UTC    INVALID\n");
#endif
				}
				break;
			case 2:
				if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
				{
					info_gprmc->status = buf_extract[0] ;
#ifdef DEBUG
					printf("status   %c\n",info_gprmc->status);
#endif
				}
				else
				{
					info_gprmc->status = '0' ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("status   INVALID\n");
#endif
				}
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
				if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
				{
					doublebuf = he_a2f(buf_extract);
					intbuf = (int) ( ((float)(doublebuf)/100) );
					doublebuf = (float)(doublebuf - intbuf *100);
					doublebuf = intbuf + (doublebuf/60);
					info_gprmc->lat = doublebuf;
#ifdef DEBUG
					printf("LAT  %f\n",info_gprmc->lat);
#endif
				}
				else
				{
					info_gprmc->lat = 0.0 ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("LAT   INVALID\n");
#endif
				}
				break;
			case 4:
				if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
				{
					info_gprmc->ns = buf_extract[0] ;
#ifdef DEBUG
					printf("NS   %c\n",info_gprmc->ns);
#endif
				}
				else
				{
					info_gprmc->ns = '0' ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("NS   INVALID\n");
#endif
				}
				break;
			case 5:
				/*
				 *  Calculation is similar to Latitude
				 */
				if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
				{
					doublebuf = he_a2f(buf_extract);
					intbuf = (int) ( ((float)(doublebuf)/100) );
					doublebuf = (float)(doublebuf - intbuf *100);
					doublebuf = intbuf + (doublebuf/60);
					info_gprmc->lon = doublebuf;
#ifdef DEBUG
					printf("LON  %f\n",info_gprmc->lon);
#endif
				}
				else
				{
					info_gprmc->lon = 0.0 ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("LON  INVALID\n");
#endif
				}
				break;
			case 6:
				if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
				{
					info_gprmc->ew = buf_extract[0] ;
#ifdef DEBUG
					printf("EW   %c\n",info_gprmc->ew);
#endif
				}
				else
				{
					info_gprmc->ew = '0' ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("EW   INVALID\n");
#endif
				}
				break;
			case 7:
				if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
				{
					info_gprmc->speed = he_a2f(buf_extract);
#ifdef DEBUG
					printf("speed   %f\n",info_gprmc->speed);
#endif
				}
				else
				{
					info_gprmc->speed = 0.0 ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("speed   INVALID\n");
#endif
				}
				break;
			case 8:
				if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
				{
					info_gprmc->direction = he_a2f(buf_extract);
#ifdef DEBUG
					printf("direction   %f\n",info_gprmc->direction);
#endif
				}
				else
				{
					info_gprmc->direction = he_a2f(buf_extract);
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("direction   INVALID\n");
#endif
				}
				break;
			case 9:
				if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
				{
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
				}
				else
				{
					info_gprmc->date.day  = 0;
					info_gprmc->date.mon  = 0;
					info_gprmc->date.year = 0;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("DATE   INVALID\n");
#endif
				}
				break;
			case 10:
				if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
				{
					info_gprmc->declination = he_a2f(buf_extract);
#ifdef DEBUG
					printf("declination   %f\n",info_gprmc->declination);
#endif
				}
				else
				{
					info_gprmc->declination = 0.0 ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("declination   INVAID\n");
#endif
				}
				break;
			case 11:
				if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
				{
					info_gprmc->declin_ew = buf_extract[0] ;
#ifdef DEBUG
					printf("declin_ew   %c\n",info_gprmc->declin_ew);
#endif
				}
				else
				{
					info_gprmc->declin_ew = '0' ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("declin_ew   INVALID\n");
#endif
				}
				break;
			case 12:
				if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
				{
					info_gprmc->mode = buf_extract[0] ;
#ifdef DEBUG
					printf("mode   %c\n",info_gprmc->mode);
#endif
				}
				else
				{
					info_gprmc->mode = '0' ;
					nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
					printf("mode   INVALID\n");
#endif
				}
				break;
			default:
				nmea_vldty = NMEA_INVALID;
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

	return nmea_vldty;
}



/**
 * \brief Extracts nmea_databuf data and fills info_gpvtg structure
 */
int extract_nmeaGPVTG(nmeaGPVTG *info_gpvtg, char *nmea_databuf)
{
	char buf_extract[8] = {0};
	int itr0 = 0;
	int itr1 = 0;
	char swt_case_idx = 0;
	int nmea_vldty = NMEA_VALID;

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

		switch(++swt_case_idx)			//Each case for each element in the buffer
		{
		case 1:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpvtg->dir = he_a2f(buf_extract);
#ifdef DEBUG
				printf("dir   %f\n",info_gpvtg->dir);
#endif
			}
			else
			{
				info_gpvtg->dir = 0.0 ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("dir  INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpvtg->dir_t = buf_extract[0] ;
#ifdef DEBUG
				printf("dir_t   %c\n",info_gpvtg->dir_t);
#endif
			}
			else
			{
				info_gpvtg->dir_t = '0' ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("dir_t  INVALID\n");
#endif
			}
			break;
		case 3:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpvtg->dec = he_a2f(buf_extract);
#ifdef DEBUG
				printf("dec   %f\n",info_gpvtg->dec);
#endif
			}
			else
			{
				info_gpvtg->dec = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("dec  INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpvtg->dec_m = buf_extract[0] ;
#ifdef DEBUG
				printf("dec_m   %c\n",info_gpvtg->dec_m);
#endif
			}
			else
			{
				info_gpvtg->dec_m = '0' ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("dec_m  INVALID\n");
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpvtg->spn = he_a2f(buf_extract);
#ifdef DEBUG
				printf("spn   %f\n",info_gpvtg->spn);
#endif
			}
			else
			{
				info_gpvtg->spn = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("spn  INVALID\n");
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpvtg->spn_n = buf_extract[0] ;
#ifdef DEBUG
				printf("spn_n   %c\n",info_gpvtg->spn_n);
#endif
			}
			else
			{
				info_gpvtg->spn_n = '0' ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("spn_n  INVALID\n");
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				info_gpvtg->spk = he_a2f(buf_extract);
#ifdef DEBUG
				printf("spk   %f\n",info_gpvtg->spk);
#endif
			}
			else
			{
				info_gpvtg->spk = 0.0;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("spk  INVALID\n");
#endif
			}
			break;
		case 8:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				info_gpvtg->spk_k = buf_extract[0] ;
#ifdef DEBUG
				printf("spk_k   %c\n",info_gpvtg->spk_k);
#endif
			}
			else
			{
				info_gpvtg->spk_k = '0' ;
				nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
				printf("spk_k  INVALID\n");
#endif
			}
			break;
		default:
			nmea_vldty = NMEA_INVALID;
#ifdef DEBUG
			printf("\ngpvtg:  NMEA_INVALID\n");
#endif
			break;
		}
	}
	return nmea_vldty;
}

int he_nmea_extract(char *nmea_inputbuf, nmea_info_grp_type *nmea_outputbuf)
{
	int ptype;
	int nmeabuf_sts = NMEA_INVALID;
	printf("WARNING\nLimits of Sting Length is not still measured, TO BE DONE ON HARDWARE CHECKS\n");
	nmeabuf_sts = nmea_canbeparsed(nmea_inputbuf);
	if( NMEA_VALID == nmeabuf_sts)
	{
		if(CHECKSUM_VALID == nmea_checksum(nmea_inputbuf))
		{
			ptype = nmeatype_check(nmea_inputbuf);
			printf("ptype   %d\n",ptype);
			switch (ptype)
			{
			case GPGGA:
				printf("GPGGA");
				nmeabuf_sts = extract_nmeaGPGGA(&nmea_outputbuf->info_gpgga,nmea_inputbuf);
				break;
			case GPGSA:
				printf("GPGSA");
				nmeabuf_sts = extract_nmeaGPGSA(&nmea_outputbuf->info_gpgsa,nmea_inputbuf);
				break;
			case GPGSV:
				printf("GPGSV");
				nmeabuf_sts = extract_nmeaGPGSV(&nmea_outputbuf->info_gpgsv,nmea_inputbuf);
				break;
			case GPRMC:
				printf("GPRMC");
				nmeabuf_sts = extract_nmeaGPRMC(&nmea_outputbuf->info_gprmc,nmea_inputbuf);
				break;
			case GPVTG:
				printf("GPVTG");
				nmeabuf_sts = extract_nmeaGPVTG(&nmea_outputbuf->info_gpvtg,nmea_inputbuf);
				break;
			default:
				nmeabuf_sts = NMEA_INVALID;
				printf("error");
				break;
			}
			fflush(stdout);
		}
		else
		{
			nmeabuf_sts = NMEA_INVALID;
			printf("WRONG INPUT:  %s",nmea_inputbuf);
		}
	}
	else
	{
		nmeabuf_sts = NMEA_INVALID;
	}

	return nmeabuf_sts;
}

int he_nmea_process(nmea_info_grp_type *nmea_data,NMEA_DATA_STR *nmea_data_str)
{
	int nmeabuf_sts = NMEA_INVALID;
	he_f2a(nmea_data->info_gpgga.lat,nmea_data_str->lat,6);
	printf("%lf  lat in string => %s\n BUG IN FLOAT FUNCTION\n ",nmea_data->info_gpgga.lat,nmea_data_str->lat);
	return nmeabuf_sts;
}
