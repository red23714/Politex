#ifndef BIN_TREE
#define BIN_TREE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// === типы операций ===
typedef enum operation {
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    POWER,
    EQUAL_OP,
    VARIABLE,
    VALUE,
    DIFF_OP,    // d^n z / dx^n
    DIFF_VAR,   // dx, dy, dz
    // функции
    SIN, COS, TAN,
    ASIN, ACOS, ATAN,
    LOG, LN,
    NONE_OP
} operation;

// === структура узла ===
typedef struct node {
    struct node* left;
    struct node* right;
    operation op;
    double value;
    char var_name;
    int order; // для DIFF_OP
} node;

// === дерево ===
typedef struct math_tree {
    node* head;
} math_tree;

// === функции создания узлов ===
node* create_value_node(double val) {
    node* n = (node*)calloc(1, sizeof(node));
    n->op = VALUE;
    n->value = val;
    return n;
}

node* create_variable_node(char name) {
    node* n = (node*)calloc(1, sizeof(node));
    n->op = VARIABLE;
    n->var_name = name;
    return n;
}

node* create_diffvar_node(char var) {
    node* n = (node*)calloc(1, sizeof(node));
    n->op = DIFF_VAR;
    n->var_name = var;
    return n;
}

node* create_op_node(operation op, node* left, node* right) {
    node* n = (node*)calloc(1, sizeof(node));
    n->op = op;
    n->left = left;
    n->right = right;
    return n;
}

node* create_function_node(operation op, node* arg) {
    node* n = (node*)calloc(1, sizeof(node));
    n->op = op;
    n->left = arg;   // функция хранит аргумент в left
    n->right = NULL;
    return n;
}

node* create_diff_node(char func, char var, int order) {
    node* left = create_variable_node(func);
    node* right = create_variable_node(var);
    node* diff = create_op_node(DIFF_OP, left, right);
    diff->order = order;
    return diff;
}

// === печать дерева ===
void print_infix(node* n) {
    if(!n) return;

    if(n->op == VALUE) {
        // выводим без дробной части, так как у тебя раньше использовалось %.0f
        printf("%.0f", n->value);
    } else if(n->op == VARIABLE) {
        printf("%c", n->var_name);
    } else if(n->op == DIFF_VAR) {
        printf("d%c", n->var_name);
    } else if(n->op == DIFF_OP) {
        printf("d%d", n->order);
        print_infix(n->left);
        printf("/d");
        print_infix(n->right);
        printf("%d", n->order);
    } else {
        // функции
        switch(n->op) {
            case SIN:
                printf("sin("); print_infix(n->left); printf(")"); return;
            case COS:
                printf("cos("); print_infix(n->left); printf(")"); return;
            case TAN:
                printf("tan("); print_infix(n->left); printf(")"); return;
            case ASIN:
                printf("asin("); print_infix(n->left); printf(")"); return;
            case ACOS:
                printf("acos("); print_infix(n->left); printf(")"); return;
            case ATAN:
                printf("atan("); print_infix(n->left); printf(")"); return;
            case LOG:
                printf("log("); print_infix(n->left); printf(")"); return;
            case LN:
                printf("ln("); print_infix(n->left); printf(")"); return;
            default:
                break;
        }

        // бинарные операторы
        printf("(");
        print_infix(n->left);
        switch(n->op) {
            case PLUS: printf(" + "); break;
            case MINUS: printf(" - "); break;
            case MULTIPLY: printf(" * "); break;
            case DIVIDE: printf(" / "); break;
            case POWER: printf("^"); break;
            case EQUAL_OP: printf(" = "); break;
            default: break;
        }
        print_infix(n->right);
        printf(")");
    }
}

