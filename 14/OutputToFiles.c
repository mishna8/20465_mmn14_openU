#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <errno.h>

#include "OutputToFiles.h"

#define obFileExt ".ob"
#define entFileExt ".ent"
#define extFileExt ".ext"


#define FILE_NUMBER 500 	/* Maximum files the program can create */

FILE *Assemblyfp;				/*pointer to the assembly file made from the original*/
FILE *Objectfp;				/*pointer to an object file*/
FILE *Entryfp;				/*pointer to an entry file*/
FILE *Externalfp;			/*pointer to an external file*/

char *createdFiles[FILE_NUMBER];
char outputFileName[MAX_FILENAME_LEN];
int currentFileIndex = 0;



/*allocate memory for new string */
/* Check if the row is only with white spaces
 * Input row - string to check
 * Output - 0 in case of a failure
 * 			1 in case success
 * Allocate memory for new string
 * Input row - size of string(integer)
 * Output - return empty string in case of successful memory allocation
 *			return 0 in case memory allocation is failed */
char* new_string(int size)
{
	char*str = (char*)malloc(sizeof(char)*size);
	if(!str)
	{
		fprintf(stderr, "Error Failed to allocate memory for a new string");
		exit(0);
	}

	return str;
}

/* This function gets the prefix file name for all output files */
void SetFilesName(char *fileName)
{	

	if(fileName)
	{
		strcpy(outputFileName, fileName);
	}
	else
	{
		fprintf(stderr, "Error File name cannot be null ");
	}
}
/*_______________________________________*/

/*creating the assembly format file from the original file 
 * copy the content 
 * close file to reopen to read
 */
int LoadAssemblyFile(){
	
	FILE *ogFile;
	char ch;
	/*Add extension ".as" to the file name*/
	char* FullName = new_string(strlen(outputFileName) + strlen(asmFileExt));
	strcpy(FullName, outputFileName);
	strcat(FullName, asmFileExt);    
	 
    ogFile  = fopen(outputFileName, "r");
    /* fopen() return NULL if unable to open file in given mode. */
    if (ogFile == NULL)
    {
        /* Unable to open file hence exit */
        fprintf(stderr, "Error unable to get[1] file: %s\n", FullName);
	fprintf(stderr, "ERROR %s\n", strerror(errno));
	exit(1);
    }
    
    Assemblyfp    = fopen(FullName,   "w+");
    if (Assemblyfp == NULL)
    {
        /* Unable to open file hence exit */
        fprintf(stderr, "Error unable to get[2] file : %s\n", FullName);
	fprintf(stderr, "ERROR %s\n", strerror(errno));
	exit(1);
    }

    /* Copy file contents character by character.  */
    ch = fgetc(ogFile);
    while (ch != EOF)
    {
        /* Write to destination file */
        fputc(ch, Assemblyfp);

        /* Read next character from source file */
        ch = fgetc(ogFile);
    }

	/*close file to open later in a "r" mode*/
	/* Check if the file need to be close and close it */
	if(!Assemblyfp || (fclose(Assemblyfp)) == 0)
	{
		createdFiles[currentFileIndex] = malloc(strlen(FullName) + 1);
		strcpy(createdFiles[currentFileIndex], FullName);
		currentFileIndex++;
		return 1;
	}
	else
	{
		 fprintf(stderr, "Error unable to ready[3] file: %s\n", FullName);
		return 0;
	}
}

/*--------------------------------------------------------------------------------------------------------*/

/*Close stream to Object file
 * Input  - global pointer "Objectfp" to an object file
 * Output - 0 in case of a failure
 * 			1 in case success*/
