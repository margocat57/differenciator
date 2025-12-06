#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "../core/tree_func.h"
#include "read_expr_from_file.h"
#include "../utils/metki.h"
#include "../dump/graphviz_dump.h"
#include "../core/forest.h"
#include "../core/operator_func.h"
#include "../dump/latex_dump.h"

//----------------------------------------------------------------------------
// Helping functions to find spaces

static void skip_space(const char *str, size_t *pos)
{
    assert(str);
    assert(pos);

    while(str[(*pos)] != '\0' && isspace(str[(*pos)])){
        (*pos)++;
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Creating buffer for reading expression from disk

static bool is_stat_err(const char *name_of_file, struct stat *all_info_about_file);

static char *read_file_to_string_array(const char *name_of_file)
{
    assert(name_of_file != NULL);
    FILE *fptr = fopen(name_of_file, "r");
    if (!fptr)
    {
        fprintf(stderr, "Can't open file\n");
        return NULL;
    }

    struct stat file_info = {};
    if (is_stat_err(name_of_file, &(file_info)))
    {
        return NULL;
    }

    char *all_strings_in_file = (char *)calloc((size_t)(file_info.st_size + 1), sizeof(char));
    if (!all_strings_in_file)
    {
        fprintf(stderr, "Array for strings allocation error\n");
        return NULL;
    }

    if (fread(all_strings_in_file, sizeof(char), (size_t)file_info.st_size, fptr) != (size_t)file_info.st_size)
    {
        fprintf(stderr, "Can't read all symbols from file\n");
        return NULL;
    }

    fclose(fptr);
    return all_strings_in_file;
}

static bool is_stat_err(const char *name_of_file, struct stat *all_info_about_file)
{
    assert(name_of_file != NULL);
    assert(all_info_about_file != NULL);

    if (stat(name_of_file, all_info_about_file) == -1)
    {
        perror("Stat error");
        fprintf(stderr, "Error code: %d\n", errno);
        return true;
    }
    return false;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
static void buffer_free(const char *buffer)
{
    if (buffer){
        /**
         * Так как free и memset не принимают в качестве аргументов
         * const char*, выдавая ошибку "no matching function for call и 
         * candidate function not viable: no known conversion from 'const char * 
         * to 'void *' for 1st argument", то 
         * так как программа программа в процессе парсинга в целях безопасности не изменяет 
         * исходную строчку, то чтобы не снимать const с нее,
         * но и дать возможность функции освобождать память под буффер в конце работы, 
         * для которого память была выделена динамически, приходится снимать const здесь
         */
        memset((char*)buffer, 0, strlen(buffer));
        free((char*)buffer);
    }
}
#pragma GCC diagnostic pop

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Make differenciator tree

//-----------------------------------------------------------------------------------------
// Grammar rules

/* G ::= "E$" */
static TreeNode_t *GetGrammarConstruction(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err);

/* E ::= T{[+,-] T}* */
static TreeNode_t *GetExpression(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err);

/* T ::= D{[*,/] D}* */
static TreeNode_t *GetTerm(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err);

/* D ::= P{[^] P}* */
static TreeNode_t *GetDeg(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err);

/* P ::= (E) | N | V | F */
static TreeNode_t *GetPrimary(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err);

/* N ::= ['0' - '9']+ */
static TreeNode_t *GetNumber(size_t *pos, const char *buffer);

/* V ::= ['a' - 'z'] */
static TreeNode_t *GetVariable(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err);

/* F ::= ["sin", "cos", ... ] '(' E ')'   comment : (Проверка F зашита в проверку V) */
static TreeNode_t *GetFunction(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err, OPERATORS op);

//-----------------------------------------------------------------------------------------

static void GetAdditionalParams(const char *buffer, size_t *pos, Forest_t *forest);

Forest_t *ReadAndCreateExpr(const char *name_of_file)
{
    assert(name_of_file);

    char *buffer = read_file_to_string_array(name_of_file);
    if (!buffer)
    {
        return NULL;
    }
    TreeHead_t *head = TreeCtor();
    Forest_t *forest = ForestCtor(10);
    size_t pos = 0;
    TreeErr_t err = NO_MISTAKE;
    head->root = GetGrammarConstruction(&pos, buffer, forest->mtk, &err);
    if (!head->root)
    {
        TreeDel(head);
        ForestDtor(forest);
        buffer_free(buffer);
        return NULL;
    }
    DEBUG_TREE(
        if (TreeVerify(head)) {
            fprintf(stderr, "File is not correct - can't work with created tree\n");
            TreeDel(head);
            ForestDtor(forest);
            buffer_free(buffer);
            return NULL;
        })
    ForestAddElem(head, forest);
    GetAdditionalParams(buffer, &pos, forest);
    buffer_free(buffer);
    return forest;
}

static TreeNode_t *GetGrammarConstruction(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err)
{
    skip_space(buffer, pos);
    TreeNode_t *head = GetExpression(pos, buffer, mtk, err);
    skip_space(buffer, pos);
    if (buffer[*pos] != '$')
    {
        fprintf(stderr, "Syntax error\n");
        *err = INCORR_FILE;
        TreeDelNodeRecur(head);
        return NULL;
    }
    if (*err)
    {
        fprintf(stderr, "Syntax error\n");
        TreeDelNodeRecur(head);
        return NULL;
    }
    (*pos)++;
    tree_dump_func(head, __FILE__, __func__, __LINE__, mtk, "Before ret GetG node %s", buffer + *pos);
    return head;
}

static TreeNode_t *GetExpression(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err)
{
    skip_space(buffer, pos);
    TreeNode_t *left = GetTerm(pos, buffer, mtk, err);
    skip_space(buffer, pos);
    while (buffer[*pos] == '+' || buffer[*pos] == '-')
    {
        int op = buffer[*pos];
        (*pos)++;
        skip_space(buffer, pos);

        TreeNode_t *right = GetTerm(pos, buffer, mtk, err);
        if (*err)
        {
            TreeDelNodeRecur(left);
            return NULL;
        }
        skip_space(buffer, pos);

        TreeNode_t *new_node = NULL;
        if (op == '+')
        {
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_ADD}, NULL, left, right);
        }
        else if (op == '-')
        {
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_SUB}, NULL, left, right);
        }
        left->parent = new_node;
        right->parent = new_node;
        left = new_node;
    }
    tree_dump_func(left, __FILE__, __func__, __LINE__, mtk, "Before ret GetE node %s", buffer + *pos);
    return left;
}

