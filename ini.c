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

int freeKey(keyData **key)
{
    // If the pointer is null, there is nothing to free
    if (*key == NULL)
        return 0;

    // It should be called only for keys with no pointers to other keys
    if ((*key)->nextKey != NULL) {
        printf("Error - cannot free key %s, it points to a next key\n", (*key)->name);
        return 1;
    }
    free((*key)->name);
    free((*key)->valStr);
    free(*key);
    *key = NULL;

    return 0;
}

int freeAllKeys(keyData **firstKey)
{
    // If the pointer is null, there is nothing to free
    if (*firstKey == NULL)
        return 0;

    // Calls itself recursively to free all the keys
    if ((*firstKey)->nextKey != NULL) {
        if (freeAllKeys(&((*firstKey)->nextKey)) != 0) {
            return 1;
        }
    }
    if (freeKey(&(*firstKey)) != 0) {
        printf("Error - did not free all the keys\n");
        return 1;
    }

    return 0;
}

int freeSection(sectionData **section)
{
    // If the pointer is null, there is nothing to free
    if (*section == NULL)
        return 0;

    // It should be called only for sections with no pointers to other sections
    if ((*section)->nextSection != NULL) {
        printf("Error - cannot free section [%s], it points to a next section\n", (*section)->name);
        return 1;
    }

    // Firstly, free all the keys in this section if there are any
    if (freeAllKeys(&((*section)->firstKey)) != 0) {
        printf("Error - did not free section [%s]\n", (*section)->name);
        return 1;
    }
    free((*section)->name);
    free(*section);
    *section = NULL;

    return 0;
}

int freeAllSections(sectionData **firstSection)
{
    // If the pointer is null, there is nothing to free
    if (*firstSection == NULL) {
        return 0;
    }
    // Calls itself recursively to free all the sections
    if ((*firstSection)->nextSection != NULL) {
        if (freeAllSections(&((*firstSection)->nextSection)) != 0) {
            return 1;
        }
    }
    if (freeSection(&(*firstSection)) != 0) {
        printf("Error - did not free all the sections\n");
        return 1;
    }

    return 0;
}

int skipSpaces(char *buf, int startIndex)
{
    if (buf == NULL) {
        printf("\nError in skipSpaces() - invalid buf\n");
        return -1;
    }
    if (startIndex < 0) {
        printf("\nError in skipSpaces() - startIndex cannot be negative\n");
        return -1;
    }
    int index = startIndex;
    while (buf[index] == ' ')
        index++;

    return index;
}

int searchElement(sectionData *firstSection, char *sectionName, char *keyName, keyData **keyAddress)
{
    if (sectionName == NULL) {
        printf("Error - cannot search if no section name is specified\n");
        return 1;
    }
    if (keyName == NULL) {
        printf("Error - cannot search if no key is specified\n");
        return 1;
    }
    if (keyAddress == NULL) {
        printf("Error - cannot search if the keyAdress is not specified\n");
        return 1;
    }
    if (*keyAddress != NULL) {
        printf("Error - cannot search if the keyAddress already points to data\n");
        return 1;
    }

    sectionData *currentSection = firstSection;
    keyData *currentKey = NULL;
    while (currentSection != NULL) {
        if (strcmp(currentSection->name, sectionName) == 0) {
            currentKey = currentSection->firstKey;
            while (currentKey != NULL) {
                if (strcmp(currentKey->name, keyName) == 0) {
                    *keyAddress = currentKey;
                    printf("Element %s.%s is valid\n", sectionName, keyName);
                    return 0;
                } else {
                    currentKey = currentKey->nextKey;
                }
            }
            printf("Error - no such key %s\n", keyName);
            return 1;
        } else {
            currentSection = currentSection->nextSection;
        }
    }
    printf("Error - no such section [%s]\n", sectionName);
    return 1;
}

