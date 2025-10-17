#ifndef EQUATION_TRANSFORMER_H
#define EQUATION_TRANSFORMER_H

#include "math_tree.h"

/*
 Универсальный трансформер уравнений.
 - Использует структуры и функции из math_tree.h
 - Основная точка входа: math_tree transform_to_standard(math_tree* input)
*/

// ---------------------- Утилиты: клонирование / освобождение -----------------------

node* clone_node(node* n) {
    if (!n) return NULL;
    node* c = (node*)calloc(1, sizeof(node));
    c->op = n->op;
    c->value = n->value;
    c->var_name = n->var_name;
    c->order = n->order;
    if (n->left) c->left = clone_node(n->left);
    if (n->right) c->right = clone_node(n->right);
    return c;
}

void free_node(node* n) {
    if (!n) return;
    if (n->left) free_node(n->left);
    if (n->right) free_node(n->right);
    free(n);
}

math_tree clone_tree(math_tree* mt) {
    math_tree r = { NULL };
    if (!mt) return r;
    r.head = clone_node(mt->head);
    return r;
}

// ---------------------- Побочные проверки -----------------------

int node_is_diffop(node* n) {
    return n && n->op == DIFF_OP;
}

int node_is_diffvar(node* n) {
    return n && n->op == DIFF_VAR;
}

int node_is_var(node* n, char v) {
    return n && n->op == VARIABLE && n->var_name == v;
}

// Поиск наличия переменной v в дереве
int contains_var(node* n, char v) {
    if (!n) return 0;
    if (n->op == VARIABLE && n->var_name == v) return 1;
    if (n->op == DIFF_VAR && n->var_name == v) return 0; // dx/dy как отдельный токен не считаем переменной в выражении
    return contains_var(n->left, v) || contains_var(n->right, v);
}

// Поиск DIFF_OP в поддереве
int contains_diffop(node* n) {
    if (!n) return 0;
    if (n->op == DIFF_OP) return 1;
    return contains_diffop(n->left) || contains_diffop(n->right);
}

int contains_diffvar(node* n)
{
    if (!n) return 0;
    if (n->op == DIFF_VAR) return 1;
    return contains_diffop(n->left) || contains_diffop(n->right);
}

// Проверка, является ли поддерево множителем вида (coef * DIFF_OP) или (DIFF_OP * coef).
// Возвращает 1 и выставляет *coef (клонирован) и *diffptr (клонирован) если найдено.
// В простейшей реализации предполагаем, что MULTIPLY с одной стороной DIFF_OP.
int extract_coef_and_diff(node* term, node** coef_out, node** diff_out) {
    if (!term) return 0;
    if (term->op == MULTIPLY) {
        if (node_is_diffop(term->left)) {
            *diff_out = clone_node(term->left);
            *coef_out = clone_node(term->right);
            return 1;
        } else if (node_is_diffop(term->right)) {
            *diff_out = clone_node(term->right);
            *coef_out = clone_node(term->left);
            return 1;
        }
    } else if (node_is_diffop(term)) {
        // coef = 1
        *diff_out = clone_node(term);
        *coef_out = create_value_node(1);
        return 1;
    }
    return 0;
}

// Удаление одного слагаемого term из выражения expr (на верхнем уровне свёртки только для PLUS/MINUS).
// Возвращает новый узел (клонированный) без данного слагаемого.
// Простая версия: если expr == term -> возвращает NULL (значит 0), если expr op PLUS/MINUS, пытаемся удалить один из сыновей.
node* remove_term_from_sum(node* expr, node* term) {
    if (!expr) return NULL;
    if (expr->op != PLUS && expr->op != MINUS) {
        // если выражение равно искомому терму — возвращаем 0 (NULL означает 0)
        // сравнение по указателю не подходит — используем простую печать/строку? для простоты будем сравнивать структурно только DIFF_OP случай
        if (contains_diffop(expr) && contains_diffop(term) && expr->op == term->op && expr->left && term->left && expr->left->op == term->left->op) {
            // best-effort: если оба содержат дифференциал — считаем совпадающими
            return NULL; // 0
        }
        // не нашли -> возвращаем clone expr
        return clone_node(expr);
    } else {
        // expr = left +/- right
        node* left_removed = remove_term_from_sum(expr->left, term);
        node* right_removed = remove_term_from_sum(expr->right, term);

        if (!left_removed && !right_removed) {
            return NULL; // 0
        } else if (!left_removed) {
            // только right part remains, but if expr was MINUS and term was in left, sign matters.
            if (expr->op == MINUS) {
                // original: left - right
                // left removed -> 0 - right_removed = -right_removed
                node* neg = create_op_node(MINUS, create_value_node(0), right_removed);
                return neg;
            } else {
                return right_removed;
            }
        } else if (!right_removed) {
            // only left remains
            return left_removed;
        } else {
            // both parts remain -> reconstruct op
            return create_op_node(expr->op, left_removed, right_removed);
        }
    }
}

