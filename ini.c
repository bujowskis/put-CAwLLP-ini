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

// TODO - REMOVING ELEMENTS ONCE THEY'RE NOT NEEDED

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
    // Stores the first and last index of currently read element, as well as its size
    int elementFirstIndex, elementLastIndex, elementSize;
    // Stores a pointer to the current section
    sectionData *currentSection;

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

        // Here, the line can be either a new section, or a new key

        // Line is a section
        if (buf[bufIndex] == '[') {
            // Read and validate the name
            bufIndex++;
            elementFirstIndex = bufIndex;
            // First character in the section name must be an alphabetic char
            if (isalpha(buf[bufIndex]) == 0) {
                printf("Error - first char of section name must be alphabetic\n");
                return 10;
            }
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
            elementLastIndex = bufIndex - 1;
            elementSize = elementLastIndex - elementFirstIndex + 1;

            // Allocates memory for this section and saves its elements
            sectionData *newSection;
            if ((newSection = malloc(sizeof(sectionData))) == NULL) {
                printf("Error - did not allocate memory for a new section\n");
                return 1;
            }
            char *newSectionName;
            if ((newSectionName = malloc((elementSize + 1) * sizeof(char))) == NULL) { // + 1 for '\0'
                printf("Error - did not allocate memory for the name of new section\n");
                return 1;
            }
            // Put the name together char by char, write '\0' at the end
            for (int i = 0; i < elementSize; i++)
                newSectionName[i] = buf[elementFirstIndex + i];
            newSectionName[elementSize] = '\0';

            // On initialization, a section contains only its name
            newSection->name = newSectionName;
            newSection->firstKey = NULL;
            newSection->nextSection = NULL;

            // Update the current section
            currentSection = newSection;

            // If this is the first section in the file, make it one
            if (firstSection == NULL) {
                firstSection = newSection;
            } else {
                // Seek for the first section with free pointer place
                sectionData *freePlaceSection;
                freePlaceSection = firstSection;
                while (freePlaceSection->nextSection != NULL)
                    freePlaceSection = freePlaceSection->nextSection;
                freePlaceSection->nextSection = newSection;
            }

            // Check if there's something else after section declaration
            if ((bufIndex = skipSpaces(buf, bufIndex + 1)) == -1) { // +1 to compensate for ']'
                printf("Error - did not skip spaces at the end of section declaration\n");
                return 9;
            }
            // Formatting is valid if there is a comment or a new line
            if (buf[bufIndex] != '\n' && buf[bufIndex] != ';') {
                printf("Error - section declaration ends with no comment or new line\n");
                return 10;
            }

            bufIndex = 0;
            continue;
        }

        // Line is a key
        // Reads and validates the key name

        elementFirstIndex = bufIndex;
        // First character in the key name must be an alphabetic char
        if (isalpha(buf[bufIndex]) == 0) {
            printf("Error - first char of key name must be alphabetic\n");
            return 11;
        }
        bufIndex++;
        while (buf[bufIndex] != '=') {
            // ' ' may also indicate an end of the key name
            if (buf[bufIndex] == ' ')
                break;
            // Key name is valid if the rest of it consists of:
            // 1. alphanumeric characters
            // 2. hyphen '-'
            // 3. underscore '_'
            if ((isalnum(buf[bufIndex]) != 0) || (buf[bufIndex] == '-') || (buf[bufIndex] == '_')) {
                bufIndex++;
            } else {
                printf("Error - invalid key name\n");
                return 10;
            }
        }
        elementLastIndex = bufIndex - 1;
        elementSize = elementLastIndex - elementFirstIndex + 1;
        // Stores the name of new key
        char *newKeyName;
        if ((newKeyName = malloc((elementSize + 1) * sizeof(char))) == NULL) { // + 1 for '\0'
            printf("Error - did not allocate memory for the name of new section\n");
            return 1;
        }
        // Put the name together char by char, write '\0' at the end
        for (int i = 0; i < elementSize; i++)
            newKeyName[i] = buf[elementFirstIndex + i];
        newKeyName[elementSize] = '\0';

        // Skips possible spaces to compensate for formatting such that
        // key = value
        // key =value
        // key    =    value etc.
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces after key name\n");
            return 9;
        }
        if (buf[bufIndex] != '=') {
            printf("Error - invalid key declaration (expected '=', got '%c')\n", buf[bufIndex]);
            return 11;
        }
        bufIndex++;
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces after '=' in key declaration\n");
            return 9;
        }

        // Reads and validates the key value
        elementFirstIndex = bufIndex;
        printf("buf[elementFirstIndex] = %c\n", buf[elementFirstIndex]);
        bool isNumber = true;

        // Check if the key value exists
        // First character in the key value must be an alphabetic char
        if (isalnum(buf[bufIndex]) == 0) {
            printf("Error - first char of key value must be alphanumeric\n");
            return 11;
        }
        if (isdigit(buf[bufIndex]) == 0)
            isNumber = false;
        bufIndex++;

        // EOF may also indicate the end of the key value
        // It requires different handling
        if (feof(fp)) {
            // READ KEY IF IT ENDS WITH EOF
            elementLastIndex = (int) lastIndex;

            // NOTE - THERE CAN STILL BE COMMENTS, WHITESPACES, ETC.

            continue;
        }

        // Regular handling
        while (buf[bufIndex] != '\n') {
            // ' ' and ';' may also indicate the end of the key value
            if (buf[bufIndex] == ' ' || buf[bufIndex] == ';')
                break;
            if (isalnum(buf[bufIndex]) == 0) {
                printf("Error - key value can only have alphanumeric characters\n");
                return 11;
            }
            // If there is still possibility it is a number, check it
            if (isNumber == true) {
                if (isdigit(buf[bufIndex]) == 0)
                    isNumber = false;
            }
            bufIndex++;
        }
        elementLastIndex = bufIndex - 1;
        elementSize = elementLastIndex - elementFirstIndex + 1;
        int newKeyvalNum = 0;
        char *newKeyvalStr = NULL;
        // Processes data based on the type of value
        if (isNumber == true) {
            // Adds the numbers with their corresponding base10 multiplier,
            // according to their place
            // NOTE - this works, provided the values are non-negative,
            // which is the case of this task
            int baseMultiplier = 1;
            printf("(for loop starts)\n");
            printf("(elementFirstIndex = %d, elementLastIndex = %d)\n", elementFirstIndex, elementLastIndex);
            for (int i = elementLastIndex; i >= elementFirstIndex; i--) {
                printf("i = %d, baseMultiplier = %d, (int) buf[i] - '0' = %d\n", i, baseMultiplier, (int) buf[i] - '0');
                newKeyvalNum = newKeyvalNum + ((int) buf[i] - '0') * baseMultiplier;
                baseMultiplier = baseMultiplier * 10;
                printf("newKeyvalNum = %d\n", newKeyvalNum);
            }
            printf("(for loop ends)\n");
        } else {
            if ((newKeyvalStr = malloc((elementSize + 1) * sizeof(char))) == NULL) { // +1 for '\0'
                printf("Error - did not allocate memory for string-type key value\n");
                return 1;
            }
            // Put the name together char by char, write '\0' at the end
            for (int i = 0; i < elementSize; i++)
                newKeyvalStr[i] = buf[elementFirstIndex + i];
            newKeyvalStr[elementSize] = '\0';
        }
        // Allocates memory for the newKey, binds the data
        keyData *newKey;
        if ((newKey = malloc(sizeof(keyData))) == NULL) {
            printf("Error - did not allocate memory for a new key\n");
            return 1;
        }
        newKey->name = newKeyName;
        newKey->valStr = newKeyvalStr;
        newKey->valNum = newKeyvalNum;
        newKey->nextKey = NULL;

        // If this is the first key in current section, make it one
        if (currentSection->firstKey == NULL) {
            currentSection->firstKey = newKey;
        } else {
            // Seek for the first key with free pointer place
            keyData *freePlaceKey;
            freePlaceKey = currentSection->firstKey;
            while (freePlaceKey->nextKey != NULL)
                freePlaceKey = freePlaceKey->nextKey;
            freePlaceKey->nextKey = newKey;
        }

        // Check if there's something else after key declaration
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces at the end of section declaration\n");
            return 9;
        }
        // Formatting is valid if there is a comment or a new line
        if (buf[bufIndex] != '\n' && buf[bufIndex] != ';') {
            printf("Error - key declaration ends with no comment or new line\n");
            return 11;
        }

        // Just for testing
        printf("newKey->name = %s\n", newKey->name);
        if (newKey->valStr != NULL) {
            printf("newKey->valStr = \"%s\"\n", newKey->valStr);
        } else {
            printf("newKey->valNum = %d\n", newKey->valNum);
        }

        bufIndex = 0;
    }
    if (feof(fp)) { printf("(ended reading file)\n"); return 0; }

    printf("Error - did not reach EOF\n");
    return 9;
}
