#include <stdio.h>

#include "akinator.h"
#include "colors.h"

int main(int argc, const char *argv[])
{
    int err = 0;

    int run_mode = HELP_OPTION;
    const char *db_filename = "akinator_db.txt",
               *obj1        = "null1",
               *obj2        = "null2";

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }

    if (options[DB_FILE_OPTION])
        db_filename = argv[options[DB_FILE_OPTION] + 1];

    if (options[HELP_OPTION])
        run_mode = HELP_OPTION;

    if (options[PREDICT_OPTION])
        run_mode = PREDICT_OPTION;
    if (options[LIST_OPTION])
        run_mode = LIST_OPTION;
    if (options[DESCRIBE_OPTION])
    {
        run_mode = DESCRIBE_OPTION;

        ASSERT(options[DESCRIBE_OPTION] + 1 < argc);
        obj1     = argv[options[DESCRIBE_OPTION] + 1];
    }
    if (options[COMPARE_OPTION])
    {
        run_mode = COMPARE_OPTION;

        ASSERT(options[COMPARE_OPTION] + 2 < argc);
        obj1     = argv[options[COMPARE_OPTION] + 1];
        obj2     = argv[options[COMPARE_OPTION] + 2];
    }

    if (run_mode == HELP_OPTION)
    {
        for (int32_t i = 0; i < N_EXEC_OPTIONS; ++i)
            printf("%10s %5s %s\n",
                    EXEC_OPTIONS[i].strFormLong,
                    EXEC_OPTIONS[i].strFormShort,
                    EXEC_OPTIONS[i].description);
        return 0;
    }

    Akinator aktr = {};
    AkinatorCtor(&aktr, db_filename, &err);

    AkinatorDump(&aktr);

    switch (run_mode)
    {
        case PREDICT_OPTION:
            AkinatorPredict(&aktr);
            break;
        case DESCRIBE_OPTION:
            AkinatorDescribe(&aktr, obj1);
            break;
        case COMPARE_OPTION:
            AkinatorCompare(&aktr, obj1, obj2);
            break;
        case LIST_OPTION:
            AkinatorPrintList(&aktr);
            break;
        default:
            printf(RED "Nothing to do" NORMAL);
    }
    printf("\n");

    AkinatorSaveDbToFile(&aktr);

    AkinatorDump(&aktr);

    AkinatorDtor(&aktr);
    
    return 0;
}