// === вывод дерева в виде дерева ===
void print_tree_structure(node* root, int depth, char* prefix) {
    if (!root) return;

    // Вывод текущего узла
    printf("%s", prefix);

    if (depth > 0) {
        printf("├── ");
    } else {
        printf("    ");
    }

    // Вывод содержимого узла
    switch(root->op) {
        case VALUE:
            printf("%.0f\n", root->value);
            break;
        case VARIABLE:
            printf("%c\n", root->var_name);
            break;
        case DIFF_VAR:
            printf("d%c\n", root->var_name);
            break;
        case DIFF_OP:
            printf("d%d%c/d%c%d\n", root->order,
                   root->left ? root->left->var_name : '?',
                   root->right ? root->right->var_name : '?',
                   root->order);
            break;
        case PLUS:
            printf("+\n");
            break;
        case MINUS:
            printf("-\n");
            break;
        case MULTIPLY:
            printf("*\n");
            break;
        case DIVIDE:
            printf("/\n");
            break;
        case POWER:
            printf("^\n");
            break;
        case EQUAL_OP:
            printf("=\n");
            break;
        case SIN:
            printf("sin\n"); break;
        case COS:
            printf("cos\n"); break;
        case TAN:
            printf("tan\n"); break;
        case ASIN:
            printf("asin\n"); break;
        case ACOS:
            printf("acos\n"); break;
        case ATAN:
            printf("atan\n"); break;
        case LOG:
            printf("log\n"); break;
        case LN:
            printf("ln\n"); break;
        default:
            printf("?\n");
            break;
    }

    // Подготовка префикса для дочерних узлов
    char new_prefix[256];
    strcpy(new_prefix, prefix);
    if (depth > 0) {
        strcat(new_prefix, "│   ");
    } else {
        strcat(new_prefix, "    ");
    }

    // Рекурсивный вывод дочерних узлов
    if (root->left || root->right) {
        if (root->left) {
            print_tree_structure(root->left, depth + 1, new_prefix);
        }
        if (root->right) {
            print_tree_structure(root->right, depth + 1, new_prefix);
        }
    }
}

void print_math_tree(math_tree* mt) {
    if (!mt || !mt->head) {
        printf("(пустое дерево)\n");
        return;
    }
    printf("Дерево выражения:\n");
    print_tree_structure(mt->head, 0, "");
}


// === лексер ===
typedef enum { TOK_NUM, TOK_VAR, TOK_OP, TOK_END, TOK_DIFFVAR, TOK_DIFF, TOK_FUNC } tok_type;

typedef struct {
    tok_type type;
    operation op;   // для операторов или функций
    double num;
    char var;
    int order;
} token;

typedef struct {
    const char* s;
    size_t pos;
} lexer;

// Проверка на функцию (sin, cos, tan, asin, acos, atan, log, ln)
operation match_function(const char* s, size_t* pos) {
    const char* funcs[] = {"sin","cos","tan","asin","acos","atan","log","ln", NULL};
    operation ops[] = {SIN,COS,TAN,ASIN,ACOS,ATAN,LOG,LN};
    for (int i = 0; funcs[i]; ++i) {
        size_t len = strlen(funcs[i]);
        if (strncmp(&s[*pos], funcs[i], len) == 0) {
            *pos += len;
            return ops[i];
        }
    }
    return NONE_OP;
}

token next_token(lexer* L) {
    token t = { .type = TOK_END, .op = NONE_OP, .num = 0, .var = 0, .order = 0 };
    const char* s = L->s;

    while(isspace((unsigned char)s[L->pos])) L->pos++;
    char c = s[L->pos];
    if(!c) return t;

    // числа
    if(isdigit((unsigned char)c)) {
        double val = 0;
        while(isdigit((unsigned char)s[L->pos])) {
            val = val * 10 + (s[L->pos] - '0');
            L->pos++;
        }
        t.type = TOK_NUM;
        t.num = val;
        return t;
    }

    // функции (проверяем до обычной переменной)
    size_t save = L->pos;
    operation fop = match_function(s, &L->pos);
    if(fop != NONE_OP) {
        t.type = TOK_FUNC;
        t.op = fop;
        return t;
    }
    L->pos = save; // не функция — вернёмся

    // переменные и дифференциалы
    if(isalpha((unsigned char)c)) {
        // d...
        if(c == 'd') {
            size_t start = L->pos;
            L->pos++;
            if(isalpha((unsigned char)s[L->pos])) {
                // dx, dy, dz ...
                t.type = TOK_DIFFVAR;
                t.var = s[L->pos];
                L->pos++;
                return t;
            } else if(isdigit((unsigned char)s[L->pos])) {
                // d2z/dx2
                int order = 0;
                while(isdigit((unsigned char)s[L->pos])) {
                    order = order * 10 + (s[L->pos] - '0');
                    L->pos++;
                }
                if(isalpha((unsigned char)s[L->pos])) {
                    char func = s[L->pos++];
                    if(s[L->pos] == '/' && s[L->pos+1] == 'd') {
                        L->pos += 2;
                        char var = s[L->pos++];
                        int order2 = 0;
                        while(isdigit((unsigned char)s[L->pos])) {
                            order2 = order2 * 10 + (s[L->pos] - '0');
                            L->pos++;
                        }
                        if(order2 == 0) order2 = order;
                        t.type = TOK_DIFF;
                        t.op = DIFF_OP;
                        t.var = var;
                        t.order = order2;
                        t.num = func; // хранит имя функции/переменной как число — будет преобразовано при создании узла
                        return t;
                    }
                }
                L->pos = start;
            }
        }

        // обычная переменная
        t.type = TOK_VAR;
        t.var = c;
        L->pos++;
        return t;
    }

    // операторы и скобки
    switch(c) {
        case '+': t.type = TOK_OP; t.op = PLUS; break;
        case '-': t.type = TOK_OP; t.op = MINUS; break;
        case '*': t.type = TOK_OP; t.op = MULTIPLY; break;
        case '/': t.type = TOK_OP; t.op = DIVIDE; break;
        case '^': t.type = TOK_OP; t.op = POWER; break;
        case '=': t.type = TOK_OP; t.op = EQUAL_OP; break;
        case '(': t.type = TOK_OP; t.op = '('; break;
        case ')': t.type = TOK_OP; t.op = ')'; break;
        default: break;
    }
    L->pos++;
    return t;
}

