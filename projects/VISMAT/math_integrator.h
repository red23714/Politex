#ifndef MATH_INTEGRATOR_H
#define MATH_INTEGRATOR_H

#include "math_tree.h"

// === Создание простых узлов ===
node* create_ln_of(node* arg) { return create_function_node(LN, copy_node(arg)); }
node* create_C() { return create_variable_node('C'); } // константа интегрирования

// === Распознавание линейного аргумента a*x ===
int match_linear(node* arg, double* a, char* v) {
    if (!arg) return 0;
    if (arg->op == MULTIPLY) {
        if (is_value(arg->left) && is_variable(arg->right)) {
            *a = arg->left->value;
            *v = arg->right->var_name;
            return 1;
        }
        if (is_variable(arg->left) && is_value(arg->right)) {
            *a = arg->right->value;
            *v = arg->left->var_name;
            return 1;
        }
    }
    if (is_variable(arg)) {
        *a = 1.0;
        *v = arg->var_name;
        return 1;
    }
    return 0;
}

// === Интегрирование узла ===
node* integrate_node(node* n, char var) {
    if (!n) return NULL;

    switch (n->op) {
        // ∫ c dx = c*x
        case VALUE:
            return create_op_node(MULTIPLY, copy_node(n), create_variable_node(var));

        // ∫ x dx = x^2 / 2
        case VARIABLE:
            if (n->var_name == var) {
                node* pow = create_op_node(POWER, copy_node(n), create_value_node(2));
                return create_op_node(DIVIDE, pow, create_value_node(2));
            } else
                return create_op_node(MULTIPLY, copy_node(n), create_variable_node(var));

        // ∫ (u+v) dx
        case PLUS:
            return create_op_node(PLUS, integrate_node(n->left, var), integrate_node(n->right, var));

        // ∫ (u-v) dx
        case MINUS:
            return create_op_node(MINUS, integrate_node(n->left, var), integrate_node(n->right, var));

        // ∫ c*f dx
        case MULTIPLY:
            if (n->left && n->left->op == VALUE)
                return create_op_node(MULTIPLY, copy_node(n->left), integrate_node(n->right, var));
            if (n->right && n->right->op == VALUE)
                return create_op_node(MULTIPLY, copy_node(n->right), integrate_node(n->left, var));

            // (f)*dx
            if (n->left && n->left->op == DIFF_VAR)
                return integrate_node(n->right, n->left->var_name);
            if (n->right && n->right->op == DIFF_VAR)
                return integrate_node(n->left, n->right->var_name);
            return create_value_node(0);

        // ∫ c/x dx = c*ln|x|
        case DIVIDE:
            if (n->left && n->left->op == VALUE &&
                n->right && n->right->op == VARIABLE &&
                n->right->var_name == var)
                return create_op_node(MULTIPLY, copy_node(n->left), create_ln_of(n->right));

            // ∫ c/x^n dx = c * x^{-n+1}/(-n+1)
            if (n->left && n->left->op == VALUE &&
                n->right && n->right->op == POWER &&
                n->right->left && n->right->left->op == VARIABLE &&
                n->right->left->var_name == var &&
                n->right->right && n->right->right->op == VALUE) {
                double c = n->left->value;
                double nval = n->right->right->value;
                if (fabs(nval - 1.0) < 1e-9)
                    return create_op_node(MULTIPLY, create_value_node(c), create_ln_of(create_variable_node(var)));
                else {
                    double newexp = 1 - nval;
                    node* pow = create_op_node(POWER, create_variable_node(var), create_value_node(newexp));
                    return create_op_node(MULTIPLY, create_value_node(c / newexp), pow);
                }
            }
            return create_value_node(0);

        // ∫ x^n dx = x^(n+1)/(n+1)
        case POWER:
            if (n->left && n->left->op == VARIABLE && n->left->var_name == var &&
                n->right && n->right->op == VALUE) {
                double nval = n->right->value;
                if (fabs(nval + 1) < 1e-9)
                    return create_ln_of(n->left);
                else {
                    node* newexp = create_value_node(nval + 1);
                    node* pow = create_op_node(POWER, copy_node(n->left), newexp);
                    return create_op_node(DIVIDE, pow, create_value_node(nval + 1));
                }
            }

            // === e^(a*x) ===
            if (n->left && n->left->op == VARIABLE && n->left->var_name == 'e') {
                double a; char v;
                if (match_linear(n->right, &a, &v) && v == var) {
                    node* pow_e = create_op_node(POWER, create_variable_node('e'), copy_node(n->right));
                    return create_op_node(DIVIDE, pow_e, create_value_node(a));
                }
                // e^(x)
                if (is_variable(n->right) && n->right->var_name == var)
                    return create_op_node(POWER, create_variable_node('e'), copy_node(n->right));
            }
            return create_value_node(0);

        // sin, cos, tan, ln
        case SIN:
            if (n->left && n->left->op == VARIABLE && n->left->var_name == var)
                return create_op_node(MINUS, create_value_node(0),
                                      create_function_node(COS, copy_node(n->left)));
            else {
                double a; char v;
                if (match_linear(n->left, &a, &v) && v == var)
                    return create_op_node(DIVIDE,
                        create_op_node(MINUS, create_value_node(0),
                            create_function_node(COS, copy_node(n->left))),
                        create_value_node(a));
            }
            break;

        case COS:
            if (n->left && n->left->op == VARIABLE && n->left->var_name == var)
                return create_function_node(SIN, copy_node(n->left));
            else {
                double a; char v;
                if (match_linear(n->left, &a, &v) && v == var)
                    return create_op_node(DIVIDE,
                        create_function_node(SIN, copy_node(n->left)),
                        create_value_node(a));
            }
            break;

        case TAN:
            if (n->left && n->left->op == VARIABLE && n->left->var_name == var) {
                node* lncos = create_function_node(LN, create_function_node(COS, copy_node(n->left)));
                return create_op_node(MINUS, create_value_node(0), lncos);
            } else {
                double a; char v;
                if (match_linear(n->left, &a, &v) && v == var) {
                    node* lncos = create_function_node(LN, create_function_node(COS, copy_node(n->left)));
                    return create_op_node(DIVIDE,
                        create_op_node(MINUS, create_value_node(0), lncos),
                        create_value_node(a));
                }
            }
            break;

        case LN:
            if (n->left && n->left->op == VARIABLE && n->left->var_name == var) {
                node* term1 = create_op_node(MULTIPLY, copy_node(n->left),
                                             create_function_node(LN, copy_node(n->left)));
                node* term2 = copy_node(n->left);
                return create_op_node(MINUS, term1, term2);
            }
            break;

        default:
            break;
    }
    return copy_node(n);
}

// === Добавление +C ===
node* add_constant(node* expr) {
    return create_op_node(PLUS, expr, create_C());
}

// === Основной интерфейс ===
node* integrate(node* mt, char var) {
    node* result = integrate_node(mt, var);
    result = add_constant(result);
    return simplify(result);
}

#endif
