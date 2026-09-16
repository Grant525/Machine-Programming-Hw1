#include <stdlib.h>
#include <stdio.h>

#include "readaline.h"
//Function Declerations
static FILE *open_or_abort(int argc, char *argv[]);


int main(int argc, char *argv[]){
    FILE *fp = open_or_abort(argc, argv);
    char *datap;
    int num_chars = readaline(fp, &datap);
    printf("Readaline returned %d\n", num_chars);
    if(datap == NULL){
        printf("datap was set to NULL\n");
    }
    //free heap memory, close file
    free(datap);
    fclose(fp);
    return EXIT_SUCCESS;
}

/* open_or_abort
Parameters: argc and argv from command line
Returns: a file pointer that either represents the file passed to command line,
or stdin if no file was provided.
Throws a runtime error if more than one filename provided in argv, or filename
provided could not be opened.
*/
static FILE *open_or_abort(int argc, char *argv[]){
    FILE *fp;
    if (argc > 2) {
        fprintf(stderr, "Runtime Error: expected ./restoration filename\n");
        exit(EXIT_FAILURE);
    }
    if (argc == 2) {
        fp = fopen(argv[1], "r");
        //check if the file opened correctly
        if (fp == NULL) {
                fprintf(stderr,
                        "Could not open file %s with mode %s\n",
                        argv[1],
                        "r");
                exit(EXIT_FAILURE);
        }
        printf("case 2\n");
    }
    if (argc == 1) {
        //Move file pointer to stdin
        fp = stdin;
        printf("case 3\n");
    }
    return fp;
}
