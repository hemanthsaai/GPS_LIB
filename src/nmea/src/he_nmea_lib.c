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
#ifdef DEBUG
#include <stdio.h>
#endif

/*GLOBAL NMEA BUFFERS*/
nmea_info_grp_type 	nmea_data;
NMEA_DATA_STR		nmea_data_str;

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
 *  GPGGA/GSA/GSV/RMC/VTG
 * @return Received packet type
 * @see nmeaPACKTYPE
 */

int nmea_typecheck(char *nmea_databuf)
{
	int nmeatype_id =  GPNON;
	int itr, status;
	char * nmea_packname[] = {
			"GPGGA",
			"GPGSA",
			"GPGSV",
			"GPRMC",
			"GPVTG"
	};

	/* Iterate through the predefined nmea_packname and check for Matching packtype */
	for(itr = 0; itr < GPSIZE-1; itr++)
	{
		status = he_strcmp_len(&nmea_databuf[1],nmea_packname[itr],5);
		if(status)
		{
			/* Found the packet type, update the id and break the loop */
			nmeatype_id = itr+1;
			break;
		}
		else
		{
			/* Not Matched, Continue to next */
		}

	}

	return nmeatype_id;
}


/**
 * \brief Check the checksum of received packet
 *  Performs Software Calculation of Checksum.
 *  XOR of entire received buffer compared to received checksum
 *  which is at end of the same buffer
 *
 * @return CHECKSUM_VALID if checksum is valid else CHECKSUM_INVALID
 * @see checksum_status
 */
int nmea_checksum(char *nmea_databuf, int nmea_endmarker)
{
	int itr = 1;  /* 1 bcz Skip the $ at start of buffer*/
	int checksum_received = 0;
	int checksum_calculated = 0;
	char checksum_ascii[3] = {0};
	int checksum_status = CHECKSUM_INVALID;

	if (NMEA_INVALID != nmea_endmarker)
	{
		/*Loop till end of buffer and calculate the checksum*/
		while( itr < nmea_endmarker - 1 )
		{
			checksum_calculated ^= nmea_databuf[itr];
			itr = itr + 1;
		}
		/*Extract the received checksum at end of buffer and convert it to hex*/
		checksum_ascii[0] = nmea_databuf[itr+1];
		checksum_ascii[1] = nmea_databuf[itr+2];
		checksum_received  = he_a2hex(checksum_ascii);

		if(checksum_received == checksum_calculated)
		{
			/*Valid Checksum is received, update status to valid*/
#ifdef DEBUG
			printf("FUNC--nmea_checksum\nreceived checksum %x    calculated checksum %x\n",checksum_received,checksum_calculated);
			printf("CHECKSUM Match\n");
#endif
			checksum_status = CHECKSUM_VALID;
		}
		else
		{
#ifdef DEBUG
			printf("FUNC--nmea_checksum\nreceived checksum %x    calculated checksum %x\n",checksum_received,checksum_calculated);
			printf("CHECKSUM MISMATCH\n");
#endif
		}
	}
	else
	{
#ifdef DEBUG
		printf("FUNC--nmea_checksum\nInvalid EndMarker received\n");
#endif
	}

	return checksum_status;
}


/**
 * \brief Extracts nmea_databuf data and fills info_gpgga structure
 */
int extract_nmeaGPGGA(char *nmea_databuf)
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
				nmea_data.info_gpgga.utc.hour = he_a2i(charbuf);
				he_strcpy(&buf_extract[2],charbuf,2);
				nmea_data.info_gpgga.utc.min = he_a2i(charbuf);
				he_strcpy(&buf_extract[4],charbuf,2);
				nmea_data.info_gpgga.utc.sec = he_a2i(charbuf);

				/*Update Status*/
				nmea_data.sts_gpgga.B.utc = 1;

#ifdef DEBUG
				printf("UTC    hh  %d  mm %d  ss %d\n",nmea_data.info_gpgga.utc.hour, nmea_data.info_gpgga.utc.min, nmea_data.info_gpgga.utc.sec);
