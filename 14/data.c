#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "code.h"
#include "OutputToFiles.h"

#define quoteMark '\"'
#define MAX_NAM 2047/* the maximum number we can insert to the data table (12bit) */
#define MIN_NAM -2048/* the minimum number we can insert to the data table (12bit)*/

int data[ARR_DATA_SIZE]; /*holds all the "data encoding" in .data/.string instructions*/
int DC = 0;/*DATA counter*/


/*Write DC address and Machine code of their data to object file */
void WriteDataToFile(int fromAddress)
{
	int i;
	for(i = fromAddress;i < fromAddress+DC ;i++)
	{
		ObjectFileWriteLine(i, data[i - fromAddress],A);
	}
}

/* save single number to data memory. if data is a number return 1 otherwise 0 */
int saveDataToMem(char *str_num)
{
	int errflag = false;

	/*Check if array for "data encoding" was created successfully */
	if(data != NULL)
	{
		if(DC < ARR_DATA_SIZE)
		{
			int num;
			char *end_ptr;/*the reference to an object of type char*, whose value is set by the function to the next character in str_num after the numerical value.*/
			num = (int)strtol(str_num, &end_ptr, 10);	/*assign numerical value to str_num (decimal base)*/

			/* Check if the number is valid (in a range that can be represent with 12 bits) */
			if(num <= MAX_NAM && num >= MIN_NAM)
			{
				/* Check if after numerical value the end_ptr is  space/tab/new line symbol */
				if ((end_ptr != NULL && end_ptr != str_num && IsRowEmpty(end_ptr) ) || end_ptr == NULL)
				{
					data[DC] = num;
					DC++;
				}
				/*if after numerical value the end_ptr is not space/tab/new line symbol print error and return 0(false)*/
				else
				{
					fprintf(stdout, "Error unresolved parameter: %s . expecting integer \n", str_num);
					errflag = true;
				}
			}
			/*Check if number is in expected size range*/
			else
			{
				fprintf(stdout, "Error the number %s is not in range  [%d - %d] canot be represented  ", str_num, MAX_NAM, MIN_NAM);
				errflag = true;
			}
		}
		/*If no more space in "data encoding array" print the error and return 0(false)*/
		else
		{
			fprintf(stderr, "Error No more space, can't add new value to the DATA array ");
			errflag = true;
		}
	}
	else
	{
		errflag = true;
	}

	return !errflag;
}

/* store string *str in the data array starting from ind returns updated ind value after storing */
int saveStringToMem(char *str)
{
	int errflag = false;

	/*Check if array for "data encoding" was created successfully */
	if(data!=NULL)
	{
		int strEnd = strlen(str)-1;

		/* Remove \n */
		if(str[strEnd] == '\n')
			str[strEnd] = '\0';

		/* Check if string starts and finishes with quotation marks */
		if (str[0] == quoteMark && str[strEnd] == quoteMark)
		{
			/* Continue the loop until didn't encounter on quotation marks or no more free space in the "data encoding array" */
			/* str++ => Avoid the first " */
			for (str++; *str != '\"' && DC<ARR_DATA_SIZE; str++, DC++)
			{	
				data[DC] = *str;
			}

			data[DC] = '\0';
			DC++;

			/* If no more space in "data encoding array" print the error and return 0(false) */
			if(DC==ARR_DATA_SIZE)
			{
				fprintf(stderr, "Error No more space, can't add new value to the DATA array ");
				errflag = true;
			}
		}
		/*Print error and return 0 if a  string doesn't start and finish with quotation marks*/
		else
		{
			fprintf(stdout, "Error unresolved variable: %s . expecting string\n", str);
			errflag = true;
		}
	}
	else
	{
		errflag = true;
	}

	return !errflag;
}

/* Get the DC number */
int GetDC()
{
	return DC;
}
/*Set DC value to 0*/
void ResetDC()
{
	DC = 0;
}

