#include <stdlib.h>
#include <string.h>
#include "data_structures.h"

void init_assembler(Assembler ** asm){
	*asm = safe_calloc(sizeof **asm,1);
	(*asm)->ic = 100;
	(*asm)->dc = 0;
	(*asm)->file_info = safe_calloc(sizeof *(*asm)->file_info,1);
	(*asm)->file_info->fp = NULL;
	(*asm)->label_list_head = NULL;
	(*asm)->entry_list_head = NULL;
	(*asm)->extern_list_head = NULL;
	(*asm)->error_list_head = NULL;
	(*asm)->lines_list_head = NULL;
	(*asm)->curr_line = safe_calloc(sizeof *(*asm)->curr_line,1);
	(*asm)->curr_line->line_num = 0;
}
void free_assembler(Assembler * asm){
	free(asm->file_info);
	free(asm->curr_line);
	free_label_list(asm->label_list_head);
	free_entry_list(asm->entry_list_head);
	free_extern_list(asm->extern_list_head);
	free_error_list(asm->error_list_head);
	free_lines_list(asm->lines_list_head);
	free(asm);
}
int has_error(Assembler*asm){
	return !(!(asm->error_list_head));
}
void print_errors(Assembler*asm){
	Error *perr = asm->error_list_head;
	if(perr)
		printf("\nerrors found:\n");
	for(;perr;perr=perr->next)
		printf("in file %s.as: line %d: %s\n",asm->file_info->file_name,perr->line_num,perr->error_msg);

}

void insert_label_node(Label**head_ref,char *label_name,int address,Label_typ typ){
	Label * new = (Label*)safe_calloc(sizeof *new,1);
	Label *last;
	strcpy(new->label,label_name);
	new->address = address;
	new->typ = typ;
	new->next = NULL;
	if(!*head_ref){
		*head_ref = new;
		return;
	}
	for(last = *head_ref;last->next;last = last->next);
	last->next = new;
	return;
}
int is_on_label_list(Label*head,char*label){
	for(;head;head=head->next)
		if(!strcmp(head->label,label))
			return 1;
	return 0;
}
int is_defined_in_file_label(Label *head,char*label){
	for(;head;head=head->next)
		if(!strcmp(head->label,label) && head->typ != ext_label)
			return 1;
	return 0;
}
int get_label_address(Label*head,char*label){
	for(;head;head=head->next)
		if(!strcmp(head->label,label))
			return head->address;
	return -1;
}

void insert_entry_node(Entry **head_ref,char*label_name,int address){
	Entry * new = (Entry*)safe_calloc(sizeof *new,1);
	Entry *last;
	strcpy(new->label,label_name);
	new->address = address;
	new->next = NULL;
	if(!*head_ref){
		*head_ref = new;
		return;
	}
	for(last = *head_ref;last->next;last = last->next);
	last->next = new;
	return;
}

void insert_extern_node(Extern **head_ref,char*label_name,int address){
	Extern * new = (Extern*)safe_calloc(sizeof *new,1);
	Extern *last;
	strcpy(new->label,label_name);
	new->address = address;
	new->next = NULL;
	if(!*head_ref){
		*head_ref = new;
		return;
	}
	for(last = *head_ref;last->next;last = last->next);
	last->next = new;
	return;	
}

void insert_error(Error **head_ref,char*error_msg,int line_num){
	Error * new = (Error*)safe_calloc(sizeof *new,1);
	Error *last;
	strcpy(new->error_msg,error_msg);
	new->line_num = line_num;
	new->next = NULL;
	if(!*head_ref){
		*head_ref = new;
		return;
	}
	for(last = *head_ref;last->next;last = last->next);
	last->next = new;
	return;	
}

void insert_line_node(Line_info **head_ref,Line_info *other){
	Line_info *new = (Line_info*)safe_calloc(sizeof *new,1);
	Line_info * last;
	*new = *other;
	new ->next = NULL;
	if(!*head_ref){
		*head_ref = new;
		return;
	}
	for(last = *head_ref;last->next;last = last->next);
	last->next = new;
	return;	
}

void free_label_list(Label *head){
	Label *next;
	for(;head;head=next){
		next = head->next;
		free(head);
	}
}
void free_entry_list(Entry *head){
	Entry *next;
	for(;head;head=next){
		next = head->next;
		free(head);
	}
}
void free_extern_list(Extern *head){
	Extern *next;
	for(;head;head=next){
		next = head->next;
		free(head);
	}
}
void free_error_list(Error *head){
	Error *next;
	for(;head;head=next){
		next = head->next;
		free(head);
	}
}
void free_lines_list(Line_info *head){
	Line_info *next;
	for(;head;head=next){
		next = head->next;
		free(head);
	}
}
/* check ifassembler has file open , (needed for fclose)*/
int file_is_open(Assembler*asm){
	if(asm == NULL) return 0;
	return !(!asm->file_info->fp);
}
/*increment ic*/
int calc_incr_ic(Inst * inst){
	if(inst->src_oprnd_typ == no_oprnd && inst->dest_oprnd_typ == no_oprnd)
		return 1;
	if(inst->src_oprnd_typ == no_oprnd || (inst->src_oprnd_typ == reg && inst->dest_oprnd_typ == reg))
		return 2;
	return 3;
}
/*increment dc*/
/*return how much we need to increment dc in data directive command*/
static int data_dir_incr_dc(Dir  *dir);
/*return how much we need to increment dc in string directive command*/
static int string_dir_incr_dc(Dir  *dir);
int calc_incr_dc(Dir* dir){
	Dir_typ typ = dir->typ;
	if(typ == string) return string_dir_incr_dc(dir);
	if(typ == data) return data_dir_incr_dc(dir);
	return 0;
}
static int data_dir_incr_dc(Dir *dir){
	return dir->extracted_oprnd[0];
}
static int string_dir_incr_dc(Dir *dir){
	int i;
	for(i=0;dir->extracted_oprnd[i];i++);
	return i+1;
}