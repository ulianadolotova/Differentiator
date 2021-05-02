#include "Differentiator.h"


void File_Processing (Node* root, Node* old_root)
{
    assert (root);
    assert (old_root);

    FILE* tex_code = fopen ("Tex_code.latex", "w");

    Fill_Latex_Header (tex_code);

    fprintf (tex_code, "\\begin{document}\n\n");
    fprintf (tex_code, "\\maketitle\n");
    fprintf (tex_code, "\\newpage\n");



    fprintf (tex_code, "Изначальное выражение:\n");

    fprintf (tex_code, "\\[");
    Print_Tree_Latex (old_root, tex_code);
    fprintf (tex_code, "\\]\n\n");

    fprintf (tex_code, "Продифференцированное выражение:\n");

    fprintf (tex_code, "\\[");
    Print_Tree_Latex (root, tex_code);
    fprintf (tex_code, "\\]\n\n");

    Node* simple_result = Tree_Simplification (root, root, tex_code);

    Tree_Print (simple_result);

    fprintf (tex_code, "Окончательное выражение:\n");

    fprintf (tex_code, "\\[");
    Print_Tree_Latex (simple_result, tex_code);
    fprintf (tex_code, "\\]\n\n");


    fprintf (tex_code, "\\end{document}");

    fclose (tex_code);


    system ("pdflatex -output-format=pdf Tex_code.latex");
    system ("start Tex_code.pdf");
}


void Print_Tree_Latex (Node* node, FILE* file_name)
{
    assert (node);
    assert (file_name);

    switch (node->type)
    {
        case OP:

            Print_Operation (node, file_name);
            break;

        case VAL:

            Print_Value     (node, file_name);
            break;

        case FUNC:

            Print_Function  (node, file_name);
            break;

        case VAR:

            Print_Variable  (node, file_name);
            break;

        default:

            assert (!"OK");
    }
}



