#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "mem.h"

#include "readaline.h"
//Function Declerations
static FILE *open_or_abort(int argc, char *argv[]);
void print_line(void **linepp, void*cl);

struct line {
    size_t length;
    char *orig_cont;
    char *clean_cont;
    char *raw;
    void *nondigits_atom;
};

int main(int argc, char *argv[]){
    FILE *fp = open_or_abort(argc, argv);
    char *datap;
    
    //Step 13: create Hanson list lines and push a line onto it
    List_T lines = List_list(NULL);
    
    //Test: create a struct, set its length and orig_cont, call print_line on it
    struct line *my_line;
    NEW(my_line);
    my_line->length = 5;
    my_line->orig_cont = "hello";

    void *my_line_vp = (void *)my_line;
    printf("try calling print function on one line:\n"); 
    print_line(&my_line_vp, NULL);

   // printf("try mapping print function:\n");
    List_push(lines, my_line_vp);
    //testing step: does push work?
    //try popping, pass List_pop a void ** to store address that will then 
    //be set to point to the first thing that was in list, and now is popped.
    //List_pop returns the list without the first elt. 
    struct line *first_thing_p;
    first_thing_p = malloc(sizeof(struct line *));

    void *first_thing_vp = (void *)first_thing_p;
    
    lines = List_pop(lines, &first_thing_vp);
    printf("The orig_cont of the line that was just popped from our list of \
            lines is: %s\n", first_thing_p->orig_cont);

    //List_map(lines, print_line, NULL);
    

    //Step 11
    int num_chars = readaline(fp, &datap);
    while(num_chars != 0){
    //    printf("num of characters in the line:%d\n", num_chars);
    //    printf("length of datap: %lu\n", strlen(datap));
    //    printf("%s\n", datap);
    //    printf("\nattempting to print out 999 bytes from datap\n");
    //    printf("%.*s\n", 999, datap);
        // struct line new_line;
        // new_line.length = num_chars;
        // new_line.orig_cont = datap;
        // List_push(lines, (void *)&new_line);
        
        //last step: increment num_chars + datap
        num_chars = readaline(fp, &datap);
    }
    // printf("calling map\n");
    // List_map(lines, print_line, NULL);
    //  printf("called map\n");

    //TODO: figure out what to do with this
    // if(datap == NULL){
    //     printf("datap was set to NULL\n");
    // }
    
    //free heap memory, close file
    FREE(my_line);
    free(datap);
    fclose(fp);
    return EXIT_SUCCESS;
}

/* cont_deconstructor
   Parameters: 
*/


/* print_line
   Parameters: pointer to pointer to line struct (cast as void), NULL closure
   Output: prints out the line's orig_cont
*/
void print_line(void **linepp, void*cl){
    (void)cl;
    printf("%s\n", ((struct line *)(*linepp))->orig_cont);
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
    }
    if (argc == 1) {
        //Move file pointer to stdin
        fp = stdin;
    }
    return fp;
}
