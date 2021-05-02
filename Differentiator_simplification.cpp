#include "Differentiator.h"


Node* Tree_Simplification (Node* node, Node* root, FILE* file_name)
{
    assert (node);

    if (node->left_child  != NULL)
    {
        node->left_child = Tree_Simplification (node->left_child, root, file_name);
        node->left_child->parent = node;

        if (node->left_child->changed)
            Update_Tex (root, file_name);
    }

    if (node->right_child != NULL)
    {
        node->right_child = Tree_Simplification (node->right_child, root, file_name);
        node->right_child->parent = node;

        if (node->right_child->changed)
            Update_Tex (root, file_name);
    }

    switch (node->type)
    {
        case VAL:
            return node;
            break;

        case VAR:
            return node;
            break;

        case OP:
            return Simplification_Operation (node);
            break;

        case FUNC:
            return Simplification_Function  (node);
            break;

        default:
            assert (!"OK");
    }
}


Node* Simplification_Operation (Node* node)
{
    assert (node);

    switch (node->value.operation_type)
    {
        case OP_SUM:
            return Simplification_Sum (node);

        case OP_SUB:
            return Simplification_Sub (node);

        case OP_MUL:
            return Simplification_Mul (node);;

        case OP_DIV:
            return Simplification_Div (node);

        case OP_POW:
            return Simplification_Pow (node);

        default:
            assert (!"OK");
    }
}


Node* Simplification_Sum (Node* node)
{
    if      (node->left_child->type == VAL && node->right_child->type == VAL)
    {
        return If_Numbers (node);
    }

    else if (node->left_child->type == VAL && node->left_child->value.constant == NEUTRAL_SUM)
    {
        return Left_Child_Neutral (node);
    }

    else if (node->right_child->type == VAL && node->right_child->value.constant == NEUTRAL_SUM)
    {
        return Right_Child_Neutral (node);
    }

    else return node;
}


Node* Simplification_Sub (Node* node)
{
    if      (node->left_child->type == VAL && node->right_child->type == VAL)
    {
        return If_Numbers (node);
    }

    else if (node->left_child->type == VAL && node->left_child->value.constant == NEUTRAL_SUB)
    {
        Node* new_node = Left_Child_Neutral (node);

        if (new_node->type == VAL)
            new_node->value.constant *= -1;
        else
            new_node = MUL(NODE_NUM(-1.0), new_node);

        new_node->changed = YES;

        return new_node;
    }

    else if (node->right_child->type == VAL && node->right_child->value.constant == NEUTRAL_SUB)
    {
        return Right_Child_Neutral (node);
    }

    else return node;
}


Node* Simplification_Mul (Node* node)
{
    if      (node->left_child->type == VAL && node->right_child->type == VAL)
    {
        return If_Numbers (node);
    }

    else if (node->left_child->type == VAL && node->left_child->value.constant == NEUTRAL_MUL)
    {
        return Left_Child_Neutral (node);
    }

    else if (node->right_child->type == VAL && node->right_child->value.constant == NEUTRAL_MUL)
    {
        return Right_Child_Neutral (node);
    }

    else if ((node->right_child->type == VAL && node->right_child->value.constant == KILLER_MUL) ||
             (node->left_child->type  == VAL && node->left_child->value.constant  == KILLER_MUL))
    {
        Destruct_Node (node);
        Node* new_node = NODE_NUM(0.0);
        new_node->changed = YES;
        return new_node;
    }

    else return node;
}


Node* Simplification_Div (Node* node)
{
    if      (node->left_child->type == VAL && node->right_child->type == VAL)
    {
        return If_Numbers (node);
    }

    else if (node->right_child->type == VAL && node->right_child->value.constant == NEUTRAL_MUL)
    {
        return Right_Child_Neutral (node);
    }

    else if ((node->right_child->type == VAL && node->right_child->value.constant == KILLER_DIV) &&
             (node->left_child->type  == VAL && node->left_child->value.constant  == KILLER_DIV))
    {
        Destruct_Node (node);
        Node* new_node = NODE_NUM(0.0);
        new_node->changed = YES;
        return new_node;
    }

    else return node;
}


