#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "code.h"
#include "OutputToFiles.h"

#define IMMADIET_MAX 2047 /*the range of valid immidiate operand*/
#define IMMADIET_MIN -2048

#define OPERAND_PREFIX 'r'/*register*/
#define OPERAND_MAX_ID '7'/*r0-r7, 8 registers*/



typedef struct {
	int 		instructionCode;/*binary instruction*/
	LINK_TYPE 	linkType;/*A R E*/
}instruction;

/* this array will hold all the machine instructions encoding*/
instruction MachineCodeImg_Arr[CODE_ARR_SIZE+CODE_ADDRESS_BASE];
/*---*/



/* This array will give the info about the parameters number for each command (instruction type) */
CMDWord cmdTab[CMD_TABLE_SIZE] = {
			{"mov", mov, none, 2},
			{"cmp", cmp, none, 2},
			{"add", op2, add, 2},
			{"sub", op2, sub, 2},
			{"lea", lea, none, 2},
			{"clr", op5, clr, 1},
			{"not", op5, not, 1},
			{"inc", op5, inc, 1},
			{"dec", op5, dec, 1},
			{"jmp", op9, jmp, 1},
			{"bne", op9, bne, 1},
			{"jsr", op9, jsr, 1},
			{"red", red, none, 1},	
			{"prn", prn, none, 1},
			{"rts", rts, none, 0},
			{"stop",stop, none, 0}
			};

/*------------------------------------------------------------------------------------------------------*/

/* Check if the row is only with white spaces
 * Input row - string to check
 * Output -if there is at least one non white spaces character 0
 * 	if contains only white spaces 1
 */
int IsRowEmpty(char* row)
{
	/* Char Index */
	int i = 0;
	int isEmpty = 1;

	/* strcmp returns a nonzero value if the strings are different and 0 if they're the same */
	if(strcmp(row, ""))
	{
		while (isEmpty && row && (row[i] != '\0') && (row[i] != '\n'))
		{
			/* isspace returns a nonzero value if the char is a space and 0 if not */
		   if(isspace(row[i])|| row[i]==tablim[0])
		   {
			   i++;
		   }
		   else
		   {
			   isEmpty = 0;
		   }
		}
	}

	return isEmpty;
}

/*-----------------------------------------------------------------------------------*/

/* This function will check for errors and extract the operands
 * Input - string with parameters and row number where we got the string
 * Output - if valid returns a param struct each memeber is the operand withput spaces or comma, if a single operand the second member is null
 * 		if invalid will print the error and return a null in both members of the param struct
 */
