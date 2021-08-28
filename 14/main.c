#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "definitions.h"
#include "pass1.h"
#include "pass2.h"
#include "OutputToFiles.h"
#include "SymbolTable.h"
#include "data.h"
#include "code.h"


long IC=CODE_ADDRESS_BASE; /*Instruction Counter*/
 
 /*prototypes*/
 void SetOuputFilesName(char *);
 int AssemblyCompailer(char **);
 
/***************************************************************************************/

/* *argv - file names list
 *  argc -  */
int main(int argc, char *argv[])
{

	int errflag;
	char **filesList = argv;

	/* The first item is the program name */
	filesList++;

	/* Compile assembly file list */
	errflag = !AssemblyCompailer(filesList);

	/* Check if the file exists */
	if (errflag)
	{
		fprintf(stderr, "Error cannot compile files,\n  exit complite :( \n");
		exit(1);
	}
	else puts("end successfuly :)");
	
	return 0;
}


/**************************************************************************************/

int AssemblyCompailer(char **filesList)
{
	int errflag = false;	/* Error flag */
	char fname[MAX_FILENAME_LEN + ASM_FILE_EXT_LEN];
	char *asmName;
	FILE *ifp;

	/* Go over the assemblies files */
	while (*filesList != NULL)
	{
		IC = CODE_ADDRESS_BASE;
		ResetDC();

		/* initialize symbol table */
		InitializeSymbolTable();

		/*get file*/
		strcpy(fname, *filesList);
		SetOuputFilesName(fname);
		asmName= strcat(fname, asmFileExt); 
		LoadAssemblyFile();

		/* Open the file to read */
		ifp = fopen(asmName, "r");
		/* Check if the file exists */
		if ( ifp == NULL )
		{
			fprintf(stderr, "Error unable to open[4] file: %s\n", fname);
			fprintf(stderr, "ERROR %s\n", strerror(errno));
			exit(1);
		}

		/* First Scan */
		if(pass1(ifp, &IC))
		{		
			/* Update the symbol table data addresses */
			UpdateSymbolTable(IC);

			/* sets the file position to the beginning of the file */
			rewind (ifp);

			/* Second scan */
			if(pass2(ifp, CODE_ADDRESS_BASE))
			{		
				/*object file*/			
				ObjectFileWriteLine(IC, GetDC(), skip);
				WriteInstructionToFile(CODE_ADDRESS_BASE, IC);/*in code*/
				WriteDataToFile(IC);/*in data*/
				/* entry file - EntryFileWriteLine - in pass2*/
				/*extern file - - in */
			}
			else
			{
				fprintf(stderr, "Error found on the Second scan \n");
				errflag = true;
			}
		}
		else
		{
			fprintf(stderr, "Error found on the First scan \n");
			errflag = true;
		}

		CloseAllOpenFiles();
		FreeSymbolTable();

		filesList++;
	}

	/* if there was an error delete all the output files if not save them*/
	if(errflag)
	{
		fprintf(stderr, "Error there were some errors in the assembly files, deleting created files\n");
		DeleteAllFiles();
	}

	return !errflag;
}

/* Set the output files prefix */
void SetOuputFilesName(char *file)
{
	char *index = strrchr(file, '/');
	if(!index)
	{
		index = file;
	}
	else
	{
		index++;
	}
	
	SetFilesName(index);
}


/*opens an assembly */


