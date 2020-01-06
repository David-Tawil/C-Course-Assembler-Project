#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "parsing.h"

/* internal funcs*/
/* this 3 funcs help copying a label to array  */
/*in case the line has a valid label it wiil return pointer to start of label  */
static const char* get_label_start(const char*input);
/*in case the line has a valid label it wiil return pointer to end of label */
static const char* get_label_end(const char*input);
/*in case  the line has a valid label it will return the label length */
static int get_label_len(const char*input);

/*inst functions*/
/*get a line and return 1 if it contains a valid inst*/
 static int has_valid_inst(const char*input);
/*in case is an inst line it will return pointer to start of instruction name */
static const char* get_inst_start(const char*input);
/*in case is an inst line it will return pointer to end of instruction name */
static const char* get_inst_end(const char*input);
/*return length of inst name */
static int get_inst_len(const char*input);

/*inst oprends funcs*/
/*check if line has no oprnds */
static int has_no_oprnd(const char*input);
/*check if line has one oprnds */
static int has_one_oprnd(const char*input);
/*check if line has two oprnds */
static int has_two_oprnd(const char*input); 

/*src oprnd funcs*/
/*if line has source oprnd */
static int has_src_oprnd(const char*input);
/*return pointer to start of source oprnd */
static const char* get_src_oprnd_start(const char*input);
/*return pointer to end of source oprnd */
static const char* get_src_oprnd_end(const char* input);
/*return length of  source oprnd */
static int get_src_oprnd_len(const char* input);

/*dest funcs*/
/*if line has dest oprnd */
static int has_dest_oprnd(const char*input);
/*return pointer to start of dest  oprnd */
static const char* get_dest_oprnd_start(const char*input);
/*return pointer to end of dest oprnd */
static const char* get_dest_oprnd_end(const char*input);
/*return length of  dest oprnd */
static int get_dest_oprnd_len(const char*input);

/*directive func*/
/*return 1 if line has valid directive statment*/
static int has_valid_dir(const char*input);
/*in case is a directive line it will return pointer to start of directive name */
static const char* get_dir_name_start(const char*input);
/*in case is a directive line it will return pointer to end of directive name */
static const char* get_dir_name_end(const char*input);
/*return length of directive name */
static int get_dir_name_len(const char*input);

/*in case is a directive line it wiil return pointer to end of oprnd */
static const char* get_dir_oprnd_end(const char*input);
/*in case  is a directive line it will return the oprnd length */
static int get_dir_oprnd_len(const char*input);
/*check for valid data oprnd */
static int has_valid_data_oprnd(const char*input);
/*check for valid string oprnd */
static int has_valid_string_oprnd(const char*input);
/*check for valid LABEL oprnd for entry and extern dir commands */
static int has_valid_label_oprnd(const char*input);

/*empty and comment line func*/
static int is_empty_line(const char*input);
static int is_comment_line(const char*input);

/*tokens funcs (ALL INTERNAL)*/
/*check if a token is inst name */
static int is_inst_name_token(const char*token);
/*check if a token is directive name */
static int is_dir_name_token(const char*token);
/*get opcode of a token instrution command e.g. "add" */
static Opcode get_opcode_token(const char*token);
/*return type of directive name token */
static Dir_typ get_dir_typ_token(const char*token);
/*get type of oprnd token*/
static Oprnd_typ get_oprnd_typ_token(const char*token);
/*return value of oprend depnding of type oprnd if oprnd is label the value is 0*/
static int get_oprnd_value_token(const char*token);
/*return value for register token e.g. "@r3" will return 3 */
static int get_reg_value_token(const char*token);
/*check if given token is valid reg(with @ prefix)*/
static int is_reg_token(const char*token);
/*check if given token is valid imm(with optional '-' or'+')*/
static int is_immediate_token(const char*token);
/*check if given token is valid label*/
static int is_label_token(const char*token);
/*check if a token is resreved word(help for has_valid_label*/
static int is_resreved_word_token(const char* token);

static const char*  reserved_words [] =
		{"mov","cmp","add","sub","not","clr","lea","inc","dec",
		 "jmp","bne","red","prn","jsr","rts","stop",
		 "data","string","entry","extern",
		 "r0","r1","r2","r3","r4","r5","r6","r7",NULL};

static const char*  directive_names[] ={"data","string","entry","extern"};
static const char* instructions[] = {
	"mov","cmp","add","sub","not","clr","lea","inc",
	"dec","jmp","bne","red","prn","jsr","rts","stop"};

