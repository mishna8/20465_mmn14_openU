
#ifndef OUTPUTTOFILES_H_
#define OUTPUTTOFILES_H_

#include "definitions.h"

#define asmFileExt ".as"

/*creating the assembly format file from the original file 
 * copy the content 
 * close file to reopen to read
 */
int LoadAssemblyFile();

/* Close stream to all open files(object, entry and extension)
 * In case of success return 1, otherwise return 0 and print error
 */
int CloseAllOpenFiles();
/* Delete  all files(object, entry and extension)
 * In case of success return 1, otherwise return 0 and print error
 */
int DeleteAllFiles();

/* Write row into the Object file
 * Input - address code to write
 * Output - write to an Object file IC address and machine code */
void ObjectFileWriteLine(int address, int machineCode, int lt);

/* Write row to an External file
 * Input - pointer to array of chars
 * Output - write to an External file label name and machine code */
void ExternalFileWriteLine(char *label , int address);

/* Write row to an Entry file
 * Input - pointer to array of chars
 * Output - write to an Entry file label name and machine code */
void EntryFileWriteLine(char *label , int address);

/* This function gets the prefix file name for all output files */
void SetFilesName(char *fileName);

#endif
