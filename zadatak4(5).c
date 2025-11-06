#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef struct _Node {
    double value;
    struct _Node* next;
} Node;

typedef struct {
    Node* top;
} Stack;

void initStack(Stack* s) { s->top = NULL; }
int isEmpty(Stack* s) { return s->top == NULL; }

int push(Stack* s, double x) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (!n) return 0;
    n->value = x;
    n->next = s->top;
    s->top = n;
    return 1;
}

int pop(Stack* s, double* out) {
    if (isEmpty(s)) return 0;
    Node* t = s->top;
    *out = t->value;
    s->top = t->next;
    free(t);
    return 1;
}

void clearStack(Stack* s) { double d; while (pop(s, &d)); }

int isOperator(const char* tok) {
    return strlen(tok) == 1 && strchr("+-*/^", tok[0]) != NULL;
}

int isNumberToken(const char* tok, double* outVal) {
    char* endptr;
    double v = strtod(tok, &endptr);
    if (endptr == tok || *endptr != '\0') return 0;
    *outVal = v;
    return 1;
}

int evalPostfixLine(const char* line, double* result, char* errMsg, size_t errSize) {
    Stack s; initStack(&s);
    char buf[2048];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    for (char* tok = strtok(buf, " \t\r\n"); tok; tok = strtok(NULL, " \t\r\n")) {
        double val;
        if (isNumberToken(tok, &val)) {
            if (!push(&s, val)) { snprintf(errMsg, errSize, "Memory error"); clearStack(&s); return 0; }
        } else if (isOperator(tok)) {
            double a, b, r;
            if (!pop(&s, &b) || !pop(&s, &a)) { snprintf(errMsg, errSize, "Too few operands"); clearStack(&s); return 0; }
            switch (tok[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/': if (b == 0) { snprintf(errMsg, errSize, "Div by 0"); clearStack(&s); return 0; } r = a / b; break;
                case '^': r = pow(a, b); break;
                default: snprintf(errMsg, errSize, "Unknown op"); clearStack(&s); return 0;
            }
            if (!push(&s, r)) { snprintf(errMsg, errSize, "Memory error"); clearStack(&s); return 0; }
        } else {
            snprintf(errMsg, errSize, "Bad token '%s'", tok);
            clearStack(&s);
            return 0;
        }
    }

    double res;
    if (!pop(&s, &res)) { snprintf(errMsg, errSize, "Empty expr"); clearStack(&s); return 0; }
    if (!isEmpty(&s)) { snprintf(errMsg, errSize, "Extra operands"); clearStack(&s); return 0; }
    *result = res;
    return 1;
}

int main(int argc, char** argv) {
    const char* filename = (argc > 1) ? argv[1] : "postfix.txt";
    FILE* f = fopen(filename, "r");
    if (!f) { printf("Can't open %s\n", filename); return 1; }

    char line[2048];
    int n = 0;
    while (fgets(line, sizeof(line), f)) {
        int onlyWS = 1;
        for (char* p = line; *p; ++p) if (!isspace((unsigned char)*p)) { onlyWS = 0; break; }
        if (onlyWS) continue;

        double res; char err[256];
        if (evalPostfixLine(line, &res, err, sizeof(err))) printf("Line %d -> %.10g\n", ++n, res);
        else printf("Line %d -> %s\n", ++n, err);
    }

    fclose(f);
    return 0;
}