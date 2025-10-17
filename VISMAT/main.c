// tree_with_diff.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef BIN_TREE
#define BIN_TREE

typedef enum operation 
{
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    POWER,
    EQUAL_OP,
    VARIABLE,
    VALUE,
    DIFF_OP,
    NONE_OP
} operation;

typedef struct node 
{
    struct node* left;
    struct node* right;
    operation op;
    long long value;   // для числовых значений или дополнительного числа (например порядок для DIFF_OP)
    char var_name;     // для переменных (один символ)
} node;

typedef struct math_tree
{
    node* head; 
} math_tree;

/* ======= node creators ======= */
node* create_op_node(operation op, node* left, node* right)
{
    node* n = (node*)malloc(sizeof(node));
    if(!n) { perror("malloc"); exit(1); }
    n->left = left;
    n->right = right;
    n->op = op;
    n->value = 0;
    n->var_name = '\0';
    return n;
}

node* create_value_node(long long v)
{
    node* n = (node*)malloc(sizeof(node));
    if(!n) { perror("malloc"); exit(1); }
    n->left = n->right = NULL;
    n->op = VALUE;
    n->value = v;
    n->var_name = '\0';
    return n;
}

node* create_variable_node(char c)
{
    node* n = (node*)malloc(sizeof(node));
    if(!n) { perror("malloc"); exit(1); }
    n->left = n->right = NULL;
    n->op = VARIABLE;
    n->value = 0;
    n->var_name = c;
    return n;
}

node* create_diff_node(char func_name, char var_name, int order)
{
    node* left = create_variable_node(func_name);  // function/variable being differentiated
    node* right = create_variable_node(var_name);  // variable to differentiate by
    node* d = create_op_node(DIFF_OP, left, right);
    d->value = order;
    return d;
}

/* ======= utilities ======= */

int precedence_of_op(operation op)
{
    switch(op)
    {
        case POWER: return 4;
        case MULTIPLY:
        case DIVIDE: return 3;
        case PLUS:
        case MINUS: return 2;
        case EQUAL_OP: return 1; // lowest precedence
        default: return 0;
    }
}

/* associativity: 1 = left, 0 = right */
int is_left_associative(operation op)
{
    switch(op)
    {
        case POWER: return 0; // right associative
        case EQUAL_OP: return 0; // '=' right-assoc: a = b = c => a = (b = c)
        case MULTIPLY:
        case DIVIDE:
        case PLUS:
        case MINUS: return 1;
        default: return 1;
    }
}

operation op_from_char(char c)
{
    switch(c)
    {
        case '+': return PLUS;
        case '-': return MINUS;
        case '*': return MULTIPLY;
        case '/': return DIVIDE;
        case '^': return POWER;
        case '=': return EQUAL_OP;
        default: return NONE_OP;
    }
}

const char* op_to_str(operation op)
{
    switch(op)
    {
        case PLUS: return "+";
        case MINUS: return "-";
        case MULTIPLY: return "*";
        case DIVIDE: return "/";
        case POWER: return "^";
        case EQUAL_OP: return "=";
        case VARIABLE: return "VAR";
        case VALUE: return "VAL";
        case DIFF_OP: return "d/d";
        default: return "?";
    }
}

void free_tree(node* n)
{
    if(!n) return;
    free_tree(n->left);
    free_tree(n->right);
    free(n);
}

/* print tree in infix with parentheses for clarity */
void print_infix(node* n)
{
    if(!n) return;
    if(n->op == VALUE)
    {
        printf("%lld", n->value);
    }
    else if(n->op == VARIABLE)
    {
        printf("%c", n->var_name);
    }
    else if(n->op == DIFF_OP)
    {
        // print as d{order}{func}/d{var}{order}
        int ord = (int)n->value;
        if(ord <= 1)
        {
            printf("d");
            print_infix(n->left);
            printf("/d");
            print_infix(n->right);
        }
        else
        {
            printf("d%d", ord);
            print_infix(n->left);
            printf("/d");
            print_infix(n->right);
            printf("%d", ord);
        }
    }
    else
    {
        printf("(");
        print_infix(n->left);
        printf(" %s ", op_to_str(n->op));
        print_infix(n->right);
        printf(")");
    }
}

/* ========== Lexer & Parser (shunting-yard -> AST) ========== */

typedef enum {
    TOK_END,
    TOK_NUM,
    TOK_VAR,
    TOK_OP,
    TOK_LP,
    TOK_RP
} tok_type;

typedef struct {
    tok_type type;
    long long num;   // for TOK_NUM or for DIFF order
    char var;        // for TOK_VAR or DIFF diff variable
    operation op;    // for TOK_OP
    char func;       // for DIFF: the function/variable being differentiated
} token;