Node* Simplification_Pow (Node* node)
{
    if      (node->left_child->type == VAL && node->right_child->type == VAL)
    {
        return If_Numbers (node);
    }

    else if (node->left_child->type == VAL && node->left_child->value.constant == NEUTRAL_POW)
    {
        Destruct_Node (node);
        Node* new_node = NODE_NUM(1.0);
        new_node->changed = YES;
        return new_node;
    }

    else if (node->right_child->type == VAL && node->right_child->value.constant == NEUTRAL_POW)
    {
        return Right_Child_Neutral (node);
    }

    /*else if (node->left_child->type == VAL && node->left_child->value.constant == EXP_POW)
    {
        return If_Exp_Pow (node);
    }*/

    else return node;
}


Node* If_Numbers (Node* node)
{
    double first_value  = node->left_child->value.constant;
    double second_value = node->right_child->value.constant;

    int op_type = node->value.operation_type;

    Destruct_Node (node->left_child);
    Destruct_Node (node->right_child);

    Node* new_node = NULL;

    switch (op_type)
    {
        case OP_SUM:
            new_node = NODE_NUM(first_value + second_value);
            new_node->changed = YES;
            return new_node;

        case OP_SUB:
            new_node = NODE_NUM(first_value - second_value);
            new_node->changed = YES;
            return new_node;

        case OP_MUL:
            new_node = NODE_NUM(first_value * second_value);
            new_node->changed = YES;
            return new_node;

        case OP_DIV:
            new_node = NODE_NUM(first_value / second_value);
            new_node->changed = YES;
            assert (second_value != 0);
            return new_node;

        case OP_POW:
            new_node = NODE_NUM(pow (first_value, second_value));
            new_node->changed = YES;
            return new_node;

        default:
            assert (!"OK");
    }
}


Node* Left_Child_Neutral (Node* node)
{
    Node* survivor = node->right_child;

    node->right_child = NULL;

    Destruct_Node (node);

    survivor->changed = YES;

    return survivor;
}


Node* Right_Child_Neutral (Node* node)
{
    Node* survivor = node->left_child;

    node->left_child = NULL;

    Destruct_Node (node);

    survivor->changed = YES;

    return survivor;
}


/*Node* If_Exp_Pow (Node* node)
{
    if (node->right_child->type == FUNC &&
        node->right_child->value.operation_type == FUNC_LN)
    {
        Node* ln_arg = node->right_child->left_child;

        node->right_child->left_child = NULL;

        Destruct_Node (node);

        ln_arg->changed = YES;

        return ln_arg;
    }

    else if (node->right_child->type == OP &&
                node->right_child->value.operation_type == OP_MUL)
    {
        if (node->right_child->left_child->type == FUNC &&
            node->right_child->left_child->value.operation_type == FUNC_LN)
        {
            Destruct_Node (node->left_child);
            node->left_child = node->right_child->left_child->left_child;
            node->left_child->parent = node;
            node->right_child->left_child->left_child = NULL;

            Node* new_degree = node->right_child->right_child;
            node->right_child->right_child = NULL;
            Destruct_Node (node->right_child);

            node->right_child  = new_degree;
            new_degree->parent = node;

            node->changed = YES;

            return node;
        }

    }

    else return node;
}*/


Node* Simplification_Function (Node* node)
{
    if (node->value.operation_type       == FUNC_LN &&
        node->left_child->type           == VAL     &&
        node->left_child->value.constant == M_E)
    {
        Node* new_node = NODE_NUM(1.0);
        Destruct_Node (node);
        new_node->changed = YES;
        return new_node;
    }

    else return node;
}