#endif
			}
			else
			{
				nmea_data.info_gpgga.utc.hour 	= 0;
				nmea_data.info_gpgga.utc.min  	= 0;
				nmea_data.info_gpgga.utc.sec	= 0;
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
				nmea_data.info_gpgga.lat = doublebuf;

				/*Update Status*/
				nmea_data.sts_gpgga.B.lat = 1;

#ifdef DEBUG
				printf("LAT  %f\n",nmea_data.info_gpgga.lat);
#endif

			}
			else
			{
				nmea_data.info_gpgga.lat = 0.0;
#ifdef DEBUG
				printf("LAT  INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpgga.ns = buf_extract[0] ;

				/*Update Status*/
				nmea_data.sts_gpgga.B.ns = 1;

#ifdef DEBUG
				printf("NS   %c\n",nmea_data.info_gpgga.ns);
#endif
			}
			else
			{
				nmea_data.info_gpgga.ns = '0';
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
				nmea_data.info_gpgga.lon = doublebuf;

				/*Update Status*/
				nmea_data.sts_gpgga.B.lon = 1;

#ifdef DEBUG
				printf("LON  %f\n",nmea_data.info_gpgga.lon);
#endif
			}
			else
			{
				nmea_data.info_gpgga.lon = 0.0;
#ifdef DEBUG
				printf("LON  INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpgga.ew = buf_extract[0] ;

				/*Update Status*/
				nmea_data.sts_gpgga.B.ew = 1;

#ifdef DEBUG
				printf("EW   %c\n",nmea_data.info_gpgga.ew);
#endif
			}
			else
			{
				nmea_data.info_gpgga.ew = '0';
#ifdef DEBUG
				printf("EW  INVALID\n");
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgga.sig = he_a2i(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgga.B.sig = 1;

#ifdef DEBUG
				printf("SIG  %d\n",nmea_data.info_gpgga.sig);
#endif
			}
			else
			{
				nmea_data.info_gpgga.sig = 0;
#ifdef DEBUG
				printf("SIG  INVALID\n");
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgga.satinuse = he_a2i(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgga.B.satinuse = 1;

#ifdef DEBUG
				printf("satinuse  %d\n",nmea_data.info_gpgga.satinuse);
#endif
			}
			else
			{
				nmea_data.info_gpgga.satinuse = 0;
#ifdef DEBUG
				printf("satinuse  INVALID\n");
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpgga.HDOP = he_a2f(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgga.B.HDOP = 1;

#ifdef DEBUG
				printf("HDOP   %f\n",nmea_data.info_gpgga.HDOP);
#endif
			}
			else
			{
				nmea_data.info_gpgga.HDOP = 0.0;
#ifdef DEBUG
				printf("HDOP  INVALID\n");
#endif
			}
			break;
		case 8:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpgga.elv = he_a2f(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgga.B.elv = 1;

#ifdef DEBUG
				printf("elv   %f\n",nmea_data.info_gpgga.elv);
#endif
			}
			else
			{
				nmea_data.info_gpgga.elv = 0.0;
#ifdef DEBUG
				printf("elv  INVALID\n");
#endif
			}
			break;
		case 9:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpgga.elv_units = buf_extract[0] ;

				/*Update Status*/
				nmea_data.sts_gpgga.B.elv_units = 1;

#ifdef DEBUG
				printf("elv_units   %c\n",nmea_data.info_gpgga.elv_units);
#endif
			}
			else
			{
				nmea_data.info_gpgga.elv_units = '0';
#ifdef DEBUG
				printf("elv_units  INVALID\n");
#endif
			}
			break;
		case 10:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpgga.diff = he_a2f(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgga.B.diff = 1;

#ifdef DEBUG
				printf("diff   %f\n",nmea_data.info_gpgga.diff);
#endif
			}
			else
			{
				nmea_data.info_gpgga.diff = 0.0;
#ifdef DEBUG
				printf("diff  INVALID\n");
#endif
			}
			break;
		case 11:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpgga.diff_units = buf_extract[0] ;

				/*Update Status*/
				nmea_data.sts_gpgga.B.diff_units = 1;

#ifdef DEBUG
				printf("diff_units   %c\n",nmea_data.info_gpgga.diff_units);
#endif
			}
			else
			{
				nmea_data.info_gpgga.diff_units = '0';
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
	}
#ifdef DEBUG
		printf(" \n");
		printf("Status:  %d\n",nmea_data.sts_gpgga.U);
		fflush(stdout);
#endif
	return nmea_vldty;
}

/**
 * \brief Extracts nmea_databuf data and fills info_gpgsa structure
 */
int extract_nmeaGPGSA(char *nmea_databuf)
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
				nmea_data.info_gpgsa.fix_mode = buf_extract[0] ;

				/*Update Status*/
				nmea_data.sts_gpgsa.B.fix_mode = 1;

#ifdef DEBUG
				printf("fix_mode   %c\n",nmea_data.info_gpgsa.fix_mode);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.fix_mode = '0' ;

#ifdef DEBUG
				printf("fix_mode   INVALID\n");
#endif
			}
			break;
		case 1:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.fix_type = he_a2i(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgsa.B.fix_type = 1;

#ifdef DEBUG
				printf("fix_type  %d\n",nmea_data.info_gpgsa.fix_type);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.fix_type = 0 ;

#ifdef DEBUG
				printf("fix_type  INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[0] = he_a2i(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;

#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[0]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[0] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 3:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[1] = he_a2i(buf_extract);

				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[1]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[1] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[2] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[2]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[2] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[3] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[3]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[3] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[4] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[4]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[4] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[5] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[5]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[5] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 8:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[6] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[6]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[6] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 9:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[7] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[7]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[7] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 10:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[8] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[8]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[8] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 11:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[9] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[9]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[9] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 12:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[10] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[10]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[10] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 13:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsa.sat_prn[11] = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.sat_prn = 1;
#ifdef DEBUG
				printf("sat_prn  %d\n",nmea_data.info_gpgsa.sat_prn[11]);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.sat_prn[11] = 0 ;

#ifdef DEBUG
				printf("sat_prn  INVALID\n");
#endif
			}
			break;
		case 14:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpgsa.PDOP = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.PDOP = 1;
#ifdef DEBUG
				printf("PDOP   %f\n",nmea_data.info_gpgsa.PDOP);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.PDOP = 0.0 ;

#ifdef DEBUG
				printf("PDOP  INVALID\n");
#endif
			}
			break;
		case 15:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpgsa.HDOP = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.HDOP = 1;
#ifdef DEBUG
				printf("HDOP   %f\n",nmea_data.info_gpgsa.HDOP);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.HDOP = 0.0 ;

#ifdef DEBUG
				printf("HDOP  INVALID\n");
#endif
			}
			break;
		case 16:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpgsa.VDOP = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsa.B.VDOP = 1;
