#ifndef DU_SOLVER
#define DU_SOLVER

#include "math_tree.h"
#include "math_integrator.h"
#include "equation_transformer.h"
#include <stdbool.h>

typedef enum du_type
{
    RASDEL,
    ODNOROD,
    LIN_I,
    BERNULI,
    FULL,
    UNKNOW
}du_type;

char prev_vars[2] ={'0', '0'};

int get_variables_count(node* node, int variable_count)
{ 
    if(node->op == VARIABLE && node->var_name != 'e' && node->var_name != 'p' && node->var_name != prev_vars[0] && node->var_name != prev_vars[1])
    {        
        if(prev_vars[0] == '0') prev_vars[0] = node->var_name;
        else prev_vars[1] = node->var_name;
        variable_count++;    
    }

    if(node->right != NULL) variable_count = get_variables_count(node->right, variable_count);
    if(node->left != NULL) variable_count = get_variables_count(node->left, variable_count);

    return variable_count; 
}

int count_variables(node* node)
{
   prev_vars[0] = '0';
   prev_vars[1] = '0';
   return get_variables_count(node, 0);
}

char get_dif_var(node* root)
{
    node* temp = root;
    while(temp->op != DIFF_VAR)
    {
        if(temp->right) temp = temp->right;
        else 
        {
            temp = root;
            while(temp->op != DIFF_VAR)
            {
                if(temp->left) temp = temp->left;
                else break; 
            }
            break;
        }
    }

    return temp->var_name;
}

bool is_op_of_var(node* root, operation op, char var) {
    if (!root) return false;

    // Если текущий узел — нужный оператор
    if (root->op == op) {
        // Проверяем, есть ли операнд с нужной переменной
        if ((root->left && root->left->var_name == var) || 
            (root->right && root->right->var_name == var)) {
            return true;
        }
    }

    // Рекурсивно проверяем левое и правое поддерево
    return is_op_of_var(root->left, op, var) || is_op_of_var(root->right, op, var);
}

node* solve_rasdel(node* root)
{   
    node* right = integrate(root->right, get_dif_var(root->right)); 
    node* left = integrate(root->left, get_dif_var(root->left));
    return create_op_node(EQUAL_OP, left, right);
}

int is_ratio_of(node* n, char num, char den) {
    if (!n || n->op != DIVIDE) return 0;
    if (n->left && n->left->op == VARIABLE &&
        n->right && n->right->op == VARIABLE &&
        n->left->var_name == num && n->right->var_name == den)
        return 1;
    return 0;
}

// Проверка на (x/y)
int is_inverse_ratio_of(node* n, char num, char den) {
    if (!n || n->op != DIVIDE) return 0;
    if (n->left && n->left->op == VARIABLE &&
        n->right && n->right->op == VARIABLE &&
        n->left->var_name == den && n->right->var_name == num)
        return 1;
    return 0;
}

// Подстановка через рекурсию
node* substitute_ratio_with_u(node* n, char num, char den, char new_var) {
    if (!n) return NULL;

    // === Прямое соотношение: y/x -> u ===
    if (is_ratio_of(n, num, den)) {
        return create_variable_node(new_var);
    }

    // === Обратное: x/y -> 1/u ===
    if (is_inverse_ratio_of(n, num, den)) {
        return create_op_node(DIVIDE, create_value_node(1),
                              create_variable_node(new_var));
    }

    // === Степени вида (y/x)^n → u^n ===
    if (n->op == POWER && is_ratio_of(n->left, num, den)) {
        node* exponent = substitute_ratio_with_u(n->right, num, den, new_var);
        return create_op_node(POWER, create_variable_node(new_var), exponent);
    }

    // === Общий случай: рекурсивный обход дерева ===
    node* left = substitute_ratio_with_u(n->left, num, den, new_var);
    node* right = substitute_ratio_with_u(n->right, num, den, new_var);

    node* new_node = (node*)calloc(1, sizeof(node));
    memcpy(new_node, n, sizeof(node));
    new_node->left = left;
    new_node->right = right;

    return new_node;
}

// Вспомогательная функция: если нужно вывести результат подстановки
void print_after_substitution(node* expr, char num, char den, char new_var) {
    printf("До подстановки: ");
    print_infix(expr);
    printf("\n");

    node* substituted = substitute_ratio_with_u(expr, num, den, new_var);

    printf("После подстановки u = %c/%c: ", num, den);
    print_infix(substituted);
    printf("\n");

    free(substituted);
}

node* solve_odnorod(math_tree* mt)
{
    node* substituted = substitute_ratio_with_u(mt->head, 'y', 'x', 'u');
    node* diff_u = create_diff_node('u', 'x', 1);
    node* x_var = create_variable_node('x');
    node* mul_x = create_op_node(MULTIPLY, diff_u, x_var);
    node* u_var = create_variable_node('u');
    node* left_side = create_op_node(PLUS, mul_x, u_var);

    substituted->left = left_side;

    print_infix(substituted);

    printf("\n");

    node* transformed1 = transform_to_standard(substituted);

    print_infix(transformed1); 
}

void solve_du(math_tree* mt)
{
    node* right = mt->head->right;
    node* left = mt->head->left;
   
    printf("Тип диференциального уровнения: ");
    if(right->op == VALUE && right->value == 0)
    {
        printf("в полной форме\n");
    }
    else if (left->op == MULTIPLY && left->right->op == DIFF_VAR && count_variables(left->left) &&
            right->op == MULTIPLY && right->right->op == DIFF_VAR && count_variables(right->left)) {
        printf("с разделяющимеся переменными\n");
        struct math_tree result;
        result.head = solve_rasdel(mt->head);
        print_tree(&result);
    } 
    else if(left->op == DIFF_OP){
        char left_diff = left->left->var_name;
        char right_diff = left->right->var_name;

        if(is_op_of_var(right, MULTIPLY, left_diff))
        {
            if(is_op_of_var(right, POWER, left_diff))
            {
                printf("Уравнение бернули\n");
            }
            else 
            {
            printf("Линейноне уравнение первого порядка\n");
            }
        }
        else if(is_op_of_var(right, DIVIDE, left_diff))
        {
            printf("Однородное\n");
            solve_odnorod(mt);
        }
    }
    else {
        printf("Тип неизвестен\n");
    }
}

#endif