static TreeNode_t *GetTerm(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err)
{
    skip_space(buffer, pos);
    TreeNode_t *left = GetDeg(pos, buffer, mtk, err);
    skip_space(buffer, pos);

    while (buffer[*pos] == '*' || buffer[*pos] == '/')
    {
        int op = buffer[*pos];
        (*pos)++;
        skip_space(buffer, pos);

        TreeNode_t *right = GetDeg(pos, buffer, mtk, err);
        if (*err)
        {
            TreeDelNodeRecur(left);
            return NULL;
        }
        skip_space(buffer, pos);

        TreeNode_t *new_node = NULL;
        if (op == '*')
        {
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_MUL}, NULL, left, right);
        }
        else if (op == '/')
        {
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_DIV}, NULL, left, right);
        }
        left->parent = new_node;
        right->parent = new_node;
        left = new_node;
    }
    tree_dump_func(left, __FILE__, __func__, __LINE__, mtk, "Before ret GetT node %s", buffer + *pos);
    return left;
}

static TreeNode_t *GetDeg(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err)
{
    skip_space(buffer, pos);
    TreeNode_t *left = GetPrimary(pos, buffer, mtk, err);
    skip_space(buffer, pos);

    while (buffer[*pos] == '^')
    {
        int op = buffer[*pos];
        (*pos)++;
        skip_space(buffer, pos);

        TreeNode_t *right = GetPrimary(pos, buffer, mtk, err);
        if (*err)
        {
            TreeDelNodeRecur(left);
            return NULL;
        }
        skip_space(buffer, pos);

        TreeNode_t *new_node = NULL;
        if (op == '^')
        {
            new_node = NodeCtor(OPERATOR, (TreeElem_t){.op = OP_DEG}, NULL, left, right);
        }
        left->parent = new_node;
        right->parent = new_node;
        left = new_node;
    }
    tree_dump_func(left, __FILE__, __func__, __LINE__, mtk, "Before ret GetD node %s", buffer + *pos);
    return left;
}

