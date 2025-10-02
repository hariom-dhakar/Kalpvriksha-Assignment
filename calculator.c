#include<stdio.h>
#include<stdlib.h>

int valid = 1;

// get precendence of the operators
int get_precedence_operator(char operator){
    if(operator == '+' || operator == '-'){
        return 1;
    }
    if(operator == '*' || operator == '/'){
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
        if(num2 == 0){
            printf("Cannot be divided by 0\n");
            valid = 0;
            exit(1);
        }
        return num1/num2;
        default:
        printf("Invalid operator or character\n");
        valid = 0;
        exit(1);
    }
}

// evaluate the expression
int evaluate_expression(char *expression){
    int i = 0;
    int numbers_stack[200];
    int num_top = -1;
    char operators_stack[200];
    int op_top = -1;

    while(expression[i] != '\0' && expression[i] != '\n'){
        if(expression[i] == ' '){
            i++;
            continue;
        }
        if(expression[i] >= '0' && expression[i] <= '9'){
            int num = 0;
            while(expression[i]>='0' && expression[i]<='9'){
                num = num*10 + (expression[i]-'0');
                i++;
            }
            numbers_stack[++num_top] = num;
            continue;
        }
        if(expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/'){
            while(op_top != -1 && get_precedence_operator(operators_stack[op_top]) >= get_precedence_operator(expression[i])){
                int num2 = numbers_stack[num_top--];
                int num1 = numbers_stack[num_top--];
                char op = operators_stack[op_top--];
                int result = calculation(num1, num2, op);
                numbers_stack[++num_top] = result;
            }
            operators_stack[++op_top] = expression[i];
            i++;
            continue;
        }
        printf("Invalid character or operator: %c\n", expression[i]);
        valid = 0;
        break;
    }
    while(op_top != -1){
        int num2 = numbers_stack[num_top--];
        int num1 = numbers_stack[num_top--];
        char op = operators_stack[op_top--];
        int result = calculation(num1, num2, op);
        numbers_stack[++num_top] = result;
    }
    return numbers_stack[num_top];
}

int main(){
    char expression[200];
    printf("Enter the expression: ");
    fgets(expression, sizeof(expression), stdin);
    int result = evaluate_expression(expression);
    if(valid){
        printf("Result: %d\n", result);
    }
    return 0;
}