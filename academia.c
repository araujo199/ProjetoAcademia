/* =====================================================================
 * ACADEMIA - CONTROLE DE TREINOS E FREQUENCIA
 * ---------------------------------------------------------------------
 * Disciplina : Algoritmos e Estrutura de Dados I
 * Curso      : Engenharia de Software
 * Linguagem  : C (C11)
 * ---------------------------------------------------------------------
 * Conceitos aplicados (conforme roteiro):
 *   - Modularizacao (funcoes coesas, baixo acoplamento)
 *   - Passagem de parametros por valor e por referencia (ponteiros)
 *   - Tipos Abstratos de Dados (TAD): interface x implementacao
 *   - Alocacao estatica  -> ListaRotinas  (vetor de tamanho fixo)
 *   - Alocacao dinamica  -> HistoricoPresencas (lista encadeada)
 *   - Recursividade      -> percurso da lista encadeada
 *
 * Compilacao:
 *   gcc -Wall -Wextra -std=c11 -g -o academia academia.c
 * Execucao:
 *   ./academia          (Linux/macOS)
 *   academia.exe        (Windows)
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =====================================================================
 * SECAO 1 - TAD ListaRotinas  (INTERFACE PUBLICA)
 * Armazenamento ESTATICO: vetor de tamanho fixo definido em compilacao.
 * ===================================================================== */

#define MAX_ROTINAS 50
#define TAM_NOME    60
#define TAM_GRUPO   30
#define TAM_DATA    11   /* "dd/mm/aaaa" + '\0' */

/* Registro que representa uma rotina de treino. */
typedef struct {
    int  id;
    char nome[TAM_NOME];
    char grupoMuscular[TAM_GRUPO];
    int  duracaoMin;
} Rotina;

/* TAD: lista sequencial de rotinas. */
typedef struct {
    Rotina itens[MAX_ROTINAS];
    int    quantidade;
    int    proximoId;
} ListaRotinas;

/* Prototipos publicos do TAD ListaRotinas ------------------------------ */
void rotinas_iniciar(ListaRotinas *lista);
int  rotinas_inserir(ListaRotinas *lista, const char *nome,
                     const char *grupo, int duracao);
int  rotinas_buscar_indice(const ListaRotinas *lista, int id);
int  rotinas_remover(ListaRotinas *lista, int id);
void rotinas_listar(const ListaRotinas *lista);
int  rotinas_vazia(const ListaRotinas *lista);

/* =====================================================================
 * SECAO 2 - TAD HistoricoPresencas  (INTERFACE PUBLICA)
 * Armazenamento DINAMICO: lista simplesmente encadeada.
 * ===================================================================== */

/* No da lista encadeada: um registro de presenca (ou falta). */
typedef struct Presenca {
    int  idRotina;
    char data[TAM_DATA];
    int  compareceu;   /* 1 = presente | 0 = falta */
    struct Presenca *prox;
} Presenca;

/* TAD: historico com ponteiros de inicio e fim (insercao O(1) no final). */
typedef struct {
    Presenca *inicio;
    Presenca *fim;
    int       totalRegistros;
    int       totalPresencas;
} HistoricoPresencas;

/* Prototipos publicos do TAD HistoricoPresencas ------------------------ */
void   historico_iniciar(HistoricoPresencas *h);
int    historico_registrar(HistoricoPresencas *h, int idRotina,
                           const char *data, int compareceu);
double historico_percentual(const HistoricoPresencas *h);
double historico_percentual_por_rotina(const HistoricoPresencas *h,
                                       int idRotina, int *total);
int    historico_presencas_recursivo(const HistoricoPresencas *h);
void   historico_liberar(HistoricoPresencas *h);

/* =====================================================================
 * SECAO 3 - IMPLEMENTACAO do TAD ListaRotinas
 * ===================================================================== */

void rotinas_iniciar(ListaRotinas *lista)
{
    if (lista == NULL) return;
    lista->quantidade = 0;
    lista->proximoId  = 1;
}

int rotinas_vazia(const ListaRotinas *lista)
{
    return (lista == NULL || lista->quantidade == 0);
}

