#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "first_pass.h"

/*interns*/
/* update the addresses of the data labeles acording to final IC*/
static void update_label_list(Assembler*asm);
/* create the entryh list*/
static void builed_entry_list(Assembler*asm);
/* insert label to label list in case has label or is extern command*/
static void insert_label_to_label_list(Assembler*asm);
/*extract dir oprnd*/
/*extract oprnd depnding on directive type*/
static void extract_dir_oprnd(Line_info*curr_line);					
/*errors*/
/* check for invalid label name*/
static int invalid_label(Assembler*asm);
/* find a syntax error in inst line */
static int has_syntax_error_inst(Assembler*asm);
/* find a logic error in inst line e.x invalid opernd type*/
static int has_logic_error_inst(Assembler*asm);
/* find a syntax error in directive line */
static int has_syntax_error_dir(Assembler*asm);

void first_pass(Assembler*asmb){
	while(fgets(asmb->curr_line->input,LINE_LEN +1,asmb->file_info->fp)){/*read each line*/
		asmb->curr_line->line_num++;
		if(invalid_label(asmb)) continue;/*handle possible label errors*/
		copy_label(asmb->curr_line->label,asmb->curr_line->input);/*if has no label do nothing*/
		switch(asmb->curr_line->typ = get_line_typ(asmb->curr_line->input)){
			case empty:
			case comment:
				continue;
			case inst:
				if(has_syntax_error_inst(asmb)) continue;
				asmb->curr_line->info->inst->opcode = get_inst_opcode(asmb->curr_line->input);
				asmb->curr_line->info->inst->src_oprnd_typ = get_src_oprnd_typ(asmb->curr_line->input);
				asmb->curr_line->info->inst->dest_oprnd_typ = get_dest_oprnd_typ(asmb->curr_line->input);
				asmb->curr_line->info->inst->src_oprnd_val = get_src_oprnd_value(asmb->curr_line->input);
				asmb->curr_line->info->inst->dest_oprnd_val = get_dest_oprnd_value(asmb->curr_line->input);
				if(has_logic_error_inst(asmb)) continue;
				insert_label_to_label_list(asmb);		
				asmb->ic += calc_incr_ic(asmb->curr_line->info->inst);/*increment ic as needed*/
				break;
			case directive:
				if(has_syntax_error_dir(asmb)) continue;/*syntax error*/
				asmb->curr_line->info->dir->typ = get_dir_typ(asmb->curr_line->input);
				extract_dir_oprnd(asmb->curr_line);
				insert_label_to_label_list(asmb);
				asmb->dc += calc_incr_dc(asmb->curr_line->info->dir);/*increment dc as needed*/
				break;
			case error:
				insert_error(&asmb->error_list_head,"undefined command name",asmb->curr_line->line_num);
				continue;
				break;
		}
		insert_line_node(&asmb->lines_list_head,asmb->curr_line);
	}
	update_label_list(asmb);/* update the addresses of the data labeles acording to final IC*/
	builed_entry_list(asmb);
}

