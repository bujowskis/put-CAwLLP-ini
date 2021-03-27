// Contains used functions functions

#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

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

// TODO -

int skipSpaces(char *buf, int startIndex)
{
    if (buf == NULL) { printf("\nError in skipSpaces() - invalid buf\n"); return -1; }
    if (startIndex < 0) { printf("\nError in skipSpaces() - startIndex cannot be negative\n"); return -1; }
    int index = startIndex;
    while (buf[index] == ' ')
        index++;

    return index;
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

    // Just for testing
    int lineIndex = 1;

    // Stores the last index of given buffer
    size_t lastIndex;
    // Keeps track of the position inside buffer
    int bufIndex = 0;
    // Store the first and last index of currently read element, as well as its size
    int elementFirstIndex, elementLastIndex, elementSize;

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
            // The end of line might also be EOF, which is also valid
            if (feof(fp))
                break;
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

        // Skip possible spaces at the beginning of buf
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces at the beginning of buf\n");
            return 9;
        }
        printf("(skipped spaces at the beginning of buf)\n");

        // Here, the line can be either a new section, or a new key

        // Line is a section
        if (buf[bufIndex] == '[') {
            // Just for checking
            printf("(detected section)\n");

            // Read and validate the name
            bufIndex++;
            elementFirstIndex = bufIndex;
            // First character in the section name must be an alphabetic char
            if (isalpha(buf[bufIndex]) == 0) {
                printf("Error - first char of section name must be alphabetic\n");
                return 10;
            }
            printf("(first char is valid)\n");
            bufIndex++;
            while (buf[bufIndex] != ']') {
                // Section name is valid if the rest of it consists of:
                // 1. alphanumeric characters
                // 2. hyphen '-'
                // 3. underscore '_'
                if ((isalnum(buf[bufIndex]) != 0) || (buf[bufIndex] == '-') || (buf[bufIndex] == '_')) {
                    bufIndex++;
                } else {
                    printf("Error - invalid section name\n");
                    return 10;
                }
            }
            printf("(the name is valid)\n");
            elementLastIndex = bufIndex - 1;
            elementSize = elementLastIndex - elementFirstIndex + 1;

            // Allocates memory for this section and saves its elements
            sectionData *newSection;
            if ((newSection = malloc(sizeof(sectionData))) == NULL) {
                printf("Error - did not allocate memory for a new section\n");
                return 1;
            }
            printf("(newSection allocated)\n");
            char *newSectionName;
            if ((newSectionName = malloc((elementSize + 1) * sizeof(char))) == NULL) { // + 1 for '\0'
                printf("Error - did not allocate memory for the name of new section\n");
                return 1;
            }
            printf("(newSectionName allocated)\n");
            // Put the name together char by char, write '\0' at the end
            for (int i = 0; i < elementSize; i++)
                newSectionName[i] = buf[elementFirstIndex + i];
            newSectionName[elementSize] = '\0';
            printf("newSectionName = \"%s\"\n", newSectionName);

            // On initialization, a section contains only its name
            newSection->name = newSectionName;
            newSection->firstKey = NULL;
            newSection->nextSection = NULL;
            printf("(newSection fully initialized)\n");

            // If this is the first section in the file, make it one
            if (firstSection == NULL) {
                printf("(detected firstSection)\n");
                firstSection = newSection;

                // To test if section reading works
                printf("firstSection->name = %s\n", firstSection->name);

                continue;
            }

            printf("(not a first section)\n");
            // Else, seek for the first section with free pointer place
            sectionData *freePlaceSection;
            freePlaceSection = firstSection;
            while (freePlaceSection->nextSection != NULL)
                freePlaceSection = freePlaceSection->nextSection;
            freePlaceSection->nextSection = newSection;

            // To test if section reading works
            printf("newSection->name = %s\n", newSection->name);

            continue;
        }

        // Line is a key

        bufIndex = 0;
    }
    if (feof(fp)) { printf("(ended reading file)\n"); return 0; }

    printf("Error - did not reach EOF\n");
    return 9;
}