int rotinas_inserir(ListaRotinas *lista, const char *nome,
                    const char *grupo, int duracao)
{
    if (lista == NULL || nome == NULL || grupo == NULL)
        return -1;                         /* dados invalidos */
    if (lista->quantidade >= MAX_ROTINAS)
        return -2;                         /* lista cheia     */
    if (duracao <= 0)
        return -3;                         /* duracao invalida */

    Rotina *nova = &lista->itens[lista->quantidade];
    nova->id = lista->proximoId++;

    /* Copia defensiva: nunca confiar no tamanho da string recebida. */
    strncpy(nova->nome, nome, TAM_NOME - 1);
    nova->nome[TAM_NOME - 1] = '\0';

    strncpy(nova->grupoMuscular, grupo, TAM_GRUPO - 1);
    nova->grupoMuscular[TAM_GRUPO - 1] = '\0';

    nova->duracaoMin = duracao;

    lista->quantidade++;
    return nova->id;
}

int rotinas_buscar_indice(const ListaRotinas *lista, int id)
{
    if (lista == NULL) return -1;
    for (int i = 0; i < lista->quantidade; i++)
        if (lista->itens[i].id == id)
            return i;
    return -1;
}

int rotinas_remover(ListaRotinas *lista, int id)
{
    int idx = rotinas_buscar_indice(lista, id);
    if (idx < 0) return 0;

    /* Desloca os elementos seguintes: O(n). */
    for (int i = idx; i < lista->quantidade - 1; i++)
        lista->itens[i] = lista->itens[i + 1];

    lista->quantidade--;
    return 1;
}

void rotinas_listar(const ListaRotinas *lista)
{
    if (rotinas_vazia(lista)) {
        printf("\nNenhuma rotina cadastrada ate o momento.\n");
        return;
    }

    printf("\n%-4s | %-32s | %-20s | %s\n",
           "ID", "Nome da rotina", "Grupo muscular", "Duracao");
    printf("-------------------------------------------------------------------------\n");

    for (int i = 0; i < lista->quantidade; i++) {
        const Rotina *r = &lista->itens[i];
        printf("%-4d | %-32s | %-20s | %3d min\n",
               r->id, r->nome, r->grupoMuscular, r->duracaoMin);
    }
    printf("-------------------------------------------------------------------------\n");
    printf("Total: %d rotina(s) cadastrada(s).\n", lista->quantidade);
}

/* =====================================================================
 * SECAO 4 - IMPLEMENTACAO do TAD HistoricoPresencas
 * ===================================================================== */

void historico_iniciar(HistoricoPresencas *h)
{
    if (h == NULL) return;
    h->inicio         = NULL;
    h->fim            = NULL;
    h->totalRegistros = 0;
    h->totalPresencas = 0;
}

int historico_registrar(HistoricoPresencas *h, int idRotina,
                        const char *data, int compareceu)
{
    if (h == NULL || data == NULL) return 0;

    /* Alocacao dinamica: cada registro ocupa memoria sob demanda. */
    Presenca *novo = (Presenca *) malloc(sizeof(Presenca));
    if (novo == NULL) return 0;   /* memoria insuficiente */

    novo->idRotina   = idRotina;
    novo->compareceu = compareceu ? 1 : 0;
    novo->prox       = NULL;

    strncpy(novo->data, data, TAM_DATA - 1);
    novo->data[TAM_DATA - 1] = '\0';

    /* Encadeia no final em O(1) gracas ao ponteiro 'fim'. */
    if (h->fim == NULL) {
        h->inicio = novo;
        h->fim    = novo;
    } else {
        h->fim->prox = novo;
        h->fim       = novo;
    }

    h->totalRegistros++;
    if (novo->compareceu)
        h->totalPresencas++;

    return 1;
}

double historico_percentual(const HistoricoPresencas *h)
{
    if (h == NULL || h->totalRegistros == 0)
        return 0.0;                        /* evita divisao por zero */
    return (100.0 * (double) h->totalPresencas) / (double) h->totalRegistros;
}

double historico_percentual_por_rotina(const HistoricoPresencas *h,
                                       int idRotina, int *total)
{
    int totalReg = 0;
    int presencas = 0;

    if (h != NULL) {
        for (const Presenca *p = h->inicio; p != NULL; p = p->prox) {
            if (p->idRotina == idRotina) {
                totalReg++;
                if (p->compareceu)
                    presencas++;
            }
        }
    }

    if (total != NULL)
        *total = totalReg;                 /* passagem por referencia */

    if (totalReg == 0)
        return 0.0;
    return (100.0 * (double) presencas) / (double) totalReg;
}

/* Percorrido recursivo da lista encadeada (Modulo 1 - recursividade). */
static int contar_presencas_rec(const Presenca *p)
{
    if (p == NULL)                         /* caso base */
        return 0;
    return (p->compareceu ? 1 : 0) + contar_presencas_rec(p->prox);
}

