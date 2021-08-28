
#ifndef SYMBOLTEABLE_H_
#define SYMBOLTEABLE_H_

#include "definitions.h"

/*the symble table is managed as a linked list*/
/*Initialize ead node and set pointer of root node
 *In case of error exit from the program and print error message
 */
void InitializeSymbolTable();

/* Insert node to the tree
 * Input:
 *	1. Label name
 *	2. If label is External(true or false)
 *	3. Address of IC/DC
 *	4. Row number from the provided input
 * Output - in case of success return 1
 *	 - in case of failure return 0 and print error message
 */
int InsertToSymbolTable (char *label, int addr, bool isCod, bool Dat, bool isEx);

/* Search for node according to the label value
 * Input - pointer to array of char that present label name
 		check - used in insert function when looking for duplication in labels
 * Output -  in case of label name match return pointer to the node in  list
 *		in case label name wasn't found in the tree print error message 
 */
symbol* FindSymbol (char *label, bool check);

/* Update address filed of a relevant node => add to it the offset the function gets
 * Input - offset - the number to add to all the relevant nodes */
void UpdateSymbolTable(int offset);

/*free allocated memory for every node*/
void FreeSymbolTable();

#endif /* SYMBOLTEABLE_H_ */


