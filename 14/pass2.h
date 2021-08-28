
#ifndef PASS2_H_
#define PASS2_H_

/* This function performs the second scan on the source file.
 * called from first pass after it finishes.
 * arguments - the IC first address and a pointer to the assembly file
 * Output - in case of a failure 0
 * 	in case success 1 
 */
int pass2(FILE *, long );

#endif 
