/* restoration.c
 * Authors: Willa Andrade wandra01, Grant Caile gcaile01
 * Date: 09/21/2026
 * Assignment: filesofpix
 * Summary: Restores a corrupted "plain" pgm file to a functional "raw" pgm file
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "mem.h"
#include "table.h"
#include "atom.h"

#include "readaline.h"
//Function Declarations
static FILE *open_or_abort(int argc, char *argv[]);
void print_line(void **linepp, void*cl);
void free_line(void **line, void *cl);
void convert_to_clean(void **linepp, void*cl);
void convert_to_raw(void **linepp, void *cl);
void find_width_height(void **linepp, void *cl);
void print_real(void **linepp, void *cl);
void check_for_malloc_err(void *ptr);
void digit_check(void **linepp, char **junk);

struct line {
        size_t length;
        char *orig_cont;
        char *clean_cont;
        char *raw;
        void *nondigits_atom;
};

struct closure {
        void *table;
        const char *infusion_seq_atom;
        int *width;
        int *height;
};


int main(int argc, char *argv[])
{
        FILE *fp = open_or_abort(argc, argv);
        char *datap;
        
        //Each line in the file will be represented as a (struct line *) 
        // in a Hanson list
        List_T lines = List_list(NULL);
        
        //Call readaline for each line in provided file, push (struct line *)
        // representing text from that line onto our Hanson list
        int num_lines = 0;
        int num_chars = readaline(fp, &datap);
        while(num_chars != 0) {
                num_lines++;
                struct line *new_line;
                NEW(new_line);
                new_line->length = num_chars;
                new_line->orig_cont = datap;
                lines = List_push(lines, (void *)new_line);
                
                //last step: increment num_chars + datap
                num_chars = readaline(fp, &datap);
        }
        //List_push pushes to front, so reverse list to get original line order
        lines = List_reverse(lines);
        
        //Set up our closure struct, to be used by find_width_height 
        // and convert_to_clean 
        struct closure *cl_p;
        NEW(cl_p);
        Table_T atom_to_struct = Table_new(0, NULL, NULL);
        cl_p->table = atom_to_struct;
        cl_p->infusion_seq_atom = NULL;
        cl_p->height = malloc(sizeof(int));
        *(cl_p->height) = 0;
        cl_p->width = malloc(sizeof(int));
        *(cl_p->width) = 0;

        //calling convert_to_clean with address of our closure, cl_p
        List_map(lines, convert_to_clean, (void **)cl_p);

        //calling convert_to_raw 
        List_map(lines, convert_to_raw, NULL);

        //calling find_width_height
        List_map(lines, find_width_height, (void **)cl_p);
        
        //print final output: header + raw pgm format
        printf("P5\n%d %d\n255\n", *(cl_p->width), *(cl_p->height));
        List_map(lines, print_real, (void **)cl_p);
        
        //free heap memory, close file
        List_map(lines, free_line, NULL);
        List_free(&lines);
        Table_free(&atom_to_struct);
        free(cl_p->height);
        free(cl_p->width);
        FREE(cl_p);
        fclose(fp);
        return EXIT_SUCCESS;
}


/* print_real
 * Parameters: pointer to pointer to line struct (cast as void), closure closure
 * Output: prints all original, uncorrupted lines in raw pgm format
*/
void print_real(void **linepp, void *cl)
{
        struct line *this_line = (struct line *)(*linepp);
        const char *true_inf_seq = ((struct closure *)cl)->infusion_seq_atom;

        if(this_line->nondigits_atom == true_inf_seq) {
                printf("%s\n", this_line->raw);
        }

}

/* find_width_height
 * Parameters: pointer to pointer to line struct (cast as void), closure 
 *             containing int *width and int*height
 * Output: updates width to the length of an original (uninjected) row
 *         updates height to be the number of original rows
*/
void find_width_height(void **linepp, void *cl)
{
        //local variables, unpacking parameters
        int *w = ((struct closure *)cl)->width;
        int *h = ((struct closure *)cl)->height;
        const char *true_inf_seq = ((struct closure *)cl)->infusion_seq_atom;
        struct line *this_line = (struct line *)(*linepp);
        
        if(this_line->nondigits_atom == true_inf_seq) {
                //set width
                *w = (int)(strlen(this_line->raw));
                //increment height
                *h = *h + 1;
        }
}

