#ifndef AKINATOR_H
#define AKINATOR_H

#include "iostr.h"

enum OPTIONS
{
    DB_FILE_OPTION,
    HELP_OPTION,
    PREDICT_OPTION,
    COMPARE_OPTION,
    N_OPTIONS
};

extern const Option       EXEC_OPTIONS[];
extern const size_t       N_EXEC_OPTIONS;

void AkinatorCtor    (Akinator *aktr);
void AkinatorDtor    (Akinator *aktr);

void AkinatorPredict (Akinator *aktr);
void AkinatorCompare (Akinator *aktr, const char *obj1, const char *obj2);

char GetAnsYesNo();

#endif  // AKINATOR_H