param GetOperands(char *str, int RC){
	
	char *err = str; /*a string to hold the original before the analyses*/
	int errflag = false; /*error flag*/
	
	char *pass;
	int commacount=0;
	int element=0;
	int i=0;


	char op1[MAX_ROW_LEN]={'0'};
	char op2[MAX_ROW_LEN]={'0'};
	
	param operands;
	operands.first=(char *) malloc(MAX_ROW_LEN);
	operands.second=(char *) malloc(MAX_ROW_LEN);
	if(!operands.first || !operands.second ){
		fprintf(stderr, "Error cannot allocate memory \n");
		exit(1);
	}
	
	/*remove initial space*/
	while(*str == spacelim[0] || *str == tablim[0]) {
			str++;
	}
	pass=str;
	
	/*comma check*/
	while(*pass!='\0'){
		if (*pass == commalim[0]) commacount++;
		i++;pass++;
	
	}
	if(commacount >1)/*cannot be more then one comma*/
	{
		errflag=true;
	 	fprintf(stdout,"Error invalid number of commas : %s , row %d\n",err,RC );	
	}
	
	/*no comma - we expect 1 operand*/
	if(commacount==0)
	{
		i=0;
		/*while not end*/
		while(*str!='\0' && *str!='\n'){	
			/*while it is not space check operand*/
			while(*str != spacelim[0] && *str != tablim[0]){	
				/*check if valid charecter immediate - a number / labels - low/high case characters / register - in the ranges */
				if((*str>='0' && *str<='9')||(*str>='a' && *str<='z')||(*str>='A' && *str<='Z')||*str=='%'||*str=='#'||*str=='-'||*str=='+'){
					op1[i]=*str; 					
					str++;i++;
				}
				 /*if not valid and not a comma and not end of row - error*/
			 	else{
			 		/*check if its not breaker*/
			 		if(*str!='\0' && *str!='\n' && *str != spacelim[0] && *str != tablim[0] && *str!=commalim[0]){
			 			errflag=true;
			 			fprintf(stdout,"Error invalid operand: %s , row %d\n",err,RC );
			 		}
			 		break; 
			 	}
			}						
			if(errflag==true){break;}
			/*count sub token*/
			element++;	
			/*remove space*/
			while(*str == spacelim[0] || *str == tablim[0]) {
				str++;
			}
		}
		/*check sub tokens*/
		if(element>1){
		errflag=true;
	 	fprintf(stdout,"Error invalid number of operands : %s , row %d\n",err,RC );	
		}
	/*load operand*/
	if(errflag==false){
	strcpy(operands.first, op1);
	operands.second=NULL;
	}
	
	}
	

	if(commacount == 1)/*we exprct 2 operands*/
	{
		/*check first char not comma*/
		if(*str==commalim[0])
		{
			errflag=true;
	 		fprintf(stdout,"Error invalid placement of commas : %s , row %d\n",err,RC );	
		}
		else{
			/* check first operand*/
			i=0;
			/*while not end of first part*/
			while(*str!=commalim[0]){	
				/*while it is not space check operand*/
				while(*str != spacelim[0] && *str != tablim[0]){	
					/*check if valid charecter immediate - a number / labels - low/high case characters / register - in the ranges */
					if((*str>='0' && *str<='9')||(*str>='a' && *str<='z')||(*str>='A' && *str<='Z')||*str=='%'||*str=='#'||*str=='-'||*str=='+'){
						op1[i]=*str; 											
						str++;i++;
					}
				 	/*if not valid and not a comma and not end of row - error*/
			 		else{
			 			/*check if its not breaker*/
			 			if(*str!='\0' && *str!='\n' && *str != spacelim[0] && *str != tablim[0]&& *str!=commalim[0]){
			 				errflag=true;
			 				fprintf(stdout,"Error invalid operand: %s , row %d\n",err,RC );
			 			}
			 			break; 
			 		}
				}						
				if(errflag==true){break;}
				/*count sub token*/
				element++;					
				/*remove space*/
				while(*str == spacelim[0] || *str == tablim[0]) {
					str++;
				}
			}
			/*check sub tokens*/	
			if(element>1){
			errflag=true;
	 		fprintf(stdout,"Error invalid number of operands in first part: %s , row %d\n",err,RC );	
			}
			
			/*handel between operands*/
			str++; /*skip comma*/
			/*remove space*/
			while(*str == spacelim[0] || *str == tablim[0]) {
				str++;
			}
			element=0;/*restart*/
			/*check last char not comma*/
			if(*str=='\0'||*str=='\n')
			{
				errflag=true;
	 			fprintf(stdout,"Error invalid placement of commas : %s , row %d\n",err,RC );	
			}
			
			/*check second operand*/
			i=0;
			/*while not end of second part*/
			while(*str!='\0' && *str!='\n'){	
				/*while it is not space check operand*/
				while(*str != spacelim[0] && *str != tablim[0]){	
					/*check if valid charecter immediate - a number / labels - low/high case characters / register - in the ranges */
					if((*str>='0' && *str<='9')||(*str>='a' && *str<='z')||(*str>='A' && *str<='Z')||*str=='%'||*str=='#'||*str=='-'||*str=='+'){
						op2[i]=*str; 											
						str++;i++;
					}
				 	/*if not valid and not a comma and not end of row - error*/
			 		else{
			 			/*check if its not breaker*/
			 			if(*str!='\0' && *str!='\n' && *str != spacelim[0] && *str != tablim[0]){
			 			errflag=true;
			 			fprintf(stdout,"Error invalid operand: %s , row %d\n",err,RC );
			 			}
			 			break; 
			 		}
				}						
				if(errflag==true){break;}
				/*count sub token*/
				element++;					
				/*remove space*/
				while(*str == spacelim[0] || *str == tablim[0]) {
					str++;
				}
			}
			/*check sub tokens*/
			if(element>1){
			errflag=true;
	 		fprintf(stdout,"Error invalid number of operands in second part: %s , row %d\n",err,RC );	
			}
		}
	
	/*load operands*/
	if(errflag==false){
	strcpy(operands.first, op1);
	strcpy(operands.second, op2);
	}
	}
	
	/*if invalid*/
	if(errflag==true){
		operands.first=NULL;
		operands.second=NULL;
	}
	return operands;
}