int historico_presencas_recursivo(const HistoricoPresencas *h)
{
    if (h == NULL) return 0;
    return contar_presencas_rec(h->inicio);
}

void historico_liberar(HistoricoPresencas *h)
{
    if (h == NULL) return;

    Presenca *p = h->inicio;
    while (p != NULL) {
        Presenca *proximo = p->prox;       /* guarda antes de liberar */
        free(p);
        p = proximo;
    }

    h->inicio         = NULL;
    h->fim            = NULL;
    h->totalRegistros = 0;
    h->totalPresencas = 0;
}

/* =====================================================================
 * SECAO 5 - CAMADA DE INTERFACE (menu e relatorios)
 * ===================================================================== */

/* -------------------- utilitarios internos de I/O -------------------- */

static void lerTexto(const char *rotulo, char *destino, int tamanho)
{
    printf("%s", rotulo);
    fflush(stdout);

    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    size_t n = strlen(destino);
    if (n > 0 && destino[n - 1] == '\n') {
        destino[n - 1] = '\0';
    } else {
        /* Linha maior que o buffer: descarta o restante. */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
}

static int lerInteiro(const char *rotulo)
{
    char buffer[32];
    int  valor = 0;
    lerTexto(rotulo, buffer, sizeof(buffer));
    if (sscanf(buffer, "%d", &valor) != 1)
        return -1;
    return valor;
}

static void pausar(void)
{
    char buffer[8];
    printf("\nPressione ENTER para continuar...");
    fflush(stdout);
    fgets(buffer, sizeof(buffer), stdin);
}

static void imprimirBarra(double percentual)
{
    int cheios = (int) (percentual / 5.0 + 0.5);  /* 20 blocos = 100% */
    if (cheios > 20) cheios = 20;
    if (cheios < 0)  cheios = 0;

    printf("[");
    for (int i = 0; i < 20; i++)
        putchar(i < cheios ? '#' : '.');
    printf("] %.2f%%\n", percentual);
}

/* ----------------------------- acoes -------------------------------- */

static void acaoCadastrarRotina(ListaRotinas *rotinas)
{
    char nome[TAM_NOME];
    char grupo[TAM_GRUPO];

    printf("\n=== Cadastrar rotina de treino ===\n");
    lerTexto("Nome da rotina (ex.: Treino A - Peito e Triceps): ", nome, TAM_NOME);
    if (nome[0] == '\0') {
        printf("Nome vazio. Operacao cancelada.\n");
        return;
    }

    lerTexto("Grupo muscular principal: ", grupo, TAM_GRUPO);

    int duracao = lerInteiro("Duracao em minutos: ");
    if (duracao <= 0) {
        printf("Duracao invalida. Operacao cancelada.\n");
        return;
    }

    int id = rotinas_inserir(rotinas, nome, grupo, duracao);

    if (id > 0)
        printf("\nRotina cadastrada com sucesso! ID = %d\n", id);
    else if (id == -2)
        printf("\nErro: limite de %d rotinas atingido.\n", MAX_ROTINAS);
    else
        printf("\nErro: dados invalidos.\n");
}

static void acaoRemoverRotina(ListaRotinas *rotinas)
{
    if (rotinas_vazia(rotinas)) {
        printf("\nNao ha rotinas para remover.\n");
        return;
    }

    rotinas_listar(rotinas);
    int id = lerInteiro("\nID da rotina a remover: ");

    if (rotinas_remover(rotinas, id))
        printf("\nRotina removida. Os registros de frequencia foram mantidos.\n");
    else
        printf("\nErro: rotina inexistente.\n");
}

static void acaoRegistrarPresenca(ListaRotinas *rotinas,
                                  HistoricoPresencas *historico)
{
    if (rotinas_vazia(rotinas)) {
        printf("\nCadastre ao menos uma rotina antes de registrar frequencia.\n");
        return;
    }

    rotinas_listar(rotinas);

    int id = lerInteiro("\nID da rotina prevista para o dia: ");
    if (rotinas_buscar_indice(rotinas, id) < 0) {
        printf("Erro: rotina inexistente.\n");
        return;
    }

    char data[TAM_DATA];
    lerTexto("Data do treino (dd/mm/aaaa): ", data, TAM_DATA);
    if (strlen(data) != 10) {
        printf("Data invalida: use o formato dd/mm/aaaa.\n");
        return;
    }

    char resposta[8];
    lerTexto("Voce compareceu a academia? (s/n): ", resposta, sizeof(resposta));
    int compareceu = (resposta[0] == 's' || resposta[0] == 'S') ? 1 : 0;

    if (historico_registrar(historico, id, data, compareceu))
        printf("\nRegistro armazenado: %s\n",
               compareceu ? "PRESENCA confirmada." : "FALTA registrada.");
    else
        printf("\nErro: falha ao alocar memoria para o registro.\n");
}

static void acaoExibirHistorico(const HistoricoPresencas *historico,
                                const ListaRotinas *rotinas)
{
    if (historico->totalRegistros == 0) {
        printf("\nNenhum registro de frequencia ainda.\n");
        return;
    }

    printf("\n=== Historico de frequencia (%d registro(s)) ===\n",
           historico->totalRegistros);
    printf("%-12s | %-32s | %s\n", "Data", "Rotina", "Status");
    printf("--------------------------------------------------------------------\n");

    for (const Presenca *p = historico->inicio; p != NULL; p = p->prox) {
        int idx = rotinas_buscar_indice(rotinas, p->idRotina);
        const char *nome = (idx >= 0) ? rotinas->itens[idx].nome
                                      : "(rotina removida)";
        printf("%-12s | %-32s | %s\n",
               p->data, nome, p->compareceu ? "PRESENTE" : "FALTA");
    }
    printf("--------------------------------------------------------------------\n");
}

static void acaoExibirPercentual(const HistoricoPresencas *historico,
                                 const ListaRotinas *rotinas)
{
    if (historico->totalRegistros == 0) {
        printf("\nSem registros: percentual indefinido (0.00%%).\n");
        return;
    }

    int faltas = historico->totalRegistros - historico->totalPresencas;

    printf("\n=== PERCENTUAL DE COMPARECIMENTO ===\n");
    printf("Dias registrados .......: %d\n", historico->totalRegistros);
    printf("Dias com presenca ......: %d\n", historico->totalPresencas);
    printf("Dias com falta .........: %d\n", faltas);

    double geral = historico_percentual(historico);
    printf("Comparecimento geral ...: ");
    imprimirBarra(geral);

    /* Conferencia usando a contagem recursiva da lista encadeada. */
    int conferencia = historico_presencas_recursivo(historico);
    printf("(conferencia recursiva .: %d presenca(s))\n", conferencia);

    printf("\n--- Detalhamento por rotina ---\n");
    int alguma = 0;
    for (int i = 0; i < rotinas->quantidade; i++) {
        int total = 0;
        double pct = historico_percentual_por_rotina(historico,
                                                     rotinas->itens[i].id,
                                                     &total);
        if (total == 0)
            continue;
        alguma = 1;
        printf("[%2d] %-28s ", rotinas->itens[i].id, rotinas->itens[i].nome);
        imprimirBarra(pct);
    }

    if (!alguma)
        printf("Nenhuma rotina ativa possui registros.\n");
}

/* --------------------------- laco principal ------------------------- */

void interface_executar(void)
{
    ListaRotinas       rotinas;
    HistoricoPresencas historico;

    rotinas_iniciar(&rotinas);
    historico_iniciar(&historico);

    int opcao = -1;

    do {
        printf("\n=========== ACADEMIA - CONTROLE DE TREINOS ===========\n");
        printf(" 1 - Cadastrar rotina de treino\n");
        printf(" 2 - Listar rotinas cadastradas\n");
        printf(" 3 - Remover rotina\n");
        printf(" 4 - Registrar presenca / falta\n");
        printf(" 5 - Ver historico de registros\n");
        printf(" 6 - Ver percentual de comparecimento\n");
        printf(" 0 - Sair\n");
        printf("======================================================\n");

        opcao = lerInteiro("Opcao: ");

        switch (opcao) {
            case 1: acaoCadastrarRotina(&rotinas);                 pausar(); break;
            case 2: rotinas_listar(&rotinas);                      pausar(); break;
            case 3: acaoRemoverRotina(&rotinas);                   pausar(); break;
            case 4: acaoRegistrarPresenca(&rotinas, &historico);   pausar(); break;
            case 5: acaoExibirHistorico(&historico, &rotinas);     pausar(); break;
            case 6: acaoExibirPercentual(&historico, &rotinas);    pausar(); break;
            case 0: printf("\nEncerrando o programa...\n");        break;
            default: printf("\nOpcao invalida. Tente novamente.\n"); break;
        }
    } while (opcao != 0);

    /* Libera toda a memoria alocada dinamicamente antes de terminar. */
    historico_liberar(&historico);
}

/* =====================================================================
 * SECAO 6 - PONTO DE ENTRADA
 * ===================================================================== */

int main(void)
{
    interface_executar();
    return 0;
}