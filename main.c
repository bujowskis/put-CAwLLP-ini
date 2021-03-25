// Driver program combining all header files

#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Standard format of arguments: $ ./program PATH-TO-INI-FILE.ini section.key
// Enhanced format of arguments: $ ./program PATH-TO-INI-FILE.ini expression "distance.velocity * travel.time"

int main(/*int argc, char *argv[]*/) // NOTE - at some point, there will be need to accept arguments here
{
    // NOTE - this is a workaround to compile the program without using the command line
    int argc;
    char *argv[3 /* SOME SIZE VALUE */];

    return 0;
}
