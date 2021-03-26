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
int saveSection(char *sectionName, sectionData *firstSection)
{
    // TODO (saves a new section and allocates a pointer to it in first free pointer
    // place, starting from firstSection)

    return 0;
}

int saveKey(char *keyName, char *keyValue, sectionData *section)
{
    // TODO (allocates memory for a new key and stores pointer to it in first free
    // pointer place, starting from firstSection->firstKey)

    return 0;
}

int readIni(char *filePath, sectionData *firstSection)
{
    FILE *fp = fopen(filePath, "r");
    if (!fp) {
        printf("\nError: No such file (%s)\n", filePath);
        return 7;
    }
    printf("(file opened successfully)\n");

    // Buffer for reading lines from the file
    char *buf;
    int bufsize = 100; // Will be increased if a longer line is encountered
    if ((buf = malloc(bufsize * sizeof(char))) == NULL) {
        printf("Error - did not allocate memory for buffer\n");
        return 1;
    }
    printf("(buffer initialized)\n");

    //

    // Just for testing
    int lineIndex = 1;

    // Stores the last index of given buffer
    size_t lastIndex;
    // Stores index of current section
    int sectionIndex = -1;

    // Reads all the file, processing data line-by-line
    while (fgets(buf, bufsize, fp) != NULL)
    {
        // Just for debugging
        printf("\tLine %d\t", lineIndex);
        lineIndex++;

        // If this line is a comment or blank, reading all of it is not necessary
        // Instead, skips the line right away
        if (buf[0] == ';' || buf[0] == '\n') { printf("(skipped comment or blank line)\n"); continue; }

        // Save the index of the last element of this buffer
        lastIndex = strlen(buf) - 1;

        // Check if all line was read, if not, increase buffer till it was
        // The size of buffer will be increased permanently in case of new long lines
        while (buf[lastIndex] != '\n') {
            bufsize = bufsize + 100;
            if ((buf = realloc(buf, bufsize * sizeof(char))) == NULL) {
                printf("\tError - did not reallocate memory for greater buffer\n");
                return 1;
            }
            // Write the rest of the line to the buffer
            if (fgets(buf + 99, 100, fp) == NULL) {
                printf("\tError - did not read the rest of line\n");
                return 1;
            }
            // Update the lastIndex
            lastIndex = strlen(buf) - 1;
        }
        // To check if reading works
        printf("buf = \"%s\"\n", buf);


    }
    if (feof(fp)) { printf("(ended reading file)\n"); return 0; }

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