static TreeNode_t *GetPrimary(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err)
{
    TreeNode_t *val = NULL;
    skip_space(buffer, pos);

    if (buffer[*pos] == '(')
    {
        (*pos)++;
        skip_space(buffer, pos);

        val = GetExpression(pos, buffer, mtk, err);
        skip_space(buffer, pos);

        if (buffer[*pos] == ')')
        {
            (*pos)++;
        }
        else
        {
            *err = INCORR_FILE;
        }
    }
    else
    {
        val = GetNumber(pos, buffer);
        if (!val)
        {
            val = GetVariable(pos, buffer, mtk, err);
        }
        if (!val)
        {
            *err = INCORR_FILE;
        }
    }
    tree_dump_func(val, __FILE__, __func__, __LINE__, mtk, "Before ret GetP(val) node %s", buffer + *pos);
    return val;
}

static TreeNode_t *GetNumber(size_t *pos, const char *buffer)
{
    if (!isdigit(buffer[*pos]))
    {
        return NULL;
    }
    char *endptr = NULL;
    double val = strtod(buffer + *pos, &endptr);
    *pos += (size_t)(endptr - (buffer + *pos));
    return NodeCtor(CONST, (TreeElem_t){.const_value = val}, NULL, NULL, NULL);
}

static bool FindFunction(size_t *pos, const char *buffer, OPERATORS *op);

static size_t FindVar(char dest, metki *mtk);

static TreeNode_t *GetVariable(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err)
{
    if (!isalpha(buffer[*pos]))
    {
        return NULL;
    }

    OPERATORS op = INCORR;
    if (FindFunction(pos, buffer, &op))
    {
        return GetFunction(pos, buffer, mtk, err, op);
    }

    char num_of_var = buffer[*pos];
    (*pos)++;

    return NodeCtor(VARIABLE, (TreeElem_t){.var_code = FindVar(num_of_var, mtk)}, NULL, NULL, NULL);
}

static bool FindFunction(size_t *pos, const char *buffer, OPERATORS *op)
{
    size_t num_of_op = sizeof(OPERATORS_INFO) / sizeof(op_info);
    for (size_t idx = 1; idx < num_of_op; idx++)
    {
        if (OPERATORS_INFO[idx].op == OP_ADD || OPERATORS_INFO[idx].op == OP_SUB || OPERATORS_INFO[idx].op == OP_MUL || OPERATORS_INFO[idx].op == OP_DIV || OPERATORS_INFO[idx].op == OP_DEG)
        {
            continue;
        }
        if (!strncmp(buffer + *pos, OPERATORS_INFO[idx].op_name, OPERATORS_INFO[idx].num_of_symb))
        {
            (*pos) += OPERATORS_INFO[idx].num_of_symb;
            *op = OPERATORS_INFO[idx].op;
            return true;
        }
    }
    return false;
}

static TreeNode_t *GetFunction(size_t *pos, const char *buffer, metki *mtk, TreeErr_t *err, OPERATORS op)
{
    TreeNode_t *val = NULL;
    skip_space(buffer, pos);

    if (buffer[*pos] == '(')
    {
        (*pos)++;
        TreeNode_t *left = GetExpression(pos, buffer, mtk, err);
        if (*err)
        {
            TreeDelNodeRecur(left);
            return NULL;
        }
        skip_space(buffer, pos);
        if (buffer[*pos] != ')')
        {
            *err = INCORR_FILE;
            TreeDelNodeRecur(left);
            return NULL;
        }
        (*pos)++;

        val = NodeCtor(OPERATOR, (TreeElem_t){.op = op}, NULL, left, NULL);
        left->parent = val;
    }
    else
    {
        *err = INCORR_FILE;
    }
    return val;
}

