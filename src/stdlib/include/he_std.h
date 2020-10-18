/*
 ============================================================================
 Name        : gps_lib_sentence.h
 Author      : HemanthJsai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Ansi-style
 ============================================================================
 */


#ifndef HE_STD_H_
#define HE_STD_H_

#include <stdio.h>

#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
#define __DEBUG if(1)
#else
#define __DEBUG if(0)
#endif

#define APPL_AS_MAIN
//#define NMEA_AS_MAIN

#define TRUE	1U
#define FALSE	0U

extern int 		he_strlen			(char * buf							);
extern int 		he_strcmp_len		(char * buf1, char * buf2, int len	);
extern int 		he_a2hex			(char * buf							);
extern int 		he_a2i				(char * buf							);
extern double 	he_a2f				(char * buf							);
extern void 	he_i2a				(int 	num,  char * buf			);
extern void 	he_f2a				(double num,  char * buf,  int len	);
extern void 	he_strcpy			(char * buf1, char * buf2, int len	);
extern int 		he_strchr			(char * buf1, char   buf2			);
extern int 		he_strcmp			(char * buf1, char * buf2			);
extern int 		he_isadouble		(char * buf							);
extern int 		he_isanint0_9		(char * buf							);
extern int 		he_isacharA_Z		(char   buf							);
extern void 	he_reverse			(char * buf							);
extern int 		he_powOfX			(int num, 	  int pow				);
extern void 	he_f2a				(double num,  char* buf,    int len );
#endif
