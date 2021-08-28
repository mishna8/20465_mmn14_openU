#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "code.h"
#include "data.h"
#include "OutputToFiles.h"

/* arguments -  pointers to an assembly file and to IC
 * Output - in case of  failure 0
 * 	in case success 1 */
int pass1(FILE *fp,long *IC)
{	
	char buffer[MAX_ROW_LEN + MAX_LABEL_LEN];/* This variable will hold the row we need to read from the assembly file */
	char *row;/* Pointer to the row we analysing */
	int RC = 0;/* Row counter */
	char *token;/* Temp argument for analysing row elements  */
	int errflag = false;/* Error flag */
	char label[MAX_LABEL_LEN];/* Label Name */
	char cmp[MAX_LABEL_LEN];
	
	/*holding the variables to build the code*/
	param paramet={NULL,NULL};
	char *parameter;
	AddresType srctype=Error, dsttype=Error;
	char dst_op[MAX_ROW_LEN],
		 src_op[MAX_ROW_LEN]; 
	CMDWord cmdword;
	long offset=0;
	
	instructionFields machineCommand;/* This variable will hold instructions that we can put in the array in pass1 */
	

	/* Go over the file rows */
	while (fgets(buffer, MAX_ROW_LEN + MAX_LABEL_LEN, fp) != NULL )
	{
		RC++;
		ResetCommand(&machineCommand);
		memset(label, '\0', sizeof(label));
		memset(cmp, '\0', sizeof(cmp));
		memset(src_op, '\0', sizeof(src_op));
		memset(dst_op, '\0', sizeof(dst_op));

		/* Check if the row is valid */
		if (strlen(buffer) > MAX_ROW_LEN)
		{
			errflag = true;
			fprintf(stdout, "Error : The row is not valid");
		}

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

		/* Check if the row is empty, with white spaces or a comment row(that starts in ; and must be ignored) */
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
			/* Label name cannot be too long */
			if(strlen(token) > MAX_LABEL_LEN)
			{
				errflag = true;
				fprintf(stdout, "Error Label name is too long : %s  row: %d\n", token, RC);
			}

			/* Label name cannot starts with a number */
			if(isdigit(*token))
			{
				errflag = true;
				fprintf(stdout, "Error Label name can not start with a number : %s  row: %d\n", token, RC);
			}

			/* Cut the label name and add null character to the end of it */
			strncpy(label, token,strlen(token)-1);
			label[strlen(token)-1] = '\0';
			
			/* Get the next element of the row */
			token = strtok(NULL, spacelim);

			/* Check if the token is null or is empty */
			if(!token || token[0] == '\n' || IsRowEmpty(token))
			{
				errflag = true;
				fprintf(stdout, "Error Can not be only label without a command : %s  row: %d\n", token, RC);
			}
		}
/*-----*/
		/* Check if the first char is a dot : .string / .data / .extern / .entry */
		if (*token == '.')
		{
			char *str_num; 	/*holds numbers in a .data to be saved to memory*/												
	 		/* .string */
	  		if(!strcmp(token, DOT_STRING))
	  		{
		  		if (label!='\0' && !IsRowEmpty(label))
		  		{
			  		/* insert the label to the symbol table  */
			  		if(!InsertToSymbolTable(label, GetDC(), false, true, false))
			  		{
						  errflag = true;
						  fprintf(stdout, "Error can not insert label to symbol table : %s, row number : %d \n", label, RC);
					}
				}

		  		token = strtok(NULL, spacelim);

		  		/* In case of a string we need to save it in the data table */
		  		if(!saveStringToMem(token))
		  		{
		  			errflag=true; 
			  		fprintf(stdout, "Error can not insert string to memory : %s, row number : %d \n", token, RC);
		  		}
	  		}
	  		/* .data */
	  		else if(!strcmp(token, DOT_DATA))
	  		{
		  		if (label!='\0' && !IsRowEmpty(label))
		  		{
			  		/* insert the label to the symbol table  */
			  		if(!InsertToSymbolTable(label, GetDC(), false, true, false))
			  		{
						  errflag = true;
						  fprintf(stdout, "Error can not insert label to symbol table : %s, row number : %d \n", label, RC);
					}
		  		}

		  		/* Split all the numbers in the .data label */
		  		str_num = strtok(NULL, commalim); 

		  		/* Go over the numbers and add them to the memory data section */
		  		while (str_num)
		  		{
			  		/* add them to the memory data section */
			  		if (saveDataToMem(str_num))
			  		{
				  		/* Get the next number */
				  		str_num = strtok(NULL, commalim);    
			  		}
			  		else
			  		{
				  		/* In case there is an error in the memory insert */
				  		errflag = true;
				  		fprintf(stdout, "Error , the data is not valid, row number : %d  \n" , RC);
				  		break;
			  		}
		  		}
	  		}
			/* .extern */
			else if(!strcmp(token, DOT_EXTERN))
			{
				/* Check for label */
				if(strcmp(label,cmp))
		  		{
			  		fprintf(stdout, "Warning ignored label on EXTERN statement \n");
		  		}

		  		/* Get the label name after the .*/
		  		token = strtok(NULL, spacelim);
				
				/* Insert the label to the symbol table */
		  		if(!InsertToSymbolTable(token, 0, false, false, true))
		  		{
					errflag = true;
					fprintf(stdout, "error could not insert label %s row number: %d \n",label,RC);
				}
				
	  		}
	  		/* .entry */
	  		else if(!strcmp(token, DOT_ENTRY))
	  		{
		 		/* This instruction will be handled in the Second Scan  */
		 		if(strcmp(label,cmp))
		 		{
			 		fprintf(stdout, "Warning ignored label on ENTRY statement \n");
		 		}
	  		}
	  		else
	  		{
		  		errflag = true;	  		
		  		printf("Invalid Directive Reference \n" );
	  		}
		}
/*-----*/
		else /* if not, then check if it is an Instruction */
		{		
			if(!IsRowEmpty(label))
			{
				/* Insert the label to the symbol table */
				errflag = !InsertToSymbolTable(label, *IC, true, false,false);
			}			

			/* Get the instruction word */
			cmdword = GetCMDName(token);

			/* Check if the command is valid  */
			if (cmdword.op == opcodeErr )
			{
				errflag = true;
				fprintf(stdout, "Error unknown command: %s , row number:%d\n", token , RC);
			}

			/* Get the instruction command */
			machineCommand.opword = cmdword.op;
			machineCommand.funcword = cmdword.fn;

			/* Get the rest of the row */
			token = strtok(NULL, "");

			if(token !=NULL) paramet = GetOperands(token, RC); else {paramet.first=NULL; paramet.second=NULL;}

			/*get the operands*/
			/* If the command gets 2 operands */
			if (cmdword.operandsInputNum == 2)
			{	
				/*check the number of elements*/
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
				machineCommand.src_type = srctype;

				/*handel destination*/
				parameter =  paramet.second;	
				strcpy(dst_op, parameter);
				dsttype= GetAddressing(dst_op, cmdword, RC);
				machineCommand.dst_type = dsttype;
									
				/*build the instruction*/
				offset=*IC;
				makeInstruction(machineCommand, offset);
				
				/*build operands*/
				if(srctype == Error || dsttype == Error)
				{
					errflag = true;
					fprintf(stdout, "Error inavlid addressing types row : %d \n",RC);
				}
				else {
					
					if(srctype != Error) {
						offset=*IC;
						if(srctype == Register || srctype == Immediate){
							if(makeOperand(src_op,srctype,offset+1,RC))errflag=true;
							}
							
					}	
					if(dsttype != Error){
						offset=*IC;
						if(dsttype == Register || dsttype == Immediate){
							if(makeOperand(dst_op,dsttype,offset+2,RC))errflag=true;
							}
						
					}
				}

				/*increment counter*/
				*IC += 3;/* one slot for the instruction + one slot for source + one slot for destination */
			}
			/* If the command gets only one operand */
			else if (cmdword.operandsInputNum == 1)
			{
				/*check the number of elements*/				
				if(paramet.second != NULL || paramet.first == NULL)
				{
					errflag = true;
					fprintf(stdout, "Error this command should have one operand, command: %s , row : %d \n", cmdword.cmd_str, RC);
					paramet.first="0";/*mark to be not null for strcpy core dump*/
				}
				
				machineCommand.src_type = 0;
				
				/*handel destination*/
				strcpy(dst_op, paramet.first);
				dsttype= GetAddressing(dst_op, cmdword, RC);
				machineCommand.dst_type = dsttype;
				
				/*build the instruction*/
				offset = *IC;
				makeInstruction(machineCommand, offset);
				
				/*build operands*/
				if(dsttype == Error)
				{
					errflag = true;
					fprintf(stdout, "Error inavlid addressing types row : %d \n",RC);
				}
				else {
					offset=*IC;
					 if(dsttype == Register || dsttype == Immediate){
							if(makeOperand(dst_op,dsttype,offset+1,RC))errflag=true;
					}
						
				}
				
				/*increment counter*/				
				*IC += 2;/* one slot for the instruction + one slot for destination*/
			}		
			else if (cmdword.operandsInputNum == 0)
			{
				/*check the number of elements*/			
				if( paramet.first != NULL ||  paramet.second != NULL)
				{
					errflag = true;
					fprintf(stdout, "Error this command should not have any operands, command: %s , row : %d \n", cmdword.cmd_str, RC);

				}
				
				/*handel operands*/
				machineCommand.src_type = 0;
				machineCommand.dst_type = 0;
				
				/*build the instruction*/
				offset=*IC;
				makeInstruction(machineCommand, offset);
				
				/*increment counter*/
				(*IC)++;/* one slot for the instruction*/
			}
		}

	}

	return !errflag;
}	