#ifdef DEBUG
				printf("VDOP   %f\n",nmea_data.info_gpgsa.VDOP);
#endif
			}
			else
			{
				nmea_data.info_gpgsa.VDOP = 0.0 ;

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
	}
#ifdef DEBUG
		printf("Status:  %d\n",nmea_data.sts_gpgsa.U);
		fflush(stdout);
#endif
	return nmea_vldty;
}


/**
 * \brief Extracts nmea_databuf data and fills info_gpgsv structure
 */
int extract_nmeaGPGSV(char *nmea_databuf)
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
				nmea_data.info_gpgsv.pack_count = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsv.B.pack_count = 1;
#ifdef DEBUG
				printf("pack_count  %d\n",nmea_data.info_gpgsv.pack_count);
#endif
			}
			else
			{
				nmea_data.info_gpgsv.pack_count = 0 ;

#ifdef DEBUG
				printf("pack_count INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsv.pack_index = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsv.B.pack_index = 1;
#ifdef DEBUG
				printf("pack_index  %d\n",nmea_data.info_gpgsv.pack_index);
#endif
			}
			else
			{
				nmea_data.info_gpgsv.pack_index = 0 ;

#ifdef DEBUG
				printf("pack_index INVALID\n");
#endif
			}
			break;
		case 3:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsv.sat_count = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsv.B.sat_count = 1;
#ifdef DEBUG
				printf("sat_count  %d\n",nmea_data.info_gpgsv.sat_count);
#endif
			}
			else
			{
				nmea_data.info_gpgsv.sat_count = 0 ;

#ifdef DEBUG
				printf("sat_count INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].id = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsv.B.sat_data = 1;
#ifdef DEBUG
				printf("sat_data[%d].id  %d\n",sat_data_idx,nmea_data.info_gpgsv.sat_data[sat_data_idx].id);
#endif
			}
			else
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].id = 0 ;

