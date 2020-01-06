#ifndef __DEFS_H
#define __DEFS_H
/*constants*/
#define LABEL_LEN 31 /* max label name length*/
#define LINE_LEN 80 /* max line length*/
#define WORD_SIZE 12 /*  memory word length*/
#define ERR_MSG_LEN 256 /* max error message length*/
#define MAX_MEM 1024 /* max words in memory */
#define PATH_MAX 2048 /* max file path length*/
#define INST_NAME_LEN 4 /* max instraction name length*/
#define DIR_NAME_LEN 6 /* max directive name length*/

typedef enum {ext_label,data_label,code_label}Label_typ;/*3 label type for label table*/
typedef enum {empty,comment,directive,inst,error}Line_typ;/* line type*/
typedef enum {mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop}Opcode; 
typedef enum {no_oprnd = 0,imm = 1,label = 3,reg = 5}Oprnd_typ; /*operand type / addressing types */
typedef enum {data,string,entry,external}Dir_typ;/* directive types*/
typedef enum {lab=1,lab_reg,imm_lab_reg}Add_typ; /* possible lagal addressing types*/
#endif