#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "code.h"
#include "OutputToFiles.h"

/* arguments -  pointers to an assembly file and to IC
 * Output - in case of  failure 0
 * 	in case success 1 */
int pass2(FILE *fp, long IC)
{
	char buffer[MAX_ROW_LEN];/* This variable will hold the row we need to read from the assembly file */
	char *row;/* Pointer to the row we analysing */
	int RC = 0;/* Row counter */
	char *token;
	int errflag = false;
	
	/*for entry*/
	symbol *sym; 
	/*param trimed;*/

	/*holding the variables to build the code*/
	param paramet;
	char *parameter;
	AddresType srctype=Error, dsttype=Error;
	char dst_op[MAX_ROW_LEN],
		 src_op[MAX_ROW_LEN]; 
		 
	/* Go over the file rows */
	while (fgets(buffer, MAX_ROW_LEN, fp) != NULL )
	{
		RC++;

		memset(src_op, '\0', sizeof(src_op));
		memset(dst_op, '\0', sizeof(dst_op));

		row = buffer;

		/* Remove all the prefix white spaces */
		while(row[0] == spacelim[0] || row[0] == tablim[0])
		{
			row++;
		}

		/* Remove the row-break from the end of the line if exists */
		if (row[strlen(row)-1]=='\n')
		{
			row[strlen(row)-1]='\0';
		}

		/* Check if the row is empty, with white spaces or a comment row */
		if (row[0] == semicolonlim[0] || IsRowEmpty(row))
		{
			continue;
		}

		/* Split the row by spaces */
		token = (char *) malloc(MAX_ROW_LEN);
		
		if(!token)
		{
			fprintf(stderr, "Error cannot allocate memory  row : %d \n",RC);
			exit(1);
		}
		
		token = strtok(row, spacelim);
/*-----*/
		/* Check if the first element of the split is a label
		 * the last char of the element need to be ':' in case of a label */
		if (token[strlen(token)-1]==':')
		{
			/* Go to the next row element */
			token = strtok(NULL, spacelim);
		}
/*-----*/
		/*.STRING / .DATA / .EXTERN / .ENTRY */
		if (*token == '.')
		{
			/* Check if the element is .entry */
			if (!strcmp(token,DOT_ENTRY))
			{
				/* Get the symbol from the symbol table */
				token = strtok(NULL, spacelim);	
				
				sym = FindSymbol(token,false);

				/* Check if the label name exists in the symbol table */
				if (!sym)
				{
					errflag = true;
					fprintf(stdout, "Error label name - \"%s\" , was not declared row number %d \n",token,RC);
					continue;
				}
				else
				{
					if(sym->lt != E)/*if not external*/
					{
						/* write the label to the Entry file */
						EntryFileWriteLine(sym->label, sym->address);
					}
					else
					{
						fprintf(stdout, "Error label name - \"%s\" , Is external and cannot be entry. row number %d \n",token,RC);
					}
				}
			}
		}
/*-----*/
		else /* if not, then check if it is an Instruction */
		{
			/* Get the instruction */
			CMDWord cmdword = GetCMDName(token);

			/* Check if the command is valid  */
			if(cmdword.op == opcodeErr )
			{
				errflag = true;
				fprintf(stdout, "Error invalid command - %s, row number %d \n",token,RC);
				continue;
			}

			/*the command is already in the array*/
			
			/* Get the rest of the row */
			token = strtok(NULL, "");
			
			/*remove excessive space and hold the operands*/
			if(token !=NULL) paramet = GetOperands(token, RC); else {paramet.first=NULL; paramet.second=NULL;}
			
			/*get the operands*/
			/* if the command takes 2 operands */
			if (cmdword.operandsInputNum == 2)
			{
				/*check the number of elements*/
				/*if Trim returned a string with a comma there are two valid elements in the operand*/	
				if( paramet.first == NULL ||  paramet.second == NULL)
				{
					errflag = true;
					fprintf(stdout, "Error this command should have two operands, command: %s , row : %d \n", cmdword.cmd_str, RC);
					paramet.first="0";paramet.second="0";/*mark to be not null for strcpy core dump*/
				}

				/*handel source*/
				parameter =  paramet.first;	
				strcpy(src_op, parameter);	
				srctype= GetAddressing(src_op, cmdword, RC);	
				
				/*handel destination*/
				parameter =  paramet.second;	
				strcpy(dst_op, parameter);	
				dsttype= GetAddressing(dst_op, cmdword, RC);	

				/*build the remaining operands*/
				if(srctype == Error || dsttype == Error)
				{
					errflag = true;
					fprintf(stdout, "Error inavlid addressing types row : %d \n",RC);
				}
				else {	
					if(srctype != Error){	
						if(srctype == Relative || srctype == Direct)	
							if(makeOperand(src_op,srctype,IC+1,RC))errflag=true;
					}
					if(dsttype != Error){
						if(dsttype == Relative || dsttype == Direct)	
							if(makeOperand(dst_op,dsttype,IC+2,RC))errflag=true;
					}
				}
				
				/*increment counter*/
				IC += 3;/* one slot for the instruction + one slot for source + one slot for destination */
			}
			
			else if (cmdword.operandsInputNum == 1)
			{
				/*check the number of elements*/				
				/*if Trim returned a string with a comma there are two valid elements in the operand*/
				if(paramet.second != NULL || paramet.first == NULL)
				{
					errflag = true;
					fprintf(stdout, "Error this command should have one operand, command: %s , row : %d \n", cmdword.cmd_str, RC);
					paramet.first="0";/*mark to be not null for strcpy core dump*/
				}
				
				/*handel destination*/
				strcpy(dst_op, paramet.first);	
				dsttype= GetAddressing(dst_op, cmdword, RC);

				
				/*build operands*/
				if(dsttype == Error)
				{
					errflag = true;
					fprintf(stdout, "Error inavlid addressing types row : %d \n",RC);
				}
				else {
					if(dsttype != Error ){
						 if(dsttype == Relative || dsttype == Direct)	
							if(makeOperand(dst_op,dsttype,IC+1,RC))errflag=true;
					}
				}
				
				/*increment counter*/				
				IC += 2;/* one slot for the instruction + one slot for destination*/
			}
			else if (cmdword.operandsInputNum == 0 )
			{
				/*check the number of elements*/			
				if( paramet.first != NULL ||  paramet.second != NULL)
				{
					errflag = true;
					fprintf(stdout, "Error this command should not have any operands, command: %s , row : %d \n", cmdword.cmd_str, RC);

				}
				
				/*increment counter*/
				(IC)++;/* one slot for the instruction*/
			}

		}
	}

	return !errflag;
}