#ifdef DEBUG
				printf("sat_data[%d].id  INVALID\n",sat_data_idx);
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].elv = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsv.B.sat_data = 1;
#ifdef DEBUG
				printf("sat_data[%d].elv  %d\n",sat_data_idx,nmea_data.info_gpgsv.sat_data[sat_data_idx].elv);
#endif
			}
			else
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].elv = 0 ;

#ifdef DEBUG
				printf("sat_data[%d].elv  INVALID\n",sat_data_idx);
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].azimuth = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsv.B.sat_data = 1;
#ifdef DEBUG
				printf("sat_data[%d].azimuth  %d\n",sat_data_idx,nmea_data.info_gpgsv.sat_data[sat_data_idx].azimuth);
#endif
			}
			else
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].azimuth = 0 ;

#ifdef DEBUG
				printf("sat_data[%d].azimuth  INVALID\n",sat_data_idx);
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].sig = he_a2i(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpgsv.B.sat_data = 1;
#ifdef DEBUG
				printf("sat_data[%d].sig  %d\n",sat_data_idx,nmea_data.info_gpgsv.sat_data[sat_data_idx].sig);
#endif
			}
			else
			{
				nmea_data.info_gpgsv.sat_data[sat_data_idx].sig = 0 ;

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
	printf("Status:  %d\n",nmea_data.sts_gpgsv.U);
	printf("GPGSV ERROR:  IMPLEMENTATION PENDING \nGPGSV has multiple packets this function handles only one packet\n");
	fflush(stdout);
#endif
	return nmea_vldty;
}


/**
 * \brief Extracts nmea_databuf data and fills info_gprmc structure
 */
int extract_nmeaGPRMC(char *nmea_databuf)
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
				nmea_data.info_gprmc.utc.hour = he_a2i(charbuf);
				he_strcpy(&buf_extract[2],charbuf,2);
				nmea_data.info_gprmc.utc.min = he_a2i(charbuf);
				he_strcpy(&buf_extract[4],charbuf,2);
				nmea_data.info_gprmc.utc.sec = he_a2i(charbuf);

				/*Update Status*/
				nmea_data.sts_gprmc.B.utc = 1;
#ifdef DEBUG
				printf("\nUTC    hh  %d  mm %d  ss %d\n",nmea_data.info_gprmc.utc.hour, nmea_data.info_gprmc.utc.min, nmea_data.info_gprmc.utc.sec);
