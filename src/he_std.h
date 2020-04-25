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


extern int he_strlen(char * buf);
extern int he_strcmp(char * buf1, char * buf2, int len);
extern int he_a2hex(char * buf);
extern int he_a2i(char * buf);
extern double he_a2f(char *buf);
extern void he_strcpy(char * buf1, char * buf2, int len);
extern int he_strchr(char * buf1,char buf2);

#endif