// Умножение обеих сторон уравнения на factor (создаёт новый EQUAL_OP, клонирует)
node* multiply_both_sides(node* eq_root, node* factor) {
    if (!eq_root || eq_root->op != EQUAL_OP) return NULL;
    node* lhs = create_op_node(MULTIPLY, clone_node(eq_root->left), clone_node(factor));
    node* rhs = create_op_node(MULTIPLY, clone_node(eq_root->right), clone_node(factor));
    return create_op_node(EQUAL_OP, lhs, rhs);
}

// Деление обеих сторон уравнения на divisor
node* divide_both_sides(node* eq_root, node* divisor) {
    if (!eq_root || eq_root->op != EQUAL_OP) return NULL;
    node* lhs = create_op_node(DIVIDE, clone_node(eq_root->left), clone_node(divisor));
    node* rhs = create_op_node(DIVIDE, clone_node(eq_root->right), clone_node(divisor));
    return create_op_node(EQUAL_OP, lhs, rhs);
}

// Перенос expression 'term' из left->right или right->left (to_right=1 перенести из левой в правую)
// Возвращает новый корень уравнения (клонированный).
node* move_term_across_eq(node* eq_root, node* term, int from_left_to_right) {
    if (!eq_root || eq_root->op != EQUAL_OP) return NULL;
    node* lhs = clone_node(eq_root->left);
    node* rhs = clone_node(eq_root->right);

    if (from_left_to_right) {
        // lhs = ... + term + ...
        node* new_lhs = remove_term_from_sum(lhs, term);
        // rhs' = rhs +/- term (with sign change)
        // Implement: rhs + (-1)*term
        node* neg_term = create_op_node(MULTIPLY, create_value_node(-1), clone_node(term));
        node* new_rhs = create_op_node(PLUS, rhs, neg_term);
        // handle NULL new_lhs -> 0
        if (!new_lhs) new_lhs = create_value_node(0);
        return create_op_node(EQUAL_OP, new_lhs, new_rhs);
    } else {
        // from right to left
        node* new_rhs = remove_term_from_sum(rhs, term);
        node* neg_term = create_op_node(MULTIPLY, create_value_node(-1), clone_node(term));
        node* new_lhs = create_op_node(PLUS, lhs, neg_term);
        if (!new_rhs) new_rhs = create_value_node(0);
        return create_op_node(EQUAL_OP, new_lhs, new_rhs);
    }
}

// ---------------------- Изоляция производной (сделать вид: d(...) / dx = RHS) -----------------------

// На верхнем уровне левой части уравнения ищем тот терм, что содержит DIFF_OP.
// Если найден, возвращаем новый корень, где слева — DIFF_OP (без коэффициента), справа — остальные члены / coef.
node* isolate_diff_to_left(node* eq_root) {
    if (!eq_root || eq_root->op != EQUAL_OP) return NULL;

    node* left = eq_root->left;
    node* right = eq_root->right;

    // If left contains diffop somewhere (sum/diff/multiply), try extract
    // Cases:
    // 1) left is MULTIPLY(coef, DIFF_OP) -> divide both sides by coef
    // 2) left is PLUS/MINUS including a term that is MULTIPLY(coef, DIFF_OP)
    // 3) left is DIFF_OP itself -> already isolated

    // Case 3
    if (node_is_diffop(left)) {
        return clone_node(eq_root);
    }

    // Case 1
    node* coef = NULL;
    node* diffn = NULL;
    if (extract_coef_and_diff(left, &coef, &diffn)) {
        // формируем: DIFF = (right) / coef
        node* new_rhs = create_op_node(DIVIDE, clone_node(right), coef);
        return create_op_node(EQUAL_OP, diffn, new_rhs);
    }

    // Case 2: left is sum/diff: попробуем найти слагаемое, которое содержит DIFF_OP
    if (left->op == PLUS || left->op == MINUS) {
        // если левая часть является PLUS/MINUS, ищем ветвь содержащую DIFF_OP
        // простая стратегия: если left->left содержит DIFF_OP — считаем это термом
        node* term = NULL;
        if (contains_diffop(left->left)) term = left->left;
        else if (contains_diffop(left->right)) term = left->right;

        if (term) {
            node* coef2 = NULL;
            node* diffn2 = NULL;
            if (!extract_coef_and_diff(term, &coef2, &diffn2)) {
                // если term сам по себе не MULTIPLY(DIFF, coef), может быть DIFF_OP - тогда coef2 = 1
                if (node_is_diffop(term)) {
                    diffn2 = clone_node(term);
                    coef2 = create_value_node(1);
                } else {
                    // неизвестный формат — не можем
                    return clone_node(eq_root);
                }
            }
            // rest = left - term
            node* rest = remove_term_from_sum(left, term);
            // new rhs = right + (-rest)
            node* neg_rest = rest ? create_op_node(MULTIPLY, create_value_node(-1), rest) : create_value_node(0);
            node* new_rhs = create_op_node(PLUS, clone_node(right), neg_rest);
            // divide both sides by coef2
            node* divided_rhs = create_op_node(DIVIDE, new_rhs, coef2);
            return create_op_node(EQUAL_OP, diffn2, divided_rhs);
        }
    }

    // Если правой части есть дифференциал (вариант, когда дифф. стоит справа) — поменяем местами
    if (contains_diffop(right)) {
        // переместим всё что не относится к diff на другую сторону
        // для простоты поменяем местами lhs<->rhs and recurse
        node* swapped = create_op_node(EQUAL_OP, clone_node(right), clone_node(left));
        node* res = isolate_diff_to_left(swapped);
        free_node(swapped);
        return res;
    }

    // Не удалось изолировать diff
    return clone_node(eq_root);
}