int CloseObjectFile()
{
	/* Check if the file need to be close and close it */
	if(!Objectfp || (fclose(Objectfp)) == 0)
	{
		char* FullName;

		/*Set pointer to object file to NULL */
		Objectfp=NULL;

		/* Add the file to the created file list */
		FullName = new_string(strlen(outputFileName) + strlen(obFileExt));
		strcpy(FullName, outputFileName);
		strcat(FullName, obFileExt);

		createdFiles[currentFileIndex] = malloc(strlen(FullName) + 1);
		strcpy(createdFiles[currentFileIndex], FullName);
		currentFileIndex++;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*Close stream to Entry file
 * Input  - global pointer "Entryfp" to an object file
 * Output - 0 in case of failure
 * 			1 in case of success*/
int CloseEntryFile()
{
	/* Check if the file need to be close and close it */
	if(!Entryfp || (fclose(Entryfp)) == 0)
	{
		char* FullName;

		/*Set pointer to entry file to NULL */
		Entryfp=NULL;

		/* Add the file to the created file list */
		FullName = new_string(strlen(outputFileName) + strlen(entFileExt));
		strcpy(FullName, outputFileName);
		strcat(FullName, entFileExt);

		createdFiles[currentFileIndex] = malloc(strlen(FullName) + 1);
		strcpy(createdFiles[currentFileIndex], FullName);
		currentFileIndex++;
		return 1;
	}
	else
	{
		return 0;
	}
}
/*Close stream to External file
 * Input  - global pointer "Externalfp" to an object file
 * Output - 0 in case of failure
 * 			1 in case of success*/
int CloseExternalFile()
{
	/* Check if the file need to be close and close it */
	if(!Externalfp || (fclose(Externalfp)) == 0)
	{
		char* FullName;

		/*Set pointer to external file to NULL */
		Externalfp=NULL;
		/* Add the file to the created file list */
		FullName = new_string(strlen(outputFileName) + strlen(extFileExt));
		strcpy(FullName, outputFileName);
		strcat(FullName, extFileExt);

		createdFiles[currentFileIndex] = malloc(strlen(FullName) + 1);
		strcpy(createdFiles[currentFileIndex], FullName);
		currentFileIndex++;

		return 1;
	}
	else
	{
		return 0;
	}
}

/*Close stream to all open files(object, entry and extension)
 * In case of success return 1, otherwise return 0 and print error
 */
int CloseAllOpenFiles()
{
	if((CloseExternalFile() + CloseEntryFile() + CloseObjectFile()) != 3)
	{
		fprintf(stderr, "Error cannot close files \n");
		return 0;
	}
	
	return 1;
}

/*Delete  all files(object, entry and extension)
 * In case of success return 1, otherwise return 0 and print error
 */
int DeleteAllFiles()
{
	int errflag = false;
	FILE *check;
	int i;
	for(i = 0; i < currentFileIndex; i++)
	{
		/*if file exists*/check=fopen(createdFiles[i],"r");
		if(check!=NULL) {fclose(check); errflag = remove(createdFiles[i]);}/*0 if success = false*/
		if(errflag)
		{
			fprintf(stderr, "problem deleting file %s assembly \n", createdFiles[i]);
			fprintf(stderr, "errno report: %s\n", strerror(errno));
		}
	}
	
	if(errflag==false)fprintf(stderr,"all files deleted succesfully\n");
	
	return !errflag;
}

/*_______________________________________________________________________________________________________________________________*/

/*Open stream to  an Object file
 * Input - Pointer to the Object file
 * Output - In case the file was opened successfully return 1
 * 		    In case the file failed to be opened return 0 
 */
int OpenObjectFile()
{
	/*Add extension ".ob" to the file name*/
	char* FullName = new_string(strlen(outputFileName) + strlen(obFileExt));puts("here");
	strcpy(FullName, outputFileName);
	strcat(FullName, obFileExt);

	/*Open the file for writing*/
	Objectfp = fopen(FullName, "w+");

	/*If the file opened successfully return true else return false */
	if(Objectfp)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*Open stream to  an Entry file
 * Input - Pointer to the Entry file
 * Output - In case the file was opened successfully return 1
 * 		    In case the file failed to be opened return 0 
 */
int OpenEntryFile()
{
	/*Add extension ".ent" to the file name*/
	char* FullName = new_string(strlen(outputFileName) + strlen(entFileExt));
	strcpy(FullName, outputFileName);
	strcat(FullName, entFileExt);

	/*Open the file for writing*/
	Entryfp = fopen(FullName, "w+");

	/*If the file opened successfully return 1 else return 0 */
	if(Entryfp)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*Open stream to  an External file
 * Output - In case the file was opened successfully return 1
 * 		    In case the file failed to be opened return 0 
 */
int OpenExternalFile()
{
	/*Add extension ".ext" to the file name*/
	char* FullName = new_string(strlen(outputFileName) + strlen(extFileExt));
	strcpy(FullName, outputFileName);
	strcat(FullName, extFileExt);

	/*Open the file for writing*/
	Externalfp = fopen(FullName, "w+");

	/*If the file opened successfully return true else return false */
	if(Externalfp)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

/* Write row into the Object file
 * Input - address code to write
 * Output - write to an Object file IC address and machine code 
 */
void ObjectFileWriteLine(int address, int machineCode, int lt)
{ 	
	int i = machineCode;
	int mask = 0x000000000FFF;
	if(!Objectfp && !OpenObjectFile())
    	{
        	fprintf(stderr, "Error cannot create  Object file\n");
    	}
    	else
    	{
        	/*If file is empty write size of data and size of the code tables*/
        	if(ftell(Objectfp) == 0)
        	{
        	    fprintf(Objectfp,"    %d %03X\n",(address-CODE_ADDRESS_BASE),machineCode);
        	}
        	else
        	{   
        	    if(lt==A)fprintf(Objectfp,"%04d\t%03X A\n", address,i&mask);
        	    if(lt==R)fprintf(Objectfp,"%04d\t%03X R\n", address,i&mask);
        	    if(lt==E)fprintf(Objectfp,"%04d\t%03X E\n", address,i&mask);
        	}
    	}	

}

/* Write row to an Entry file
 * Input - pointer to array of chars
 * Output - write to an Entry file label name and machine code 
 */
void EntryFileWriteLine(char *label , int address)
{
	if(!Entryfp && !OpenEntryFile())
	{
		fprintf(stderr, "Error cannot create  Entry file\n");
	}
	else
	{
		fprintf(Entryfp,"%s\t%04d\n", label, address);
	}
}

/* Write row to an External file
 * Input - pointer to array of chars
 * Output - write to an External file label name and machine code 
 */
void ExternalFileWriteLine(char *label , int address)
{
	if(!Externalfp && !OpenExternalFile())
	{
		fprintf(stderr, "Error cannot create  External file\n");
	}
	else
	{
		fprintf(Externalfp,"%s\t%04d\n", label, address);
	}
}

