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
    printf("Интегрируем обе части:\n");
    node* right = integrate(root->right, get_dif_var(root->right)); 
    node* left = integrate(root->left, get_dif_var(root->left));
    return create_op_node(EQUAL_OP, left, right);
}


node* solve_odnorod(node* root)
{
    // 1️⃣ Определяем переменные
    char y_var = root->left->left->var_name;   // y
    char x_var = root->left->right->var_name;  // x

    // 2️⃣ Подстановка u = y/x
    node* substituted = substitute_binop_with_var(root, DIVIDE, y_var, x_var, 'u');

    // Создаем дифференциал du/dx = ...
    node* diff_u = create_diff_node('u', x_var, 1);
    node* u_var = create_variable_node('u');
    node* x_node = create_variable_node(x_var);

    // Левая часть уравнения после замены: du/dx = (уравнение с разделяющимися переменными)
    node* left_side = create_op_node(PLUS, create_op_node(MULTIPLY, diff_u, x_node), u_var);
    substituted->left = left_side;

    printf("После подстановки y/x = u:\n");
    print_tree(substituted);

    // 3️⃣ Приводим к стандартной форме (разделяющиеся переменные)
    node* standard_eq = transform_to_standard_2_times(substituted);

    printf("Уравнение с разделяющимися переменными:\n");
    print_tree(standard_eq);

    // 4️⃣ Решаем уравнение с разделяющимися переменными
    node* res_u = solve_rasdel(standard_eq);

    printf("Решение для u:\n");
    print_tree(res_u);

    // 5️⃣ Обратная замена u -> y/x
    node* final_result = restore_substituted_var(res_u, DIVIDE, y_var, x_var, 'u');

    printf("Общее решение исходного уравнения:\n");
    print_tree(final_result);

    return final_result;
}


node* solve_lin(node* root)
{
    node* equal_zero;
    if(count_variables(root->right->right) == 2) equal_zero = create_op_node(EQUAL_OP, root->left, root->right->right);
    else equal_zero = create_op_node(EQUAL_OP, root->left, root->right->left);

    printf("Приравниваем часть без y к нулю: ");
    print_tree(equal_zero);
    
    node* transformed_zero = transform_to_standard_2_times(equal_zero);

    printf("Решаем уравнение с разделяющимися переменными: ");
    node* res_zero = solve_rasdel(transformed_zero);
    print_tree(res_zero);

    node* transformed_res_zero = transform_to_standard_2_times(res_zero);
}

node* solve_bernuli()
{

}

void solve_du(math_tree* mt)
{
    node* res = transform_to_standard_2_times(mt->head);
    mt->head = res;

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
        node* res_rasdel = solve_rasdel(mt->head);
        print_tree(res_rasdel);
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
                solve_lin(mt->head);
            }
        }
        else if(is_op_of_var(right, DIVIDE, left_diff))
        {
            printf("Однородное\n");
            node* res_odnorod = solve_odnorod(mt->head);
        }
    }
    else {
        printf("Тип неизвестен\n");
    }
}

#endif
