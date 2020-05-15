/*
 ============================================================================
 Name        : he_std.c
 Author      : HemanthJsai
 Version     : 1.0
 Copyright   : Copyright Protected, HS
 Description : Redefined Standard Library
 ============================================================================
 */
#include "..\include\he_std.h"

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
*   Returns the float value of given string
*/
double he_a2f(char *buf)
{
	int significand_int = 0;
	int exponent_int = 0;
	int itr,t,c=1;
	double significand_float,exponent_float;
	if(he_strchr(buf,'.'))
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
*   if len = x then compare till string[x]
*	Returns the TRUE/FALSE
*/
int he_strcmp_len(char * buf1, char * buf2, int len)
{
	int itr = 0;
	if(len <= he_strlen(buf1))
	{
		while(len)
		{
			if (buf1[itr] != buf2[itr])
			{
				return FALSE;
			}
			itr++;
			len--;
		}
	}
	return TRUE;
}


/*
*   compare 2 strings
*   if len = x then compare till string[x]
*	Returns the TRUE/FALSE
*/
int he_strcmp(char * buf1, char * buf2)
{
	int itr = 0;
	while(buf1[itr])
	{
		if (buf1[itr] != buf2[itr])
		{
			return FALSE;
		}
		itr++;
	}
	return TRUE;
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
*   Returns pos if the char in buf2 is available in buf1
*   else returns 0
*/
int he_strchr(char * buf1,char buf2)
{
	int itr = 0;
	while(buf1[itr])
	{
		if (buf1[itr] == buf2)
		{
			return itr;
		}
		itr++;
	}
	return 0;
}


/*
*   Returns TRUE if buffer is in a valid char range A-Z
*/
int he_isacharA_Z(char buf)
{
	if( (buf >= 'A') && (buf <= 'Z') )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*
*   Returns TRUE if buffer is in a valid integer range 0-9
*/
int he_isanint0_9(char *buf)
{
	int iter = 0;
	while(buf[iter])
	{
		if( ! ((buf[iter] >= '0') && (buf[iter] <= '9')) )
		{
			return FALSE;
		}
		iter++;
	}
	return TRUE;
}


/*
*   Returns TRUE if buffer is in a valid integer range 0-9 and has '.'
*/
int he_isadouble(char *buf)
{
	int iter = 0;
	int count = 0;
	while(buf[iter])
	{
		if(buf[iter] == '.')
		{
			count ++;
		}
		else if( ! ((buf[iter] >= '0') && (buf[iter] <= '9')) )
		{
			return FALSE;
		}
		iter++;
	}
	if(count == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
*  Reverse the string in the same BUFFER
*  WARNING
*  DO NOT PASS CONSTANT'S TO THIS FUNCTION
*/
void he_reverse(char *buf)
{
    int j = he_strlen(buf) - 1;
    char temp;
    for (int i = 0; i<j; i++, j--)
    {
        temp 	= 	buf[j];
        buf[j]	= 	buf[i];
        buf[i] 	= 	temp;
    }
}

/*
*  Converts Integer to its ascii and updates it in the
*  string of 2nd parameter
*  WARNINIG
*  Application should take care of length of string.
*/
void he_i2a(int num, char* buf)
{
    int negative_FLAG = 0;

    int itr = 0;
    int reminder = 0;

    if (num == 0)
    {
    	buf[itr++] = '0';
    	buf[itr] = '\0';
        return;
    }
    if (num < 0)
    {
    	negative_FLAG = 1;
        num = -num;
    }
    while (num != 0)
    {
    	reminder = num % 10;
    	buf[itr++] = (reminder > 9) ? ((reminder-10) + 'a') : reminder + '0';
        num = num/10;
    }
    if (negative_FLAG)
    	buf[itr++] = '-';

    buf[itr] = '\0';

    he_reverse(buf);

    return;
}


/*
*  Converts Float/Double to its ASCII and updates it in the
*  string of 2nd parameter
*  len is how many digits after '.'
*  WARNINIG
*  Application should take care of length of string.
*
*  TODO mantissa = (int) ((num - exponent)*(len) + 0.1);
*  		ADDING 0.1 made this equation perfect, This may not be the same always
*/
void he_f2a(double num, char* buf, int len)
{
    int negative_FLAG = 0;

    int itr = 0;
    int reminder = 0;
    int exponent_i;
    double mantissa;
    float exponent_f = 0;

    if (num == 0)
    {
    	buf[itr++] = '0';
    	buf[itr] = '\0';
        return;
    }
    if (num < 0)
    {
    	negative_FLAG = 1;
        num = -num;
    }

    exponent_i = (int) num;

    if(exponent_i == 0)
    {
    	buf[itr++] = '0' ;
    }
    else
    {
    	while (exponent_i != 0)
    	{
    		reminder = exponent_i % 10;
    		buf[itr++] = (reminder > 9) ? ((reminder-10) + 'a') : reminder + '0';
    		//buf[itr++] = reminder  + '0';
    		exponent_i = exponent_i/10;
    	}
    }

    if (negative_FLAG)
    	buf[itr++] = '-';

    buf[itr] = '\0';
    he_reverse(buf);
    if(len > 0)
    {
    	buf[itr++] = '.';
    	mantissa = num - (int) num;
    	while(len--)
    	{
    		mantissa = mantissa * 10;
    		exponent_f = (double) (mantissa);
    		//printf("exponent_f = %f  \n",exponent_f);
    		mantissa = mantissa - (int) mantissa;
    		buf[itr++] = ((int)exponent_f > 9) ? (((int)((int)exponent_f-10)) + '0') : (int)exponent_f + '0';
    	}
    	buf[itr] = '\0';

    }

    return;
}

/*
 *  Returns the power if number
 */
int he_powOfX(int num, int pow)
{
	int ret = 1;
	if(pow == 0)
	{
		ret = 1;
	}
	else
	{
		while(pow--)
			ret = ret * num;
	}
	return ret;
}
