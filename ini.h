#ifndef INI_H
#define INI_H

/* TYPEDEFS */

typedef struct keyData keyData;
typedef struct sectionData sectionData;

/* STRUCTURES */

/* NOTE - dynamic memory allocation with no limits can be done in such way:
 * - 2 structures (one storing section and pointer to next such structure,
 *   second storing key, its value and pointer to the next such struct)
 * - allocating memory dynamically with `realloc` and assigning a new section
 *   struct containing its keys struct
 *
 * I opt for the second option, as it's a new one to me.
 */

struct keyData
{
    // Key
    char *key;

    // Value, if it's of string type (if not, it's NULL)
    char *valStr;

    // Value, if it's of number type (checked only if valStr is NULL)
    int *valNum;
};

struct sectionData
{
    // Section
    char *section;

    // Array of keys assigned to this section
    char *keysArray;
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
 * 5 - expression with operands of different types
 * 6 - expression with invalid operators
 * 7 - file not opened
 * 8 - not an .ini file
 */
int readIni(char *filePath, sectionData *holderArray);

#endif