#endif
			}
			else
			{
				nmea_data.info_gprmc.utc.hour = 0;
				nmea_data.info_gprmc.utc.min  = 0;
				nmea_data.info_gprmc.utc.sec  = 0;

#ifdef DEBUG
				printf("UTC    INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gprmc.status = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gprmc.B.status = 1;
#ifdef DEBUG
				printf("status   %c\n",nmea_data.info_gprmc.status);
#endif
			}
			else
			{
				nmea_data.info_gprmc.status = '0' ;

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
				nmea_data.info_gprmc.lat = doublebuf;
				/*Update Status*/
				nmea_data.sts_gprmc.B.lat = 1;
#ifdef DEBUG
				printf("LAT  %f\n",nmea_data.info_gprmc.lat);
#endif
			}
			else
			{
				nmea_data.info_gprmc.lat = 0.0 ;

#ifdef DEBUG
				printf("LAT   INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gprmc.ns = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gprmc.B.ns = 1;
#ifdef DEBUG
				printf("NS   %c\n",nmea_data.info_gprmc.ns);
#endif
			}
			else
			{
				nmea_data.info_gprmc.ns = '0' ;

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
				nmea_data.info_gprmc.lon = doublebuf;
				/*Update Status*/
				nmea_data.sts_gprmc.B.lon = 1;
#ifdef DEBUG
				printf("LON  %f\n",nmea_data.info_gprmc.lon);
#endif
			}
			else
			{
				nmea_data.info_gprmc.lon = 0.0 ;

#ifdef DEBUG
				printf("LON  INVALID\n");
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gprmc.ew = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gprmc.B.ew = 1;
#ifdef DEBUG
				printf("EW   %c\n",nmea_data.info_gprmc.ew);
#endif
			}
			else
			{
				nmea_data.info_gprmc.ew = '0' ;

#ifdef DEBUG
				printf("EW   INVALID\n");
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gprmc.speed = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gprmc.B.speed = 1;
#ifdef DEBUG
				printf("speed   %f\n",nmea_data.info_gprmc.speed);
#endif
			}
			else
			{
				nmea_data.info_gprmc.speed = 0.0 ;

#ifdef DEBUG
				printf("speed   INVALID\n");
#endif
			}
			break;
		case 8:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gprmc.direction = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gprmc.B.direction = 1;
#ifdef DEBUG
				printf("direction   %f\n",nmea_data.info_gprmc.direction);
#endif
			}
			else
			{
				nmea_data.info_gprmc.direction = he_a2f(buf_extract);

#ifdef DEBUG
				printf("direction   INVALID\n");
#endif
			}
			break;
		case 9:
			if(nmea_datatype_isfieldvalid(buf_extract,INT_VAL))
			{
				he_strcpy(buf_extract,charbuf,2);
				nmea_data.info_gprmc.date.day = he_a2i(charbuf);
				he_strcpy(&buf_extract[2],charbuf,2);
				nmea_data.info_gprmc.date.mon = he_a2i(charbuf);
				he_strcpy(&buf_extract[4],charbuf,2);
				nmea_data.info_gprmc.date.year = he_a2i(charbuf);
				nmea_data.info_gprmc.date.year = nmea_data.info_gprmc.date.year + 2000; // Year to be represented in YYYY hence adding 2000

				/*Update Status*/
				nmea_data.sts_gprmc.B.date = 1;

#ifdef DEBUG
				printf("DATE    dd  %d  mm %d  yy %d\n",nmea_data.info_gprmc.date.day, nmea_data.info_gprmc.date.mon, nmea_data.info_gprmc.date.year);
#endif
			}
			else
			{
				nmea_data.info_gprmc.date.day  = 0;
				nmea_data.info_gprmc.date.mon  = 0;
				nmea_data.info_gprmc.date.year = 0;

#ifdef DEBUG
				printf("DATE   INVALID\n");
#endif
			}
			break;
		case 10:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gprmc.declination = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gprmc.B.declination = 1;
#ifdef DEBUG
				printf("declination   %f\n",nmea_data.info_gprmc.declination);
#endif
			}
			else
			{
				nmea_data.info_gprmc.declination = 0.0 ;

#ifdef DEBUG
				printf("declination   INVAID\n");
#endif
			}
			break;
		case 11:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gprmc.declin_ew = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gprmc.B.declin_ew = 1;
#ifdef DEBUG
				printf("declin_ew   %c\n",nmea_data.info_gprmc.declin_ew);
