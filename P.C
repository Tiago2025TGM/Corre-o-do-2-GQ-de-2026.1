#include <stdio.h>
#include <string.h>

#define TAM_CONTAS 50
#define TAM_OPERACOES 10000

struct Conta {
    int id;
    char cliente[40];
    double saldo;
    double limite;
};

struct Operacao {
    int id;
    int idConta;
    char tipo; /* 'D', 'S' ou 'T' */
    double valor;
};

/* Implementada em outro lugar, conforme o enunciado. */
int buscaId(struct Conta v[], int tam, int x);

static void descartaLinha(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static void imprimeValor(double valor)
{
    printf("%10.2f%c", valor < 0 ? -valor : valor, valor < 0 ? 'D' : 'C');
}

void cadastraConta(struct Conta v[], int *tam)
{
    struct Conta nova;
    int i;
    int pos;

    if (*tam >= TAM_CONTAS) {
        printf("Vetor de contas cheio.\n");
        return;
    }

    printf("ID da conta: ");
    if (scanf("%d", &nova.id) != 1) {
        descartaLinha();
        printf("ID invalido.\n");
        return;
    }
    descartaLinha();

    if (buscaId(v, *tam, nova.id) != -1) {
        printf("Ja existe uma conta com esse ID.\n");
        return;
    }

    printf("Nome do cliente: ");
    if (fgets(nova.cliente, sizeof(nova.cliente), stdin) == NULL) {
        printf("Nao foi possivel ler o nome.\n");
        return;
    }
    nova.cliente[strcspn(nova.cliente, "\n")] = '\0';

    printf("Saldo inicial: ");
    if (scanf("%lf", &nova.saldo) != 1) {
        descartaLinha();
        printf("Saldo invalido.\n");
        return;
    }

    printf("Limite: ");
    if (scanf("%lf", &nova.limite) != 1) {
        descartaLinha();
        printf("Limite invalido.\n");
        return;
    }
    descartaLinha();

    if (nova.limite < 0) {
        printf("O limite nao pode ser negativo.\n");
        return;
    }

    /* Mantem o vetor ordenado por nome, requisito de buscaBinariaNome. */
    pos = 0;
    while (pos < *tam && strcmp(v[pos].cliente, nova.cliente) <= 0) {
        pos++;
    }
    for (i = *tam; i > pos; i--) {
        v[i] = v[i - 1];
    }
    v[pos] = nova;
    (*tam)++;
}

int buscaBinariaNome(struct Conta v[], int tam, char x[])
{
    int ini = 0;
    int fim = tam - 1;

    while (ini <= fim) {
        int meio = ini + (fim - ini) / 2;
        int comparacao = strcmp(v[meio].cliente, x);

        if (comparacao == 0) {
            return meio;
        }
        if (comparacao < 0) {
            ini = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    return -1;
}

void filtraDevedores(struct Conta vContas[], int tamConta,
                     struct Conta vDevedores[], int *tamDevedores)
{
    int i;

    *tamDevedores = 0;
    for (i = 0; i < tamConta; i++) {
        if (vContas[i].saldo < 0) {
            vDevedores[*tamDevedores] = vContas[i];
            (*tamDevedores)++;
        }
    }
}

int realizarSaque(struct Conta vContas[], int tamConta,
                  struct Operacao vOp[], int *tamOp)
{
    int idConta;
    int pos;
    double valor;

    if (*tamOp >= TAM_OPERACOES) {
        printf("Vetor de operacoes cheio.\n");
        return 0;
    }

    printf("ID da conta: ");
    if (scanf("%d", &idConta) != 1) {
        descartaLinha();
        printf("ID invalido.\n");
        return 0;
    }
    printf("Valor do saque: ");
    if (scanf("%lf", &valor) != 1) {
        descartaLinha();
        printf("Valor invalido.\n");
        return 0;
    }
    descartaLinha();

    pos = buscaId(vContas, tamConta, idConta);
    if (pos == -1) {
        printf("Conta nao encontrada.\n");
        return 0;
    }
    if (valor <= 0 || vContas[pos].saldo + vContas[pos].limite < valor) {
        printf("Saque invalido ou saldo/limite insuficiente.\n");
        return 0;
    }

    vContas[pos].saldo -= valor;
    vOp[*tamOp].id = *tamOp + 1;
    vOp[*tamOp].idConta = idConta;
    vOp[*tamOp].tipo = 'S';
    vOp[*tamOp].valor = -valor;
    (*tamOp)++;

    return 1;
}

void extrato(struct Conta vContas[], int tamConta,
             struct Operacao vOp[], int *tamOp)
{
    int idConta;
    int posConta;
    int i;
    int qtd = 0;
    int ignorar;
    int impressas = 0;
    double saldoAnterior = 0;

    (void)tamOp;
    printf("ID da conta para o extrato: ");
    if (scanf("%d", &idConta) != 1) {
        descartaLinha();
        printf("ID invalido.\n");
        return;
    }
    descartaLinha();

    posConta = buscaId(vContas, tamConta, idConta);
    if (posConta == -1) {
        printf("Conta nao encontrada.\n");
        return;
    }

    for (i = 0; i < *tamOp; i++) {
        if (vOp[i].idConta == idConta) {
            qtd++;
        }
    }
    ignorar = qtd > 5 ? qtd - 5 : 0;

    /* Soma as operacoes anteriores às cinco que serao exibidas. */
    qtd = 0;
    for (i = 0; i < *tamOp; i++) {
        if (vOp[i].idConta == idConta) {
            if (qtd < ignorar) {
                saldoAnterior += vOp[i].valor;
            }
            qtd++;
        }
    }

    printf("+--------------------------------------------------+\n");
    printf("| CONTA N.%03d\n", vContas[posConta].id);
    printf("| CLIENTE: %s\n", vContas[posConta].cliente);
    printf("+------+-----------------+------------+------------+\n");
    printf("| ID   | OPERACAO        | VALOR      | SALDO      |\n");
    printf("+------+-----------------+------------+------------+\n");
    printf("|      | Saldo Anterior  |            |");
    imprimeValor(saldoAnterior);
    printf("|\n");

    for (i = 0; i < *tamOp; i++) {
        if (vOp[i].idConta != idConta) {
            continue;
        }
        if (impressas < ignorar) {
            impressas++;
            continue;
        }

        saldoAnterior += vOp[i].valor;
        printf("|%04d  |", vOp[i].id);
        if (vOp[i].tipo == 'D') {
            printf(" Deposito        |");
        } else if (vOp[i].tipo == 'S') {
            printf(" Saque           |");
        } else {
            printf(" Transferencia   |");
        }
        imprimeValor(vOp[i].valor);
        printf("|");
        imprimeValor(saldoAnterior);
        printf("|\n");
        impressas++;
    }

    printf("+------+-----------------+------------+------------+\n");
    printf("Saldo Atual: ");
    imprimeValor(vContas[posConta].saldo);
    printf("\n");
}