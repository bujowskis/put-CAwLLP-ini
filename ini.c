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

int readIni(char *filePath, sectionData *holderArray)
{
    FILE *fp = fopen(filePath, "r");
    if (!fp) {
        printf("Error: No such file (%s)\n", filePath);
        return 7;
    }
    char currentChar;

    // The file is read in such way
    while ((currentChar = fgetc(fp)) != EOF)
    {
        if
    }

    return 0;
}