/*line type*/
Line_typ get_line_typ(const char*input){
	if(is_empty_line(input))
		return empty;
	if(is_comment_line(input))
		return comment;
	if(has_valid_inst(input))
		return inst;
	if(has_valid_dir(input))
		return directive;
	return error;
}
/*label funcs*/
int has_valid_label(const char*input){
	char label_name[LINE_LEN];
	char* to = strchr(input,':');
	if(!to) return 0;/* ':' not present*/
	for(;isspace(*input);input++);
	strncpy(label_name,input,to - input);
	label_name[to-input] = '\0';
	return is_label_token(label_name);
}
int has_invalid_label(const char*input){
 	char label_name[LABEL_LEN];
 	const char* from, *to = strchr(input,':');
	if(!to) return 0;/* ':' not present*/
	for(;isspace(*input);input++);
	from = input;
	for(;!isspace(*input) && *input!= ':';input++);
	if(input!=to) return 0;
	strncpy(label_name,from,to - from);
	label_name[to-from] = '\0';
	return !is_label_token(label_name);
 }
 char* copy_label(char* arr,const char*input){
	if(has_valid_label(input)){
			int n = get_label_len(input); 
			strncpy(arr,get_label_start(input),n);
			arr[n]='\0';
			return arr;
		}
	return NULL;
}
/*return input to start or end label name use only after has_valid_label func*/
static const char*  get_label_start(const char*input){
	for(;isspace(*input);input++);/*skipping white characters*/
	return input;
}
static const char*  get_label_end(const char*input){
	return strchr(input,':');
}
static int get_label_len(const char*input){
	return get_label_end(input)-get_label_start(input);
}

/*inst funcs*/
static int has_valid_inst(const char*input){
	char inst_name[INST_NAME_LEN+1];
	int i;
	if(has_valid_label(input)){
		input = get_label_end(input);
		input++;
	}
	for(;isspace(*input);input++);
	for(i=0;i<INST_NAME_LEN && isalpha(*input);i++,input++)
		inst_name[i]=*input;
	if(!isspace(*input))
		return 0;
	inst_name[i]='\0';
	return is_inst_name_token(inst_name);
}
static const char* get_inst_start(const char*input){
	if(has_valid_label(input))
		input = get_label_end(input);
	for(;!isalpha(*input);input++);
	return input;
}
static const char* get_inst_end(const char*input){
	input = get_inst_start(input);
	for(;isalpha(*input);input++);
	return input;
}
static int get_inst_len(const char*input){
	return get_inst_end(input)-get_inst_start(input);
}
Opcode get_inst_opcode(const char*input){
	char inst_name[INST_NAME_LEN+1];
	strncpy(inst_name,get_inst_start(input),get_inst_len(input));
	inst_name[get_inst_len(input)] = '\0';
	return get_opcode_token(inst_name);	
}

