#ifndef INI_H
#define INI_H

/* TYPEDEFS */

typedef struct keyData keyData;
typedef struct sectionData sectionData;
typedef struct keyArgument keyArgument;

/* STRUCTURES */

struct keyData
{
    // Name of the key
    char *name;

    // Value, if it's of string type (if not, it's NULL)
    char *valStr;

    // Value, if it's of number type (checked only if valStr is NULL)
    int valNum;

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

struct keyArgument
{
    // Name of the section
    char *sectionName;

    // Name of the key
    char *keyName;

    // Pointer to the corresponding key
    keyData *keyPointer;
};

/* FUNCTIONS */

/* Creates a new holder array for the INI file */
sectionData* createHolder();

/* Frees given key
 * Returns 0 on success, otherwise 1
 */
int freeKey(keyData **key);

/* Frees all keys recursively, starting from firstKey
 * Returns 0 on success, otherwise 1
 */
int freeAllKeys(keyData **firstKey);

/* Frees given section
 * Returns 0 on success, otherwise 1
 */
int freeSection(sectionData **section);

/* Frees all the sections (and thus all the file) recursively
 * Returns 0 on success, otherwise 1
 */
int freeAllSections(sectionData **firstSection);

/* Skips 'space' characters where possible
 *
 * Returns the index of next non-space character in the buffer
 * If error occurs, returns -1
 */
int skipSpaces(char *buf, int startIndex);

/* Searches for a key within given section and binds its address to given holder
 * Returns 0 on success, otherwise 1
 */
int searchElement(sectionData *firstSection, char *sectionName, char *keyName, keyData **keyAddress);

/* Reads key given as an argument and stores it in a structure
 * Returns 0 on success, otherwise 1
 */
int readArgKey(char *argKey, keyArgument **keyArg);

/* Reads a simple expression and binds data needed to evaluate it
 * On success, returns the id of given operator:
 *      1 = '+'
 *      2 = '-'
 *      3 = '*'
 *      4 = '/'
 * On failure, returns -1
 */
int readSimpleExpression(char *expression, keyArgument **firstkeyArg, keyArgument **secondkeyArg);

// Frees all the data from given key
void freekeyArg(keyArgument **key);

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
 *
 * TODO - MOVE EXPRESSION EVALUATION TO PROPER PLACE
 * 5 - expression with operands of different types
 * 6 - expression with invalid operators
 *
 * 7 - file not opened
 * 8 - not an .ini file
 * 9 - error reading the file
 * 10 - invalid section declaration
 * 11 - invalid key declaration
 */
int readIni(char *filePath, sectionData **firstSection);

#endif
