
#ifndef CODE_H_
#define CODE_H_

#include "definitions.h"
#include "SymbolTable.h"


/* Check if the row is only with white spaces
 * Input row - string to check
 * Output - 0 in case there is at list one non white spaces character
 * 			1 in case the string contains only white spaces 
 */
int IsRowEmpty(char *);

/*---*/

/* This function will check for errors and extract the operands
 * Input - string with parameters and row number where we got the string
 * Output - if valid returns a param struct each memeber is the operand withput spaces or comma, if a single operand the second member is null
 * 		if invalid will print the error and return a null in both members of the param struct
 */
param GetOperands(char *, int );
/*---*/

/* Get the instruction by the command name  - search for command cmd in cmdTab and return it.
 * input - a string (the command word)
 * ouput - the command CMDWord struct {the string, the op code, the func, number of operands}
 *	 if command not found return invalid command struct 
 */
CMDWord GetCMDName( char *);

/* This function get a string (operand) and check if its a valid addressing type
 * input - the operand to be checked, the command word of the operand and the row we got this operand from
 * Output - the addressing type or error if invalid 
 */
int GetAddressing(char *, CMDWord , int );

/*---*/

/* This function adds the operand according to its addressing type
 * Input - the operand string, its addressing type, IC instruction counter ,RC row counter 
 * Output - in case of  failure 0
 * 	in case success 1 
 */
int makeOperand(char *, AddresType ,  long , int );

/* This function will store the instruction in the instruction list
 * Input - 	inst_field - the instruction structure itself
 * 			IC - the index of the command in the list 
 */
void makeInstruction(instructionFields ,long );


/* reset inst_field struct to 0 */
void ResetCommand(instructionFields *inst_field);

/*---*/

/* This function writes all the instruction between the first address and the end address to the object file
 * Input - firstAddress - the address to start with
 * 		   endAddress - end address*/
void WriteInstructionToFile(int firstAddress,int endAddress);

#endif