// === приоритеты операторов ===
int precedence(operation op) {
    switch(op) {
        case POWER: return 4;
        case MULTIPLY: case DIVIDE: return 3;
        case PLUS: case MINUS: return 2;
        case EQUAL_OP: return 1;
        default: return 0;
    }
}

// === построение дерева из обратной польской записи ===
node* build_from_tokens(token* output, int count) {
    node** stack = (node**)calloc(count, sizeof(node*));
    int sp = 0;
    for(int i=0; i<count; i++) {
        token t = output[i];
        if(t.type == TOK_NUM)
            stack[sp++] = create_value_node(t.num);
        else if(t.type == TOK_VAR)
            stack[sp++] = create_variable_node(t.var);
        else if(t.type == TOK_DIFFVAR)
            stack[sp++] = create_diffvar_node(t.var);
        else if(t.type == TOK_DIFF) {
            // t.num был использован для сохранения имени переменной/функции (char) при лексинге
            char func_char = (char)t.num;
            stack[sp++] = create_diff_node(func_char, t.var, t.order);
        }
        else if(t.type == TOK_FUNC) {
            // функция: на стеке должен быть 1 аргумент
            if(sp < 1) continue;
            node* arg = stack[--sp];
            stack[sp++] = create_function_node(t.op, arg);
        }
        else if(t.type == TOK_OP) {
            // в токенах OP мы храним либо операцию, либо '(' / ')'
            if(t.op == '(' || t.op == ')') continue;
            if(sp < 2) continue;
            node* r = stack[--sp];
            node* l = stack[--sp];
            stack[sp++] = create_op_node(t.op, l, r);
        }
    }
    node* res = sp ? stack[0] : NULL;
    free(stack);
    return res;
}

// === Копирование узлов ===

node* copy_node(node* n) {

    if (!n) return NULL;

    node* c = (node*)calloc(1, sizeof(node));

    *c = *n;

    c->left = copy_node(n->left);

    c->right = copy_node(n->right);

    return c;

}

// === Упрощения ===

int is_value(node* n) { return n && n->op == VALUE; }

int is_variable(node* n) { return n && n->op == VARIABLE; }

int is_diffvar(node* n) { return n && n->op == DIFF_VAR; }

int is_zero(node* n) { return is_value(n) && fabs(n->value) < 1e-9; }

int is_one(node* n) { return is_value(n) && fabs(n->value - 1.0) < 1e-9; }

// Вспомогательная функция для сравнения узлов
int nodes_equal(node* a, node* b) {
    if (!a && !b) return 1;
    if (!a || !b) return 0;
    
    if (a->op != b->op) return 0;
    
    switch (a->op) {
        case VALUE:
            return fabs(a->value - b->value) < 1e-9;
        case VARIABLE:
            return a->var_name == b->var_name;
        case DIFF_VAR:
            return a->var_name == b->var_name;
        case DIFF_OP:
            return a->var_name == b->var_name && a->order == b->order &&
                   nodes_equal(a->left, b->left) && nodes_equal(a->right, b->right);
        default:
            // Для операторов и функций сравниваем поддеревья
            return nodes_equal(a->left, b->left) && nodes_equal(a->right, b->right);
    }
}


