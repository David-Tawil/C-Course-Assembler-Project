#ifndef __DATA_STRUCTURES_H
#define __DATA_STRUCTURES_H
#include <stdio.h>
#include "defs.h"
#include "utils.h"


/*row in label table implemented as linked list*/
typedef struct label_node{
	char label[LABEL_LEN+1];/*LABEL_LEN + '\0'at the end*/
	int address; /*IC or DC or 0 if external*/
	Label_typ typ; /*data/code/extern*/
	struct label_node *next;
}Label;

/*row in entry table implemented as linked list*/
typedef struct entry_node{
	char label[LABEL_LEN +1];/*LABEL_LEN + '\0'at the end*/
	int address;/*must be same as in the label table for the same label name*/
	struct entry_node *next;
}Entry;

/*row in external table implemented as linked list*/
typedef struct extern_node{
	char label[LABEL_LEN +1];/*LABEL_LEN + '\0'at the end*/
	int address;/*occurences in code*/
	struct extern_node *next;
}Extern;

/*list for error messages*/
typedef struct error_node{
	char error_msg[ERR_MSG_LEN];/*error msg text*/
	int line_num;/*line number in original file*/
	struct error_node *next;
}Error;

/*all info needed for instruction command*/
typedef struct inst{
	Opcode opcode; /*int representing opcode to recgonize the instrution */
	Oprnd_typ src_oprnd_typ; /*no oprand / immediate / register/ label*/
	Oprnd_typ dest_oprnd_typ; /*no oprand / immediate / register/ label*/
	int src_oprnd_val; /*value depnding on oprnd type*/
	int dest_oprnd_val; /*value depnding on oprnd type*/
}Inst;
/*all info needed for directive command*/
typedef struct directive_info{
	Dir_typ typ; /*.data/.string/.entry/.extern*/
	int extracted_oprnd[LINE_LEN];/*in case of data dir it will hold the numbers ,in case of string it will hold the char ascii value*/
	char label[LABEL_LEN+1];
}Dir;
/*union of pointers to info structurs*/
typedef union info{
	Inst inst[1];	
	Dir  dir[1];
}Info;
/*all info needed for a line in orginal assembly file*/
typedef struct line_info_node{
	int line_num; /*line number in file*/
	char input[LINE_LEN+1];/*LINE_LEN + '\0' at the end*/
	char label[LABEL_LEN +1];/* "optional", LABEL_LEN + '\0'at the end*/
	Line_typ typ; /*empty/comment/directive/inst*/
	Info info[1]; /*pointer to union of pointers to inst or dir info struct dependig of line typ*/
	struct line_info_node *next; /*next line in file info*/
}Line_info;
/*file info*/
typedef struct file_info{
	char file_name[PATH_MAX];
	FILE *fp;
}File_info;
/*bit field for a word in memory*/
typedef struct word{
	unsigned int word : WORD_SIZE;
}Word;

/*this is the structure that hold everithing the assembler need */
typedef struct assembler{
	int ic;/* inst counter*/
	int dc;/* data counter */
	File_info *file_info;/* cuurent file info*/
	Label *label_list_head;/* label table*/
	Entry *entry_list_head;/* entry table*/
	Extern *extern_list_head;/* extern table*/
	Error *error_list_head;/* error list for posible errors*/
	Line_info *curr_line;/* info for each line*/
	Line_info *lines_list_head;/* list of all line info */
	Word code_segment[MAX_MEM];/* array of words for code memory */
	Word data_segment[MAX_MEM];/* array of words for data memory*/
}Assembler;

/* assembler funcs*/
/*initialize Assembler struct*/
void init_assembler(Assembler ** asm);
/* if there are errors in assembly file*/
int has_error(Assembler*asm);
/* check ifassembler has file open , (needed for fclose)*/
int file_is_open(Assembler*asm);
/* free assembler*/
void free_assembler(Assembler * asm);

/*label list funcs*/
/*insert label to label list*/
void insert_label_node(Label**head_ref,char *label_name,int address,Label_typ typ);
/* check if a label is already in label list*/
int is_on_label_list(Label*head,char*label);
/* check if a label was defined in file in is not external */
int is_defined_in_file_label(Label *head,char*label);
/* get label address for given label, in case of external label return 0*/
int get_label_address(Label*head,char*label);
/* free label list */
void free_label_list(Label *head);

/*entry list funcs*/
/*insert entry label to entry list*/
void insert_entry_node(Entry **head_ref,char*label_name,int address);
/*free entry list*/
void free_entry_list(Entry *head);

/*extern list funcs*/
/* insert extern label to extern list*/
void insert_extern_node(Extern **head_ref,char*label_name,int address);
/* free extern list*/
void free_extern_list(Extern *head);

/*error list funcs*/
/*insert error to list*/
void insert_error(Error **head_ref,char*error_msg,int line_num);
/* print all errors in list*/
void print_errors(Assembler*asm);
/* free error list*/
void free_error_list(Error *head);

/* line info list funcs*/
/* insert current line to the list*/
void insert_line_node(Line_info **head_ref,Line_info *other);
/* free list*/
void free_lines_list(Line_info *head);

/*increment ic && dc for each line*/
/*return how much we need to increment ic for an inst line */
int calc_incr_ic(Inst *i_info);
/*return how much we need to increment dc for a directive line*/
int calc_incr_dc(Dir  *dir);
#endif