#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <time.h>


#define d(node) Differentiate_Node (node, diff_var)
#define c(node) Create_Node (node->type, node->value, node->left_child, node->right_child)

#define LEFT  node->left_child
#define RIGHT node->right_child

#define SUM(left, right) Create_Node (OP, { .operation_type = OP_SUM }, left, right)
#define SUB(left, right) Create_Node (OP, { .operation_type = OP_SUB }, left, right)
#define MUL(left, right) Create_Node (OP, { .operation_type = OP_MUL }, left, right)
#define DIV(left, right) Create_Node (OP, { .operation_type = OP_DIV }, left, right)
#define POW(left, right) Create_Node (OP, { .operation_type = OP_POW }, left, right)

#define LN(arg)  Create_Node (FUNC, { .operation_type = FUNC_LN },  arg, NULL)
#define SIN(arg) Create_Node (FUNC, { .operation_type = FUNC_SIN }, arg, NULL)
#define COS(arg) Create_Node (FUNC, { .operation_type = FUNC_COS }, arg, NULL)

#define NODE_NUM(value) Create_Node (VAL, { .constant = value }, NULL, NULL)


union element
{
    int    operation_type;
    double constant;
    char*  var_name;
};


struct str_info
{
    char*  pointer;
    size_t offset;
    int    error_check;
};


struct Node
{
    int     type;
    element value;
    Node*   parent;
    Node*   left_child;
    Node*   right_child;
    char    changed;
};


enum types
{
    OP   = 0,
    VAL  = 1,
    FUNC = 2,
    VAR  = 3,
};


enum operations
{
    OP_SUM = 0,
    OP_SUB = 1,
    OP_MUL = 2,
    OP_DIV = 3,
    OP_POW = 4,
};


enum functions
{
    FUNC_SIN = 1,
    FUNC_COS = 2,
    FUNC_LN  = 3,
};


enum neutral_elements
{
    NEUTRAL_SUM = 0,
    NEUTRAL_SUB = 0,
    NEUTRAL_MUL = 1,
    NEUTRAL_DIV = 1,
    NEUTRAL_POW = 1,
    KILLER_MUL  = 0,
    KILLER_DIV  = 0,
};

const char   YES                  = 1;
const char   NO                   = 0;
const size_t MAX_LENTH_EXPRESSION = 200;
const size_t MAX_NAME_COMMAND     = 100;
const int    FAILED               = -1;
const int    PASSED               = 1;
const double EXP_POW              = M_E;
const size_t NUM_INSERTS          = 14;

str_info* String_Info_Construct (void);
void      String_Info_Destruct  (str_info* info);

Node* Get_General            (str_info* info);
Node* Get_Number             (str_info* info);
Node* Get_Expression         (str_info* info);
Node* Get_PrimaryExpression  (str_info* info);
Node* Get_Term               (str_info* info);
Node* Get_Degree             (str_info* info);
Node* Get_Variable           (str_info* info);
Node* Get_VariableOrFunction (str_info* info);

void  Syntax_Error         (str_info* info);
void  Arithmetic_Error     (str_info* info);
void  Syntax_Require       (str_info* info, char elem);
void  Skip_Spaces          (str_info* info);
int   Define_Function_Type (char* name_var);
char* Function_Type_String (Node* tree);

void Tree_Print              (Node* tree);
void Recursive_Print_Tree    (Node* tree, FILE* Tree_image);
void Node_Print              (Node* tree, FILE* Tree_image);
void Value_Print_Logfile     (Node* tree, FILE* Tree_image);
void Operation_Print_Logfile (Node* tree, FILE* Tree_image);
void Function_Print_Logfile  (Node* tree, FILE* Tree_image);
void Variable_Print_Logfile  (Node* tree, FILE* Tree_image);

Node* Create_Node  (int type, element value, Node* left_child, Node* right_child);
void Destruct_Node (Node* node);

void File_Processing   (Node* root, Node* old_root);
void Fill_Latex_Header (FILE* tex_code);
void Print_Tree_Latex  (Node* node, FILE* file_name);
void Print_Operation   (Node* node, FILE* file_name);
void Print_Value       (Node* node, FILE* file_name);
void Print_Function    (Node* node, FILE* file_name);
void Print_Variable    (Node* node, FILE* file_name);
void Update_Tex        (Node* root, FILE* file_name);
void Insert_Tex        (FILE* file_name);

Node* Differentiate_Node (Node* node, char* diff_var);
Node* Diff_Value         (Node* node, char* diff_var);
Node* Diff_Variable      (Node* node, char* diff_var);
Node* Diff_Operation     (Node* node, char* diff_var);
Node* Diff_Function      (Node* node, char* diff_var);
Node* Indicating_Parents (Node* node);
char* If_Const           (Node* node);
bool  Is_Number          (Node* node, char* diff_var);


Node* Tree_Simplification      (Node* node, Node* root, FILE* file_name);
Node* Simplification_Operation (Node* node);
Node* Simplification_Sum       (Node* node);
Node* Simplification_Sub       (Node* node);
Node* Simplification_Mul       (Node* node);
Node* Simplification_Div       (Node* node);
Node* Simplification_Pow       (Node* node);
Node* If_Numbers               (Node* node);
Node* Left_Child_Neutral       (Node* node);
Node* Right_Child_Neutral      (Node* node);
Node* If_Exp_Pow               (Node* node);
Node* Simplification_Function  (Node* node);