typedef struct {
    const char* s;
    size_t pos;
} lexer;

static void lex_skip_ws(lexer* L)
{
    while(L->s[L->pos] && isspace((unsigned char)L->s[L->pos])) L->pos++;
}

/* Try to parse derivative pattern starting at current pos.
   Accept forms:
     dz/dx
     d2z/dx2
     d10f/dt10
   No spaces allowed inside derivative token (you may extend later).
   If pattern matches, fills token and returns 1, otherwise restores pos and returns 0.
*/
static int lex_try_derivative(lexer* L, token* out)
{
    size_t start = L->pos;
    if(L->s[L->pos] != 'd') return 0;
    L->pos++; // skip 'd'

    // optional leading number (order)
    long long order1 = 0;
    int have_order1 = 0;
    while(isdigit((unsigned char)L->s[L->pos]))
    {
        have_order1 = 1;
        order1 = order1 * 10 + (L->s[L->pos] - '0');
        L->pos++;
    }
    if(!isalpha((unsigned char)L->s[L->pos]))
    {
        L->pos = start;
        return 0;
    }
    char func_char = L->s[L->pos++];
    // next should be '/'
    if(L->s[L->pos] != '/')
    {
        L->pos = start; return 0;
    }
    L->pos++; // skip '/'

    // next should be 'd'
    if(L->s[L->pos] != 'd')
    {
        L->pos = start; return 0;
    }
    L->pos++; // skip 'd'

    if(!isalpha((unsigned char)L->s[L->pos]))
    {
        L->pos = start; return 0;
    }
    char diff_var = L->s[L->pos++];

    // optional trailing number for order (e.g. /dx2)
    long long order2 = 0;
    int have_order2 = 0;
    while(isdigit((unsigned char)L->s[L->pos]))
    {
        have_order2 = 1;
        order2 = order2 * 10 + (L->s[L->pos] - '0');
        L->pos++;
    }

    long long final_order = 1;
    if(have_order1 && have_order2)
    {
        // prefer trailing if they differ — but warn
        if(order1 != order2)
        {
            // we'll choose leading (order1) to be consistent, but prefer trailing could be done too
            // simple policy: prefer trailing (order2)
            final_order = order2;
        }
        else final_order = order1;
    }
    else if(have_order1) final_order = order1;
    else if(have_order2) final_order = order2;
    else final_order = 1;

    // fill token
    out->type = TOK_OP;
    out->op = DIFF_OP;
    out->num = final_order;
    out->var = diff_var;
    out->func = func_char;
    return 1;
}

static token lex_next(lexer* L)
{
    lex_skip_ws(L);
    token t;
    t.type = TOK_END; t.num = 0; t.var = '\0'; t.op = NONE_OP; t.func = '\0';

    char c = L->s[L->pos];
    if(c == '\0') { t.type = TOK_END; return t; }

    // Try derivative pattern first (must be contiguous, no internal spaces)
    if(c == 'd')
    {
        size_t save = L->pos;
        if(lex_try_derivative(L, &t))
        {
            return t;
        }
        else
        {
            L->pos = save; // restore if not matched
        }
    }

    if(isdigit((unsigned char)c))
    {
        long long val = 0;
        while(isdigit((unsigned char)L->s[L->pos]))
        {
            val = val * 10 + (L->s[L->pos] - '0');
            L->pos++;
        }
        t.type = TOK_NUM;
        t.num = val;
        return t;
    }

    if(isalpha((unsigned char)c) || c == '_')
    {
        // single-char variable supported; consume subsequent alnum to skip them but store first char
        t.type = TOK_VAR;
        t.var = c;
        L->pos++;
        while(isalnum((unsigned char)L->s[L->pos]) || L->s[L->pos]=='_') L->pos++; // consume rest (ignored)
        return t;
    }

    if(c == '(')
    {
        t.type = TOK_LP; L->pos++; return t;
    }
    if(c == ')')
    {
        t.type = TOK_RP; L->pos++; return t;
    }

    operation op = op_from_char(c);
    if(op != NONE_OP)
    {
        t.type = TOK_OP;
        t.op = op;
        L->pos++;
        return t;
    }

    // unknown char -> skip and return next
    L->pos++;
    t.type = TOK_END;
    return t;
}

/* helper: pop operator from op stack and build node using top two operands */
static void apply_operator(node** values, int* vtop, operation op)
{
    if(*vtop < 2)
    {
        fprintf(stderr, "Syntax error: not enough operands for operator %s\n", op_to_str(op));
        exit(1);
    }
    node* right = values[--(*vtop)];
    node* left = values[--(*vtop)];
    node* newn = create_op_node(op, left, right);
    values[(*vtop)++] = newn;
}

