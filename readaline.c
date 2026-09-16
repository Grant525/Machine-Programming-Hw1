#include <stdlib.h>
#include <stdio.h>


size_t readaline(FILE *inputfd, char **datapp){
    (void) inputfd;
    //error checking 
    if (datapp == NULL){
        fprintf(stderr, "Runtime Error: readaline passed a NULL ptr\n");
        exit(EXIT_FAILURE);
    }
    //eof check
    if(feof(inputfd)){
        *datapp = NULL;
        return 0;
    }
    //step 6: memory allocation + failure check
    *datapp = malloc(1000 * sizeof(char)); //TODO: ask if this is acceptable re: idioms
    if (datapp == NULL){
        fprintf(stderr, "Runtime Error: memory allocation failed for datapp\n");
        exit(EXIT_FAILURE);
    }
    //step 7: While loop to read in chars char by char
    int c;
    int num_chars = 0;
    while ((c = fgetc(inputfd)) != EOF && num_chars < 1000){
        printf("%c", (char) c);
    }
    printf("worked\n");
    return 1;
}