#ifndef INI_H
#define INI_H

/* TYPEDEFS */

typedef struct keyData keyData;
typedef struct sectionData sectionData;

/* STRUCTURES */

struct keyData
{
    // Name of the key
    char *name;

    // Value, if it's of string type (if not, it's NULL)
    char *valStr;

    // Value, if it's of number type (checked only if valStr is NULL)
    int *valNum;

    // Pointer to the next key in this structure
    keyData *nextKey;
};

struct sectionData
{
    // Name of the section
    char *name;

    // Pointer to the first key in this section
    keyData *firstKey;

    // Pointer to the next section
    sectionData *nextSection;
};

/* FUNCTIONS */

/* Creates a new holder array for the INI file */
sectionData* createHolder();

/* Reads the file, i.e:
 * 1. Gets:
 * - number of sections
 * - length of section names
 * - length of a key
 * - length of a value under the key
 * 2. Saves its contents into proper structures
 *
 * Return codes:
 * 0 - success
 * 1 - insufficient memory
 * 2 - missing section (+ prints message)
 * 3 - missing key (+ prints message)
 * 4 - invalid identifiers in INI file (other than alphanumeric)
 * TODO - MOVE EXPRESSION EVALUATION TO PROPER PLACE
 * 5 - expression with operands of different types
 * 6 - expression with invalid operators
 *
 * 7 - file not opened
 * 8 - not an .ini file
 * 9 - error reading the file
 */
int readIni(char *filePath, sectionData *firstSection);

#endif
