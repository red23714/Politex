#include <stdio.h>
#include "du_solver.h"
#include "equation_transformer.h"
#include "math_differenciate.h"

// const char* tests[] = {
//         "d1y/dx1 - 2/x * y = 2*x^3", // y' + P(x)y = f(x)
//         "d1y/dx1 + 2*y = y^2 * e^x", // y' + P(x)y = Q(x)y^n
//         "(3*y^2 + 1)*dy = (2*x)*dx", // M(y)dy = N(x)dx
//         "(2*x + 3 * x^2 * y)*dx + (x^3 - 3 * y^2)*dy = 0", // M(x, y)dx + N(x, y)dy = 0
//         "d1y/dx1 = y/x + x/y", // y' = f(y/x)
//         "ln(x * y) = cos(x)",
//         "d1y/dx1 = 2*x/(3 * y^2 + 1)",
//         "x*d1y/dx1 - 2 * y = 2 * x^4",
//         "d1y/dx1 = (3 * y)/x + x",
//         "(2*x + 1) * d1y/dx1 = 4 * x + 2 * y"
//     };

int main() {
    printf("=== Тест дерева выражений ===\n\n");

    const char* tests[] = {
            "d1y/dx1 = y/x + cos(y/x)",
            "d1y/dx1 - 2/x * y = 2*x^3", // y' + P(x)y = f(x)
            "x*d1y/dx1 - 2 * y = 2 * x^4",
            "d1y/dx1 = (3 * y)/x + x",
            "(2*x + 1) * d1y/dx1 = 4 * x + 2 * y",
            "d1y/dx1 = y/x + x/y",
        }; 

    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        printf("Вход: %s\n", tests[i]);
        math_tree mt = parser(tests[i]);
        solve_du(&mt);
        printf("\n");
    }

    return 0;
}
