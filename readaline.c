/* readaline.c
 * Authors: Willa Andrade wandra01, Grant Caile gcaile01
 * Date: 09/21/2026
 * Assignment: filesofpix
 * Summary: Reads a single line of input from file inputfd, presumed to be 
 *          opened for reading. Returns the number of characters in that line. 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

size_t readaline(FILE *inputfd, char **datapp)
{
        //error checking 
        if (datapp == NULL) {
                fprintf(stderr, "Runtime Error: readaline passed a NULL ptr\n");
                exit(EXIT_FAILURE);
        }
        //eof check
        int c = fgetc(inputfd); 
        if (c == EOF) {
                *datapp = NULL;
                return 0;
        }
        //put c back into input stream so it can be read later
        ungetc(c, inputfd);

        //step 6: memory allocation + failure check
        *datapp = malloc(1000 * sizeof(char)); 
        if (*datapp == NULL) {
                fprintf(stderr, 
                        "Runtime Error: memory allocation failed for datapp\n");
                exit(EXIT_FAILURE);
        }
        
        //step 7: While loop to read in chars char by char
        int char_i = 0;
        while ((c = fgetc(inputfd)) != '\n' && char_i < 999) {
                (*datapp)[char_i] = (char)c;
                char_i++;
        }
        //Step 8: add \0 char, and read through rest of oversize line if necessary
        (*datapp)[char_i] = (char)'\0';
        //printf("length of *datapp after adding backslash0 is %lu\n", strlen(*datapp));
        if(char_i == 999) {
                while (c != '\n') {
                        (c = fgetc(inputfd));
                }
        }
        //num_chars is one more than last index
        return char_i;
}