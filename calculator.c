#include<stdio.h>
#include<stdlib.h>

int gValidExpression = 1;

// get precedence of the operators
int getPrecedenceOperator(char operator){
    if (operator == '+' || operator == '-') {
        return 1;
    }
    if (operator == '*' || operator == '/') {
        return 2;
    }
    return 0;
}

// calculation of numbers and operators
int calculation(int num1, int num2, char operator){
    switch(operator){
        case '+':
        return num1 + num2;
        case '-':
        return num1 - num2;
        case '*':
        return num1 * num2;
        case '/':
        if (num2 == 0) {
            printf("Error: Division by zero.\n");
            gValidExpression = 0;
            exit(1);
        }
        return num1 / num2;
        default:
        printf("Error: Invalid expression;\n");
        gValidExpression = 0;
        exit(1);
    }
}

// evaluate the expression
int evaluateExpression(char *expression){
    int i = 0;
    int numbers_stack[200];
    int num_top = -1;
    char operators_stack[200];
    int op_top = -1;
    int expectingNumber = 1; // true at start, we expect a number or +/-

    while (expression[i] != '\0' && expression[i] != '\n') {
        if (expression[i] == ' ') {
            i++;
            continue;
        }

        // number
        if (expression[i] >= '0' && expression[i] <= '9') {
            int num = 0;
            while (expression[i] >= '0' && expression[i] <= '9') {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            numbers_stack[++num_top] = num;
            expectingNumber = 0;
            continue;
        }

        // +/- when a number is expected
        if ((expression[i] == '+' || expression[i] == '-') && expectingNumber) {
            int sign = 1;
            while (expression[i] == '+' || expression[i] == '-') {
                if (expression[i] == '-') {
                    sign = -sign;
                }
                i++;
            }

            // after +/- signs there must be a number
            if (!(expression[i] >= '0' && expression[i] <= '9')) {
                printf("Error: Invalid expression.\n");
                gValidExpression = 0;
                return 0;
            }

            int num = 0;
            while (expression[i] >= '0' && expression[i] <= '9') {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            numbers_stack[++num_top] = num * sign;
            expectingNumber = 0;
            continue;
        }

        // binary operators + or - when a number was just read
        if ((expression[i] == '+' || expression[i] == '-') && !expectingNumber) {
            while (op_top != -1 && getPrecedenceOperator(operators_stack[op_top]) >= getPrecedenceOperator(expression[i])) {
                if (num_top < 1) {
                    printf("Error: Invalid expression.\n");
                    gValidExpression = 0;
                    return 0;
                }
                int num2 = numbers_stack[num_top--];
                int num1 = numbers_stack[num_top--];
                char op = operators_stack[op_top--];
                int result = calculation(num1, num2, op);
                numbers_stack[++num_top] = result;
            }
            operators_stack[++op_top] = expression[i];
            i++;
            expectingNumber = 1;
            continue;
        }

        if ((expression[i] == '*' || expression[i] == '/') && !expectingNumber) {
            while (op_top != -1 && getPrecedenceOperator(operators_stack[op_top]) >= getPrecedenceOperator(expression[i])) {
                if (num_top < 1) {
                    printf("Error: Invalid expression.\n");
                    gValidExpression = 0;
                    return 0;
                }
                int num2 = numbers_stack[num_top--];
                int num1 = numbers_stack[num_top--];
                char op = operators_stack[op_top--];
                int result = calculation(num1, num2, op);
                numbers_stack[++num_top] = result;
            }
            operators_stack[++op_top] = expression[i];
            i++;
            expectingNumber = 1;
            continue;
        }

        // any other sequence is invalid
        printf("Error: Invalid expression.\n");
        gValidExpression = 0;
        return 0;
    }

    if (expectingNumber) {
        printf("Error: Invalid expression.\n");
        gValidExpression = 0;
        return 0;
    }
    while (op_top != -1) {
        int num2 = numbers_stack[num_top--];
        int num1 = numbers_stack[num_top--];
        char op = operators_stack[op_top--];
        int result = calculation(num1, num2, op);
        numbers_stack[++num_top] = result;
    }
    return numbers_stack[num_top];
}

int main() {
    char expression[200];
    printf("Enter the expression: ");
    fgets(expression, sizeof(expression), stdin);
    int result = evaluateExpression(expression);
    if (gValidExpression) {
        printf("Result: %d\n", result);
    }
    return 0;
}