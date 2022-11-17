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
        {"--list",     "-l",  LIST_OPTION,     "print list of all objects"},
        {"--help",     "-h",  HELP_OPTION,     "show help"}
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

void AkinatorCtor(Akinator *aktr, const char *db_filename, int *err)
{
    aktr->root        = NodeNew();
    aktr->db_filename = strdup(db_filename);

    int32_t fd = open(db_filename, 0);
    ASSERT(fd != -1);
    
    TextInfo text = {};
    TextInfoCtor(&text);

    InputText   (&text, db_filename, err);

    AkinatorParseText(aktr, &text);

    TextInfoDtor(&text);

    close(fd);
}

void AkinatorDtor(Akinator *aktr)
{
    ASSERT(aktr != NULL);

    TreeDtor(aktr->root);
    free(aktr->db_filename);

    aktr->root        = NULL;
    aktr->db_filename = NULL;
}

void AkinatorPredict(Akinator *aktr)
{
    ASSERT(aktr != NULL);

    bool ans = false;
    Node *vertex = aktr->root;

    while (!NodeIsLeaf(vertex))
    {
        AkinatorEchoAndSay("%s? ", vertex->str);

        ans = GetAnsYesNo();

        if (ans)
            vertex = vertex->right;
        else
            vertex = vertex->left;
    }

    AkinatorEchoAndSay("Is it %s? ", vertex->str);
    ans = GetAnsYesNo();

    if (ans)
    {
        AkinatorEchoAndSay("I knew it!");
    }
    else
    {
        char obj     [MAX_STR_OBJ_LEN] = "", 
             property[MAX_STR_OBJ_LEN] = "";

        AkinatorEchoAndSay("What's a pity! Please, tell me, who it was. It was ... ");
        scanf("%s", obj);

        AkinatorEchoAndSay("Please, tell me, %s differs from %s in that %s ... ", obj, vertex->str, obj);
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
        AkinatorEchoAndSay("I do not know what you are asking me about\n");

    AkinatorEchoAndSay("%s is similar to %s in that they are both ", obj1, obj2);

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
            AkinatorEchoAndSay("not ");
            vertex = vertex->left;
        }

        AkinatorEchoAndSay("%s, ", str);
    }

    AkinatorEchoAndSay("but %s ", obj1);
    AkinatorPrintByPath(vertex, &stk1);
    
    AkinatorEchoAndSay("and %s ", obj2);
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
            AkinatorEchoAndSay("not ");
            node = node->left;
        }

        AkinatorEchoAndSay("%s, ", str);
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

// finite-state machine
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

    AkinatorEchoAndSay("%s ", obj);
    AkinatorFindObj     (aktr->root, obj, &stk);
    AkinatorPrintByPath (aktr->root, &stk);

    StackDtor(&stk);
}

void AkinatorSaveDbToFile(Akinator *aktr)
{
    int32_t fd = creat(aktr->db_filename, S_IRWXU);

    ASSERT(fd != -1);
    AkinatorSaveDfs(aktr->root, 0, fd);

    close(fd);
}

void AkinatorSaveDfs(Node *node, int32_t depth, int32_t fd)
{
    ASSERT(node != NULL);

    for (int32_t i = 0; i < depth; ++i)
        dprintf(fd, "\t");
    dprintf(fd, "{ \"%s\" ", node->str);

    if (!NodeIsLeaf(node))
    {
        dprintf(fd, "\n");
        AkinatorSaveDfs(node->left,  depth + 1, fd);
        AkinatorSaveDfs(node->right, depth + 1, fd);

        for (int32_t i = 0; i < depth; ++i)
            dprintf(fd, "\t");
    }

    dprintf(fd, "}\n");
}

void AkinatorDump(Akinator *aktr)
{
    ASSERT(aktr     != NULL);

    static int32_t cnt = 0;

    char filename_txt[256] = "",
         filename_svg[256] = "";

    sprintf(filename_txt, "dump/akinator_%s_%d.txt", aktr->db_filename, cnt);
    sprintf(filename_svg, "dump/akinator_%s_%d.svg", aktr->db_filename, cnt++);

    int32_t fd = creat(filename_txt, S_IRWXU);
    ASSERT(fd != -1);

    dprintf(fd, "digraph G {\n");
    AkinatorDumpToFileDfs(aktr->root, fd, 0);
    dprintf(fd, "}\n");

    close(fd);

    char cmd[256] = "";
    sprintf(cmd, "dot %s -o %s -Tsvg", filename_txt, filename_svg);
    system(cmd);
}

void AkinatorDumpToFileDfs(Node *node, int32_t fd, int64_t idx)
{
    dprintf(fd, "node%ld[shape=polygon, label=\"%s\"];\n",
                idx, node->str);

    if (!NodeIsLeaf(node))
    {
        dprintf(fd, "node%ld->node%ld[label=\"no\"];", idx, 2 * idx + 1);
        AkinatorDumpToFileDfs(node->left,  fd, 2 * idx + 1);

        dprintf(fd, "node%ld->node%ld[label=\"yes\"];", idx, 2 * idx + 2);
        AkinatorDumpToFileDfs(node->right, fd, 2 * idx + 2);
    }
}

void AkinatorPrintList(Akinator *aktr)
{
    int32_t cnt = 0;
    AkinatorPrintListDfs(aktr->root, &cnt);
}

void AkinatorPrintListDfs(Node *node, int32_t *cnt)
{
    ASSERT(node != NULL);
    ASSERT(cnt  != NULL);

    if (!NodeIsLeaf(node))
    {
        AkinatorPrintListDfs(node->left,  cnt);
        AkinatorPrintListDfs(node->right, cnt);
    }
    else
    {
        printf("%d. %s\n", (*cnt)++, node->str);
    }
}

bool GetAnsYesNo()
{
    printf("[y/n] ");

    char ans = '\0';
    while (true)
    {
        ans = getchar();

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

        ClearInput();
    }
}

bool ClearInput()
{
    bool is_space = true;
    int temp      = '\0';

    do
    {
        temp = getchar();
        if (temp != ' ' && temp != '\t' && temp != '\n')
            is_space = false;
    }
    while (temp != '\n' && temp != EOF);

    return is_space;
}

