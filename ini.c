// Contains used functions functions

#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

sectionData *createHolder()
{
    /* Since a new holder starts as an empty array,
     * no memory is needed.
     */
     return NULL;
}

// TODO - functions that save data to respective fields in struct and tokenize



int readIni(char *filePath, sectionData *sectionArray)
{
    FILE *fp = fopen(filePath, "r");
    if (!fp) {
        printf("\nError: No such file (%s)\n", filePath);
        return 7;
    }

    char *buf;
    int bufsize = 100; // Will be increased if a longer line is encountered
    if ((buf = malloc(bufsize * sizeof(char))) == NULL) {
        printf("Error - did not allocate memory for buffer\n");
        return 1;
        }

    while (fgets(buf, bufsize, fp) != NULL)
    {
        // Check if all line was read, if not, increase buffer till it was
        // The size of buffer will be increased permanently in case of new long lines
        while (buf[strlen(buf)] != '\n') {
            bufsize = bufsize + 100;
            if ((buf = realloc(buf, bufsize * sizeof(char))) == NULL) {
                printf("Error - did not reallocate memory for greater buffer\n");
                return 1;
            }
            // Temporary buffer, to which additional line is read
            char *tempbuf;
            if ((tempbuf = malloc(100 * sizeof(char))) == NULL) {
                printf("Error - did not alloocate memory for tempbuf\n");
                return 1;
            }
            if (fgets(tempbuf, 100, fp) == NULL) {
                printf("Error - did not read a longer line\n");
                return 9;
            }
            // Concatenate new, bigger buf with newly read string
            strcat(buf, tempbuf);
            free(tempbuf);
        }
    }
    if (feof(fp)) { printf("Ended reading file\n"); return 0; }

    /* NOTE - don't try to reinvent the wheel.
     * use tokenization instead

    // The file is read char by char in such way:

    // Currently read char, represented as a number from fgetc()
    int currentChar;
    // These keep track of where we are in the code
    bool inSection = false;
    bool inKey = false;
    bool inValue = false;
    // Holder for currently read section, key or value
    // It is of length 100 at first, but will be extended if needed
    char *currentStr;
    if ((currentStr = malloc(100 * sizeof(char))) == NULL) { return 1; }
    // Respective indexes
    int

    // Read till the end of file
    while ((currentChar = fgetc(fp)) != EOF)
    {
        // If a comment is encountered, skip to new line
        if (currentChar == ';') {

            // NOTE - here we could validate the data based on where
            // we are in the code, e.g. if we read a key, but no value
            // has been provided.

            if ((currentChar = fgetc(fp)) == EOF) { break; }
            while (currentChar != '\n') {
                if ((currentChar = fgetc(fp)) == EOF) { break; }
                continue;
            }
            continue; // Skips "\n" and starts from the beginning `while`
        }


    }
    free(currentStr);
    */

    printf("Error - did not reach EOF\n");
    return 9;
}