// ---------------------- Приведение для разделяемых -----------------------

// Если у нас дифф. уже изолирован: DIFF = NUM / DEN
// и DEN содержит y, NUM содержит только x => приводим к (DEN)*dy = (NUM)*dx
// Предполагается, что дифф представляет d?y/d?x, т.е. diff->left = 'y', diff->right = 'x'
int is_fraction(node* n, node** num_out, node** den_out) {
    if (!n) return 0;
    if (n->op == DIVIDE) {
        *num_out = clone_node(n->left);
        *den_out = clone_node(n->right);
        return 1;
    }
    return 0;
}

node* make_separable_if_possible(node* eq_root) {
    if (!eq_root || eq_root->op != EQUAL_OP) return NULL;
    // ожидаем: left is DIFF_OP
    node* left = eq_root->left;
    node* right = eq_root->right;

    if (!node_is_diffop(left)) return NULL;

    node* num = NULL;
    node* den = NULL;
    if (is_fraction(right, &num, &den)) {
        // check that den contains y and num contains x (best-effort)
        char y = left->left ? left->left->var_name : 'y';
        char x = left->right ? left->right->var_name : 'x';
        if (contains_var(den, y) && !contains_var(den, x) && contains_var(num, x) && !contains_var(num, y)) {
            // build (den)*dy = (num)*dx
            node* L = create_op_node(MULTIPLY, den, create_diffvar_node(y));
            node* R = create_op_node(MULTIPLY, num, create_diffvar_node(x));
            return create_op_node(EQUAL_OP, L, R);
        }
    }
    // также случай: right = MULTIPLY(num, INV(den)) или другие - not implemented
    return NULL;
}

// ---------------------- Распознавание типа и финальная нормализация -----------------------

typedef enum {
    TYPE_LINEAR,
    TYPE_BERNOULLI,
    TYPE_SEPARABLE,
    TYPE_DIFFERENTIAL_FORM,
    TYPE_UNKNOWN
} eq_type;

