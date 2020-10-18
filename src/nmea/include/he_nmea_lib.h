/*
 ============================================================================
 Name        : he_nmea_lib.h
 Author      : HemanthJsai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Ansi-style
 ============================================================================
 */

#ifndef HE_NMEA_LIB_H_
#define HE_NMEA_LIB_H_

#include "/cygdrive/c/Users/heman/eclipse-workspace/GPS_LIB\src\restructure\Alerts.h"

#define NMEA_INVALID		(0)
#define NMEA_VALID 			(1)

#define NMEA_MIN_COMMA		(5)

#define NMEA_MIN_LEN		(30)
#define NMEA_MAX_LEN		(100)

/*  TOBE UPDATED ONCE I HAVE HARDWARE
#define GGA_MIN_LEN			(63)
#define GGA_MAX_LEN			(82)
#define GSA_MIN_LEN			(51)
#define GSA_MAX_LEN			(63)
#define GSV_MIN_LEN			(60)
#define GSV_MAX_LEN			(70)

#define LAT_LIM_MIN			(-90)
#define LAT_LIM_MAX			(90)
#define LON_LIM_MIN			(-180)
#define LON_LIM_MAX			(180)
 */

#define NMEA_SIG_BAD        (0)
#define NMEA_SIG_LOW        (1)
#define NMEA_SIG_MID        (2)
#define NMEA_SIG_HIGH       (3)

#define NMEA_FIX_BAD        (1)
#define NMEA_FIX_2D         (2)
#define NMEA_FIX_3D         (3)

#define NMEA_MAXSAT         (12)
#define NMEA_SATINPACK      (4)
#define NMEA_NSATPACKS      (NMEA_MAXSAT / NMEA_SATINPACK)

#define CHECKSUM_VALID		(1)
#define CHECKSUM_INVALID 	(0)


/**
 * time data
 */
typedef struct _nmeaTIME
{
	int     hour;       /**< Hours since midnight - [0,23] */
	int     min;        /**< Minutes after the hour - [0,59] */
	int     sec;        /**< Seconds after the minute - [0,59] */
	int     hsec;       /**< Hundredth part of second - [0,99] */

} nmeaTIME;

/**
 * DATE data
 */
typedef struct _nmeaDATE
{
	int     year;       /**< Years since 1900 */
	int     mon;        /**< Months since January - [0,11] */
	int     day;        /**< Day of the month - [1,31] */
} nmeaDATE;


/**
 * Position data in fractional degrees or radians
 */
typedef struct _nmeaPOS
{
	double lat;         /**< Latitude */
	double lon;         /**< Longitude */

} nmeaPOS;

/**
 * Information about satellite
 * @see nmeaSATINFO
 * @see nmeaGPGSV
 */
typedef struct _nmeaSATELLITE
{
	int     id;         /**< Satellite PRN number */
	int     elv;        /**< Elevation in degrees, 90 maximum */
	int     azimuth;    /**< Azimuth, degrees from true north, 000 to 359 */
	int     sig;        /**< Signal, 00-99 dB */

} nmeaSATELLITE;

/**
 * Information about all satellites in view
 * @see nmeaINFO
 * @see nmeaGPGSV
 */
typedef struct _nmeaSATINFO
{
	int     inuse;      /**< Number of satellites in use (not those in view) */
	int     inview;     /**< Total number of satellites in view */
	nmeaSATELLITE sat[NMEA_MAXSAT]; /**< Satellites information */

} nmeaSATINFO;

/**
 * Summary GPS information from all parsed packets,
 * used also for generating NMEA stream
 * @see nmea_parse
 * @see nmea_GPGGA2info,  nmea_...2info
 */
typedef struct _nmeaINFO
{
	int     smask;      /**< Mask specifying types of packages from which data have been obtained */

	nmeaTIME utc;       /**< UTC of position */

	int     sig;        /**< GPS quality indicator (0 = Invalid; 1 = Fix; 2 = Differential, 3 = Sensitive) */
	int     fix;        /**< Operating mode, used for navigation (1 = Fix not available; 2 = 2D; 3 = 3D) */

	double  PDOP;       /**< Position Dilution Of Precision */
	double  HDOP;       /**< Horizontal Dilution Of Precision */
	double  VDOP;       /**< Vertical Dilution Of Precision */

	double  lat;        /**< Latitude in NDEG - +/-[degree][min].[sec/60] */
	double  lon;        /**< Longitude in NDEG - +/-[degree][min].[sec/60] */
	double  elv;        /**< Antenna altitude above/below mean sea level (geoid) in meters */
	double  speed;      /**< Speed over the ground in kilometers/hour */
	double  direction;  /**< Track angle in degrees True */
	double  declination; /**< Magnetic variation degrees (Easterly var. subtracts from true course) */

	nmeaSATINFO satinfo; /**< Satellites information */

} nmeaINFO;