void Print_Operation (Node* node, FILE* file_name)
{
    assert (node);
    assert (file_name);

    switch (node->value.operation_type)
    {
        case OP_SUM:

            Print_Tree_Latex (node->left_child,  file_name);
            fprintf (file_name, "+");
            Print_Tree_Latex (node->right_child, file_name);
            break;

        case OP_SUB:

            Print_Tree_Latex (node->left_child,  file_name);
            fprintf (file_name, "-");

            if (node->right_child->type == OP && (node->right_child->value.operation_type == OP_SUM || node->right_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\left(");

            Print_Tree_Latex (node->right_child, file_name);

            if (node->right_child->type == OP && (node->right_child->value.operation_type == OP_SUM || node->right_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\right)");

            break;

        case OP_MUL:

            if (node->left_child->type == OP && (node->left_child->value.operation_type == OP_SUM || node->left_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\left(");

            Print_Tree_Latex (node->left_child,  file_name);

            if (node->left_child->type == OP && (node->left_child->value.operation_type == OP_SUM || node->left_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\right)");

            fprintf (file_name, "\\cdot");

            if (node->right_child->type == OP && (node->right_child->value.operation_type == OP_SUM || node->right_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\left(");

            Print_Tree_Latex (node->right_child, file_name);

            if (node->right_child->type == OP && (node->right_child->value.operation_type == OP_SUM || node->right_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\right)");

            break;

        case OP_DIV:

            fprintf (file_name, "\\frac{");

            if (node->left_child->type == OP && (node->left_child->value.operation_type == OP_SUM || node->left_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\left(");

            Print_Tree_Latex (node->left_child,  file_name);

            if (node->left_child->type == OP && (node->left_child->value.operation_type == OP_SUM || node->left_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\right)");

            fprintf (file_name, "}{");

            if (node->right_child->type == OP && (node->right_child->value.operation_type == OP_SUM || node->right_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\left(");

            Print_Tree_Latex (node->right_child, file_name);

            if (node->right_child->type == OP && (node->right_child->value.operation_type == OP_SUM || node->right_child->value.operation_type == OP_SUB))
                fprintf (file_name, "\\right)");

            fprintf (file_name, "}");
            break;

        case OP_POW:

            if (node->left_child->type == OP && node->left_child->value.operation_type != OP_POW)
                fprintf (file_name, "\\left(");

            Print_Tree_Latex (node->left_child,  file_name);

            if (node->left_child->type == OP && node->left_child->value.operation_type != OP_POW)
                fprintf (file_name, "\\right)");

            if (node->left_child->type != FUNC)
            {
                fprintf (file_name, "^");

                fprintf (file_name, "{");

                if (node->right_child->type == OP && node->right_child->value.operation_type != OP_POW)
                    fprintf (file_name, "\\left(");

                Print_Tree_Latex (node->right_child, file_name);

                if (node->right_child->type == OP && node->right_child->value.operation_type != OP_POW)
                    fprintf (file_name, "\\right)");

                fprintf (file_name, "}");
            }
            break;

    default:
            assert (!"OK");
    }
}


void Print_Value (Node* node, FILE* file_name)
{
    assert (node);
    assert (file_name);

    char* name_const = If_Const (node);


    if (name_const)
        fprintf (file_name, "%s", name_const);
    else
    {
        if (node->value.constant < 0)
            fprintf (file_name, "(", node->value.constant);
        fprintf (file_name, "%lg", node->value.constant);
        if (node->value.constant < 0)
            fprintf (file_name, ")", node->value.constant);

    }
}


void Print_Function (Node* node, FILE* file_name)
{
    assert (node);
    assert (file_name);

    fprintf (file_name, "\\%s", Function_Type_String (node));

    if (node->parent && node->parent->type == OP && node->parent->value.operation_type == OP_POW)
    {
        fprintf (file_name, "^");
        fprintf (file_name, "{");

        if (node->parent->right_child->type == OP && node->parent->right_child->value.operation_type != OP_POW)
            fprintf (file_name, "\\left(");

        Print_Tree_Latex (node->parent->right_child, file_name);

        if (node->parent->right_child->type == OP && node->parent->right_child->value.operation_type != OP_POW)
            fprintf (file_name, "\\right)");

        fprintf (file_name, "}");
    }

    fprintf (file_name, "{");

    if (node->left_child->type == OP || node->left_child->type == FUNC)
        fprintf (file_name, "\\left(");

    Print_Tree_Latex (node->left_child, file_name);

    if (node->left_child->type == OP || node->left_child->type == FUNC)
        fprintf (file_name, "\\right)");

    fprintf (file_name, "}");
}


char* If_Const (Node* node)
{
    char* const_name = (char*) calloc (MAX_NAME_COMMAND, sizeof (char));

    if      (node->value.constant == M_E)
        strcpy (const_name, " e ");

    else if (node->value.constant == M_PI)
        strcpy (const_name, "\\pi");

    else
    {
        free (const_name);
        return NULL;
    }

    return const_name;
}



void Print_Variable (Node* node, FILE* file_name)
{
    assert (node);
    assert (file_name);

    fprintf (file_name, "{");
    fprintf (file_name, "%s", node->value.var_name);
    fprintf (file_name, "}");
}



void Fill_Latex_Header (FILE* tex_code)
{
    assert (tex_code);

    setlocale (LC_ALL, "Russian");

    fprintf (tex_code, "\\documentclass[a4paper,12pt]{article}\n\n");

    fprintf (tex_code, "\\usepackage[T2A]{fontenc}\n");
    fprintf (tex_code, "\\usepackage[cp1251]{inputenc}\n");
    fprintf (tex_code, "\\usepackage[english,russian]{babel}\n\n");

    fprintf (tex_code, "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}\n\n");

    fprintf (tex_code, "\\author{Долотова Ульяна}\n");
    fprintf (tex_code, "\\title{Дифференцирование выражений}\n");
    fprintf (tex_code, "\\date{\\today}\n\n");
}


void Update_Tex (Node* root, FILE* file_name)
{
    assert (root);
    assert (file_name);
    Insert_Tex (file_name);

    fprintf (file_name, "\\[");
    Print_Tree_Latex (root, file_name);
    fprintf (file_name, "\\]\n\n");
}


void Insert_Tex (FILE* file_name)
{
    assert  (file_name);

    char** inserts = (char**) calloc (NUM_INSERTS, sizeof (char*));

    inserts[0]  = "Обращаясь к Великим Книгам, к которым всенепременно причислены оба тома Зорича, а также учебник по математике за 7 класс, преобразуем полученное выражение к следующему виду:\n";
    inserts[1]  = "Минуя долгие размышления о Математике как царице наук, приходим к упрощению:\n";
    inserts[2]  = "Абстрагируясь от бытовых проблем и закрывая глаза на все несовершенства мира, можно узреть, как выражение преобразуется к виду:\n";
    inserts[3]  = "Пока вершатся судьбы, займемся приведением нашего выражения. Преобразуем его таким образом:\n";
    inserts[4]  = "Засыпая с мыслями о гнетущем, не забудем упростить выражение:\n";
    inserts[5]  = "Жизнь не настолько длинна, чтобы тратить её на осознание сложных выражений. Преобразуем:\n";
    inserts[6]  = "Не преминем воспользоваться мудростью наших предков и приведем выражение к следующему виду:\n";
    inserts[7]  = "Танец жизни и смерти, льда и пламени – рождает в людях чувство прекрасного. А чувство прекрасного наталкивает нас на мысль о том, что не плохо бы упростить:\n";
    inserts[8]  = "Лови момент, живи сейчас, упрощай уверенно:\n";
    inserts[9]  = "Не стоит пытаться вернуть прошлое, уж лучше подумать о будущем. Чем мы сейчас и займемся. Приведем выражение к следующему виду:\n";
    inserts[10] = "Реалии мира таковы: умён не тот, кто не усложняет, умён тот, кто упрощает:\n";
    inserts[11] = "Иногда для достижения дзена достаточно остановиться и подумать. Подумать об упрощении:\n";
    inserts[12] = "Пусть дорога жизни всенепременно приведёт нас к счастью. Мы тем временем приведём наше выражение к виду:\n";
    inserts[13] = "Каждый человек просто обязан за свою жизнь построить дом, посадить дерево и упростить выражение:\n";

    static int counter = 0;

    counter++;

    int index = counter % NUM_INSERTS;

    fprintf (file_name, "%s", inserts[index]);
}
