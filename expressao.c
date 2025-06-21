#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "expressao.h"

#define MAX 512

// ---------- Estrutura de Pilhas ---------- //
typedef struct {
    float itens[MAX];
    int topo;
} PilhaFloat;

typedef struct {
    char itens[MAX][32];
    int topo;
} PilhaString;

// Inicialização e operações da pilha de float
void inicializaPilhaFloat(PilhaFloat *p) { p->topo = -1; }
int vaziaFloat(PilhaFloat *p) { return p->topo == -1; }
void pushFloat(PilhaFloat *p, float val) { p->itens[++p->topo] = val; }
float popFloat(PilhaFloat *p) { return vaziaFloat(p) ? NAN : p->itens[p->topo--]; }

// Inicialização e operações da pilha de strings
void inicializaPilhaString(PilhaString *p) { p->topo = -1; }
int vaziaString(PilhaString *p) { return p->topo == -1; }
void pushString(PilhaString *p, char *val) { strcpy(p->itens[++p->topo], val); }
char* popString(PilhaString *p) { return vaziaString(p) ? "" : p->itens[p->topo--]; }

// ---------- Funções de apoio ---------- //
int precedencia(char *op) {
    if (strcmp(op, "^") == 0) return 4;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 3;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 2;
    return 1;
}

int isOperador(char *op) {
    return (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 ||
            strcmp(op, "/") == 0 || strcmp(op, "%") == 0 || strcmp(op, "^") == 0);
}

int isFuncao(char *str) {
    return (strcmp(str, "sen") == 0 || strcmp(str, "cos") == 0 || strcmp(str, "tg") == 0 ||
            strcmp(str, "log") == 0 || strcmp(str, "raiz") == 0);
}

float grauParaRad(float grau) {
    return grau * M_PI / 180.0;
}

// ---------- Conversão de infixa para posfixa ---------- //
char* getFormaPosFixa(char *infixa) {
    static char saida[MAX];
    PilhaString operadores;
    int parenteses = 0;
    inicializaPilhaString(&operadores);
    saida[0] = '\0';

    char *ptr = infixa;

    while (*ptr) {
        if (isspace((unsigned char)*ptr)) {
            ptr++;
            continue;
        }

        if (isdigit((unsigned char)*ptr) || *ptr == '.') {
            char num_str[32];
            int i = 0;
            while (isdigit((unsigned char)*ptr) || *ptr == '.') {
                num_str[i++] = *ptr++;
            }
            num_str[i] = '\0';
            strcat(saida, num_str);
            strcat(saida, " ");
            continue;
        }

        if (isalpha((unsigned char)*ptr)) {
            char func_str[32];
            int i = 0;
            while (isalpha((unsigned char)*ptr)) {
                func_str[i++] = *ptr++;
            }
            func_str[i] = '\0';

            if (isFuncao(func_str)) {
                pushString(&operadores, func_str);
            } else {
                printf("Erro: função ou identificador desconhecido: %s\n", func_str);
                return "";
            }
            continue;
        }

        char op_char[2] = {*ptr, '\0'};

        if (strcmp(op_char, "(") == 0) {
            parenteses++;
            pushString(&operadores, op_char);
        } else if (strcmp(op_char, ")") == 0) {
            parenteses--;
            while (!vaziaString(&operadores) && strcmp(operadores.itens[operadores.topo], "(") != 0) {
                strcat(saida, popString(&operadores));
                strcat(saida, " ");
            }
            if (vaziaString(&operadores)) {
                printf("Erro: parênteses não balanceados.\n");
                return "";
            }
            popString(&operadores);

            if (!vaziaString(&operadores) && isFuncao(operadores.itens[operadores.topo])) {
                strcat(saida, popString(&operadores));
                strcat(saida, " ");
            }
        } else if (isOperador(op_char)) {
            while (!vaziaString(&operadores) && isOperador(operadores.itens[operadores.topo]) &&
                  (precedencia(op_char) < precedencia(operadores.itens[operadores.topo]) ||
                  (precedencia(op_char) == precedencia(operadores.itens[operadores.topo]) && strcmp(op_char, "^") != 0))) {
                strcat(saida, popString(&operadores));
                strcat(saida, " ");
            }
            pushString(&operadores, op_char);
        } else {
            printf("Erro: caractere não reconhecido: %c\n", *ptr);
            return "";
        }

        ptr++;
    }

    if (parenteses != 0) {
        printf("Erro: falta ou excesso de parênteses.\n");
        return "";
    }

    while (!vaziaString(&operadores)) {
        if(strcmp(operadores.itens[operadores.topo], "(") == 0) {
            printf("Erro: parênteses não balanceados.\n");
            return "";
        }
        strcat(saida, popString(&operadores));
        strcat(saida, " ");
    }

    return saida;
}