/**
 * NMEA packets type which parsed and generated by library
 */
typedef enum _nmeaPACKTYPE
{
	GPSIZE  = 0x6,		/** Count of Total Elements */
	GPNON   = 0x0000,   /**< Unknown packet type. */
	GPGGA   = 0x0001,   /**< GGA - Essential fix data which provide 3D location and accuracy data. */
	GPGSA   = 0x0002,   /**< GSA - GPS receiver operating mode, SVs used for navigation, and DOP values. */
	GPGSV   = 0x0003,   /**< GSV - Number of SVs in view, PRN numbers, elevation, azimuth & SNR values. */
	GPRMC   = 0x0004,   /**< RMC - Recommended Minimum Specific GPS/TRANSIT Data. */
	GPVTG   = 0x0005    /**< VTG - Actual track made good and speed over ground. */
}nmeaPACKTYPE;

/**
 * Data type of NMEA fields
 */
typedef enum _nmea_datatype
{
	CHAR_VAL = 0x0,
	INT_VAL,
	DOUBLE_VAL
}nmea_datatype;


/**
 * GGA packet information structure (Global Positioning System Fix Data)
 */
typedef struct _nmeaGPGGA
{
	nmeaTIME utc;       /**< UTC of position (just time) */
	double  lat;        /**< Latitude in NDEG - [degree][min].[sec/60] */
	char    ns;         /**< [N]orth or [S]outh */
	double  lon;        /**< Longitude in NDEG - [degree][min].[sec/60] */
	char    ew;         /**< [E]ast or [W]est */
	int     sig;        /**< GPS quality indicator (0 = Invalid; 1 = Fix; 2 = Differential, 3 = Sensitive) */
	int     satinuse;   /**< Number of satellites in use (not those in view) */
	double  HDOP;       /**< Horizontal dilution of precision */
	double  elv;        /**< Antenna altitude above/below mean sea level (geoid) */
	char    elv_units;  /**< [M]eters (Antenna height unit) */
	double  diff;       /**< Geoidal separation (Diff. between WGS-84 earth ellipsoid and mean sea level. '-' = geoid is below WGS-84 ellipsoid) */
	char    diff_units; /**< [M]eters (Units of geoidal separation) */
	double  dgps_age;   /**< Time in seconds since last DGPS update */
	int     dgps_sid;   /**< DGPS station ID number */

} nmeaGPGGA;

/**
 * GSA packet information structure (Satellite status)
 */
typedef struct _nmeaGPGSA
{
	char    fix_mode;   /**< Mode (M = Manual, forced to operate in 2D or 3D; A = Automatic, 3D/2D) */
	int     fix_type;   /**< Type, used for navigation (1 = Fix not available; 2 = 2D; 3 = 3D) */
	int     sat_prn[NMEA_MAXSAT]; /**< PRNs of satellites used in position fix (null for unused fields) */
	double  PDOP;       /**< Dilution of precision */
	double  HDOP;       /**< Horizontal dilution of precision */
	double  VDOP;       /**< Vertical dilution of precision */

} nmeaGPGSA;

/**
 * GSV packet information structure (Satellites in view)
 */
typedef struct _nmeaGPGSV
{
	int     pack_count; /**< Total number of messages of this type in this cycle */
	int     pack_index; /**< Message number */
	int     sat_count;  /**< Total number of satellites in view */
	nmeaSATELLITE sat_data[NMEA_SATINPACK];

} nmeaGPGSV;

/**
 * RMC packet information structure (Recommended Minimum sentence C)
 */
