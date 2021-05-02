#include "Differentiator.h"

int main ()
{
    str_info* info = String_Info_Construct ();
    printf ("\nEnter the expression you want to differentiate:\n");
    gets (info->pointer);

    Node* result = Get_General (info);

    String_Info_Destruct (info);

    assert (result);

    Tree_Print (result);

    char* diff_var = (char*) calloc (MAX_NAME_COMMAND, sizeof (char));
    printf ("\nEnter the variable by which you want to differentiate:\n");
    gets (diff_var);

    Node* calc_result = Differentiate_Node (result, diff_var);
    calc_result       = Indicating_Parents (calc_result);

    Tree_Print (calc_result);

    File_Processing (calc_result, result);

    return 0;
}


Node* Create_Node (int type, element value, Node* left_child, Node* right_child)
{
    Node* node_elem = (Node*) calloc (1, sizeof (Node));

    node_elem->type        = type;
    node_elem->value       = value;
    node_elem->parent      = NULL;
    node_elem->left_child  = left_child;
    node_elem->right_child = right_child;

    return node_elem;
}


void Destruct_Node (Node* node)
{
    if (node->left_child != NULL)
        Destruct_Node (node->left_child);

    if (node->right_child != NULL)
        Destruct_Node (node->right_child);

    *node = {};

    free (node);
}


str_info* String_Info_Construct (void)
{
    str_info* info    = (str_info*) calloc (1, sizeof (str_info));
    info->pointer     = (char*)     calloc (MAX_LENTH_EXPRESSION, sizeof (char));
    info->offset      = 0;
    info->error_check = PASSED;

    return info;
}


void String_Info_Destruct (str_info* info)
{
    assert (info);
    assert (info->pointer);

    free (info->pointer);

    info->pointer     = NULL;
    info->offset      = 0;
    info->error_check = 0;

    free (info);
}


Node* Get_General (str_info* info)
{
    Node* result = Get_Expression (info);

    if (info->error_check == FAILED)
        return NULL;

    Syntax_Require (info, '\0');

    return result;
}


Node* Get_Expression (str_info* info)
{
    Node* result = Get_Term (info);

    if (info->error_check == FAILED)
        return NULL;

    while (info->pointer[info->offset] == '+' || info->pointer[info->offset] == '-')
    {
        int operation = info->pointer[info->offset];
        info->offset++;

        Node* temp_result = Get_Term (info);

        if (info->error_check == FAILED)
            return NULL;

        if (operation == '+')
            result = Create_Node (OP, { .operation_type = OP_SUM }, result, temp_result);

        else
            result = Create_Node (OP, { .operation_type = OP_SUB }, result, temp_result);

        temp_result->parent        = result;
        result->left_child->parent = result;
    }

    return result;
}


Node* Get_Term (str_info* info)
{
    Node* result = Get_Degree (info);

    if (info->error_check == FAILED)
        return NULL;

    while (info->pointer[info->offset] == '*' || info->pointer[info->offset] == '/')
    {
        int operation = info->pointer[info->offset];
        info->offset++;

        Node* temp_result = Get_Degree (info);

        if (info->error_check == FAILED)
                return NULL;

        if (operation == '*')
            result = Create_Node (OP, { .operation_type = OP_MUL }, result, temp_result);

        else
        {
            if (temp_result->value.constant == 0)
                Arithmetic_Error (info);

            else
                result = Create_Node (OP, { .operation_type = OP_DIV }, result, temp_result);

            if (info->error_check == FAILED)
                return NULL;
        }

        temp_result->parent        = result;
        result->left_child->parent = result;
    }

    return result;
}


Node* Get_Degree (str_info* info)
{
    Node* result = Get_PrimaryExpression (info);

    if (info->error_check == FAILED)
        return NULL;

    while (info->pointer[info->offset] == '^')
    {
        info->offset++;
        Node* arg = Get_PrimaryExpression (info);

        result = Create_Node (OP, { .operation_type = OP_POW }, result, arg);

        result->left_child->parent = result;
        arg->parent                = result;

        if (info->error_check == FAILED)
            return NULL;
    }

    return result;
}


Node* Get_PrimaryExpression (str_info* info)
{
    Skip_Spaces (info);

    Node* result = NULL;

    if (info->pointer[info->offset] == '(')
    {
        info->offset++;
        result = Get_Expression (info);

        if (info->error_check == FAILED)
            return NULL;

        Syntax_Require (info, ')');

        info->offset++;
    }

    else if (isalpha (info->pointer[info->offset]) || info->pointer[info->offset] == '_')
    {
        result = Get_VariableOrFunction (info);

        if (info->error_check == FAILED)
            return NULL;
    }

    else
    {
        result = Get_Number (info);

        if (info->error_check == FAILED)
            return NULL;
    }

    Skip_Spaces (info);

    return result;
}



Node* Get_VariableOrFunction (str_info* info)
{
    Node* result = Get_Variable (info);

    if (info->error_check == FAILED)
            return NULL;

    Skip_Spaces (info);

    if (info->pointer[info->offset] == '(')
    {
        info->offset++;
        Node* arg = Get_Expression (info);

        if (info->error_check == FAILED)
            return NULL;

        result->type = FUNC;

        int func_type = Define_Function_Type (result->value.var_name);
        result->value.operation_type = func_type;

        result->left_child = arg;
        arg->parent        = result;

        Syntax_Require (info, ')');
        info->offset++;
    }

    return result;

}


Node* Get_Variable (str_info* info)
{
    char* name_var = (char*) calloc (MAX_NAME_COMMAND, sizeof (char));

    int counter = 0;

    sscanf (info->pointer + info->offset, "%[0-9,A-Z,a-z,_]%n", name_var, &counter);
    info->offset += counter;

    Node* node_elem = Create_Node (VAR, (element) { .var_name = name_var }, NULL, NULL);

    return node_elem;
}


Node* Get_Number (str_info* info)
{
    char* ptr_end = NULL;
    double result = strtod (info->pointer + info->offset, &ptr_end);

    size_t offset = ptr_end - (info->pointer + info->offset);

    if (offset == 0)
        Syntax_Error (info);

    info->offset += offset;

    Node* node_elem = Create_Node (VAL, { .constant = result }, NULL, NULL);

    return (info->error_check == PASSED) ? node_elem : NULL;
}


void Syntax_Require (str_info* info, char elem)
{
    if (info->pointer[info->offset] != elem)
        Syntax_Error (info);
}


void Skip_Spaces (str_info* info)
{
    while (isspace (info->pointer[info->offset]))
        info->offset++;
}


int Define_Function_Type (char* name_var)
{
    if (strcmpi ("sin", name_var) == 0)
        return FUNC_SIN;

    if (strcmpi ("cos", name_var) == 0)
        return FUNC_COS;

    if (strcmpi ("ln",  name_var) == 0)
        return FUNC_LN;

    return 0;
}



