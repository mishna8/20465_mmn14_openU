#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SymbolTable.h"


/*the table will be in a linked list data structure*/
/*every link wil have:
	symbol - the lable (which appears only once )
	value - decimal value of IC in the memory 
	attributes - is it code(instruction)?data? is it external?
	next node
*/
char *cmdwords[]={"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};

/* linked list Node */
struct Node
{
	bool isCode; /*attributes*/
	bool isData;
	bool isExtern;
	
	int address;/*value*/
	char *label;/*symbol*/
	
	struct Node *next;
};

typedef struct Node node;


/*prototypes*/
node * getTail();
void UpdateNode(node *,int );
void FreeAllSubNodes(node *);

/*global*/
node * Head;

/*Initialize list and set pointer of head node*/
void InitializeSymbolTable()
{
	Head=(node *) malloc(sizeof(node));
	if(Head==NULL)
	{
		fprintf(stderr,"failed to allocate memory for the node");
		exit(0);
	}
	Head->label=" ";
	Head->address=-1;
	Head->isCode=false;
	Head->isData=false;
	Head->isExtern=false;
	Head->next=NULL;
}


/*findes and returnes the tail the tail*/
node * getTail()
{
	node *nd=Head; 
	while (nd->next != NULL){
		nd=nd->next;
	}
	return nd;
}
/*
 *Insert node to the list
 *Input: Label name, IC/DC Address, attributes
 *Output - in case of success return 1
 *		 - in case of failure return 0 and print error message
 */
int InsertToSymbolTable(char *label, int addr, bool isCod, bool isDat, bool isEx)
{	
	int errflag = false;  
	int cmd=0;
	char lab[MAX_LABEL_LEN]={0};int i=0;
	node *new, *end;
	new = (node *) malloc(sizeof(node));
	/*Check if space allocation was successful*/
	if(new==NULL)
	{
		fprintf(stderr,"failed to allocate memory for the node");
		exit(0);
	}	
			
	/* Check if the label name is not null */
	if (label == NULL)
	{
		errflag = true;
	}
	/*check if label isn't a duplicate*/
	else 
	{	
		if(FindSymbol(label, true)!=NULL){
		fprintf(stdout,"Error, the label is a Duplicate: %s\n",label);
		errflag = true;
		}
	}
	/*check if label is a saved word*/
	for (cmd = 0; cmd <= 16; cmd++ )
	{
		if ( strcmp( cmdwords[cmd], label ) == 0 )
		{
			fprintf(stdout,"warning, the label is a saved word: %s\n",label);
		}
	}
	/*only of the label is acceptable it will insert*/
	if(errflag==false){		
		/*set value for node variable*/
			new->isCode=isCod; /*attributes*/
			new->isData=isDat; 
			new->isExtern=isEx;

			new->address=addr;/*value*/
			/*symbol*/
			while(*label!='\0'){
				lab[i]=*label;
				i++;label++;
			}
			new->label= (char *) malloc(MAX_LABEL_LEN); 
			strcpy(new->label,lab);
	
			new->next=NULL;				
		/*add label to end of list*/	
		end = getTail();
		end->next = new; 

	}
	return !errflag;
}
	

/*Search for node according to the label value
 * Input - label name
 	 check - used in insert function when looking for duplication in labels
 * Output - pointer to the node in  list in case of label name match
 * 	print error message in case label name wasn't found in the list
 */
symbol* FindSymbol(char *label, bool check)
{
	node *result = Head;
	symbol *foundElement = NULL;
										
	/* Go from the had to tail and find the symbol */
	while(foundElement==NULL && result!=NULL)
	{
		/*Check if label match*/ 
		if(strcmp(label, result->label))
		{								
			result = result->next;
		}
		else 
		{								
			foundElement = (symbol*)malloc(sizeof(symbol));

			if(!foundElement)
			{
				fprintf(stderr,"Error can not allocate memory in FindSymbol method, label: %s\n",label);
				exit(1);
			}
			else
			{
				strcpy(foundElement->label, result->label);/*get label*/				
				foundElement->address = result->address;/*get address*/
				/*get link type*/
				/*Check if label is external and define appropriated link type for it */
				if(result->isExtern)
				{
					foundElement->lt = E;
				}
				else
				{
					foundElement->lt = R;
				}
			}
		}
	}
	if(foundElement==NULL)
	{
		if(!check)fprintf(stdout,"Error can not find label: %s \n",label);		
	}

	return foundElement;
}

/*Update address filed of a node
 * Input - node of the labels and offset of the label node address filed
 * */
void UpdateNode(node *node,int offset)
{
	if(node)
	{
		/*Check if the label perform action and is external*/
		if(!node->isCode && !node->isExtern)
		{
			node->address += offset;
		}

		UpdateNode(node->next, offset);
	}
}

/* Here we need to go over the symbol table and update the addresses of all instruction commands
 *  (Add the IC to all of the no instruction & no external ) */
void UpdateSymbolTable(int offset)
{
	UpdateNode(Head, offset);
}


/*Delete all nodes of the list
 * Input - head node of the list
 * Output - print message if deletion process of all tree node finished successfully
 * */
void FreeAllSubNodes(node *currentNode)
{
	if(currentNode == NULL)
	{
		return;
	}

	/*go to the end of the list*/
	if(currentNode->next != NULL)
	{
		FreeAllSubNodes(currentNode->next);
		currentNode->next = NULL;
	}
	/*delete last node*/
	else
	{
		free(currentNode);
		return;
	}
}

/*free allocated memory for every node*/
void FreeSymbolTable()
{
	FreeAllSubNodes(Head);
}

