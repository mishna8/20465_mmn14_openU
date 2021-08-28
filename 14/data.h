
#ifndef DATA_H_
#define DATA_H_

#define ARR_DATA_SIZE 5000			/*Size of "data encoding" data structure*/

/*Check if string str_num starts from number
  If yes check if after  number are only space or tab or eol
  If all conditions described above are correct, enter number to the data encoding data structure*/

/* save single number to data memory. if data is a number return 1 otherwise 0 */
int saveDataToMem(char *str_num);

/*enter each char of string into "data encoding" Data structure*
 If string doesn't start and finish with quotation marks than print error and return 0
 Else return 1*/

/* store string *str in the data array starting from ind returns updated ind value after storing */
int saveStringToMem(char *str);

/* Get the DC number */
int GetDC();

/*Set DC value to 0*/
void ResetDC();

/*Write DC address and Machine code of their data to object file */
void WriteDataToFile(int fromAddress);

#endif