// ---------- Conversão de posfixa para infixa ---------- //
char* getFormaInFixa(char *posfixa) {
    static char saida[MAX];
    PilhaString pilha;
    inicializaPilhaString(&pilha);

    char temp_posfixa[MAX];
    strcpy(temp_posfixa, posfixa);

    char *ptr = strtok(temp_posfixa, " ");
    while (ptr) {
        if (isOperador(ptr)) {
            if (pilha.topo < 1) {
                printf("Erro: expressão mal formada (faltam operandos).\n");
                return "";
            }
            char b[128], a[128];
            strcpy(b, popString(&pilha));
            strcpy(a, popString(&pilha));
            char temp[256];
            sprintf(temp, "(%s %s %s)", a, ptr, b);
            pushString(&pilha, temp);
        } else if (isFuncao(ptr)) {
            if (pilha.topo < 0) {
                printf("Erro: expressão mal formada (falta argumento para função).\n");
                return "";
            }
            char a[128];
            strcpy(a, popString(&pilha));
            char temp[256];
            sprintf(temp, "%s(%s)", ptr, a);
            pushString(&pilha, temp);
        } else {
            pushString(&pilha, ptr);
        }
        ptr = strtok(NULL, " ");
    }

    if (pilha.topo == 0) {
        strcpy(saida, popString(&pilha));
    } else {
        printf("Erro: expressão mal formada (elementos sobrando).\n");
        return "";
    }

    return saida;
}

// ---------- Avaliação de expressões posfixas ---------- //
float getValorPosFixa(char *posfixa) {
    PilhaFloat pilha;
    inicializaPilhaFloat(&pilha);
    
    char temp_posfixa[MAX];
    strcpy(temp_posfixa, posfixa);

    char *ptr = strtok(temp_posfixa, " ");
    while (ptr) {
        if (isOperador(ptr)) {
            if (pilha.topo < 1) {
                printf("Erro: operador '%s' sem valores suficientes.\n", ptr);
                return NAN;
            }
            float b = popFloat(&pilha);
            float a = popFloat(&pilha);
            if (strcmp(ptr, "+") == 0) pushFloat(&pilha, a + b);
            else if (strcmp(ptr, "-") == 0) pushFloat(&pilha, a - b);
            else if (strcmp(ptr, "*") == 0) pushFloat(&pilha, a * b);
            else if (strcmp(ptr, "/") == 0) {
                if (b == 0) {
                    printf("Erro: divisão por zero.\n");
                    return NAN;
                }
                pushFloat(&pilha, a / b);
            }
            else if (strcmp(ptr, "%") == 0) pushFloat(&pilha, fmod(a, b));
            else if (strcmp(ptr, "^") == 0) pushFloat(&pilha, pow(a, b));
        } else if (isFuncao(ptr)) {
            if (pilha.topo < 0) {
                printf("Erro: função '%s' sem argumento.\n", ptr);
                return NAN;
            }
            float a = popFloat(&pilha);
            if (strcmp(ptr, "sen") == 0) pushFloat(&pilha, sin(grauParaRad(a)));
            else if (strcmp(ptr, "cos") == 0) pushFloat(&pilha, cos(grauParaRad(a)));
            else if (strcmp(ptr, "tg") == 0) pushFloat(&pilha, tan(grauParaRad(a)));
            else if (strcmp(ptr, "log") == 0) {
                if (a <= 0) {
                    printf("Erro: log de número não positivo.\n");
                    return NAN;
                }
                pushFloat(&pilha, log10(a));
            }
            else if (strcmp(ptr, "raiz") == 0) {
                if (a < 0) {
                    printf("Erro: raiz de número negativo.\n");
                    return NAN;
                }
                pushFloat(&pilha, sqrt(a));
            }
        } else {
            pushFloat(&pilha, atof(ptr));
        }
        ptr = strtok(NULL, " ");
    }

    if (pilha.topo != 0) {
        printf("Erro: operandos em excesso.\n");
        return NAN;
    }
    return popFloat(&pilha);
}

// ---------- Avaliação de expressões infixas ---------- //
float getValorInFixa(char *StrInFixa) {
    char temp[MAX];
    strcpy(temp, StrInFixa);
    char *pos = getFormaPosFixa(temp);
    if (strlen(pos) == 0) return NAN;
    return getValorPosFixa(pos);
}