static void insert_label_to_label_list(Assembler*asm){
	if(asm->curr_line->typ == inst && has_valid_label(asm->curr_line->input))
		insert_label_node(&asm->label_list_head,asm->curr_line->label,asm->ic,code_label);
	if(asm->curr_line->typ == directive && 
	  (asm->curr_line->info->dir->typ == data || asm->curr_line->info->dir->typ == string) &&
	   has_valid_label(asm->curr_line->input) )
		insert_label_node(&asm->label_list_head,asm->curr_line->label,asm->dc,data_label);
	if(asm->curr_line->typ == directive && asm->curr_line->info->dir->typ == external)
		insert_label_node(&asm->label_list_head,asm->curr_line->info->dir->label,0,ext_label);
}
/*extract dir oprnd*/
/*extract evry number of data directive oprnd and put it on arrray*/
static void extract_data_oprnd(Line_info *curr_line);
/*extract evry char of string in string directive oprnd and put it on arrray*/
static void extract_string_oprnd(Line_info *curr_line);
/*in case of entry or extern directive*/
static void extract_label_oprnd(Line_info*curr_line);
static void extract_dir_oprnd(Line_info*curr_line){
	Dir_typ typ =  curr_line->info->dir->typ;
	if(typ == data){ extract_data_oprnd(curr_line); return;}
	if(typ == string){ extract_string_oprnd(curr_line);return;}
	if(typ == external || typ == entry){ extract_label_oprnd(curr_line); return;}
}
static void extract_data_oprnd(Line_info *curr_line){
	int i = 1;
	const char *ptr = get_dir_oprnd_start(curr_line->input);
	do
		curr_line->info->dir->extracted_oprnd[i] = atoi(ptr);
	while((ptr = strchr(ptr,',')) && ptr++ && i++);
	curr_line->info->dir->extracted_oprnd[0] = i;/* the first cell holds how meny numbers we have*/
}
static void extract_string_oprnd(Line_info *curr_line){
	int i;
	const char *ptr = get_dir_oprnd_start(curr_line->input);
	for(i=0,ptr++;*ptr!='\"';ptr++,i++)
		curr_line->info->dir->extracted_oprnd[i]=*ptr;
	curr_line->info->dir->extracted_oprnd[i]=0;/* terminating NULL*/
}
static void extract_label_oprnd(Line_info*curr_line){
	int i;
	const char *ptr = get_dir_oprnd_start(curr_line->input);
	for(i =0 ;isalnum(*ptr);ptr++,i++)
		curr_line->info->dir->label[i] = *ptr;
	curr_line->info->dir->label[i]= '\0';
}
static void update_label_list(Assembler*asm){
	Label *head = asm->label_list_head;
	for(;head;head = head->next)
			if(head->typ == data_label)
				head->address +=asm->ic;
}
static void builed_entry_list(Assembler*asm){
	Line_info *line =asm->lines_list_head;
	for(;line;line = line->next){
		if(line->typ == directive){
			if(line->info->dir->typ == entry){
				if(!is_defined_in_file_label(asm->label_list_head,line->info->dir->label))
					insert_error(&asm->error_list_head,"entry label is not defined on file",line->line_num);
				else
					insert_entry_node(&asm->entry_list_head,line->info->dir->label,get_label_address(asm->label_list_head,line->info->dir->label));
			}
		}
	}	
}
/*error funcs*/
static int invalid_label(Assembler*asm){
	if(has_invalid_label(asm->curr_line->input)){
		insert_error(&asm->error_list_head,"Invalid label name",asm->curr_line->line_num);
		return 1;
	}
	if(has_valid_label(asm->curr_line->input) && is_on_label_list(asm->label_list_head,copy_label(asm->curr_line->label,asm->curr_line->input))){
		insert_error(&asm->error_list_head,"Label already declared in this file ",asm->curr_line->line_num);
		return 1;
	}
	return 0;
}
static int has_syntax_error_dir(Assembler*asm){
	if(!has_valid_dir_oprnd(asm->curr_line->input)){
		insert_error(&asm->error_list_head,"Not valid operand for this directory",asm->curr_line->line_num);
		return 1;
	}
	return 0;
}
static int has_syntax_error_inst(Assembler*asm){
	if(missing_comma(asm->curr_line->input)){
		insert_error(&asm->error_list_head,"Missing comma between operands",asm->curr_line->line_num);
		return 1;
	}
	if(has_more_then_two_oprnds(asm->curr_line->input)){
		insert_error(&asm->error_list_head,"Extrenous text after end of command",asm->curr_line->line_num);
		return 1;
	}
	if(invalid_src_oprnd(asm->curr_line->input)){
		insert_error(&asm->error_list_head,"Invalid source operand",asm->curr_line->line_num);
		return 1;
	}
	if(invalid_dest_oprnd(asm->curr_line->input)){
		insert_error(&asm->error_list_head,"Invalid destination operand",asm->curr_line->line_num);
		return 1;
	}


	return 0;
}
static const int addressing_typ [][2] =  {{imm_lab_reg,lab_reg},
									  	 {imm_lab_reg,imm_lab_reg},
										 {imm_lab_reg,lab_reg},
										 {imm_lab_reg,lab_reg},
										 {no_oprnd,lab_reg},
										 {no_oprnd,lab_reg},
										 {lab,lab_reg},
										 {no_oprnd,lab_reg},
										 {no_oprnd,lab_reg},
										 {no_oprnd,lab_reg},
										 {no_oprnd,lab_reg},
										 {no_oprnd,lab_reg},
										 {no_oprnd,imm_lab_reg},
										 {no_oprnd,lab_reg},
										 {no_oprnd,no_oprnd},
										 {no_oprnd,no_oprnd}};
/* check if an inst line is missing operands*/
static int missing_oprnd(Inst *inst);
/* check if there is too much operand for an inst line*/
static int too_much_oprend(Inst * inst);
/* check for invalid source operand type*/
static int invalid_src_oprnd_typ(Inst * inst);
/* check for invalid dest operand type*/
static int invalid_dest_oprnd_typ(Inst * inst);
static int has_logic_error_inst(Assembler*asm){
	if(missing_oprnd(asm->curr_line->info->inst)){
		insert_error(&asm->error_list_head,"Missing operand",asm->curr_line->line_num);
		return 1;
	}
	if(too_much_oprend(asm->curr_line->info->inst)){
		insert_error(&asm->error_list_head,"Too much operands for this command",asm->curr_line->line_num);
		return 1;
	}
	if(invalid_src_oprnd_typ(asm->curr_line->info->inst)){
		insert_error(&asm->error_list_head,"Invalid source operand type",asm->curr_line->line_num);
		return 1;
	}
	if(invalid_dest_oprnd_typ(asm->curr_line->info->inst)){
		insert_error(&asm->error_list_head,"Invalid destination operand type",asm->curr_line->line_num);
		return 1;	
	}
	return 0;
}
static int missing_oprnd(Inst*inst){
	if(addressing_typ[inst->opcode][0] != no_oprnd && inst->src_oprnd_typ == no_oprnd)
		 return 1;
	if(addressing_typ[inst->opcode][1] != no_oprnd && inst->dest_oprnd_typ == no_oprnd)
		return 1;
	return 0;
}
static int too_much_oprend(Inst * inst){
	if(addressing_typ[inst->opcode][0] == no_oprnd && inst->src_oprnd_typ != no_oprnd)
		return 1;
	if(addressing_typ[inst->opcode][1] == no_oprnd && inst->dest_oprnd_typ != no_oprnd)
		return 1;
	return 0;
}
static int invalid_src_oprnd_typ(Inst * inst){
	if(addressing_typ[inst->opcode][0]== lab && inst->src_oprnd_typ != label)
			return 1;
	return 0;
}
static int invalid_dest_oprnd_typ(Inst *inst){
	if(addressing_typ[inst->opcode][1]== lab_reg && inst->dest_oprnd_typ != label && inst->dest_oprnd_typ != reg)
			return 1;
	return 0;
}