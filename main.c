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

    int argc = 3;
    //char *argv[3];

    /* There can be two cases:
     * 1. User wants to see value of a single key
     *      - argc = 3
     *      - argv[1] = filePath
     *      - argv[2] = section.key
     * 2. User wants to evaluate a simple expression
     *    (in form of e.g. "distance.velocity * travel.time")
     *     NOTE - keys are separated from operators by at least 1 whitespace
     *      - argc = 4
     *      - argv[1] = filePath
     *      - argv[2] = "expression"
     *      - argv[3] = expression to be evaluated, as a whole string
     */

    if (argc < 3) {
        printf("Error - excepted at least two arguments - file path and key\n");
        return 0;
    }
    if (argc > 4) {
        printf("Error - too many arguments\n");
        printf("For simple expressions, put the expression inside quotes\n");
        return 0;
    }
/*
    keyArgument *testArgument = malloc(sizeof(keyArgument));
    char testKey[] = "testsection1.testkeystr1";
    if (readArgKey(testKey, &testArgument) != 0) {
        printf("Error - readArgKey() did not work\n");
        free(testArgument);
        return 0;
    }
    */
/* Here goes the expression evaluation test*/

    char testExpression[] = "testsection1.testspace * testsectionEOF.testkeyEOF";
    keyArgument *testFirstArg = malloc(sizeof(keyArgument));
    keyArgument *testSecondArg = malloc(sizeof(keyArgument));

    int operatorId;
    if ((operatorId = readSimpleExpression(testExpression, &testFirstArg, &testSecondArg)) == -1) {
        printf("Error - readSimpleExpression() did not work\n");
        return 0;
    }
    printf("operatorId = %d\n", operatorId);
    //

    char filePath[] = "testing-ini.ini";
    //char filePath[] = "example-4.5.ini";
    //char filePath[] = "testing-empty.ini";
    // sections is really just a pointer to the first section
    sectionData *sections;
    sections = createHolder();

    // NOTE - filePath is going to be argv[1]
    if (readIni(filePath, &sections) != 0) {
        printf("Error - readIni() did not work\n");
//        free(testArgument->keyName);
  //      free(testArgument->sectionName);
    //    free(testArgument);
        return 0;
    }

    searchElement(&(*sections), testFirstArg->sectionName, testFirstArg->keyName, &(testFirstArg->keyPointer));
    searchElement(&(*sections), testSecondArg->sectionName, testSecondArg->keyName, &(testSecondArg->keyPointer));

    printf("Outside of function:\n");
    printf("testFirstArg->keyPointer->valStr = %s\n", testFirstArg->keyPointer->valStr);
    printf("testSecondArg->keyPointer->valStr = %s\n", testSecondArg->keyPointer->valStr);

    // Stuff done on the file read into the program
/*
    if (searchElement(&(*sections), testArgument->sectionName, testArgument->keyName, &(testArgument->keyPointer)) != 0) {
        printf("Error - searchElement() did not work\n");
        if (freeAllSections(&sections) != 0) {
            printf("Error - freeAll() did not work\n");
        }
        free(testArgument->keyName);
        free(testArgument->sectionName);
        free(testArgument);
        return 0;
    }
    printf("Outside of function: %s.%s = %s\n", testArgument->sectionName, testArgument->keyPointer->name, testArgument->keyPointer->valStr);
*/



/*
    keyData *keyAddress = NULL;
    char sectionName[] = "testsection1";
    char keyName[] = "testkeystr1";
    if (searchElement(&(*sections), &(*sectionName), &(*keyName), &keyAddress) != 0) {
        printf("Error - searchElement() did not work\n");
        if (freeAllSections(&sections) != 0) {
            printf("Error - freeAll() did not work\n");
        }
        return 0;
    }
    printf("Outside of function, keyAddress->name = %s\n", keyAddress->name);
*/
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
 //       free(testArgument->keyName);
   //     free(testArgument->sectionName);
     //   free(testArgument);
        return 0;
    }

 //   free(testArgument);
    return 0;
}
