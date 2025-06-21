#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Para isnan()
#include "expressao.h"

// Limpa o buffer de entrada
void limparEntrada() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// Limpa a tela do terminal
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    char modo[16];
    char expressao[512];
    char *conversao;
    float valorCalculado;

    while (1) {
        limparTela();
        printf("=== Calculadora de Expressoes ===\n");
        printf("Informe o formato ([infixa] ou [posfixa]) ou digite 'sair':\n> ");
        fgets(modo, sizeof(modo), stdin);
        modo[strcspn(modo, "\n")] = 0;

        if (strcmp(modo, "sair") == 0) {
            printf("Encerrando...\n");
            break;
        }

        if (strcmp(modo, "infixa") == 0) {
            printf("Digite a expressao no formato infixo:\n> ");
            fgets(expressao, sizeof(expressao), stdin);
            expressao[strcspn(expressao, "\n")] = 0;

            conversao = getFormaPosFixa(expressao);
            if (conversao && strlen(conversao) > 0) {
                printf("Conversao para pos-fixa: %s\n", conversao);
            } else {
                printf("Falha na conversao para pos-fixa.\n");
                continue;
            }

            valorCalculado = getValorInFixa(expressao);
            if (!isnan(valorCalculado)) {
                printf("Resultado: %.2f\n", valorCalculado);
            } else {
                printf("Erro ao calcular valor da expressao infixa.\n");
            }

        } else if (strcmp(modo, "posfixa") == 0) {
            printf("Digite a expressao no formato pos-fixo (tokens separados por espaco):\n> ");
            fgets(expressao, sizeof(expressao), stdin);
            expressao[strcspn(expressao, "\n")] = 0;

            conversao = getFormaInFixa(expressao);
            if (conversao && strlen(conversao) > 0) {
                printf("Conversao para infixa: %s\n", conversao);
            } else {
                printf("Falha na conversao para infixa.\n");
                continue;
            }

            valorCalculado = getValorPosFixa(expressao);
            if (!isnan(valorCalculado)) {
                printf("Resultado: %.2f\n", valorCalculado);
            } else {
                printf("Erro ao calcular valor da expressao pos-fixa.\n");
            }

        } else {
            printf("Formato invalido. Utilize 'infixa' ou 'posfixa'.\n");
        }

        printf("\nPressione Enter para continuar...");
        getchar();
    }

    return 0;
}