/*oprnds funcs*/
static int has_no_oprnd(const char*input){
	input = get_inst_end(input);
	for(;isspace(*input);input++);
	return !*input;/*if reach end of input(end of line)*/
}
static int has_one_oprnd(const char*input){
	if(has_no_oprnd(input))
		return 0;
	input = get_inst_end(input);
	for(;isspace(*input);input++);/*skip white chars*/
	for(;*input && !isspace(*input) && *input!=',';input++);/*get to end of first oprnd*/
	for(;isspace(*input);input++);
	return !*input;/*if reach end of input(end of line) return true*/
}
static int has_two_oprnd(const char*input){
	if(has_no_oprnd(input) || has_one_oprnd(input))
		return 0;
	input = get_inst_end(input);
	for(;isspace(*input);input++);/*skip white chars*/
	for(;*input && !isspace(*input) && *input!=',';input++);/*get to end of first oprnd*/
	for(;isspace(*input);input++);
	if(*input !=',')
		return 0;
	for(input++;isspace(*input);input++);
	for(;*input && !isspace(*input) && *input!=',';input++);/*get to end of second oprnd*/
	for(;isspace(*input);input++);
	return !*input;
}
/* inst syntax errors*/
int has_more_then_two_oprnds(const char*input){
	return !(has_no_oprnd(input) || has_one_oprnd(input) || has_two_oprnd(input));
}
int invalid_src_oprnd(const char*input){
	char src_oprnd[LINE_LEN] = {'\0'};
	if(!has_src_oprnd(input)) return 0;
	copy_src_oprnd(src_oprnd,input);
	return !(is_immediate_token(src_oprnd) || is_reg_token(src_oprnd)|| is_label_token(src_oprnd));
}
int invalid_dest_oprnd(const char*input){
	char dest_oprnd[LINE_LEN] = {'\0'};
	if(!has_dest_oprnd(input)) return 0;
	copy_dest_oprnd(dest_oprnd,input);
	return !(is_immediate_token(dest_oprnd) || is_reg_token(dest_oprnd)|| is_label_token(dest_oprnd));
}
int missing_comma(const char*input){
	input = get_inst_end(input);
	for(;isspace(*input);input++);/*skip white chars*/
	for(;*input && !isspace(*input) && *input!=',';input++);/*get to end of first oprnd*/
	for(;isspace(*input);input++);
	return (*input && *input!=',');
}
/*src oprnd funcs*/
static int has_src_oprnd(const char*input){
	return has_two_oprnd(input);
}
static const char* get_src_oprnd_start(const char*input){
	if(!has_src_oprnd(input))
		return NULL;
	input = get_inst_end(input);
	for(;isspace(*input);input++);
	return input;
}
static const char* get_src_oprnd_end(const char* input){
	input = get_src_oprnd_start(input);
	for(;input && *input && !isspace(*input) && *input != ',';input++);
	return input;
}
static int get_src_oprnd_len(const char* input){
	return get_src_oprnd_end(input)-get_src_oprnd_start(input);
}
void copy_src_oprnd(char*arr,const char*input){
	strncpy(arr,get_src_oprnd_start(input),get_src_oprnd_len(input));
	arr[get_src_oprnd_len(input)]='\0';
}
Oprnd_typ get_src_oprnd_typ(const char*input){
	char src_oprnd[LINE_LEN];
	if(!has_src_oprnd(input))
		return no_oprnd;
	copy_src_oprnd(src_oprnd,input);
	return get_oprnd_typ_token(src_oprnd);
}
int get_src_oprnd_value(const char*input){
	char src_oprnd[LINE_LEN];
	if(!has_src_oprnd(input))
		return 0;
	copy_src_oprnd(src_oprnd,input);
	return get_oprnd_value_token(src_oprnd);
}
/*dest oprnd funcs*/
static int has_dest_oprnd(const char*input){
	return !has_no_oprnd(input);
}
static const char* get_dest_oprnd_start(const char*input){
	if(!has_dest_oprnd(input))
		return NULL;
	if(has_src_oprnd(input)){
		input = get_src_oprnd_end(input);
		for(;*input!=',';input++);
		input++;
	}
	else
		input = get_inst_end(input);
	for(;isspace(*input);input++);
	return input;
}
static const char* get_dest_oprnd_end(const char*input){
	input = get_dest_oprnd_start(input);
	for(;*input && !isspace(*input);input++);
	return input;
}
static int get_dest_oprnd_len(const char*input){
	return get_dest_oprnd_end(input)-get_dest_oprnd_start(input);	
}
void copy_dest_oprnd(char*arr,const char*input){
	strncpy(arr,get_dest_oprnd_start(input),get_dest_oprnd_len(input));
	arr[get_dest_oprnd_len(input)]='\0';
}
Oprnd_typ get_dest_oprnd_typ(const char*input){
	char dest_oprnd[LINE_LEN];
	if(!has_dest_oprnd(input))
		return no_oprnd;
	copy_dest_oprnd(dest_oprnd,input);
	return get_oprnd_typ_token(dest_oprnd);
}
int get_dest_oprnd_value(const char*input){
	char dest_oprnd[LINE_LEN];
	if(!has_dest_oprnd(input))
		return 0;
	copy_dest_oprnd(dest_oprnd,input);
	return get_oprnd_value_token(dest_oprnd);
}

