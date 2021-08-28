
#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_
/*  */
/* data related strings */
#define DOT_STRING 	".string"
#define DOT_DATA   	".data"
#define DOT_EXTERN 	".extern"
#define DOT_ENTRY  	".entry"


typedef int bool;
#define true 1
#define false 0

#define DOT '.'
#define spacelim " "
#define commalim ","
#define tablim 	"\t"
#define semicolonlim ";"

#define MAX_ROW_LEN 81/*80 bit per row*/
#define MAX_LABEL_LEN 32/*31 bit per label*/

#define CODE_ARR_SIZE 4096/*code image size*/
#define CODE_ADDRESS_BASE 100/*start slot of machine img*/
#define CMD_TABLE_SIZE 16/*number of valid instructions*/  						 

#define ASM_FILE_EXT_LEN 3	/*output file name*/
#define MAX_FILENAME_LEN 256 

/* ----------------------------------------------------- */
/* Instructions opcodes & func - all the instructions the assembler supports */
typedef enum {	mov = 0, cmp=1, op2=2, lea=4, op5=5 , op9=9, red=12, prn=13, rts=14, stop=15, opcodeErr = -1}opcode; 
typedef enum { add=10, sub=11, clr=10, not=11, inc=12, dec=13, jmp=10, bne=11, jsr=12, funcErr=-1, none=0}func;
/* ----------------------------------------------------- */
/* Addresses Types - all the addresses types the assembler supports */
typedef enum { Immediate = 0, Direct, Relative, Register,  Error = -1 } AddresType;
/* ----------------------------------------------------- */
 
/*holds the info of the command of an instruction*/	
 typedef struct command {				
	const char *cmd_str;
	opcode op;
	func fn;
	int operandsInputNum;
}CMDWord;

/*holds the info of the two operands (null if one dowsnt exists)*/
typedef struct Parameter{
	char *first;
	char *second;
}param;


/*skip is a mark for any slot in the array that can be added only in pass2, while we start adding what is known in pass1*/
typedef enum { A, E , R, skip=-1} LINK_TYPE;    					

typedef struct symbol_type {									
	char label[31];
	int address;
	LINK_TYPE lt;/*A R E */
}symbol;

/* This data structure holds all the information about single instruction */
typedef struct {												
	opcode opword;/*opcode 4 bit,11-8*/
	func funcword;/*func 4 bit,7-4*/
	AddresType src_type;/*2 bit,3-2*/
	AddresType dst_type;/*2 bit,1-0*/
}instructionFields;

#endif
