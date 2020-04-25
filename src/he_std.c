/*
 ============================================================================
 Name        : he_std.c
 Author      : HemanthJsai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Redefined Standard Library
 ============================================================================
 */
#include "he_std.h"


/*
*	Returns the hex value of string
*/
int he_a2hex(char * buf)
{
	int ret_hex = 0;
	for (int itr = 0; buf[itr]; itr++)
	{
		if(buf[itr] >= '0' && buf[itr] <= '9' )
		{
			ret_hex = ret_hex * 16 + buf[itr] - '0';
		}
		else if (buf[itr] >= 'A' && buf[itr] <= 'F')
		{
			ret_hex = ret_hex * 16 + buf[itr] - 55;
		}
		else if (buf[itr] >= 'a' && buf[itr] <= 'f')
		{
			ret_hex = ret_hex * 16 + buf[itr] - 87;
		}
		else
		{
			ret_hex = 0;
		}
	}
	return ret_hex;
}


/*
*	Returns the integer value of string
*/
int he_a2i(char * buf)
{
	int ret_integer = 0;
	for (int itr = 0; buf[itr]; itr++)
	{
		ret_integer = ret_integer * 10 + buf[itr] - '0';
	}
	return ret_integer;
}


/*
*   Calculate String Length
*	Returns the size of buffer
*/
int he_strlen(char * buf)
{
	int itr = 0, len = 0;
	while(buf[itr])
	{
		len = len + 1;
		itr = itr + 1;
	}
	return len;
}

/*
*   compare 2 strings
*   if len = 0 compare till end of 1st string
*   if len = x then compare till string[x]
*	Returns the TRUE/FALSE
*/
int he_strcmp(char * buf1, char * buf2, int len)
{
	int itr = 0;
	if(len <= he_strlen(buf1))
	{
		while(len)
		{
			if (buf1[itr] != buf2[itr])
			{
				return 0;
			}
			itr++;
			len--;
		}
	}
	return 1;
}


/*
*   Copy buf2 into buf1
*/
void he_strcpy(char * buf1, char * buf2, int len)
{
	int itr = 0;
	while(len--)
	{
		buf2[itr] = buf1[itr];
		itr++;
	}
	buf2[itr] = 0;
}


/*
*   Returns the float value of given string
*/
double he_a2f(char *buf)
{
	int significand_int = 0;
	int exponent_int = 0;
	int itr,t,c=1;
	double significand_float,exponent_float;
	if(strchr(buf,'.'))
	{
		for(itr=0;buf[itr]!='.';++itr)
		{
			if((buf[itr]>=48)&&(buf[itr]<=57))
			{
				significand_int =significand_int *10 +(buf[itr]-48);
			}
		}
		for(itr=itr+1;buf[itr];++itr)
		{
			if((buf[itr]>=48)&&(buf[itr]<=57))
			{
				exponent_int =	exponent_int *10 +(buf[itr]-48);
			}
		}
		t	= exponent_int;
		significand_float 	= significand_int;
		exponent_float 		= exponent_int;
		while(t)
		{
			t=t/10;
			c=c*10;
		}
		exponent_float 		= exponent_float/c;
		return (significand_float + exponent_float);
	}
	else
	{
		return (double)he_a2i(buf);
	}

}

/*
*   Returns 1 if the car in buf2 is available in buf1
*/
int he_strchr(char * buf1,char buf2)
{
	int itr1 = 0;
	while(buf1[itr1])
	{
		if (buf1[itr1] == buf2)
		{
			return 1;
		}
		itr1++;
	}
	return 0;
}