/*directory funcs*/
static int has_valid_dir(const char*input){
	char dir_name[DIR_NAME_LEN+1];
	int i;
	if(has_valid_label(input)){
		input = get_label_end(input);
		input++;
	}
	for(;isspace(*input);input++);
	if(*input!='.')
		return 0;
	for(i=0,input++;i<DIR_NAME_LEN && isalpha(*input);i++,input++)
		dir_name[i]= *input;
	if(!isspace(*input))
		return 0;
	dir_name[i]='\0';
	return is_dir_name_token(dir_name);
}
static const char* get_dir_name_start(const char*input){
	if(has_valid_label(input))
		input = get_label_end(input);
	for(;!isalpha(*input);input++);
	return input;
}
static const char* get_dir_name_end(const char*input){
	input = get_dir_name_start(input);
	for(;isalpha(*input);input++);
	return input;
}
static int get_dir_name_len(const char*input){
	return get_dir_name_end(input)-get_dir_name_start(input);
}
Dir_typ get_dir_typ(const char*input){
	char dir_name[DIR_NAME_LEN+1]; 
	strncpy(dir_name,get_dir_name_start(input),get_dir_name_len(input));
	dir_name[get_dir_name_len(input)] = '\0';
	return get_dir_typ_token(dir_name);	
}
/*dir oprnd*/
const char* get_dir_oprnd_start(const char*input){
	input = get_dir_name_end(input);
	for(;isspace(*input);input++);
	return input;
}
static const char* get_dir_oprnd_end(const char*input){
	input = get_dir_oprnd_start(input);
	for(;*input;input++);
	for(--input;isspace(*input);input--);/*return back to last occurence of none space char*/
	return ++input;
}
static int get_dir_oprnd_len(const char*input){
	return get_dir_oprnd_end(input)-get_dir_oprnd_start(input);
}
static int has_valid_data_oprnd(const char*input){
	char token[LINE_LEN];
	int i;
	input = get_dir_oprnd_start(input);
	do{
		for(;isspace(*input);input++);
		for(i=0;*input && !isspace(*input) && *input!=',';i++,input++)
			token[i] = *input;
		token[i]='\0';
		if(!is_immediate_token(token))
			return 0;
		for(;isspace(*input);input++);
	}while(*input++==',');	
	return !*--input;
}
static int has_valid_string_oprnd(const char*input){
	input = get_dir_oprnd_start(input);
	if(*input!='\"') return 0;
	for(input++;*input!='\"' && isprint(*input);input++);
	if(*input!='\"') return 0;
	for(input++;isspace(*input);input++);
	return !*input;
}
static int has_valid_label_oprnd(const char*input){
	char label_name[LINE_LEN];
	strncpy(label_name,get_dir_oprnd_start(input),get_dir_oprnd_len(input));
	label_name[get_dir_oprnd_len(input)]='\0';
	return is_label_token(label_name);
}
int has_valid_dir_oprnd(const char*input){
	Dir_typ typ = get_dir_typ(input);
	if(typ == data) return has_valid_data_oprnd(input);
	if(typ == string) return has_valid_string_oprnd(input);
	if(typ == external || typ == entry ) return has_valid_label_oprnd(input);
	return 0;
}
/*empty and comment*/
static int is_empty_line(const char*input){
	for(;isspace(*input);input++);
	return !*input;
}
static int is_comment_line(const char*input){
	for(;isspace(*input);input++);
	return *input==';';
}

/*tokens func*/
static int is_immediate_token(const char*token){
	if(!*token)
		return 0;
	if(*token == '-' || *token == '+')
		token++;
	for(;isdigit(*token);token++);
	return !*token;
}
static int is_reg_token(const char*token){
	return (*token=='@' && *(++token) =='r' && isdigit(*(++token)) && *token!='8' && *token!='9' && !*(++token));
}
static int is_label_token(const char*token){
	int i;
	if(!isalpha(*token)|| is_resreved_word_token(token))
		return 0;
	for(i=0;i<LABEL_LEN && isalnum(*token);i++,token++);
	if(!*token)
		return 1;
	return 0;
}
static Opcode get_opcode_token(const char*token){
	int i;
	for(i=0;i<=stop;i++)
		if(!strcmp(instructions[i],token))
			return i;
	printf("error: has no Opcode ,not valid inst\n");
	return 0;
}
static Dir_typ get_dir_typ_token(const char*token){
	int i;
	for(i=0;i<=external;i++)
		if(!strcmp(directive_names[i],token))
			return i;
	return 0;
}
static Oprnd_typ get_oprnd_typ_token(const char*token){
	if(is_immediate_token(token))
		return imm;
	if(is_reg_token(token))
		return reg;
	if(is_label_token(token))
		return label;
	return no_oprnd;	
}
static int get_oprnd_value_token(const char*token){
	if(is_immediate_token(token)){
		return atoi(token);
	}
	if(is_reg_token(token)){
		return get_reg_value_token(token);
	}
	return 0;
}
static int get_reg_value_token(const char*token){
	return *(token+2)-'0';
}
/*check if a token is valid inst name*/
static int is_inst_name_token(const char*token){
	int i;
	for(i=0;i<=stop;i++)
		if(!strcmp(instructions[i],token))
			return 1;
	return 0;
}
static int is_dir_name_token(const char*token){
	int i;
	for(i=0;i<=external;i++)
		if(!strcmp(directive_names[i],token))
			return 1;
	return 0;
}
/*check if a token is same as resrved word*/
static int is_resreved_word_token(const char* token){
	int i;
	for(i=0;reserved_words[i];i++)
		if(!strcmp(reserved_words[i],token))
			return 1;
	return 0;
}