/*-------------------------------------------------------------------------------------------------------------------------*/

/* Get the instruction by the command name  - search for command cmd in cmdTab and return it.
 * input - a string (the command word)
 * ouput - the command CMDWord struct {the string, the op code, the func, number of operands}
 *	 if command not found return invalid command struct 
 */
CMDWord GetCMDName( char *commandName)
{
	CMDWord resultCmd = {"invalid", opcodeErr, funcErr, Error};

	/* Go over the commands and find the command that equal to the command name */
	int i;
	for (i = 0; i <= CMD_TABLE_SIZE; i++ )
	{
		if ( strcmp( cmdTab[i].cmd_str, commandName ) == 0 )
		{
			resultCmd = cmdTab[i];
			break;
		}
	}

	return resultCmd;
}

/* This function get a string (operand) and check if its a valid addressing type
 * input - the operand to be checked, the command word of the operand and the row we got this operand from
 * Output - the addressing type or error if invalid 
 */
int GetAddressing(char *operand, CMDWord cmd, int RC)
{
	AddresType result=Error;
	int num=IMMADIET_MAX+1;/* will hold the Immediate number*/
	char label[MAX_LABEL_LEN]={0};/*will hold the label*/
	char ch[1]={0}; /*wil hold # or % if any */
	char oper[MAX_ROW_LEN]={0}; int i =0; /*hold the operand*/

	/*note - there should'nt be any space befor after or between any charecter in the given operand because of trim function*/
	if ( strlen( operand ) == 2 && *operand == OPERAND_PREFIX && isdigit( operand[1] ) && ((int)operand[1] - OPERAND_MAX_ID <= 0))
	{
		result = Register;
	}
	/* In case the operand test for special character at the start */
	else
	{
		/*hold the first charecter to be checked*/
		ch[0] = *operand; operand++;
		/*get the rest of the operand*/
		while(*operand != '\0' && *operand != '\n'){
				oper[i] = *operand; 
				operand++; i++;
		}
		 
		/*test the first character */
		/* a % = relative, valid only for jmp bne jst opword=9*/
		if ( ch[0] == '%' )
		{
			/*first is it a valid command*/
			if(!(cmd.op == op9 && (cmd.fn != none && cmd.fn != funcErr)))
			{
				fprintf(stdout, "Error Invalid operand addressing type for this command word : %s %s row: %d \n",cmd.cmd_str,  oper, RC);
				return Error;
			}
			/*then check if right after the % there's a label name = 31 bit string*/
			if (sscanf(oper, "%31[a-zA-Z0-9]", label)==1)
				result=Relative;
		}
		/* a # = immidiate, followed by a number*/
		else if ( ch[0] == '#' ){
			/* check if right after the # there's a number*/
			if (sscanf( oper, "%d", (int *)&num ) == 1){
				/*then check if its a valid number in range*/
				if(num <= IMMADIET_MAX && num >= IMMADIET_MIN){
					result=Immediate;
				}
				else{
				fprintf(stdout, "Error Invalid immediate for this addressing type , warning number will be trimed : %s row: %d \n", oper, RC);
					result = Immediate;
				}
			}
		}
		/* direct =  a label with no char before*/
		else {
			/*return the charecter to the operand*/
			char *dir=strcat(ch, oper);
			/*check if its a label*/
			if (sscanf(dir, "%31[a-zA-Z0-9]", label)==1)
				result=Direct;
		}
	}
	return result;
}

/*-------------------------------------------------------------------------------------------------------------------------*/

/* This function adds the operand according to its addressing type
 * Input - the operand string, its addressing type, IC instruction counter ,RC row counter 
 * Output - in case of  failure err_flag=true
 * 	in case success er_flag=false 
 */