#endif
			}
			else
			{
				nmea_data.info_gprmc.declin_ew = '0' ;

#ifdef DEBUG
				printf("declin_ew   INVALID\n");
#endif
			}
			break;
		case 12:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gprmc.mode = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gprmc.B.mode = 1;
#ifdef DEBUG
				printf("mode   %c\n",nmea_data.info_gprmc.mode);
#endif
			}
			else
			{
				nmea_data.info_gprmc.mode = '0' ;

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
	}
#ifdef DEBUG
	printf("Status:  %d\n",nmea_data.sts_gprmc.U);
	fflush(stdout);
#endif

	return nmea_vldty;
}



/**
 * \brief Extracts nmea_databuf data and fills info_gpvtg structure
 */
int extract_nmeaGPVTG(char *nmea_databuf)
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
				nmea_data.info_gpvtg.dir = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpvtg.B.dir = 1;
#ifdef DEBUG
				printf("dir   %f\n",nmea_data.info_gpvtg.dir);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.dir = 0.0 ;

#ifdef DEBUG
				printf("dir  INVALID\n");
#endif
			}
			break;
		case 2:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpvtg.dir_t = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gpvtg.B.dir_t = 1;
#ifdef DEBUG
				printf("dir_t   %c\n",nmea_data.info_gpvtg.dir_t);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.dir_t = '0' ;
#ifdef DEBUG
				printf("dir_t  INVALID\n");
#endif
			}
			break;
		case 3:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpvtg.dec = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpvtg.B.dec = 1;
#ifdef DEBUG
				printf("dec   %f\n",nmea_data.info_gpvtg.dec);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.dec = 0.0;
#ifdef DEBUG
				printf("dec  INVALID\n");
#endif
			}
			break;
		case 4:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpvtg.dec_m = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gpvtg.B.dec_m = 1;
#ifdef DEBUG
				printf("dec_m   %c\n",nmea_data.info_gpvtg.dec_m);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.dec_m = '0' ;
#ifdef DEBUG
				printf("dec_m  INVALID\n");
#endif
			}
			break;
		case 5:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpvtg.spn = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpvtg.B.spn = 1;
#ifdef DEBUG
				printf("spn   %f\n",nmea_data.info_gpvtg.spn);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.spn = 0.0;
#ifdef DEBUG
				printf("spn  INVALID\n");
#endif
			}
			break;
		case 6:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpvtg.spn_n = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gpvtg.B.spn_n = 1;
#ifdef DEBUG
				printf("spn_n   %c\n",nmea_data.info_gpvtg.spn_n);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.spn_n = '0' ;
#ifdef DEBUG
				printf("spn_n  INVALID\n");
#endif
			}
			break;
		case 7:
			if(nmea_datatype_isfieldvalid(buf_extract,DOUBLE_VAL))
			{
				nmea_data.info_gpvtg.spk = he_a2f(buf_extract);
				/*Update Status*/
				nmea_data.sts_gpvtg.B.spk = 1;
#ifdef DEBUG
				printf("spk   %f\n",nmea_data.info_gpvtg.spk);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.spk = 0.0;
#ifdef DEBUG
				printf("spk  INVALID\n");
#endif
			}
			break;
		case 8:
			if(nmea_datatype_isfieldvalid(buf_extract,CHAR_VAL))
			{
				nmea_data.info_gpvtg.spk_k = buf_extract[0] ;
				/*Update Status*/
				nmea_data.sts_gpvtg.B.spk_k = 1;
#ifdef DEBUG
				printf("spk_k   %c\n",nmea_data.info_gpvtg.spk_k);
#endif
			}
			else
			{
				nmea_data.info_gpvtg.spk_k = '0' ;
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
#ifdef DEBUG
			printf("Status:  %d\n",nmea_data.sts_gpvtg.U);
			fflush(stdout);
#endif
	return nmea_vldty;
}

/**
 * \brief Find the valid end of the buffer which is *,
 *  else return a dirty marker indicating the received buffer is invalid
 * @return nmea_endmarker
 */
