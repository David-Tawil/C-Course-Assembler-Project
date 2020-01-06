#ifndef __UTILS_H
#define __UTILS_H
/*get an int i and return a char representing the base 64 of i%64 */
char int_to_b64(int i);
/* util for alocating memory , if alocate fail error massge will be printed and the program exit  */
void * safe_calloc(size_t nobj,size_t size);
/* util for opening files , if the opretion failes error massge will be printed and the program exit*/
FILE *safe_fopen(const char*filename,const char *mode);
#endif