/* convert_to_raw
 * Parameters: pointer to pointer to line struct (cast as void), NULL closure
 * Output: for lines with nondigit_atom matching infusion_seq_atom, initializes
 * their member variable char *raw to contain a string where each char 
 * represents an int from clean_cont. 
*/
void convert_to_raw(void **linepp, void *cl)
{
        (void)cl;
        //temp variable and counter variable
        char c;
        int raw_i = 0;
        //strtol variables
        char *startptr = ((struct line *)(*linepp))->clean_cont;
        char *endptr = NULL;
        long int num;
        //allocating an array for raw
        char *rawformat = malloc(1000 * sizeof(char));
        
        //while endptr is not yet at the end of plainpgm
        while(*startptr != '\0') {
                //strtol reads in digits until it reaches a non-digit char
                num = strtol(startptr,&endptr,10);
                
                //if we've reached the end of the string, exit while loop
                if(startptr == endptr) {
                    break;
                } 
                
                //convert integer to char and add to rawformat
                c = (char)num;  
                rawformat[raw_i] = c;
            
                //increment startptr and raw_i
                startptr = endptr;
                raw_i++;
        }
        rawformat[raw_i] = '\0';
        ((struct line *)(*linepp))->raw = rawformat;
}


/* check_for_malloc_err
 * Parameters: void pointer
 * Output: prints to stderr if that pointer is NULL
*/
void check_for_malloc_err(void *ptr)
{
        if (ptr == NULL) { 
                fprintf(stderr, "Runtime Error: memory allocation failed\n");
                exit(EXIT_FAILURE);
        }
}


/* free_line
 * Parameters: pointer to pointer to line struct (cast as void), closure
 * Output: frees all memory inside struct
*/
void free_line(void **linepp, void *cl)
{
        (void)cl;
        struct line *this_line = ((struct line *)(*linepp));
        
        free(this_line->orig_cont);
        free(this_line->clean_cont);
        free(this_line->raw);
        FREE(*linepp);
}

/* digit_check
 * Parameters: pointer to line struct, pointer to junk string (uninitialized)
 * Description: Helper function for convert_to_clean.
 *              Checks if each character in the line's orig_cont is a digit. 
 *              if so, adds it to clean_cont. if not, adds it to junk string    
*/
void digit_check(void **linepp, char **junk)
{
        struct line *this_line = ((struct line *)(*linepp));
        int num_chars = this_line->length;
        // initialize junk and clean_cont
        *junk = malloc(1000 * sizeof(char)); 
        this_line->clean_cont = malloc(1000 * sizeof(char));
        //error checks for malloc
        check_for_malloc_err((void*)(*junk));
        check_for_malloc_err((void*)(this_line->clean_cont));

        //loop through each char in this_line
        int c_i;
        int j_i = 0;
        for(c_i = 0; c_i < num_chars; c_i++) {
                char c = this_line->orig_cont[c_i]; 
                //check if c is a digit (between ASCII 48-57)
                if(48 <= c && c <= 57) {
                        this_line->clean_cont[c_i] = c;
                } else {
                        this_line->clean_cont[c_i] = ' ';
                        (*junk)[j_i] = c;
                        j_i++;
                }
        }
        //add backslash 0 to end of junk and clean_cont to end them
        (*junk)[j_i] = '\0';
        this_line->clean_cont[c_i] = '\0';
}

/* convert_to_clean
 * Parameters: pointer to pointer to line struct (cast as void), NULL closure
 * Output: Initializes nondigits_atom and clean_cont for a line. 
*/
void convert_to_clean(void **linepp, void*cl)
{
        //unpack closure and give easy names to local variables
        struct closure *cl_p = (struct closure *)cl;
        Table_T table = (Table_T)(cl_p->table);
        struct line *this_line = ((struct line *)(*linepp));
        
        char *junk;
        //initialize clean_cont and junk with helper function digit_check
        digit_check((void **)&this_line, &junk);
        //create an atom out of junk and add it to line's nondigits_atom
        const char *infusion = Atom_new(junk, strlen(junk));
        this_line->nondigits_atom = (void *)infusion;
        
        void *ret = NULL;
        ret = Table_put(table, (const void *)this_line->nondigits_atom, 
                            (void *)this_line);
        if(ret != NULL) {
                cl_p->infusion_seq_atom = 
                                (const char *)(this_line->nondigits_atom);
        }
        //free junk, it is not needed outside of this function. 
        free(junk);
}

/* print_line
 * Parameters: pointer to pointer to line struct (cast as void), NULL closure
 * Output: prints out the line's orig_cont
*/
void print_line(void **linepp, void*cl)
{
        (void)cl;
        printf("Original cont:%s\n", ((struct line *)(*linepp))->orig_cont);
        printf("Clean cont:%s\n", ((struct line *)(*linepp))->clean_cont);
}

/* open_or_abort
 * Parameters: argc and argv from command line
 * Returns: a file pointer that either represents the file passed to command 
 * line, or stdin if no file was provided.
 * Throws a runtime error if more than one filename provided in argv, or 
 * filename provided could not be opened.
*/
static FILE *open_or_abort(int argc, char *argv[])
{
        FILE *fp;
        if (argc > 2) {
                fprintf(stderr,
                        "Runtime Error: expected ./restoration filename\n");
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