/* Parser main: returns root node (caller must free) */
node* parse_expression_to_ast(const char* expr)
{
    node** val_stack = NULL;
    int val_cap = 0, val_top = 0;

    operation* op_stack = NULL;
    int op_cap = 0, op_top = 0;

    lexer L = { expr, 0 };
    token t = lex_next(&L);

    while(t.type != TOK_END)
    {
        if(t.type == TOK_NUM)
        {
            if(val_top + 1 > val_cap) { val_cap = val_cap ? val_cap * 2 : 16; val_stack = realloc(val_stack, val_cap * sizeof(node*)); }
            val_stack[val_top++] = create_value_node(t.num);
        }
        else if(t.type == TOK_VAR)
        {
            if(val_top + 1 > val_cap) { val_cap = val_cap ? val_cap * 2 : 16; val_stack = realloc(val_stack, val_cap * sizeof(node*)); }
            val_stack[val_top++] = create_variable_node(t.var);
        }
        else if(t.type == TOK_LP)
        {
            if(op_top + 1 > op_cap) { op_cap = op_cap ? op_cap * 2 : 16; op_stack = realloc(op_stack, op_cap * sizeof(operation)); }
            op_stack[op_top++] = NONE_OP; // marker for '('
        }
        else if(t.type == TOK_RP)
        {
            while(op_top > 0 && op_stack[op_top-1] != NONE_OP)
            {
                operation topop = op_stack[--op_top];
                if(val_top < 2) { fprintf(stderr,"Mismatched parentheses or missing operand\n"); exit(1); }
                apply_operator(val_stack, &val_top, topop);
            }
            if(op_top == 0) { fprintf(stderr,"Mismatched parentheses\n"); exit(1); }
            op_top--; // pop '(' marker
        }
        else if(t.type == TOK_OP)
        {
            if(t.op == DIFF_OP)
            {
                // Create DIFF node right away using t.func, t.var, t.num
                if(val_top + 1 > val_cap) { val_cap = val_cap ? val_cap * 2 : 16; val_stack = realloc(val_stack, val_cap * sizeof(node*)); }
                node* dnode = create_diff_node(t.func, t.var, (int)t.num);
                val_stack[val_top++] = dnode;
            }
            else
            {
                operation cur_op = t.op;
                while(op_top > 0 && op_stack[op_top-1] != NONE_OP)
                {
                    operation topop = op_stack[op_top-1];
                    int prec_top = precedence_of_op(topop);
                    int prec_cur = precedence_of_op(cur_op);
                    if( (is_left_associative(cur_op) && prec_cur <= prec_top) ||
                        (!is_left_associative(cur_op) && prec_cur < prec_top) )
                    {
                        op_top--;
                        apply_operator(val_stack, &val_top, topop);
                    }
                    else break;
                }
                if(op_top + 1 > op_cap) { op_cap = op_cap ? op_cap * 2 : 16; op_stack = realloc(op_stack, op_cap * sizeof(operation)); }
                op_stack[op_top++] = cur_op;
            }
        }

        t = lex_next(&L);
    }

    while(op_top > 0)
    {
        operation topop = op_stack[--op_top];
        if(topop == NONE_OP) { fprintf(stderr,"Mismatched parentheses\n"); exit(1); }
        apply_operator(val_stack, &val_top, topop);
    }

    if(val_top != 1)
    {
        fprintf(stderr, "Parse error: remaining values = %d\n", val_top);
        for(int i = 0; i < val_top; ++i) free_tree(val_stack[i]);
        free(val_stack);
        free(op_stack);
        return NULL;
    }

    node* root = val_stack[0];
    free(val_stack);
    free(op_stack);
    return root;
}

#endif // BIN_TREE

/* ===== Example usage ===== */
int main(void)
{
    const char* tests[] = {
        "2 * 3 + 4 / 5 - 12 + 15 * 14",
        "a = 1 + 2 * b",
        "x = 21123 + 123",
        "y = (1 + 2) * (3 + 4)",
        "z = 2 ^ 3 ^ 2",
        "dz/dx",
        "d2z/dx2",
        "a = d3y/dt3 + 7",
        "w = d10f/ds10 + 5",
        NULL
    };

    for(const char** p = tests; *p; ++p)
    {
        printf("Expr: %s\n", *p);
        node* root = parse_expression_to_ast(*p);
        if(root)
        {
            printf("Parsed: ");
            print_infix(root);
            printf("\n\n");
            free_tree(root);
        }
        else
        {
            printf("Failed to parse\n\n");
        }
    }

    return 0;
}
