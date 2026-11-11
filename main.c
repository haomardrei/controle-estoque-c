#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQ "inventory.txt"
#define TMP "inventory.tmp"
#define TAM 100

/* Lê uma linha do teclado sem o '\n'. Se a entrada acabar (EOF), encerra. */
void ler_linha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    buf[strcspn(buf, "\n")] = '\0';
}

/* Mostra msg e lê um inteiro. Retorna 1 se a entrada for um número válido. */
int ler_numero(const char *msg, int *valor) {
    char buf[TAM], *fim;
    long v;

    printf("%s", msg);
    ler_linha(buf, TAM);
    v = strtol(buf, &fim, 10);
    if (fim == buf || *fim != '\0')
        return 0;
    *valor = (int)v;
    return 1;
}

/* Lê um item do arquivo (linha 1: nome, linha 2: quantidade). */
int ler_item(FILE *fp, char *nome, int *qtd) {
    char buf[TAM];

    if (fgets(nome, TAM, fp) == NULL) return 0;
    nome[strcspn(nome, "\n")] = '\0';
    if (fgets(buf, TAM, fp) == NULL) return 0;
    *qtd = atoi(buf);
    return 1;
}

/* Requisito 1: menu com validação (1 a 4). */
int menu(void) {
    int opc;

    printf("====================================\n");
    printf("         Controle de Estoque\n");
    printf("====================================\n");
    printf("1. Adicionar Item\n");
    printf("2. Remover Item\n");
    printf("3. Listar Estoque\n");
    printf("4. Sair\n");

    while (!ler_numero("Escolha uma opção: ", &opc) || opc < 1 || opc > 4)
        printf("Opção inválida! Tente novamente.\n");

    return opc;
}

/* Requisito 2: adicionar item. */
void adicionar(void) {
    char nome[TAM];
    int qtd;
    FILE *fp;

    do {
        printf("Digite o nome do item: ");
        ler_linha(nome, TAM);
    } while (nome[0] == '\0');

    while (!ler_numero("Digite a quantidade: ", &qtd) || qtd <= 0)
        printf("Quantidade inválida! Digite um número inteiro positivo.\n");

    fp = fopen(ARQ, "a");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    fprintf(fp, "%s\n%d\n", nome, qtd);
    fclose(fp);

    printf("Item adicionado com sucesso!\n");
}

void remover(void) {
    char alvo[TAM], nome[TAM];
    int rem, qtd, disponivel = 0;
    int resultado = 0; /* 0 = não achou, 1 = insuficiente, 2 = atualizado, 3 = removido */
    FILE *fp, *tmp;

    do {
        printf("Digite o nome do item: ");
        ler_linha(alvo, TAM);
    } while (alvo[0] == '\0');

    while (!ler_numero("Digite a quantidade a ser removida: ", &rem) || rem <= 0)
        printf("Quantidade inválida! Digite um número inteiro positivo.\n");

    fp = fopen(ARQ, "r");
    if (fp == NULL) {
        printf("Item não encontrado.\n");
        return;
    }
    tmp = fopen(TMP, "w");
    if (tmp == NULL) {
        printf("Erro ao criar arquivo temporário.\n");
        fclose(fp);
        return;
    }

    while (ler_item(fp, nome, &qtd)) {
        if (resultado == 0 && strcmp(nome, alvo) == 0) {
            if (rem > qtd) {
                resultado = 1;
                disponivel = qtd;
                fprintf(tmp, "%s\n%d\n", nome, qtd);
            } else if (rem == qtd) {
                resultado = 3; /* zerou: não grava o item */
            } else {
                resultado = 2;
                fprintf(tmp, "%s\n%d\n", nome, qtd - rem);
            }
        } else {
            fprintf(tmp, "%s\n%d\n", nome, qtd);
        }
    }
    fclose(fp);
    fclose(tmp);

    if (resultado == 0 || resultado == 1) {
        remove(TMP); /* nada mudou: descarta o temporário */
        if (resultado == 0)
            printf("Item não encontrado.\n");
        else
            printf("Estoque insuficiente. Quantidade disponível: %d\n", disponivel);
        return;
    }

    remove(ARQ);
    rename(TMP, ARQ);

    if (resultado == 2)
        printf("Quantidade atualizada com sucesso!\n");
    else
        printf("Item removido do estoque!\n");
}

/* Requisito 4: listar estoque. */
void listar(void) {
    char nome[TAM];
    int qtd, n = 0;
    FILE *fp = fopen(ARQ, "r");

    if (fp != NULL) {
        while (ler_item(fp, nome, &qtd)) {
            if (n == 0) {
                printf("==============================\n");
                printf("        Estoque Atual\n");
                printf("==============================\n");
            } else {
                printf("\n");
            }
            printf("Nome: %s\n", nome);
            printf("Quantidade: %d\n", qtd);
            n++;
        }
        fclose(fp);
    }

    if (n == 0)
        printf("O estoque está vazio.\n");
}

int main(void) {
    int opc;
    char resp[TAM];

    while (1) {
        opc = menu();

        if (opc == 4)
            break;

        if (opc == 1)
            adicionar();
        else if (opc == 2)
            remover();
        else
            listar();

        printf("Deseja realizar outra operação? (s/n): ");
        ler_linha(resp, TAM);
        if (resp[0] != 's' && resp[0] != 'S')
            break;
    }

    /* Requisito 5: despedida (opção 4 ou resposta diferente de 's') */
    printf("Obrigado por usar o Controle de Estoques! Até a próxima.\n");
    return 0;
}