int nmea_FindEndMarker_validity(char *nmea_inputbuf)
{
	int nmea_endmarker = NMEA_INVALID;
	int itr = 0;

	if ( '$' == nmea_inputbuf[itr] )
	{
		/*Loop until the NMEA max limit set by Software or till end of string \0 */
		while( (itr <= NMEA_MAX_LEN) && (nmea_inputbuf[itr] != '\0') )
		{
			/*when we get * in the buffer indicating valid buffer and end of string */
			if (nmea_inputbuf[itr] == '*')
			{
				nmea_endmarker = itr + 1;   /* Put +1 as in upcoming functions check until < nmea_endmarker  and not <= nmea_endmarker */
				break;
			}
			else
			{
				/*Nothing valid found, just continue the loop*/
				itr++;
			}
		}

		if (nmea_endmarker)
		{
#ifdef DEBUG
			printf("NMEA is Valid and can be parsed\n EndMarker set at : %d\n",nmea_endmarker);
#endif
		}
		else
		{
#ifdef DEBUG
			printf("NMEA is InValid and cannot be parsed\n");
#endif
		}
	}
	else
	{
#ifdef DEBUG
		printf("NMEA is InValid and cannot be parsed\n");
#endif
		/* Invalid Data buffer, Do Nothing*/
	}

	return nmea_endmarker;
}

int he_nmea_extract(char *nmea_inputbuf, nmea_info_grp_type *nmea_outputbuf)
{
	int ptype;
	int nmea_endmarker = NMEA_INVALID;
	int nmeabuf_sts    = NMEA_INVALID;
#ifdef DEBUG
	printf("WARNING\nLimits of Sting Length is not still measured, TO BE DONE ON HARDWARE CHECKS\n");
#endif
	/*Check if the buffer is valid, get the valid end marker*/
	nmea_endmarker = nmea_FindEndMarker_validity(nmea_inputbuf);

	if(NMEA_INVALID != nmea_checksum(nmea_inputbuf, nmea_endmarker))
	{
		ptype = nmea_typecheck(nmea_inputbuf);
#ifdef DEBUG
		printf("ptype   %d\n",ptype);
#endif
		switch (ptype)
		{
		case GPGGA:
#ifdef DEBUG
			printf("GPGGA\n");
#endif
			nmeabuf_sts = extract_nmeaGPGGA(nmea_inputbuf);
			break;
		case GPGSA:
#ifdef DEBUG
			printf("GPGSA\n");
#endif
			nmeabuf_sts = extract_nmeaGPGSA(nmea_inputbuf);
			break;
		case GPGSV:
#ifdef DEBUG
			printf("GPGSV\n");
#endif
			nmeabuf_sts = extract_nmeaGPGSV(nmea_inputbuf);
			break;
		case GPRMC:
#ifdef DEBUG
			printf("GPRMC\n");
#endif
			nmeabuf_sts = extract_nmeaGPRMC(nmea_inputbuf);
			break;
		case GPVTG:
#ifdef DEBUG
			printf("GPVTG\n");
#endif
			nmeabuf_sts = extract_nmeaGPVTG(nmea_inputbuf);
			break;
		default:
			nmeabuf_sts = NMEA_INVALID;
#ifdef DEBUG
			printf("ERROR: Undefined NMEA string Received\n");
#endif
			break;
		}
#ifdef DEBUG
		fflush(stdout);
#endif
	}
	else
	{
		nmeabuf_sts = NMEA_INVALID;
#ifdef DEBUG
		printf("WRONG INPUT:  %s\n",nmea_inputbuf);
#endif
	}

	return nmeabuf_sts;
}