int readIni(char *filePath, sectionData **firstSection)
{
    if (*firstSection != NULL) {
        printf("Error - trying to read another ini file into already existing ini file\n");
        return 9;
    }

    printf("(started reading file)\n");
    FILE *fp = fopen(filePath, "r");
    if (!fp) {
        printf("\nError: No such file (%s)\n", filePath);
        return 7;
    }

    // Buffer for reading lines from the file
    char *buf = NULL;
    int bufsize = 100; // Will be increased if a longer line is encountered
    if ((buf = malloc(bufsize * sizeof(char))) == NULL) {
        printf("Error - did not allocate memory for buffer\n");
        fclose(fp);
        return 1;
    }

    // Stores the last index of given buffer
    size_t lastIndex;
    // Keeps track of the position inside buffer
    int bufIndex = 0;
    // Stores the first, last index and the size of currently read element
    int elementFirstIndex, elementLastIndex, elementSize;
    // Pointers used to read the data
    sectionData *currentSection = NULL;
    sectionData *newSection = NULL;
    char *newSectionName = NULL;
    char *newKeyName = NULL;
    char *newKeyvalStr = NULL;
    keyData *newKey = NULL;

    // Reads all the file, processing data line-by-line
    while (fgets(buf, bufsize, fp) != NULL)
    {
        // If this line is a comment or blank, reading it is unnecessary
        if (buf[0] == ';' || buf[0] == '\n') { continue; }

        // After each reading, all pointers except currentSection must
        // forget what they were pointing to to prevent data overriding
        // All possible "forgotten" data will still be freed in case of
        // errors using freeAllSections()

        newSection = NULL;
        newSectionName = NULL;
        newKeyName = NULL;
        newKeyvalStr = NULL;
        newKey = NULL;

        // Saves the index of the last element of this buffer
        lastIndex = strlen(buf) - 1;

        // Checks if all line was read - if not, increases buffer till it was
        // The size of buffer is increased permanently in case of more long lines
        while (buf[lastIndex] != '\n') {
            // The end of line might also be EOF, which is also valid
            if (feof(fp)) { break; }
            bufsize = bufsize + 100;
            if ((buf = realloc(buf, bufsize * sizeof(char))) == NULL) {
                printf("\tError - did not reallocate memory for greater buffer\n");
                freeAllSections(firstSection);
                fclose(fp);
                return 1;
            }
            // Write the rest of the line to the buffer
            if (fgets(buf + 99, 100, fp) == NULL) {
                printf("\tError - did not read the rest of line\n");
                freeAllSections(firstSection);
                free(buf);
                fclose(fp);
                return 1;
            }
            // Update the lastIndex
            lastIndex = strlen(buf) - 1;
        }
        // Skip possible spaces at the beginning of buf
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces at the beginning of buf\n");
            freeAllSections(firstSection);
            free(buf);
            fclose(fp);
            return 9;
        }

        // Here, the line can be either a new section, or a new key

        // Line is a section
        if (buf[bufIndex] == '[') {
            // Reads and validates the name
            bufIndex++;
            elementFirstIndex = bufIndex;
            // First character in the section name must be an alphabetic char
            if (isalpha(buf[bufIndex]) == 0) {
                printf("Error - first char of section name must be alphabetic\n");
                freeAllSections(firstSection);
                free(buf);
                fclose(fp);
                return 10;
            }
            bufIndex++;
            while (buf[bufIndex] != ']' && bufIndex <= (int) lastIndex) {
                // Section name is valid if the rest of it consists of:
                // 1. alphanumeric characters
                // 2. hyphen '-'
                // 3. underscore '_'
                if ((isalnum(buf[bufIndex]) != 0) || (buf[bufIndex] == '-') || (buf[bufIndex] == '_')) {
                    bufIndex++;
                } else {
                    printf("Error - invalid section name\n");
                    freeAllSections(firstSection);
                    free(buf);
                    fclose(fp);
                    return 10;
                }
            }
            if (buf[bufIndex] != ']') {
                printf("Error - expected ']', got EOF instead\n");
                freeAllSections(firstSection);
                free(buf);
                fclose(fp);
                return 10;
            }
            elementLastIndex = bufIndex - 1;
            elementSize = elementLastIndex - elementFirstIndex + 1;

            if ((newSection = malloc(sizeof(sectionData))) == NULL) {
                printf("Error - did not allocate memory for a new section\n");
                freeAllSections(firstSection);
                free(buf);
                fclose(fp);
                return 1;
            }

            if ((newSectionName = malloc((elementSize + 1) * sizeof(char))) == NULL) { // + 1 for '\0'
                printf("Error - did not allocate memory for the name of new section\n");
                freeAllSections(firstSection);
                free(buf);
                free(newSection);
                fclose(fp);
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

            currentSection = newSection;

            // If this is the first section in the file, make it one
            if (*firstSection == NULL) {
                *firstSection = newSection;
            } else {
                // Seek for the first section with free pointer place
                // Check if section with the same name already occurred
                sectionData *freePlaceSection = NULL;
                freePlaceSection = *firstSection;
                while (freePlaceSection->nextSection != NULL) {
                    if (strcmp(freePlaceSection->name, newSection->name) == 0) {
                        printf("Error - declaring a new section with already taken name\n");
                        freeAllSections(firstSection);
                        free(buf);
                        free(newSection);
                        free(newSectionName);
                        fclose(fp);
                        return 10;
                    }
                    freePlaceSection = freePlaceSection->nextSection;
                }
                if (strcmp(freePlaceSection->name, newSection->name) == 0) {
                    printf("Error - declaring a new section with already taken name\n");
                    freeAllSections(firstSection);
                    free(buf);
                    free(newSection);
                    free(newSectionName);
                    fclose(fp);
                    return 10;
                }
                freePlaceSection->nextSection = newSection;
            }

            // Check if there's something else after section declaration
            if ((bufIndex = skipSpaces(buf, bufIndex + 1)) == -1) { // +1 to compensate for ']'
                printf("Error - did not skip spaces at the end of section declaration\n");
                freeAllSections(firstSection);
                free(buf);
                fclose(fp);
                return 9;
            }
            // Formatting is valid if there is a comment or a new line
            if (buf[bufIndex] != '\n' && buf[bufIndex] != ';') {
                // It might also indicate EOF
                if (feof(fp))
                    continue;
                printf("Error - section declaration ends with no comment or new line\n");
                freeAllSections(firstSection);
                free(buf);
                fclose(fp);
                return 10;
            }

            bufIndex = 0;
            continue;
        }

        // Line is a key
        if (currentSection == NULL) {
            printf("Error - trying to assign key before first section\n");
            freeAllSections(firstSection);
            free(buf);
            fclose(fp);
            return 9;
        }

        // Reads and validates the key name
        elementFirstIndex = bufIndex;
        // First character in the key name must be an alphabetic char
        if (isalpha(buf[bufIndex]) == 0) {
            printf("Error - first char of key name must be alphabetic\n");
            freeAllSections(firstSection);
            free(buf);
            fclose(fp);
            return 11;
        }
        bufIndex++;
        while (buf[bufIndex] != '=' && bufIndex <= (int) lastIndex) {
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
                freeAllSections(firstSection);
                free(buf);
                return 11;
                fclose(fp);
            }
        }
        if (buf[bufIndex] != '=' && buf[bufIndex] != ' ') {
            printf("Error - expected '=' after section name, got EOF\n");
            freeAllSections(firstSection);
            free(buf);
            fclose(fp);
            return 10;
        }
        elementLastIndex = bufIndex - 1;
        elementSize = elementLastIndex - elementFirstIndex + 1;

        if ((newKeyName = malloc((elementSize + 1) * sizeof(char))) == NULL) { // + 1 for '\0'
            printf("Error - did not allocate memory for the name of new section\n");
            freeAllSections(firstSection);
            free(buf);
            fclose(fp);
            return 1;
        }
        // Put the name together char by char, write '\0' at the end
        for (int i = 0; i < elementSize; i++)
            newKeyName[i] = buf[elementFirstIndex + i];
        newKeyName[elementSize] = '\0';

        // Skips possible spaces to compensate for formatting such that
        // "key = value"
        // "key =value"
        // "key    =    value" etc.
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces after key name\n");
            freeAllSections(firstSection);
            free(buf);
            free(newKeyName);
            fclose(fp);
            return 9;
        }
        if (buf[bufIndex] != '=') {
            if (bufIndex == (int) lastIndex + 1) {
                printf("Error - expected '=' after key name, got EOF\n");
            } else {
                printf("Error - invalid key declaration (expected '=', got '%c')\n", buf[bufIndex]);
            }
            freeAllSections(firstSection);
            free(buf);
            free(newKeyName);
            fclose(fp);
            return 11;
        }
        bufIndex++;
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces after '=' in key declaration\n");
            freeAllSections(firstSection);
            free(buf);
            free(newKeyName);
            fclose(fp);
            return 9;
        }
        if (bufIndex == (int) lastIndex + 1) {
            printf("Error - expected key value, got EOF\n");
            freeAllSections(firstSection);
            free(buf);
            free(newKeyName);
            fclose(fp);
            return 11;
        }

        // Reads and validates the key value
        elementFirstIndex = bufIndex;
        bool isNumber = true;

        // Check if the key value exists
        // First character in the key value must be an alphanumeric char
        if (isalnum(buf[bufIndex]) == 0) {
            printf("Error - first char of key value must be alphanumeric\n");
            freeAllSections(firstSection);
            free(buf);
            free(newKeyName);
            fclose(fp);
            return 11;
        }
        if (isdigit(buf[bufIndex]) == 0)
            isNumber = false;
        bufIndex++;

        while (buf[bufIndex] != '\n' && bufIndex <= (int) lastIndex) {
            // ' ' and ';' may also indicate the end of the key value
            if (buf[bufIndex] == ' ' || buf[bufIndex] == ';')
                break;
            if (isalnum(buf[bufIndex]) == 0) {
                printf("Error - key value can only have alphanumeric characters\n");
                freeAllSections(firstSection);
                free(buf);
                free(newKeyName);
                fclose(fp);
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

        // Processes data based on the type of value
        if (isNumber == true) {
            // Adds the numbers with their corresponding base10 multiplier,
            // according to their place
            // NOTE - this works, provided the values are non-negative,
            // which is the case of this task
            int baseMultiplier = 1;
            for (int i = elementLastIndex; i >= elementFirstIndex; i--) {
                newKeyvalNum = newKeyvalNum + ((int) buf[i] - '0') * baseMultiplier;
                baseMultiplier = baseMultiplier * 10;
            }
        } else {
            if ((newKeyvalStr = malloc((elementSize + 1) * sizeof(char))) == NULL) { // +1 for '\0'
                printf("Error - did not allocate memory for string-type key value\n");
                freeAllSections(firstSection);
                free(buf);
                free(newKeyName);
                fclose(fp);
                return 1;
            }
            // Put the name together char by char, write '\0' at the end
            for (int i = 0; i < elementSize; i++)
                newKeyvalStr[i] = buf[elementFirstIndex + i];
            newKeyvalStr[elementSize] = '\0';
        }

        if ((newKey = malloc(sizeof(keyData))) == NULL) {
            printf("Error - did not allocate memory for a new key\n");
            freeAllSections(firstSection);
            free(buf);
            free(newKeyName);
            free(newKeyvalStr);
            fclose(fp);
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
            keyData *freePlaceKey = NULL;
            freePlaceKey = currentSection->firstKey;
            while (freePlaceKey->nextKey != NULL)
                freePlaceKey = freePlaceKey->nextKey;
            freePlaceKey->nextKey = newKey;
        }

        // Check if there's something else after key declaration
        if ((bufIndex = skipSpaces(buf, bufIndex)) == -1) {
            printf("Error - did not skip spaces at the end of section declaration\n");
            freeAllSections(firstSection);
            free(buf);
            fclose(fp);
            return 9;
        }
        // Formatting is valid if there is a comment or a new line
        if (buf[bufIndex] != '\n' && buf[bufIndex] != ';') {
            // It might also indicate EOF, which is valid
            if (feof(fp))
                break;
            printf("Error - key declaration ends with no comment or new line\n");
            freeAllSections(firstSection);
            free(buf);
            fclose(fp);
            return 11;
        }

        bufIndex = 0;
    }
    if (feof(fp)) {
        printf("(ended reading file)\n");
        fclose(fp);
        free(buf);
        return 0;
    }

    printf("Error - did not reach EOF\n");
    freeAllSections(firstSection);
    free(buf);
    fclose(fp);
    return 9;
}
