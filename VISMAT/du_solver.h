#ifndef DU_SOLVER
#define DU_SOLVER

#include "math_tree.h"
#include "math_integrator.h"
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

node* solve_odnorod(math_tree* mt)
{

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
        }
    }
    else {
        printf("Тип неизвестен\n");
    }
}

#endif
