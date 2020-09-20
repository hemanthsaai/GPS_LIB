/*
 * he_std_test.c
 *
 *  Created on: 15-May-2020
 *      Author: hemanthjsai
 */
#include "he_std_test.h"
int he_f2a_test(void)
{
	double Test_input = FLOAT_ASCII_TEST_MIN;
	char buf[20];
	FILE *fptr;

	fptr = fopen("he_ftoa_test_results.log","w");
	if(fptr <= 0)
	{
		return 0;
	}
	fprintf(fptr, "INPUT\t\t\tOUTPUT\n");
	while(Test_input <= FLOAT_ASCII_TEST_MIN)
	{
		he_f2a(Test_input,buf,6);
		fprintf(fptr, "%f\t\t%s\n",Test_input,buf);
		//printf("%f\t\t%s\n",Test_input,buf);
		Test_input = Test_input + 0.000001;
	}
	fclose(fptr);
	return 1;
}
