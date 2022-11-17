#ifndef AKINATOR_H
#define AKINATOR_H

#include "iostr.h"
#include "tree.h"
#include "stack.h"

enum OPTIONS
{
    DB_FILE_OPTION,
    HELP_OPTION,
    PREDICT_OPTION,
    COMPARE_OPTION,
    DESCRIBE_OPTION,
    LIST_OPTION,
    N_OPTIONS
};

enum AKINATOR_STATES
{
    VERTEX_IN,
    VERTEX_LEFT,
    VERTEX_RIGHT
};

extern const Option       EXEC_OPTIONS[];
extern const size_t       N_EXEC_OPTIONS;

const int32_t MAX_STR_OBJ_LEN = 128;

struct Akinator
{
    Node *root;

    char *db_filename;
};

#define AkinatorEchoAndSay(...)                             \
do                                                          \
{                                                           \
    char buf_str[256] = "";                                 \
    sprintf(buf_str, __VA_ARGS__);                          \
    printf(buf_str);                                        \
    fflush(stdout);                                         \
                                                            \
    char cmd[256] = "";                                     \
    sprintf(cmd, "echo \"%s\" | festival --tts", buf_str);  \
    system(cmd);                                            \
}                                                           \
while (0)

void AkinatorCtor          (Akinator *aktr, const char *db_filename, int *err);
void AkinatorDtor          (Akinator *aktr);

void AkinatorPredict       (Akinator *aktr);
void AkinatorCompare       (Akinator *aktr, const char *obj1, const char *obj2);
void AkinatorDescribe      (Akinator *aktr, const char *obj);

void AkinatorPrintByPath   (Node *node, Stack *stk);
bool AkinatorFindObj       (Node *node, const char *str, Stack *stk);

void AkinatorParseText     (Akinator *aktr, TextInfo *text);
void AkinatorSaveDbToFile  (Akinator *aktr);
void AkinatorSaveDfs       (Node *node, int32_t depth, int32_t fd);

void AkinatorDump          (Akinator *aktr);
void AkinatorDumpToFileDfs (Node *node, int32_t fd, int64_t idx);

void AkinatorPrintList     (Akinator *aktr);
void AkinatorPrintListDfs  (Node *node, int32_t *cnt);

bool GetAnsYesNo();
bool ClearInput();

#endif  // AKINATOR_H