// Вспомогательная структура для хранения слагаемых
typedef struct term {
    node* expr;   // выражение без коэффициента
    double coeff; // коэффициент перед expr
    struct term* next;
} term;

// Функции для работы со списком слагаемых
term* flatten_sum(node* n);
term* combine_terms(term* head);
node* build_sum_from_terms(term* head);
node* simplify_plus_minus(node* n);

// === Основное упрощение ===
node* simplify(node* n) {
    if (!n) return NULL;

    // 1️⃣ Рекурсивное упрощение поддеревьев
    n->left = simplify(n->left);
    n->right = simplify(n->right);

    // 2️⃣ Свертка констант
    if (n->left && n->right && is_value(n->left) && is_value(n->right)) {
        double a = n->left->value, b = n->right->value, r = 0;
        switch (n->op) {
            case PLUS: r = a + b; break;
            case MINUS: r = a - b; break;
            case MULTIPLY: r = a * b; break;
            case DIVIDE: if (fabs(b) > 1e-9) r = a / b; else return n; break;
            case POWER: r = pow(a, b); break;
            default: return n;
        }
        return create_value_node(r);
    }

    // 3️⃣ Упрощение для SUM/DIFF с объединением одинаковых термов
    if (n->op == PLUS || n->op == MINUS) {
        node* simplified = simplify_plus_minus(n);
        if (simplified != n) return simplified;
    }

    // 4️⃣ MULTIPLY
    if (n->op == MULTIPLY) {
        if (is_zero(n->left) || is_zero(n->right)) return create_value_node(0);
        if (is_one(n->left)) return copy_node(n->right);
        if (is_one(n->right)) return copy_node(n->left);

        // (-1) * x → -x
        if (is_value(n->left) && n->left->value == -1) {
            if (n->right->op == MULTIPLY && is_value(n->right->left)) {
                node* new_node = copy_node(n->right);
                new_node->left->value *= -1;
                return new_node;
            } else {
                return create_op_node(MULTIPLY, create_value_node(-1), copy_node(n->right));
            }
        }

        // Случаи (a * (expr / a)) → expr
        if (is_value(n->left) && n->right && n->right->op == DIVIDE &&
            n->right->right && is_value(n->right->right) &&
            fabs(n->left->value - n->right->right->value) < 1e-9)
            return copy_node(n->right->left);

        if (is_value(n->right) && n->left && n->left->op == DIVIDE &&
            n->left->right && is_value(n->left->right) &&
            fabs(n->right->value - n->left->right->value) < 1e-9)
            return copy_node(n->left->left);
    }

    // 5️⃣ DIVIDE
    if (n->op == DIVIDE) {
        if (is_one(n->right)) return copy_node(n->left);
        if (is_zero(n->left)) return create_value_node(0);
        if (nodes_equal(n->left, n->right)) return create_value_node(1);

        if (n->left && n->left->op == MULTIPLY &&
            n->left->left && is_value(n->left->left) &&
            is_value(n->right) &&
            fabs(n->left->left->value - n->right->value) < 1e-9)
            return copy_node(n->left->right);
    }

    // 6️⃣ POWER
    if (n->op == POWER) {
        if (is_one(n->right)) return copy_node(n->left);
        if (is_zero(n->right)) return create_value_node(1);
        if (is_one(n->left)) return create_value_node(1);
        if (is_zero(n->left) && n->right &&
            (is_value(n->right) ? n->right->value > 0 : true)) 
            return create_value_node(0);
    }

    // 7️⃣ Упрощение двойного минуса
    if (n->op == MULTIPLY && is_value(n->left) && n->left->value == -1) {
        if (n->right->op == MULTIPLY && is_value(n->right->left) && n->right->left->value == -1)
            return copy_node(n->right->right);
    }

    return n;
}

