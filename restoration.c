#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "mem.h"
#include "table.h"
#include "atom.h"

#include "readaline.h"
//Function Declerations
static FILE *open_or_abort(int argc, char *argv[]);
void print_line(void **linepp, void*cl);
void free_line(void **line, void *cl);
void convert_to_clean(void **linepp, void*cl);
void convert_to_raw(void **linepp, void *cl);

struct line {
    size_t length;
    char *orig_cont;
    char *clean_cont;
    char *raw;
    void *nondigits_atom;
};

struct table_atom {
    void *table;
    const char *infusion_seq_atom;
};

int main(int argc, char *argv[]){
    FILE *fp = open_or_abort(argc, argv);
    char *datap;
    
    //Step 13: create Hanson list lines and push a line onto it
    List_T lines = List_list(NULL);
    
    //Step 11
    int num_lines = 0;
    int num_chars = readaline(fp, &datap);
    while(num_chars != 0){
        num_lines++;
        struct line *new_line;
        NEW(new_line);
        new_line->length = num_chars;
        new_line->orig_cont = datap;
        lines = List_push(lines, (void *)new_line);
        
        //last step: increment num_chars + datap
        num_chars = readaline(fp, &datap);
    }
    //push puts things in reverse order by pushing to front, so reverse here
    lines = List_reverse(lines);
    //List_map(lines, print_line, NULL);
    
    //Making a Hanson table, and putting it and a null ptr into our
    //table_atom struct (the closure for convert_to_clean)
    struct table_atom *tap;
    NEW(tap);
    Table_T atom_to_struct = Table_new(0, NULL, NULL);
    tap->table = atom_to_struct;
    tap->infusion_seq_atom = NULL;

    //calling convert_to_clean with address of our closure, tap
    List_map(lines, convert_to_clean, (void **)tap);

    //TODO: figure out what to do with this
    // if(datap == NULL){
    //     printf("datap was set to NULL\n");
    // }
    
    //free heap memory, close file
    List_map(lines, free_line, NULL);
    List_free(&lines);
    Table_free(&atom_to_struct);
    //TODO: confirm this can be deleted. Its a copy of a ptr to an atom
    //free(tap->infusion_seq_atom);
    FREE(tap);
    fclose(fp);
    return EXIT_SUCCESS;
}

/* convert_to_raw
   Parameters: pointer to pointer to line struct (cast as void), NULL closure
   Output: for lines with nondigit_atom matching infusion_seq_atom, initializes
   their member variable char *raw to contain a string where each char 
   represents an int from clean_cont. 
*/
void convert_to_raw(void **linepp, void *cl){
    
}





/* free_line
   Parameters: pointer to pointer to line struct (cast as void), closure
   Output: frees all memory inside struct
*/
void free_line(void **linepp, void *cl){
    (void)cl;
    free(((struct line *)(*linepp))->orig_cont);
    // TODO: uncomment once we've actually put memory on the heap for these
    free(((struct line *)(*linepp))->clean_cont);
    //free(((struct line *)(*linepp))->raw);
    FREE(*linepp);
}

/* convert_to_clean
   Parameters: pointer to pointer to line struct (cast as void), NULL closure
   Output: Initializes nondigits_atom and clean_cont for a line. 
*/
void convert_to_clean(void **linepp, void*cl){
    //unpack closure
    struct table_atom *tap = (struct table_atom *)cl;
    Table_T table = (Table_T)(tap->table);
    //TODO DELETE
    //const char *inf_seq_atom = (const char *)(tap->infusion_seq_atom);
    //local variables
    struct line *this_line = ((struct line *)(*linepp));
    int num_chars = this_line->length;
    // string to hold non-digit chars
    char *junk = malloc(1000 * sizeof(char)); 
    
    //declare array on heap for clean_cont
    this_line->clean_cont = malloc(1000 * sizeof(char));
    //error checks for malloc
    if (junk == NULL){ 
        fprintf(stderr, 
                "Runtime Error: memory allocation failed for junk str\n");
        exit(EXIT_FAILURE);
    }
    if (this_line->clean_cont == NULL){ 
        fprintf(stderr, 
                "Runtime Error: memory allocation failed for junk str\n");
        exit(EXIT_FAILURE);
    }

    //loop through each char in this_line
    int c_i;
    int j_i = 0;
    for(c_i = 0; c_i < num_chars; c_i++){
        char c = this_line->orig_cont[c_i]; 
        //check if c is a digit (between ASCII 48-57)
        if(48 <= c && c <= 57){
            this_line->clean_cont[c_i] = c;
        } else {
            this_line->clean_cont[c_i] = ' ';
            junk[j_i] = c;
            j_i++;
        }
    }
    //add backslash 0 to end of junk and clean_cont to end them
    junk[j_i] = '\0';
    this_line->clean_cont[c_i] = '\0';
    //create an atom out of junk and add it to line's nondigits_atom
    const char *infusion = Atom_new(junk, strlen(junk));
    this_line->nondigits_atom = (void *)infusion;
    
    //TODO DELETE
    // printf("junk sequence: %s\n", junk);
    // printf("clean cont is: %s\n", this_line->clean_cont);

    //free junk
    free(junk);
    //TABLE STUFF come back to this once tested table put
    void *ret = NULL;
    ret = Table_put(table, (const void *)this_line->nondigits_atom, 
                           (void *)this_line);
    if(ret != NULL){
       tap->infusion_seq_atom = (const char *)(this_line->nondigits_atom);
    }
}

/* print_line
   Parameters: pointer to pointer to line struct (cast as void), NULL closure
   Output: prints out the line's orig_cont
*/
void print_line(void **linepp, void*cl){
    (void)cl;
    printf("Original cont:%s\n", ((struct line *)(*linepp))->orig_cont);
    printf("Clean cont:%s\n", ((struct line *)(*linepp))->clean_cont);
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