static size_t FindVar(char dest, metki *mtk)
{
    assert(dest);
    assert(mtk);

    size_t metka_idx = FindVarInMtkArr(mtk, dest);
    if (metka_idx != SIZE_MAX)
    {
        return metka_idx;
    }
    return MetkiAddName(mtk, dest);
}

//---------------------------------------------------------------
// To getting additional params from file
struct min_max_value
{
    double min_value;
    double max_value;
};

static void GetX(const char *buffer, size_t *pos, Forest_t *forest);

static void GetY(const char *buffer, size_t *pos, Forest_t *forest);

static min_max_value GetMinMaxValue(const char *buffer, size_t *pos);

static void GetNumOfDerivative(const char *buffer, size_t *pos, Forest_t *forest);

static void GetVarToDiff(const char *buffer, size_t *pos, Forest_t *forest);

static void GetTaylorPoint(const char *buffer, size_t *pos, Forest_t *forest);

static void GetAdditionalParams(const char *buffer, size_t *pos, Forest_t *forest)
{
    skip_space(buffer, pos);
    while (buffer[*pos] != '\0' && (buffer[*pos] == 'X' || buffer[*pos] == 'Y' || buffer[*pos] == 'n' || !strncmp(buffer + *pos, "var", 3) || !strncmp(buffer + *pos, "Taylor", 6)))
    {
        GetX(buffer, pos, forest);
        skip_space(buffer, pos);

        GetY(buffer, pos, forest);
        skip_space(buffer, pos);

        GetNumOfDerivative(buffer, pos, forest);
        skip_space(buffer, pos);

        GetVarToDiff(buffer, pos, forest);
        skip_space(buffer, pos);

        GetTaylorPoint(buffer, pos, forest);
        skip_space(buffer, pos);
    }
}

static void GetX(const char *buffer, size_t *pos, Forest_t *forest)
{
    if (buffer[*pos] == 'X')
    {
        (*pos)++;
        min_max_value value_x = GetMinMaxValue(buffer, pos);
        forest->x_y_range.x_min_dump = value_x.min_value;
        forest->x_y_range.x_max_dump = value_x.max_value;
    }
}

static void GetY(const char *buffer, size_t *pos, Forest_t *forest)
{
    if (buffer[*pos] == 'Y')
    {
        (*pos)++;
        min_max_value value_y = GetMinMaxValue(buffer, pos);
        forest->x_y_range.y_min_dump = value_y.min_value;
        forest->x_y_range.y_max_dump = value_y.max_value;
    }
}

static min_max_value GetMinMaxValue(const char *buffer, size_t *pos){
    min_max_value value = {};
    size_t chars_read = 0;
    sscanf(buffer + *pos, " from %lg to %lg%n", &value.min_value, &value.max_value, &chars_read);
    *pos += chars_read;
    return value;
}

static void GetNumOfDerivative(const char *buffer, size_t *pos, Forest_t *forest){
    size_t chars_read = 0;
    if(buffer[*pos] == 'n'){
        sscanf(buffer + *pos, "n = %zu%n",  &(forest->params.num_of_derivative), &chars_read);
        forest->params.is_num_derivative_filled = true;
        *pos += chars_read;
    }
}

static void GetVarToDiff(const char *buffer, size_t *pos, Forest_t *forest){
    char var_name = '\0';
    size_t chars_read = 0;
    
    if(!strncmp(buffer + *pos, "var", 3)){
        sscanf(buffer + *pos, " var = %c%n", &var_name, &chars_read);
        *pos += chars_read;

        size_t metka_idx = FindVarInMtkArr(forest->mtk, var_name);
        if(metka_idx == SIZE_MAX){
            return;
        }
        forest->params.var_id = metka_idx;
        forest->params.is_var_id_filled = true;
        skip_space(buffer, pos);
    }
}

static void GetTaylorPoint(const char *buffer, size_t *pos, Forest_t *forest)
{
    if (forest->mtk->first_free != 1){
        return;
    }
    size_t chars_read = 0;
    if (!strncmp(buffer + *pos, "Taylor", 6))
    {
        sscanf(buffer + *pos, "Taylor %lg%n", &(forest->mtk->var_info[0].value), &chars_read);
        *pos += chars_read;
        forest->mtk->has_value = true;
    }
}