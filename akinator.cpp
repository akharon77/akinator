#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "akinator.h"
#include "iostr.h"
#include "colors.h"
//test

const Option EXEC_OPTIONS[] = 
    {
        {"--db",        "-f",  DB_FILE_OPTION,       "use database from file (default: input.asm)"                          },
        {"--help",        "-h",  HELP_OPTION,             "show help"                                               }
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

void AkinatorCtor(Akinator *aktr)
{

}

void AkinatorDtor(Akinator *aktr)
{

}

void AkinatorPredict(Akinator *aktr)
{

}

void AkinatorCompare(Akinator *aktr, const char *obj1, const char *obj2)
{

}

