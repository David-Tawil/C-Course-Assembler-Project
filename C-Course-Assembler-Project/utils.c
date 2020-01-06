#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

/* ---- Base64 Encoding/Decoding Table --- */
const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char int_to_b64(int i){
	return b64[i%64];
}
void * safe_calloc(size_t nobj,size_t size ){
	void * p = calloc(nobj,size);
	if(!p){
		printf("Error alocating memory\n");
		exit(EXIT_FAILURE);
	}
	return p;
}
FILE *safe_fopen(const char*filename,const char *mode){
	FILE *fp = fopen(filename,mode);
	if(!fp){
		printf("Error: coudn't open %s file.\n",filename);
		exit(EXIT_FAILURE);
	}
	return fp;
}