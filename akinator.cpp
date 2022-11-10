#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "akinator.h"
#include "tree.h"
#include "iostr.h"
#include "stack.h"
#include "colors.h"

const Option EXEC_OPTIONS[] = 
    {
        {"--db",       "-f",  DB_FILE_OPTION,  "use database from file (default: input.asm)"},
        {"--predict",  "-p",  PREDICT_OPTION,  "prediction mode"},
        {"--compare",  "-c",  COMPARE_OPTION,  "comparison mode"},
        {"--describe", "-d",  DESCRIBE_OPTION, "description mode"},
        {"--help",     "-h",  HELP_OPTION,     "show help"}
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

void AkinatorCtor(Akinator *aktr, const char *db_filename, int *err)
{
    aktr->root = NodeNew();

    int32_t fd = open(db_filename, 0);
    
    TextInfo text = {};
    TextInfoCtor(&text);

    InputText   (&text, db_filename, err);

    AkinatorParseText(aktr, &text);

    TextInfoDtor(&text);

    close(fd);
}

void AkinatorDtor(Akinator *aktr)
{
    TreeDtor(aktr->root);
}

void AkinatorPredict(Akinator *aktr)
{
    ASSERT(aktr != NULL);

    bool ans = false;
    Node *vertex = aktr->root;

    while (!NodeIsLeaf(vertex))
    {
        printf("%s? [y/n] ", vertex->str);
        ans = GetAnsYesNo();

        if (ans)
            vertex = vertex->right;
        else
            vertex = vertex->left;
    }

    printf("Is it %s? [y/n] ", vertex->str);
    ans = GetAnsYesNo();

    if (ans)
    {
        printf("I knew it!");
    }
    else
    {
        char obj     [MAX_STR_OBJ_LEN] = "", 
             property[MAX_STR_OBJ_LEN] = "";

        printf("What's a pity! Please, tell me, who it was. It was ... ");
        scanf("%s", obj);

        printf("Please, tell me, %s differs from %s in that %s ... ", obj, vertex->str, obj);
        scanf("%s", property);
        property[0] = toupper(property[0]);

        Node *node_left  = NodeNew(),
             *node_right = NodeNew();

        NodeCtor(node_left,  vertex->str);
        NodeCtor(node_right, obj);

        *vertex = 
            {
                .str   = strdup(property),
                .left  = node_left,
                .right = node_right
            };
    }
}

void AkinatorCompare(Akinator *aktr, const char *obj1, const char *obj2)
{
    Stack stk1 = {},
          stk2 = {};

    StackCtor(&stk1, 64);
    StackCtor(&stk2, 64);

    bool path1 = AkinatorFindObj(aktr->root, obj1, &stk1),
         path2 = AkinatorFindObj(aktr->root, obj2, &stk2);

    if (!path1 || !path2)
        printf("I do not know what you are asking me about\n");

    printf("%s is similar to %s in that they are both ", obj1, obj2);

    Node *vertex = aktr->root;

    // it isn't copypasta, it has difference in while condition
    while (StackTop(&stk1) == StackTop(&stk2))
    {
        char *str = vertex->str;

        bool step = StackTop(&stk1);

        StackPop(&stk1);
        StackPop(&stk2);

        if (step)
        {
            vertex = vertex->right;
        }
        else
        {
            printf("not ");
            vertex = vertex->left;
        }

        printf("%s, ", str);
    }

    printf("but %s ", obj1);
    AkinatorPrintByPath(vertex, &stk1);
    
    printf("and %s ", obj2);
    AkinatorPrintByPath(vertex, &stk2);

    StackDtor(&stk1);
    StackDtor(&stk2);
}

void AkinatorPrintByPath(Node *node, Stack *stk)
{
    ASSERT(node != NULL);
    ASSERT(stk  != NULL);

    while (StackGetSize(stk) > 0 && !NodeIsLeaf(node))
    {
        char *str = node->str;

        if (StackPop(stk))
        {
            node = node->right;
        }
        else
        {
            printf("not ");
            node = node->left;
        }

        printf("%s, ", str);
    }
}

bool AkinatorFindObj(Node *node, const char *str, Stack *stk)
{
    ASSERT(node != NULL);
    ASSERT(str  != NULL);
    ASSERT(stk  != NULL);

    if (strcasecmp(str, node->str) == 0)
        return true;
    if (NodeIsLeaf(node))
        return false;

    if (AkinatorFindObj(node->left, str, stk))
    {
        StackPush(stk, 0);
        return true;
    }
    else if (AkinatorFindObj(node->right, str, stk))
    {
        StackPush(stk, 1);
        return true;
    }

    return false;
}

void AkinatorParseText(Akinator *aktr, TextInfo *text)
{
    Stack stk_quot = {},
          stk_stat = {};

    StackCtor(&stk_quot, 64);
    StackCtor(&stk_stat, 64);

    StackPush(&stk_stat, VERTEX_IN);
    Node *vertex = aktr->root;

    for (int32_t i = strchr(text->base, '{') - text->base + 1; i < text->size; ++i)
    {
        switch (text->base[i])
        {
            case '{':
                switch (StackPop(&stk_stat))
                {
                    case VERTEX_IN:
                        {
                            StackPush(&stk_stat, VERTEX_LEFT);
                            StackPush(&stk_stat, VERTEX_IN);

                            Node *node   = NodeNew();
                            node->ancstr = vertex;
                            vertex->left = node;
                            vertex       = node;
                        }
                        break;

                    case VERTEX_LEFT:
                        {
                            StackPush(&stk_stat, VERTEX_RIGHT);
                            StackPush(&stk_stat, VERTEX_IN);

                            Node *node    = NodeNew();
                            node->ancstr  = vertex;
                            vertex->right = node;
                            vertex        = node;
                        }
                        break;

                    default:
                        break;
                }
                break;

            case '"':
                StackPush(&stk_quot, i);
                break;

            case '}':
                {
                    StackPop(&stk_stat);

                    int32_t quot_r = StackPop(&stk_quot);
                    int32_t quot_l = StackPop(&stk_quot);

                    vertex->str = strndup(text->base + quot_l + 1, quot_r - quot_l - 1);
                    vertex = vertex->ancstr;
                }
                break;

            default:
                break;
        }
    }

    StackDtor(&stk_quot);
    StackDtor(&stk_stat);
}

void AkinatorDescribe(Akinator *aktr, const char *obj)
{
    Stack stk = {};
    StackCtor(&stk, 64);

    printf("%s is ", obj);
    AkinatorFindObj     (aktr->root, obj, &stk);
    AkinatorPrintByPath (aktr->root, &stk);

    StackDtor(&stk);
}

bool GetAnsYesNo()
{
    char ans = '\0';
    while (true)
    {
        ans = getc(stdin);

        switch (tolower(ans))
        {
            case 'y':
                return true;
            case 'n':
                return false;
            case '\n':
                break;
            default:
                printf(RED "Wrong answer. " NORMAL "Enter [y/n] ");
        }
    }
}

