// Driver program combining all header files

#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Standard format of arguments: $ ./program PATH-TO-INI-FILE.ini section.key
// Enhanced format of arguments: $ ./program PATH-TO-INI-FILE.ini expression "distance.velocity * travel.time"

int main(/*int argc, char *argv[]*/) // NOTE - at some point, there will be need to accept arguments here
{
    // NOTE - this is a workaround to compile the program without using the command line
    int argc;
    char *argv[3 /* SOME SIZE VALUE */];

    char filePath[] = "testing-ini.ini";
    // sections is really just a pointer to the first section
    sectionData *sections;
    sections = createHolder();

    if (readIni(filePath, &sections) != 0) {
        printf("Error - readIni() did not work\n");
        return 0;
    }

    // Stuff done on the file read into the program
/*
    // Just to check - print out all sections, their keys and corresponding values
    sectionData *cSection = NULL;
    keyData *cKey = NULL;
    cSection = sections;
    while (cSection != NULL) {
        printf("[%s]\n", cSection->name);
        cKey = cSection->firstKey;
        while (cKey != NULL) {
            printf("\t%s = ", cKey->name);
            if (cKey->valStr != NULL) {
                printf("\"%s\"\n", cKey->valStr);
            } else {
                printf("%d\n", cKey->valNum);
            }
            cKey = cKey->nextKey;
        }
        cSection = cSection->nextSection;
    }
*/
    if (freeAllSections(&sections) != 0) {
        printf("Error - freeAll() did not work\n");
        return 0;
    }
/*
    // Just to check - try to print out it again
    cSection = sections;
    while (cSection != NULL) {
        printf("[%s]\n", cSection->name);
        cKey = cSection->firstKey;
        while (cKey != NULL) {
            printf("\t%s = ", cKey->name);
            if (cKey->valStr != NULL) {
                printf("\"%s\"\n", cKey->valStr);
            } else {
                printf("%d\n", cKey->valNum);
            }
            cKey = cKey->nextKey;
        }
        cSection = cSection->nextSection;
    }
*/
    return 0;
}