int he_nmea_process(nmea_info_grp_type *nmea_data,NMEA_DATA_STR *nmea_data_str)
{
	int nmeabuf_sts = NMEA_INVALID;

	he_i2a(nmea_data->info_gpgga.sig,nmea_data_str->gps_vldty);
	//DATE
	if(nmea_data->info_gprmc.date.day <= 9)
	{
		he_i2a(nmea_data->info_gprmc.date.day,&nmea_data_str->date[1]);
		nmea_data_str->date[0] = 48;
	}
	else
	{
		he_i2a(nmea_data->info_gprmc.date.day,nmea_data_str->date);
	}

	if(nmea_data->info_gprmc.date.mon <= 9)
	{
		he_i2a(nmea_data->info_gprmc.date.mon,&nmea_data_str->date[3]);
		nmea_data_str->date[2] = 48;
	}
	else
	{
		he_i2a(nmea_data->info_gprmc.date.mon,&nmea_data_str->date[2]);
	}
	he_i2a(nmea_data->info_gprmc.date.year,&nmea_data_str->date[4]);
	// TIME
	if(nmea_data->info_gprmc.utc.hour <= 9)
	{
		he_i2a(nmea_data->info_gprmc.utc.hour,&nmea_data_str->time[1]);
		nmea_data_str->time[0] = 48;
	}
	else
	{
		he_i2a(nmea_data->info_gprmc.utc.hour,nmea_data_str->time);
	}
	if(nmea_data->info_gprmc.utc.min <= 9)
	{
		he_i2a(nmea_data->info_gprmc.utc.min,&nmea_data_str->time[3]);
		nmea_data_str->time[2] = 48;
	}
	else
	{
		he_i2a(nmea_data->info_gprmc.utc.min,&nmea_data_str->time[2]);
	}
	if(nmea_data->info_gprmc.utc.sec <= 9)
	{
		he_i2a(nmea_data->info_gprmc.utc.sec,&nmea_data_str->time[5]);
		nmea_data_str->time[4] = 48;
	}
	else
	{
		he_i2a(nmea_data->info_gprmc.utc.sec,&nmea_data_str->time[4]);
	}

	//LAT and LONG
	he_f2a(nmea_data->info_gpgga.lat,nmea_data_str->lat,6);
	nmea_data_str->lat_dir = nmea_data->info_gpgga.ns ;
	he_f2a(nmea_data->info_gpgga.lon,nmea_data_str->lon,6);
	nmea_data_str->lon_dir = nmea_data->info_gpgga.ew ;
	//SPEED
	he_f2a(nmea_data->info_gpvtg.spk,nmea_data_str->speed,3);

	//SAT_COUNT
	if(nmea_data->info_gpgsv.sat_count <= 9U)
	{
		he_i2a(nmea_data->info_gpgsv.sat_count,&nmea_data_str->num_sat_fix[1]);
		nmea_data_str->num_sat_fix[0] = 48;
	}
	else
	{
		he_i2a(nmea_data->info_gpgsv.sat_count,nmea_data_str->num_sat_fix);
	}

	//ALTITUDE
	he_f2a(nmea_data->info_gpgga.elv,nmea_data_str->altitude,3);

#ifdef DEBUG
	printf("%s  GPS VALIDITY\n",nmea_data_str->gps_vldty);
	printf("%s Date\n",nmea_data_str->date);
	printf("%s Time\n",nmea_data_str->time);
	printf("%lf  lat in string => %s %c\n BUG IN FLOAT FUNCTION\n",nmea_data->info_gpgga.lat,nmea_data_str->lat,nmea_data_str->lat_dir);
	printf("%lf  lon in string => %s %c\n BUG IN FLOAT FUNCTION\n",nmea_data->info_gpgga.lon,nmea_data_str->lon,nmea_data_str->lon_dir);
	printf("%lf  speed in string => %s \n BUG IN FLOAT FUNCTION\n",nmea_data->info_gpvtg.spk,nmea_data_str->speed);
	printf("%s SAT_COUNT\n",nmea_data_str->num_sat_fix);
	printf("%s ALTITUDE\n",nmea_data_str->altitude);
#endif
	return nmeabuf_sts;

}
