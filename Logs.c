#include "Differentiator.h"


void Tree_Print (Node* tree)
{
    static size_t counter = 0;

    FILE* Tree_image = fopen ("Tree_image.txt", "w");

    fprintf (Tree_image, "digraph G {\n");
    fprintf (Tree_image, "node [shape = \"record\", style = \"filled\", fillcolor = \" #E7ECFF\"]\n");

    Node_Print (tree, Tree_image);

    if (tree->left_child != NULL)
    {
        Recursive_Print_Tree (tree->left_child, Tree_image);
        fprintf (Tree_image, "\"%p\" -> \"%p\"\n", tree, tree->left_child);
    }

    if (tree->right_child != NULL)
    {
        Recursive_Print_Tree (tree->right_child, Tree_image);
        fprintf (Tree_image, "\"%p\" -> \"%p\"\n", tree, tree->right_child);
    }

    fprintf (Tree_image, "}");

    fclose (Tree_image);

    char name_1[MAX_NAME_COMMAND] = {};
    char name_2[MAX_NAME_COMMAND] = {};

    sprintf (name_1, "dot -Tjpeg Tree_image.txt > Tree_image_%d.jpeg", counter);
    sprintf (name_2, "start Tree_image_%d.jpeg", counter);

    system (name_1);
    system (name_2);

    counter++;
}


void Node_Print (Node* tree, FILE* Tree_image)
{
    switch (tree->type)
    {
        case VAL:
            Value_Print_Logfile     (tree, Tree_image);
            break;

        case OP:
            Operation_Print_Logfile (tree, Tree_image);
            break;


        case FUNC:
            Function_Print_Logfile  (tree, Tree_image);
            break;

        case VAR:
            Variable_Print_Logfile  (tree, Tree_image);
            break;

        default:
            assert (!"OK");
    }

}


void Recursive_Print_Tree (Node* tree, FILE* Tree_image)
{
    Node_Print (tree, Tree_image);
    fprintf (Tree_image, "\"%p\" -> \"%p\"\n", tree, tree->parent);

    if (tree->left_child != NULL)
    {
        Recursive_Print_Tree (tree->left_child, Tree_image);
        fprintf (Tree_image, "\"%p\" -> \"%p\"\n", tree, tree->left_child);
    }

    if (tree->right_child != NULL)
    {
        Recursive_Print_Tree (tree->right_child, Tree_image);
        fprintf (Tree_image, "\"%p\" -> \"%p\"\n", tree, tree->right_child);
    }
}


void Value_Print_Logfile (Node* tree, FILE* Tree_image)
{
    assert (tree);
    fprintf (Tree_image, "\"%p\" [fillcolor = \"#E7ECFF\", label = \"{%lg}\"]\n", tree, tree->value.constant);
}


void Operation_Print_Logfile (Node* tree, FILE* Tree_image)
{
    switch (tree->value.operation_type)
    {
        case OP_SUM:
            fprintf (Tree_image, "\"%p\" [fillcolor = \"#FFFADD\", label = \"{+}\"]\n", tree);
            break;

        case OP_SUB:
            fprintf (Tree_image, "\"%p\" [fillcolor = \"#FFFADD\", label = \"{-}\"]\n", tree);
            break;

        case OP_MUL:
            fprintf (Tree_image, "\"%p\" [fillcolor = \"#FFFADD\", label = \"{*}\"]\n", tree);
            break;

        case OP_DIV:
            fprintf (Tree_image, "\"%p\" [fillcolor = \"#FFFADD\", label = \"{:}\"]\n", tree);
            break;

        case OP_POW:
            fprintf (Tree_image, "\"%p\" [fillcolor = \"#FFFADD\", label = \"{^}\"]\n", tree);
            break;

        default:
            assert (!"OK");
    }
}


void Function_Print_Logfile (Node* tree, FILE* Tree_image)
{
    fprintf (Tree_image, "\"%p\" [fillcolor = \"#C3FBD8\", label = \"{%s}\"]\n", tree, Function_Type_String (tree));
}


void Variable_Print_Logfile (Node* tree, FILE* Tree_image)
{
    fprintf (Tree_image, "\"%p\" [fillcolor = \"#FED6BC\", label = \"{%s}\"]\n", tree, tree->value.var_name);
}


void Syntax_Error (str_info* info)
{
    assert (info);

    printf ("\nSyntax_Error:\n");
    printf ("%s\n", info->pointer);

    for (size_t i = 0; i < info->offset ; i++)
        printf (" ");

    printf ("^\n");
    info->error_check = FAILED;
}


void Arithmetic_Error (str_info* info)
{
    assert (info);

    printf ("\nArithmetic_Error:\n");
    printf ("%s\n", info->pointer);

    for (size_t i = 0; i < info->offset - 1; i++)
        printf (" ");

    printf ("^\n");
    info->error_check = FAILED;
}


char* Function_Type_String (Node* tree)
{
    char name_func[MAX_NAME_COMMAND] = {};

    switch (tree->value.operation_type)
    {
        case FUNC_SIN:
            return "sin";
            break;

        case FUNC_COS:
            return "cos";
            break;

        case FUNC_LN:
            return "ln";
            break;

        default:
            assert (!"OK");
    }

    return name_func;
}