eq_type detect_equation_type(node* eq_root) {
    if (!eq_root || eq_root->op != EQUAL_OP) return TYPE_UNKNOWN;
    node* left = eq_root->left;
    node* right = eq_root->right;

    // differential form: содержит dx или dy в верхнем выражении и уравнение равно 0
    if ((contains_var(eq_root, 'x') || contains_var(eq_root, 'y')) && (contains_diffvar(eq_root->left) || contains_diffvar(eq_root->right) || contains_diffvar(eq_root->left))) {
        // более точная проверка: expression = 0, и в expr есть DIFF_VAR (dx/dy)
        if ((left && (contains_var(left, 'x') || contains_var(left, 'y')) && contains_diffvar(left)) ||
            (right && (contains_var(right, 'x') || contains_var(right, 'y')) && contains_diffvar(right))) {
            return TYPE_DIFFERENTIAL_FORM;
        }
    }

    // Если есть DIFF_OP -> возможно separable/linear/Bernoulli
    if (contains_diffop(eq_root)) {
        // isolate diff to left to inspect right
        node* isolated = isolate_diff_to_left(eq_root);
        node* r = isolated->right;

        // separable: right is fraction num/den where den contains y and num contains x
        node* nnum = NULL; node* nden = NULL;
        if (is_fraction(r, &nnum, &nden)) {
            char y = isolated->left->left ? isolated->left->left->var_name : 'y';
            char x = isolated->left->right ? isolated->left->right->var_name : 'x';
            int den_has_y = contains_var(nden, y);
            int num_has_x = contains_var(nnum, x);
            free_node(isolated);
            if (den_has_y && num_has_x) return TYPE_SEPARABLE;
        } else {
            // Бернулли: правая часть содержит y^n как множитель
            // простая проверка: есть POWER с базой y
            // рекурсивно проверим right на POWER с base VARIABLE y
            // функция:
            int found_power_with_y = 0;
            // stackless traversal
            node* stack[256]; int sp=0;
            if (r) stack[sp++] = r;
            while(sp) {
                node* cur = stack[--sp];
                if (!cur) continue;
                if (cur->op == POWER && cur->left && cur->left->op == VARIABLE) {
                    // candidate
                    found_power_with_y = 1;
                    break;
                }
                if (cur->right) stack[sp++] = cur->right;
                if (cur->left) stack[sp++] = cur->left;
            }
            free_node(isolated);
            if (found_power_with_y) return TYPE_BERNOULLI;
        }

        // иначе, видим как линейное уравнение
        return TYPE_LINEAR;
    }

    return TYPE_UNKNOWN;
}

// ---------------------- Главная функция трансформации -----------------------

math_tree transform_to_standard(math_tree* input) {
    math_tree result = { NULL };
    if (!input || !input->head) return result;
    node* root = clone_node(input->head);

    // 1) если уравнение уже в форме дифф.формы: M(x,y)dx + N(x,y)dy = 0 -> просто вернуть (нормализовано)
    // detect: уравнение = 0 и содержатся DIFF_VAR в левой части (или right-left)
    if (root->op == EQUAL_OP) {
        // если right == 0 и left содержит DIFF_VAR -> differential form
        if (root->right->op == VALUE && root->right->value == 0 && contains_diffvar(root->left)) {
            result.head = root;
            return result;
        }
    }

    // 2) Изолируем производную d.../d... в левую часть
    node* isolated = isolate_diff_to_left(root);
    free_node(root);
    root = isolated;

    // 3) Попытаемся привести к разделяемому виду (M(y)dy = N(x)dx)
    node* sep = make_separable_if_possible(root);
    if (sep) {
        result.head = sep;
        return result;
    }

    // 4) Определим тип и сделаем дополнительные шаги
    eq_type t = detect_equation_type(root);
    if (t == TYPE_SEPARABLE) {
        // уже покрыто в make_separable_if_possible но покроем случай, если detect увидел separable
        node* sep2 = make_separable_if_possible(root);
        if (sep2) {
            result.head = sep2;
            return result;
        }
    } else if (t == TYPE_DIFFERENTIAL_FORM) {
        // если обнаружили diff form — просто вернуть (или можно нормализовать в left=0)
        result.head = clone_node(root);
        return result;
    } else if (t == TYPE_LINEAR) {
        // Нормализуем линейное уравнение: сделаем коэфф. перед y' = 1
        // root: DIFF_OP = RHS (или left side DIFF_OP)
        if (root->op == EQUAL_OP && node_is_diffop(root->left)) {
            // diff already isolated
            result.head = simplify(root);
            return result;
        } else {
            // возможно left part is something like (x*DIFF_OP - 2*y)
            node* isolated2 = isolate_diff_to_left(root);
            free_node(root);
            root = isolated2;
            // теперь left should be DIFF_OP or MULTIPLY(DIFF_OP, coef)
            if (node_is_diffop(root->left)) {
                // ensure coefficient is 1 (isolate_diff_to_left tries to do that)
                // but isolate_diff_to_left may have already divided by coef
                // final simplify
                result.head = simplify(root);
                return result;
            }
            result.head = clone_node(root);
            return result;
        }
    } else if (t == TYPE_BERNOULLI) {
        // Изолируем дифферениал, приведём к виду y' + P(x)y = Q(x) y^n
        node* isolated2 = isolate_diff_to_left(root);
        free_node(root);
        root = isolated2;
        result.head = simplify(root);
        return result;
    }

    // fallback: вернуть изолированную форму (если содержится DIFF_OP)
    if (contains_diffop(root)) {
        result.head = simplify(root);
        return result;
    }

    // если ничего не подошло — вернуть клонированное исходное (нормализация не выполнена)
    result.head = clone_node(input->head); 
    return result;
}

#endif