int makeOperand(char *Operand, AddresType type,  long IC, int RC)
{
	int errflag=false;/*error flag*/
	int num; int sign;
	symbol *sym; 

	char *oprnd = Operand;/*keep the string*/
	int len = strlen(oprnd), i=0;
	char operand[81]={0};

	while(i !=len){
		if(*oprnd =='#'){oprnd++;}/*so the number wil load with out the #*/
		operand[i]=*oprnd;	
		i++,oprnd++;
	}

	if ( type == Register)
	{
		/*get the digit of the register*/
		num = operand[1]-'0';		
		MachineCodeImg_Arr[IC].instructionCode = 1u << num;
		MachineCodeImg_Arr[IC].linkType = A;
	}

	else if (type == Immediate )
	{
		/*get the digits of the immediate*/
		num = atoi(operand);
		sign = (num&0x800)/0x800;
		num = num&0x7FF;
		if(sign!=0)num= num*-1;
		MachineCodeImg_Arr[IC].instructionCode = num;
		MachineCodeImg_Arr[IC].linkType = A;
	}

	else if (type == Relative)
	{
		Operand++;/*to remove the %*/
		/*get the symbol for its address*/
		sym = FindSymbol(Operand,false);
		if(sym!=NULL)/*exists*/
		{
			/*check if the label is external*/
			if(sym->lt == E)
			{
				errflag = true;
				fprintf(stdout, "Error the label for this adderssing type cannot be external: %s row: %d \n", Operand, RC);
			}
			else {
				/*get the addressing distance*/
				num = (sym->address) - IC;
				MachineCodeImg_Arr[IC].instructionCode = num;
				MachineCodeImg_Arr[IC].linkType = sym->lt;
			}
		}
		else/*doesnt exist*/
		{
			errflag = true;
			fprintf(stdout, "Error Invalid symbol name: %s row: %d \n", Operand, RC);
		}
		
	}

	else if( type == Direct )
	{
		sym = FindSymbol(Operand,false);

		if(sym!=NULL)/*exists*/
		{
			MachineCodeImg_Arr[IC].instructionCode = sym->address;
			MachineCodeImg_Arr[IC].linkType = sym->lt;
			/*check if the label is external*/
			if(sym->lt == E)
			{	
				/* write the label to the Entry file */
				ExternalFileWriteLine(sym->label, IC);
			}
		}
		else/*doesnt exist*/
		{
			errflag = true;
			fprintf(stdout, "Error Invalid symbol name: %s row: %d \n", Operand, RC);
		}
	}
							
	return errflag;
}

/* This function will store the instruction in the code image
 * Input - 	inst_field - the instruction structure itself
 * 			IC - the index of the command in the list 
 */
void makeInstruction(instructionFields inst_field, long IC)
{
	/*4 bits opcode*/
	MachineCodeImg_Arr[IC].instructionCode = (int)inst_field.opword;
	
	/*4 bits func*/
	MachineCodeImg_Arr[IC].instructionCode = (MachineCodeImg_Arr[IC].instructionCode << 4) + inst_field.funcword;

	/*2 bits source address type  */
	MachineCodeImg_Arr[IC].instructionCode = (MachineCodeImg_Arr[IC].instructionCode << 2) + inst_field.src_type;

	/*2 bits destination address type  */
	MachineCodeImg_Arr[IC].instructionCode = (MachineCodeImg_Arr[IC].instructionCode << 2) + inst_field.dst_type;
	
	/*the first word of the instraction is always of link type A*/
	MachineCodeImg_Arr[IC].linkType = A;

}


/* Get an instructionFields variable and reset it */
void ResetCommand(instructionFields *machineCommand)
{
	machineCommand->opword = opcodeErr;
	machineCommand->funcword = funcErr;	
	machineCommand->dst_type = 0;
	machineCommand->src_type = 0;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/


/* This function writes all the instruction between the first address and the end address to the object file
 * Input - firstAddress - the address to start with
 * 		   endAddress - end address*/
void WriteInstructionToFile(int firstAddress, int endAddress)
{
	int i;
	for (i = firstAddress; i < endAddress; ++i)
	{
		ObjectFileWriteLine(i, MachineCodeImg_Arr[i].instructionCode, MachineCodeImg_Arr[i].linkType);
	}
}