typedef struct _nmeaGPRMC
{
	nmeaTIME utc;       /**< UTC of position */
	char    status;     /**< Status (A = active or V = void) */
	double  lat;        /**< Latitude in NDEG - [degree][min].[sec/60] */
	char    ns;         /**< [N]orth or [S]outh */
	double  lon;        /**< Longitude in NDEG - [degree][min].[sec/60] */
	char    ew;         /**< [E]ast or [W]est */
	double  speed;      /**< Speed over the ground in knots */
	double  direction;  /**< Track angle in degrees True */
	nmeaDATE  date;     /**< UT date */
	double declination; /**< Magnetic variation degrees (Easterly var. subtracts from true course) */
	char   declin_ew;   /**< [E]ast or [W]est */
	char mode;          /**< A=Autonomous, D=DGPS, E=DR */

} nmeaGPRMC;

/**
 * VTG packet information structure (Track made good and ground speed)
 */
typedef struct _nmeaGPVTG
{
	double  dir;        /**< True track made good (degrees) */
	char    dir_t;      /**< Fixed text 'T' indicates that track made good is relative to true north */
	double  dec;        /**< Magnetic track made good */
	char    dec_m;      /**< Fixed text 'M' */
	double  spn;        /**< Ground speed, knots */
	char    spn_n;      /**< Fixed text 'N' indicates that speed over ground is in knots */
	double  spk;        /**< Ground speed, kilometers per hour */
	char    spk_k;      /**< Fixed text 'K' indicates that speed over ground is in kilometers/hour */

} nmeaGPVTG;


typedef struct _nmeaGPGGA_stsbits
{
	unsigned int 	utc			:1;	/**< GPGGA Status BIT*/
	unsigned int	lat			:1;	/**< GPGGA Status BIT*/
	unsigned int	ns			:1;	/**< GPGGA Status BIT*/
	unsigned int	lon			:1;	/**< GPGGA Status BIT*/
	unsigned int	ew			:1;	/**< GPGGA Status BIT*/
	unsigned int	sig			:1;	/**< GPGGA Status BIT*/
	unsigned int	satinuse 	:1;	/**< GPGGA Status BIT*/
	unsigned int	HDOP		:1;	/**< GPGGA Status BIT*/
	unsigned int	elv			:1;	/**< GPGGA Status BIT*/
	unsigned int	elv_units	:1;	/**< GPGGA Status BIT*/
	unsigned int	diff		:1;	/**< GPGGA Status BIT*/
	unsigned int	diff_units	:1;	/**< GPGGA Status BIT*/
	unsigned int	dgps_age	:1;	/**< GPGGA Status BIT*/
	unsigned int	dgps_sid	:1;	/**< GPGGA Status BIT*/
}nmeaGPGGA_stsbits_type;

typedef union _nmeaGPGGA_sts
{
	unsigned int U;				/**< GPGGA Status*/
	nmeaGPGGA_stsbits_type B;	/**< GPGGA Status bits*/
}nmeaGPGGA_sts_type;

typedef struct _nmeaGPGSA_stsbits
{
	unsigned int	fix_mode	:1;	/**< GPGSA Status BIT*/
	unsigned int	fix_type	:1;	/**< GPGSA Status BIT*/
	unsigned int	sat_prn		:1;	/**< GPGSA Status BIT*/
	unsigned int	PDOP		:1;	/**< GPGSA Status BIT*/
	unsigned int	HDOP		:1;	/**< GPGSA Status BIT*/
	unsigned int	VDOP		:1;	/**< GPGSA Status BIT*/
}nmeaGPGSA_stsbits_type;

typedef union _nmeaGPGSA_sts
{
	unsigned int U;				/**< GPGSA Status*/
	nmeaGPGSA_stsbits_type B;	/**< GPGSA Status bits*/
}nmeaGPGSA_sts_type;

typedef struct _nmeaGPGSV_stsbits
{
	unsigned int	pack_count	:1;	/**< GPGSV Status BIT*/
	unsigned int	pack_index	:1;	/**< GPGSV Status BIT*/
	unsigned int	sat_count	:1;	/**< GPGSV Status BIT*/
	unsigned int	sat_data	:1;	/**< GPGSV Status BIT*/
}nmeaGPGSV_stsbits_type;

typedef union _nmeaGPGSV_sts
{
	unsigned int U;				/**< GPGSV Status*/
	nmeaGPGSV_stsbits_type B;	/**< GPGSV Status bits*/
}nmeaGPGSV_sts_type;

