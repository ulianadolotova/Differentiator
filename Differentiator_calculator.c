#include "Differentiator.h"


Node* Differentiate_Node (Node* node, char* diff_var)
{
    assert (node);
    assert (diff_var);

    switch (node->type)
    {
        case VAL:
            return Diff_Value     (node, diff_var);

        case VAR:
            return Diff_Variable  (node, diff_var);

        case OP:
            return Diff_Operation (node, diff_var);

        case FUNC:
            return Diff_Function  (node, diff_var);

        default:
            assert (!"OK");
    }
}


Node* Diff_Value (Node* node, char* diff_var)
{
    assert (node);
    assert (diff_var);

    return Create_Node (VAL, { .constant = 0.0 }, NULL, NULL);
}


Node* Diff_Variable (Node* node, char* diff_var)
{
    assert (node);
    assert (diff_var);

    return (strcmpi (diff_var, node->value.var_name) == 0) ? NODE_NUM (1.0) : NODE_NUM (0.0);
}


Node* Diff_Operation (Node* node, char* diff_var)
{
    assert (node);
    assert (diff_var);

    switch (node->value.operation_type)
    {
        case OP_SUM:
            return SUM(d(LEFT), d(RIGHT));

        case OP_SUB:
            return SUB(d(LEFT), d(RIGHT));

        case OP_MUL:
            return SUM(MUL(d(LEFT), c(RIGHT)), MUL(c(LEFT), d(RIGHT)));

        case OP_DIV:
            return DIV(SUB(MUL(d(LEFT), c(RIGHT)), MUL(c(LEFT), d(RIGHT))), MUL(c(RIGHT), c(RIGHT)));

        case OP_POW:
            if (Is_Number(RIGHT, diff_var))
            {
                return MUL(d(LEFT), MUL(c(RIGHT), POW(c(LEFT), SUB(c(RIGHT), NODE_NUM(1.0)))));
            }

            else if (Is_Number(LEFT, diff_var))
            {
                return MUL(LN(c(LEFT)), MUL(c(node), d(c(RIGHT))));
            }

            else
            {
                return MUL(POW(NODE_NUM(M_E), MUL(LN(c(LEFT)), c(RIGHT))), d(MUL(LN(c(LEFT)), c(RIGHT))));
            }

        default:
            assert (!"OK");
    }
}


Node* Diff_Function (Node* node, char* diff_var)
{
    assert (node);
    assert (diff_var);

    switch (node->value.operation_type)
    {
        case FUNC_SIN:
            return MUL(COS(c(LEFT)), d(LEFT));

        case FUNC_COS:
            return MUL(MUL(SIN(c(LEFT)), d(LEFT)), NODE_NUM(-1.0));

        case FUNC_LN:
            return MUL(DIV(NODE_NUM(1.0), c(LEFT)), d(LEFT));

        default:
            assert (!"OK");
    }
}


Node* Indicating_Parents (Node* node)
{
    assert (node);

    if (node->left_child != NULL)
    {
        node->left_child->parent = node;
        Indicating_Parents (node->left_child);
    }

    if (node->right_child != NULL)
    {
        node->right_child->parent = node;
        Indicating_Parents (node->right_child);
    }

    return node;
}


bool Is_Number (Node* node, char* diff_var)
{
    switch (node->type)
    {
        case VAL:   return true;

        case VAR:   return (strcmpi (node->value.var_name, diff_var) == 0) ? false : true;

        case OP:    return (Is_Number(LEFT, diff_var) && Is_Number(RIGHT, diff_var));

        case FUNC:  return Is_Number(LEFT, diff_var);
    }
}
