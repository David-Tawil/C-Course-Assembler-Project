#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "first_pass.h"
#include "second_pass.h"



int main(int argc, char const *argv[]){
	Assembler * asmb = NULL;
	init_assembler(&asmb);
	while(--argc){/* iterate through all files*/
		if(file_is_open(asmb)) fclose(asmb->file_info->fp);
		free_assembler(asmb);/*free here since in possible errors continue is performed */
		init_assembler(&asmb);
		/*open file*/
		if(!(asmb->file_info->fp = fopen(strcat(strcpy(asmb->file_info->file_name,argv[argc]),".as"),"r"))){
			printf("coudn't open file %s.\n",asmb->file_info->file_name);
			printf("\n\nFinished procesing of file %s!\n\n\n",strcat(asmb->file_info->file_name,".as") );
			continue;
		}
		printf("\n\nStart procesing of file %s\n\n",asmb->file_info->file_name);
		strcpy(asmb->file_info->file_name,argv[argc]); /*store again the file name without the postfix .as (needed to create files)*/
		first_pass(asmb);/* execute first pass algoritem similar to what is subscribed in mamman */
		if(has_error(asmb)){
			printf("first pass terminate with errors!\n");
			print_errors(asmb);
			printf("\n\nFinished procesing of file %s!\n\n\n",strcat(asmb->file_info->file_name,".as") );
			continue;
		}
		printf("first pass tarminate successfully!\n");
		second_pass(asmb);/* execute second pass algoritem similar to what is subscribed in mamman */
		if(has_error(asmb)){
			printf("second pass terminate with errors!\n");
			print_errors(asmb);
			printf("Finished procesing of file %s!\n\n\n",strcat(asmb->file_info->file_name,".as") );
			continue;
		}
		printf("\n\nFinished procesing of file %s!\n\n\n",strcat(asmb->file_info->file_name,".as") );
	}
	if(file_is_open(asmb)) fclose(asmb->file_info->fp);
	free_assembler(asmb);
	exit(EXIT_SUCCESS);
}