typedef struct _nmeaGPRMC_stsbits
{
	unsigned int	utc			:1;	/**< GPRMC Status BIT*/
	unsigned int	status		:1;	/**< GPRMC Status BIT*/
	unsigned int	lat			:1;	/**< GPRMC Status BIT*/
	unsigned int	ns			:1;	/**< GPRMC Status BIT*/
	unsigned int	lon			:1;	/**< GPRMC Status BIT*/
	unsigned int	ew			:1;	/**< GPRMC Status BIT*/
	unsigned int	speed		:1;	/**< GPRMC Status BIT*/
	unsigned int	direction	:1;	/**< GPRMC Status BIT*/
	unsigned int	date		:1;	/**< GPRMC Status BIT*/
	unsigned int	declination	:1;	/**< GPRMC Status BIT*/
	unsigned int	declin_ew	:1;	/**< GPRMC Status BIT*/
	unsigned int	mode		:1;	/**< GPRMC Status BIT*/
}nmeaGPRMC_stsbits_type;

typedef union _nmeaGPRMC_sts
{
	unsigned int U;				/**< GPRMC Status*/
	nmeaGPRMC_stsbits_type B;	/**< GPRMC Status bits*/
}nmeaGPRMC_sts_type;

typedef struct _nmeaGPVTG_stsbits
{
	unsigned int	dir		:1;	/**< GPVTG Status BIT*/
	unsigned int	dir_t	:1;	/**< GPVTG Status BIT*/
	unsigned int	dec		:1;	/**< GPVTG Status BIT*/
	unsigned int	dec_m	:1;	/**< GPVTG Status BIT*/
	unsigned int	spn		:1;	/**< GPVTG Status BIT*/
	unsigned int	spn_n	:1;	/**< GPVTG Status BIT*/
	unsigned int	spk		:1;	/**< GPVTG Status BIT*/
	unsigned int	spk_k	:1;	/**< GPVTG Status BIT*/
}nmeaGPVTG_stsbits_type;

typedef union _nmeaGPVTG_sts
{
	unsigned int U;				/**< GPVTG Status*/
	nmeaGPVTG_stsbits_type B;	/**< GPVTG Status bits*/
}nmeaGPVTG_sts_type;

typedef struct _nmea_info_grp
{
	nmeaGPGGA info_gpgga;			/**< Size: 112 bytes*/
	nmeaGPGGA_sts_type sts_gpgga;	/**< Size: 4 bytes*/

	nmeaGPGSA info_gpgsa;			/**< Size: 80  bytes*/
	nmeaGPGSA_sts_type sts_gpgsa;	/**< Size: 4 bytes*/

	nmeaGPGSV info_gpgsv;			/**< Size: 76  bytes*/
	nmeaGPGSV_sts_type sts_gpgsv;	/**< Size: 4 bytes*/

	nmeaGPRMC info_gprmc;			/**< Size: 104 bytes*/
	nmeaGPRMC_sts_type sts_gprmc;	/**< Size: 4 bytes*/

	nmeaGPVTG info_gpvtg;			/**< Size: 64  bytes*/
	nmeaGPVTG_sts_type sts_gpvtg;	/**< Size: 4 bytes*/

}nmea_info_grp_type;			/**< Total Size:  bytes*/


/*GLOBAL NMEA BUFFERS DECLERATION*/
extern nmea_info_grp_type	nmea_data;
extern NMEA_DATA_STR		nmea_data_str;


int  nmea_typecheck						(char * nmea_databuf   );
int  nmea_FindEndMarker_validity		(char * nmea_inputbuf  );
extern int 	nmea_checksum				(char * nmea_databuf,     int   nmea_endmarker);
extern int  extract_nmeaGPGGA			(char * nmea_databuf);
extern int  extract_nmeaGPGSA			(char * nmea_databuf);
extern int  extract_nmeaGPGSV			(char * nmea_databuf);
extern int  extract_nmeaGPRMC			(char * nmea_databuf);
extern int  extract_nmeaGPVTG			(char * nmea_databuf);
extern int  nmea_datatype_isfieldvalid	(char 	   * buf,         nmea_datatype check);

extern int 	he_nmea_extract				(char *nmea_inputbuf,     		nmea_info_grp_type *nmea_outputbuf);
extern int  he_nmea_process				(nmea_info_grp_type *nmea_data, NMEA_DATA_STR 	   *nmea_data_str);

extern  int nmea_main(void);

#endif /* HE_NMEA_LIB_H_ */