// =======================
// === Реализация sum ===
// =======================
term* flatten_sum(node* n) {
    term* head = NULL;
    if (!n) return NULL;

    if (n->op == PLUS || n->op == MINUS) {
        term* left_terms = flatten_sum(n->left);
        term* right_terms = flatten_sum(n->right);

        // объединяем слева
        term* t = left_terms;
        while (t) { term* next = t->next; t->next = head; head = t; t = next; }

        // объединяем справа
        t = right_terms;
        while (t) {
            term* next = t->next;
            if (n->op == MINUS) t->coeff *= -1;
            t->next = head; head = t; t = next;
        }
        return head;
    }

    if (n->op == MULTIPLY && is_value(n->left)) {
        term* t = (term*)malloc(sizeof(term));
        t->coeff = n->left->value;
        t->expr = copy_node(n->right);
        t->next = NULL;
        return t;
    }

    term* t = (term*)malloc(sizeof(term));
    t->coeff = 1;
    t->expr = copy_node(n);
    t->next = NULL;
    return t;
}

term* combine_terms(term* head) {
    for (term* t1 = head; t1 != NULL; t1 = t1->next) {
        if (fabs(t1->coeff) < 1e-9) continue;
        for (term* t2 = t1->next; t2 != NULL; t2 = t2->next) {
            if (nodes_equal(t1->expr, t2->expr)) {
                t1->coeff += t2->coeff;
                t2->coeff = 0;
            }
        }
    }
    return head;
}

node* build_sum_from_terms(term* head) {
    node* result = NULL;
    for (term* t = head; t; t = t->next) {
        if (fabs(t->coeff) < 1e-9) continue;

        node* term_node;
        if (fabs(t->coeff - 1) < 1e-9) term_node = copy_node(t->expr);
        else term_node = create_op_node(MULTIPLY, create_value_node(t->coeff), copy_node(t->expr));

        if (!result) result = term_node;
        else result = create_op_node(PLUS, result, term_node);
    }

    if (!result) return create_value_node(0);
    return result;
}

node* simplify_plus_minus(node* n) {
    term* terms = flatten_sum(n);
    terms = combine_terms(terms);
    node* simplified = build_sum_from_terms(terms);

    for (term* t = terms; t;) { term* next = t->next; free(t); t = next; }
    return simplified;
}


// === парсер (shunting-yard) ===
node* parse_expression(const char* expr) {

    lexer L = { .s = expr, .pos = 0 };

    token stack[512];

    token output[512];

    int op_sp = 0, out_sp = 0;



    token t;

    while((t = next_token(&L)).type != TOK_END) {

        if(t.type == TOK_NUM || t.type == TOK_VAR || t.type == TOK_DIFFVAR || t.type == TOK_DIFF) {

            output[out_sp++] = t;

        } else if(t.type == TOK_FUNC) {

            // функция в стек

            stack[op_sp++] = t;

        } else if(t.type == TOK_OP) {

            if(t.op == '(') {

                stack[op_sp++] = t;

            } else if(t.op == ')') {

                // вынести все до '('

                while(op_sp > 0 && !(stack[op_sp-1].type == TOK_OP && stack[op_sp-1].op == '(')) {

                    output[out_sp++] = stack[--op_sp];

                }

                if(op_sp > 0 && stack[op_sp-1].type == TOK_OP && stack[op_sp-1].op == '(')

                    op_sp--; // убрать '('

                // если сверху функция, то она применима к аргументу в скобках

                if(op_sp > 0 && stack[op_sp-1].type == TOK_FUNC)

                    output[out_sp++] = stack[--op_sp];

            } else {

                // обычные бинарные операторы

                while(op_sp > 0 && 

                      ((stack[op_sp-1].type == TOK_OP && precedence(stack[op_sp-1].op) >= precedence(t.op)) ||

                       stack[op_sp-1].type == TOK_FUNC)) {

                    output[out_sp++] = stack[--op_sp];

                }

                stack[op_sp++] = t;

            }

        }

    }



    while(op_sp > 0) {

        output[out_sp++] = stack[--op_sp];

    }



    return build_from_tokens(output, out_sp);

}


// === основной интерфейс ===
math_tree parser(const char* expression) {
    math_tree mt;
    mt.head = parse_expression(expression);
    return mt;
}

void print_tree(math_tree* mt) {
    if (!mt || !mt->head) {
        printf("(пустое дерево)\n");
        return;
    }
    print_infix(mt->head);
    printf("\n");
}

#endif
