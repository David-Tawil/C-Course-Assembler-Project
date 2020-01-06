#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "second_pass.h"

/* update label oprends value according to label list and in the same scan also build the extern list*/
static void update_label_oprnds_and_extern_list(Assembler*asm);
/* build memory segment */
static void build_memory(Assembler*asm);
/* create files*/
static void create_object_file(Assembler*asm);
static void create_entry_file(Assembler*asm);
static void create_extern_file(Assembler*asm);

void second_pass(Assembler*asmb){
	asmb->ic=100;
	/* update label oprends value according to label list and in the same scan also build the extern list*/
	update_label_oprnds_and_extern_list(asmb);
	if(has_error(asmb))/*stop second pass if found an error */
		return;
	printf("second pass terminate seccessfully!\ncreating files... done!\n");
	asmb->dc = asmb->ic = 0; /* will be index for memory array*/
	build_memory(asmb);
	create_object_file(asmb);
	create_entry_file(asmb); /* if needed otherwise do nothing*/
	create_extern_file(asmb); /* if needed otherwise do nothing*/
}

static void update_label_oprnds_and_extern_list(Assembler*asm){
	char label_oprnd[LABEL_LEN+1];
	Line_info * curr_line = asm->lines_list_head;
	/* iterate trough all lines*/
	for(;curr_line;curr_line = curr_line->next){
		/* handling only inst lines*/
		if(curr_line->typ == inst){
			if(curr_line->info->inst->src_oprnd_typ == label){
				copy_src_oprnd(label_oprnd,curr_line->input);
				if((curr_line->info->inst->src_oprnd_val = get_label_address(asm->label_list_head,label_oprnd)) == -1)	
					insert_error(&asm->error_list_head,"label on source operand is not declared",curr_line->line_num);	
				if(!get_label_address(asm->label_list_head,label_oprnd))
					insert_extern_node(&asm->extern_list_head,label_oprnd,asm->ic+1);
			}
			if(curr_line->info->inst->dest_oprnd_typ == label){
				copy_dest_oprnd(label_oprnd,curr_line->input);
				if((curr_line->info->inst->dest_oprnd_val = get_label_address(asm->label_list_head,label_oprnd)) == -1)
					insert_error(&asm->error_list_head,"label on destination operand is not declared",curr_line->line_num);	
				if(!get_label_address(asm->label_list_head,label_oprnd)){
					if(curr_line->info->inst->src_oprnd_typ == no_oprnd)
						insert_extern_node(&asm->extern_list_head,label_oprnd,asm->ic+1);
					else		
						insert_extern_node(&asm->extern_list_head,label_oprnd,asm->ic+2);

				}
			}
			asm->ic +=calc_incr_ic(curr_line->info->inst);
		}
	}
}
/* build memory of an inst line */
static void build_memory_inst(Assembler*asm);
/* bulid memory of a data or string dir line*/
static void build_memory_data(Assembler*asm);
/* build first word of an inst line*/
static void build_first_word_inst(Assembler* asm);
/* build second word of an inst line*/
static void build_second_word_inst(Assembler* asm);
/* build third word of an inst line*/
static void build_third_word_inst(Assembler*asm);
/* get source operand code type Absolute-0/External-1/Relocatable-2 */
static int get_src_ARE(Assembler*asm);
/* get dest operand code type Absolute-0/External-1/Relocatable-2 */
static int get_dest_ARE(Assembler*asm);
/* check if an inst line has two register operands*/
static int has_two_reg_oprnd(Inst *inst);
/* check if an inst line is one word in memory*/
static int is_one_word_inst(Inst * inst);
/* check if an inst line is three word in memory*/
static int is_three_word_inst(Inst *inst);

