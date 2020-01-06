#ifndef __PARSING_H
#define __PARSING_H
#include "defs.h"
#include "utils.h"

/*EXTERNAL FUNCTIONS - API*/
/* return type of line , empty/comment/inst/dir/error line*/
Line_typ get_line_typ(const char*input);
/*label functions*/
/*get a line and return 1 if it containes label */
int has_valid_label(const char*input);
/* copy label from input line to array and return pointer to array */
char* copy_label(char *arr,const char*input);
/* inst line  funcs*/
/*get the opcode of an inst line */
Opcode get_inst_opcode(const char*input);
/* copy src operand to a char array given as param*/
void copy_src_oprnd(char*arr,const char*input);
/*get src operand type  */
Oprnd_typ get_src_oprnd_typ(const char*input);
/* return source oprend value in case of label 0 */
int get_src_oprnd_value(const char*input);
/* copy dest operand to a char array given as param*/
void copy_dest_oprnd(char*arr,const char*input);
/*get dest operand type EXTERN FUNC */
Oprnd_typ get_dest_oprnd_typ(const char*input);
/* return dest oprend value in case of label 0 */
int get_dest_oprnd_value(const char*input);
/* inst syntax errors*/
/* check if has invalid labe name*/
int has_invalid_label(const char*input);
/* check if an inst line has more then 2 operands*/
int has_more_then_two_oprnds(const char*input);
/* check if missing comma separator between operands*/
int missing_comma(const char*input);
/*check if source opernd is not immidiate num or label or register*/
int invalid_src_oprnd(const char*input);
/*check if dest opernd is not immidiate num or label or register*/
int invalid_dest_oprnd(const char*input);
/* directive funcs*/
/*check for valid  dir oprnd */
int has_valid_dir_oprnd(const char*input);
/*get the type of a directive line */
Dir_typ get_dir_typ(const char*input);
/*dir oprnd*/
/*in case it is a directive line it wiil return pointer to start of oprenad  */
const char *get_dir_oprnd_start(const char*input);
#endif