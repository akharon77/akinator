#include "akinator.h"

int main(int argc, const char *argv[])
{
    int err = 0;

    int run_mode = HELP;
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
        db_filename = argv[options[INPUT_FILE_OPTION] + 1];

    if (options[HELP_OPTION])
        run_mode = HELP_OPTION;

    if (options[PREDICT_OPTION])
        run_mode = PREDICT_OPTION;
    if (options[COMPARE_OPTION])
    {
        run_mode = COMPARE_OPTION;
        obj1     = argv[options[COMPARE_OPTION] + 1];
        obj2     = argv[options[COMPARE_OPTION] + 2];
    }

    Akinator prec_tree = {};
    AkinatorCtor(&prec_tree, db_filename);

    switch (run_mode)
    {
        case PREDICT:
            AkinatorPredict(&prec_tree);
            break;
        case COMPARE:
            AkinatorCompare(&prec_tree, obj1, obj2);
            break;
        case HELP:
            OptionsPrintHelp();
            break;
        default:
            printf(RED "Nothing to do" NORMAL);
    }

    AkinatorDtor(&prec_tree);
    
    return 0;
}
