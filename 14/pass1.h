
#ifndef PASS1_H_
#define PASS1_H_

/* this function performs the first scan on the source file 
 * arguments - the IC first address and a pointer to the assembly file
 * Output - in case of a failure 0
 * 	in case success 1 
 */
int pass1(FILE *, long *);

#endif
