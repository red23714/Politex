
#ifndef DIFF_H
#define DIFF_H

#include "math_tree.h"

node* diff(node* n, char var)
{
    if (!n) return NULL;

    switch (n->op) {
        case VALUE:
            return create_value_node(0);

        case VARIABLE:
            return create_value_node(n->var_name == var ? 1 : 0);

        case PLUS:
            return create_op_node(PLUS,
                diff(n->left, var),
                diff(n->right, var));

        case MINUS:
            return create_op_node(MINUS,
                diff(n->left, var),
                diff(n->right, var));

        case MULTIPLY: {
            // (f*g)' = f'*g + f*g'
            node* f_prime = diff(n->left, var);
            node* g_prime = diff(n->right, var);
            node* term1 = create_op_node(MULTIPLY, f_prime, copy_node(n->right));
            node* term2 = create_op_node(MULTIPLY, copy_node(n->left), g_prime);
            return create_op_node(PLUS, term1, term2);
        }

        case POWER: {
            // (f^n)' = n * f^(n-1) * f'
            if (n->left && n->right && n->right->op == VALUE) {
                double exponent = n->right->value;
                node* new_exp = create_value_node(exponent - 1);
                node* power_part = create_op_node(POWER, copy_node(n->left), new_exp);
                node* coeff = create_value_node(exponent);
                node* f_prime = diff(n->left, var);
                return create_op_node(MULTIPLY,
                        coeff,
                        create_op_node(MULTIPLY, power_part, f_prime));
            }
            // Общий случай для f(x)^g(x): (a^b)' = a^b * (b'*ln(a) + b*a'/a)
            else {
                node* a = copy_node(n->left);
                node* b = copy_node(n->right);
                node* a_prime = diff(n->left, var);
                node* b_prime = diff(n->right, var);

                node* ln_a = create_function_node(LN, copy_node(a));
                node* term1 = create_op_node(MULTIPLY, b_prime, ln_a);
                node* term2 = create_op_node(DIVIDE,
                                    create_op_node(MULTIPLY, b, a_prime),
                                    a);
                node* sum = create_op_node(PLUS, term1, term2);
                node* pow_ab = create_op_node(POWER, a, b);
                return create_op_node(MULTIPLY, pow_ab, sum);
            }
        }

        default:
            return create_value_node(0);
    }
}

#endif

