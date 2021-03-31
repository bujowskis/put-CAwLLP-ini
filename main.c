// Driver program combining all header files

#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <float.h>

// Standard format of arguments: $ ./program PATH-TO-INI-FILE.ini section.key
// Enhanced format of arguments: $ ./program PATH-TO-INI-FILE.ini expression "distance.velocity * travel.time"

int main(int argc, char *argv[]) // NOTE - at some point, there will be need to accept arguments here
{
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
     *      - argv[3] = simple expression to be evaluated, as a whole string
     */

    if (argc < 3) {
        printf("Error - excepted at least two arguments - file path and key\n");
        return 0;
    }
    if (argc > 4) {
        printf("Error - too many arguments\n");
        printf("For simple expressions, put the whole expression inside quotes\n");
        return 0;
    }

    keyArgument *keyArg1 = NULL;
    keyArgument *keyArg2 = NULL;
    int operatorID;

    // At least one key will be needed regardless of expression
    if ((keyArg1 = malloc(sizeof(keyArgument))) == NULL) {
        printf("Error - did not allocate memory for the first key\n");
        return 0;
    }
    keyArg1->sectionName = NULL;
    keyArg1->keyName = NULL;
    keyArg1->keyPointer = NULL;

    if (argc == 3) {
        // Expecting a single key to be printed
        if (readArgKey(argv[2], &keyArg1) != 0) {
            printf("Error - did not read the key into keyArgument\n");
            freekeyArg(&keyArg1);
            free(keyArg1);
            return 0;
        }
    } else {
        // Expecting a simple expression to be evaluated
        if ((keyArg2 = malloc(sizeof(keyArgument))) == NULL) {
            printf("Error - did not allocate memory for the second key\n");
            freekeyArg(&keyArg1);
            free(keyArg1);
            return 0;
        }
        keyArg2->sectionName = NULL;
        keyArg2->keyName = NULL;
        keyArg2->keyPointer = NULL;

        if (strcmp(argv[2], "expression") != 0) {
            printf("Error - expected \"expression\" keyword declaration\n");
            printf("Proper arguments formatting: filePath expression \"section1.key1 <operator> section2.key2\"\n");
            return 0;
        }

        if ((operatorID = readSimpleExpression(argv[3], &keyArg1, &keyArg2)) == -1) {
            printf("Error - could not read simple expression\n");
            freekeyArg(&keyArg1);
            free(keyArg1);
            freekeyArg(&keyArg2);
            free(keyArg2);
            return 0;
        }
    }

    // Pointer to the first section
    sectionData *sections;
    sections = createHolder();

    if (readIni(argv[1], &sections) != 0) {\
        printf("Error - could not read the ini file\n");
        freekeyArg(&keyArg1);
        free(keyArg1);
        freekeyArg(&keyArg2);
        free(keyArg2);
        return 0;
    }

/*
    // NOTE - uncomment to see the structure of the ini file read into program

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
    // This keyArg will be needed no matter if user wants expression or single key
    if (searchElement(sections, keyArg1->sectionName, keyArg1->keyName, &(keyArg1->keyPointer))) {
            printf("Error - Could not bind the data to keyArg1\n");
            if (freeAllSections(&sections) != 0)
                printf("Error - freeAllSections() did not work\n");
            freekeyArg(&keyArg1);
            free(keyArg1);
            freekeyArg(&keyArg2);
            free(keyArg2);
            return 0;
    }

    // Printing data required by the user
    if (argc == 3) {
        // Only single key to be printed
        printf("%s.%s = ", keyArg1->sectionName, keyArg1->keyName);
        if (keyArg1->keyPointer->valStr != NULL) {
            // Data is of string type
            printf("\"%s\"\n", keyArg1->keyPointer->valStr);
        } else {
            // Data is of integer type
            printf("%d\n", keyArg1->keyPointer->valNum);
        }
    } else {
        // Expression to be evaluated
        if (searchElement(sections, keyArg2->sectionName, keyArg2->keyName, &(keyArg2->keyPointer))) {
            printf("Error - Could not bind the data to keyArg2\n");
            if (freeAllSections(&sections) != 0)
                printf("Error - freeAllSections() did not work\n");
            freekeyArg(&keyArg1);
            free(keyArg1);
            freekeyArg(&keyArg2);
            free(keyArg2);
            return 0;
        }
        switch (operatorID) {
            // String-type data is supported
            case 1: // '+'
                if ((keyArg1->keyPointer->valStr != NULL) && (keyArg2->keyPointer->valStr != NULL)) {
                    // String concatenation
                    printf("concatenated string: \"%s%s\"\n", keyArg1->keyPointer->valStr, keyArg2->keyPointer->valStr);
                } else if ((keyArg1->keyPointer->valStr == NULL) && (keyArg2->keyPointer->valStr == NULL)) {
                    // Sum of integer-type data
                    printf("sum = %d\n", keyArg1->keyPointer->valNum + keyArg2->keyPointer->valNum);
                } else {
                    printf("Error - expression involves operands of different types\n");
                }
                break;

            // String-type data is not supported
            case 2: // '-'
                if ((keyArg1->keyPointer->valStr != NULL) || (keyArg2->keyPointer->valStr != NULL)) {
                    printf("Error - string-type data does not support subtraction\n");
                    break;
                }
                printf("difference = %d\n", keyArg1->keyPointer->valNum - keyArg1->keyPointer->valNum);
                break;
            case 3: // '*'
                if ((keyArg1->keyPointer->valStr != NULL) || (keyArg2->keyPointer->valStr != NULL)) {
                    printf("Error - string-type data does not support multiplication\n");
                    break;
                }
                printf("product = %d\n", keyArg1->keyPointer->valNum * keyArg2->keyPointer->valNum);
                break;
            case 4: // '/'
                if ((keyArg1->keyPointer->valStr != NULL) || (keyArg2->keyPointer->valStr != NULL)) {
                    printf("Error - string-type data does not support division\n");
                    break;
                }
                if (keyArg2->keyPointer->valNum == 0) {
                    printf("Cannot divide by 0\n");
                    break;
                }
                printf("quotient = %.2e\n", (float) keyArg1->keyPointer->valNum / keyArg2->keyPointer->valNum);
                break;

            // default will most probably never occur
            // It's just a safety measure
            default:
                printf("Error - unknown operator\n");
        }
    }

    // Free the rest of the data
    if (freeAllSections(&sections) != 0)
        printf("Error - freeAllSections() did not work\n");
    freekeyArg(&keyArg1);
    free(keyArg1);
    freekeyArg(&keyArg2);
    free(keyArg2);
    return 0;
}

/*
    // TESTING SINGLE INPUT
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
    // EXPRESSION EVALUATION INTRO
    char testExpression[] = "testsection1.testspace + testsectionEOF.testkeyEOF";
    keyArgument *testFirstArg = malloc(sizeof(keyArgument));
    keyArgument *testSecondArg = malloc(sizeof(keyArgument));

    int operatorId;
    if ((operatorId = readSimpleExpression(testExpression, &testFirstArg, &testSecondArg)) == -1) {
        printf("Error - readSimpleExpression() did not work\n");
        return 0;
    }
*/

/*
    // TESTING SIMPLE EXPRESSION INPUT
    searchElement(&(*sections), testFirstArg->sectionName, testFirstArg->keyName, &(testFirstArg->keyPointer));
    searchElement(&(*sections), testSecondArg->sectionName, testSecondArg->keyName, &(testSecondArg->keyPointer));

    printf("Outside of function:\n");
    printf("testFirstArg->keyPointer->valStr = %s\n", testFirstArg->keyPointer->valStr);
    printf("testSecondArg->keyPointer->valStr = %s\n", testSecondArg->keyPointer->valStr);
*/