static void build_memory(Assembler*asm){
	free(asm->curr_line);
	asm->curr_line = asm->lines_list_head;
	for(;asm->curr_line;asm->curr_line = asm->curr_line->next){
		if(asm->curr_line->typ == inst)
			build_memory_inst(asm);
		if(asm->curr_line->typ == directive)
			build_memory_data(asm);
	}
}
static void build_memory_data(Assembler*asm){
	Dir_typ typ = asm->curr_line->info->dir->typ;
	if(typ == data || typ == string){
		int i;
		int *data_arr = asm->curr_line->info->dir->extracted_oprnd;
		if(typ==data)
			for(i=1;i<=data_arr[0];i++,asm->dc++)
				asm->data_segment[asm->dc].word = data_arr[i];
		if(typ==string){
			for(i=0;data_arr[i];i++,asm->dc++)
				asm->data_segment[asm->dc].word = data_arr[i];
			asm->data_segment[asm->dc++].word = data_arr[i];
		}
	}
}
static void build_memory_inst(Assembler*asm){
	build_first_word_inst(asm);
	build_second_word_inst(asm);
	build_third_word_inst(asm);
}
static void build_first_word_inst(Assembler* asm){
	asm->code_segment[asm->ic].word = asm->curr_line->info->inst->src_oprnd_typ;
	asm->code_segment[asm->ic].word <<= 4;
	asm->code_segment[asm->ic].word += asm->curr_line->info->inst->opcode;
	asm->code_segment[asm->ic].word <<= 3;
	asm->code_segment[asm->ic].word += asm->curr_line->info->inst->dest_oprnd_typ;
	asm->code_segment[asm->ic].word <<= 2;
	asm->ic +=1;
}
static void build_second_word_inst(Assembler* asm){
	if(!is_one_word_inst(asm->curr_line->info->inst)){
		if(has_two_reg_oprnd(asm->curr_line->info->inst)){
			asm->code_segment[asm->ic].word = asm->curr_line->info->inst->src_oprnd_val;
			asm->code_segment[asm->ic].word <<=5;
			asm->code_segment[asm->ic].word +=asm->curr_line->info->inst->dest_oprnd_val;
			asm->code_segment[asm->ic].word <<=2;
			asm->ic++;
			return;
		}
		if(asm->curr_line->info->inst->src_oprnd_typ == reg){
			asm->code_segment[asm->ic].word = asm->curr_line->info->inst->src_oprnd_val;
			asm->code_segment[asm->ic].word <<=7;
			asm->ic++;
			return;
		}
		if(asm->curr_line->info->inst->src_oprnd_typ != no_oprnd){
			asm->code_segment[asm->ic].word = asm->curr_line->info->inst->src_oprnd_val;
			asm->code_segment[asm->ic].word <<=2;
			asm->code_segment[asm->ic].word += get_src_ARE(asm);
			asm->ic++;
			return;
		} 
		/* if only has dest operand*/
		asm->code_segment[asm->ic].word = asm->curr_line->info->inst->dest_oprnd_val;
		asm->code_segment[asm->ic].word <<=2;
		asm->code_segment[asm->ic].word += get_dest_ARE(asm);
		asm->ic++;
		return;
	
	}

}
static void build_third_word_inst(Assembler*asm){
	if(is_three_word_inst(asm->curr_line->info->inst)){
		asm->code_segment[asm->ic].word = asm->curr_line->info->inst->dest_oprnd_val;
		asm->code_segment[asm->ic].word <<= 2;
		asm->code_segment[asm->ic].word += get_dest_ARE(asm);
		asm->ic++;
	}
}
static int get_src_ARE(Assembler*asm){
	if(asm->curr_line->info->inst->src_oprnd_typ==label){
		char label_oprnd[LABEL_LEN+1];
		copy_src_oprnd(label_oprnd,asm->curr_line->input);
		if(get_label_address(asm->label_list_head,label_oprnd))
			return 2;/* relocatable*/
		else 
			return 1;/*external*/
	}
	return 0;/* absolute*/
}
static int get_dest_ARE(Assembler*asm){
	if(asm->curr_line->info->inst->dest_oprnd_typ==label){
		char label_oprnd[LABEL_LEN+1];
		copy_dest_oprnd(label_oprnd,asm->curr_line->input);
		if(get_label_address(asm->label_list_head,label_oprnd))/*check if is not extern label*/
			return 2; /* relocatable*/
		else 
			return 1;/*external*/
	}
	return 0;/* absolute*/
}
static int has_two_reg_oprnd(Inst*inst){
	return inst->src_oprnd_typ ==reg && inst->dest_oprnd_typ ==reg;
}
static int is_one_word_inst(Inst* inst){
	return inst->opcode >=14; /* for rts and stop commands*/
}
static int is_three_word_inst(Inst*inst){
	return (inst->src_oprnd_typ != no_oprnd  && !has_two_reg_oprnd(inst));/* if has source oprnd ,must have dest */
}
/* creating files funcs*/
static void create_object_file(Assembler*asm){
	char file_name[PATH_MAX];
	int i;
	FILE *fp = safe_fopen(strcat(strcpy(file_name,asm->file_info->file_name),".ob"),"w");
	fprintf(fp,"%d %d\n",asm->ic,asm->dc);
	for(i=0;i<asm->ic;i++){
		fprintf(fp,"%c%c\n",int_to_b64(asm->code_segment[i].word>>6),int_to_b64(asm->code_segment[i].word));
	}
	for(i=0;i<asm->dc;i++){
		fprintf(fp,"%c%c\n",int_to_b64(asm->data_segment[i].word>>6),int_to_b64(asm->data_segment[i].word));
	}
	fclose(fp);
}
static void create_entry_file(Assembler*asm){
	char file_name[PATH_MAX];
	FILE *fp;
	Entry *pent = asm->entry_list_head;
	if(!pent) return;
	fp = safe_fopen(strcat(strcpy(file_name,asm->file_info->file_name),".ent"),"w");
	for(;pent;pent = pent->next){
		fprintf(fp,"%s\t%d\n",pent->label,pent->address);
	}
	fclose(fp);
}
static void create_extern_file(Assembler*asm){
	char file_name[PATH_MAX];
	FILE *fp; 
	Extern *pext = asm->extern_list_head;
	if(!pext) return;
	fp = safe_fopen(strcat(strcpy(file_name,asm->file_info->file_name),".ext"),"w"); 
	for(;pext;pext = pext->next){
		fprintf(fp,"%s\t%d\n",pext->label,pext->address);
	}
	fclose(fp);
}