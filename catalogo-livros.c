#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef _WIN32
    #define strcasecmp _stricmp
    #define CLEAR "cls"
    #define LOCALE "Portuguese_Brazil.1252"
#else
    #define CLEAR "clear"
    #define LOCALE "pt_BR.UTF-8"
#endif
#define COR_RESET  "\033[0m"
#define COR_VERDE  "\033[32m"
#define COR_VERMELHA "\033[31m"
#define COR_AMARELA "\033[33m"
/* ================================================
   ESTRUTURAS DE DADOS
   ================================================ */

/* Nó da Árvore Binária de Busca (BST) */

typedef struct Livro {
    char titulo[100];
    char autor[100];
    struct Livro *esq;   /* filho menor (alfabeticamente) */
    struct Livro *dir;   /* filho maior (alfabeticamente) */
} Livro;

/* Nó da Pilha — para o histórico de buscas */
typedef struct Historico {
    char titulo[100];
    struct Historico *prox;
} Historico;

/* ================================================
   FUNÇÕES DA BST
   ================================================ */

/* Cria e retorna um novo nó com título e autor */
Livro *bst_novo(const char *titulo, const char *autor) {
    Livro *l = malloc(sizeof(Livro));
    if (l == NULL) { printf("Erro: memoria insuficiente.\n"); exit(1); }
    strcpy(l->titulo, titulo);
    strcpy(l->autor,  autor);
    l->esq = NULL;
    l->dir = NULL;
    return l;
}

/* Insere um livro na BST em ordem alfabética pelo título */
Livro *bst_inserir(Livro *raiz, const char *titulo, const char *autor) {
    if (raiz == NULL)
        return bst_novo(titulo, autor);

    int cmp = strcasecmp(titulo, raiz->titulo);

    if (cmp < 0)
        raiz->esq = bst_inserir(raiz->esq, titulo, autor);
    else if (cmp > 0)
        raiz->dir = bst_inserir(raiz->dir, titulo, autor);
    else
        printf("Livro \"%s\" já cadastrado.\n", titulo);

    return raiz;
}

/* Busca um livro pelo título; retorna o nó ou NULL */
Livro *bst_buscar(Livro *raiz, const char *titulo) {
    if (raiz == NULL)
        return NULL;

    int cmp = strcasecmp(titulo, raiz->titulo);

    if (cmp == 0) return raiz;
    if (cmp  < 0) return bst_buscar(raiz->esq, titulo);
    return bst_buscar(raiz->dir, titulo);
}

/* Retorna o nó com o menor título dentro de uma subárvore */
Livro *bst_menor(Livro *raiz) {
    while (raiz->esq != NULL)
        raiz = raiz->esq;
    return raiz;
}

/*
 * Remove um livro da BST.
 * Três situações:
 *   1. Nó sem filhos   ? simplesmente apaga
 *   2. Nó com 1 filho  ? o filho sobe no lugar
 *   3. Nó com 2 filhos ? substitui pelo sucessor (menor da direita)
 */
Livro *bst_remover(Livro *raiz, const char *titulo) {
    if (raiz == NULL) {
        printf(COR_VERMELHA "Erro: Livro \"%s\" não encontrado para remoção.\n" COR_RESET, titulo);
        return NULL;
    }

    int cmp = strcasecmp(titulo, raiz->titulo);

    if (cmp < 0) {
        raiz->esq = bst_remover(raiz->esq, titulo);
    } else if (cmp > 0) {
        raiz->dir = bst_remover(raiz->dir, titulo);
    } else {
        /* Caso 1 e 2: zero ou um filho */
        if (raiz->esq == NULL) { Livro *t = raiz->dir; free(raiz); return t; }
        if (raiz->dir == NULL) { Livro *t = raiz->esq; free(raiz); return t; }

        /* Caso 3: dois filhos — copia o sucessor e o remove */
        Livro *suc = bst_menor(raiz->dir);
        strcpy(raiz->titulo, suc->titulo);
        strcpy(raiz->autor,  suc->autor);
        raiz->dir = bst_remover(raiz->dir, suc->titulo);
    }
    return raiz;
}

/* Lista todos os livros em ordem alfabética (in-order) */
void bst_listar(Livro *raiz, int *n) {
    if (raiz == NULL) return;
    bst_listar(raiz->esq, n);
    printf("  [%d] %s \n   Autor(a): %s\n", ++(*n), raiz->titulo, raiz->autor);
    bst_listar(raiz->dir, n);
}

/* ================================================
   FUNÇÕES DA PILHA (histórico)
   ================================================ */

/* Empilha um título buscado */
void pilha_push(Historico **topo, const char *titulo) {
    Historico *novo = malloc(sizeof(Historico));
    if (novo == NULL) { printf("Erro: memoria insuficiente.\n"); exit(1); }
    strcpy(novo->titulo, titulo);
    novo->prox = *topo;
    *topo = novo;
}

/* Exibe o histórico do mais recente para o mais antigo */
void pilha_listar(Historico *topo) {
    if (topo == NULL) {
        printf("  Historico vazio.\n");
        return;
    }
    int i = 1;
    while (topo != NULL) {
        printf("  [%d] %s\n", i++, topo->titulo);
        topo = topo->prox;
    }
}

/* ================================================
   PERSISTÊNCIA (salvar / carregar em arquivos)
   ================================================ */

void salvar_livros(Livro *raiz, FILE *fp) {
    if (raiz == NULL) return;
    fprintf(fp, "%s;%s\n", raiz->titulo, raiz->autor);
    salvar_livros(raiz->esq, fp);
    salvar_livros(raiz->dir, fp);
}

void carregar_livros(Livro **raiz) {
    FILE *fp = fopen("livros.csv", "r");
    if (fp == NULL) return;

    char linha[210], titulo[100], autor[100];
    while (fgets(linha, sizeof(linha), fp)) {
        linha[strcspn(linha, "\n")] = '\0';
        char *sep = strchr(linha, ';');
        if (sep == NULL) continue;
        *sep = '\0';
        strncpy(titulo, linha, sizeof(titulo) - 1);
		titulo[sizeof(titulo) - 1] = '\0';
		
		strncpy(autor, sep + 1, sizeof(autor) - 1);
		autor[sizeof(autor) - 1] = '\0';
		
		if (strlen(titulo) > 0 && strlen(autor) > 0) {
		    *raiz = bst_inserir(*raiz, titulo, autor);
		}
    }
    fclose(fp);
}

void salvar_historico(Historico *topo, FILE *fp) {
    if (topo == NULL) return;
    salvar_historico(topo->prox, fp);   /* salva do mais antigo ao mais novo */
    fprintf(fp, "%s\n", topo->titulo);
}

void carregar_historico(Historico **topo) {
    FILE *fp = fopen("historico.txt", "r");
    if (fp == NULL) return;

    char linha[110];
    while (fgets(linha, sizeof(linha), fp)) {
        linha[strcspn(linha, "\n")] = '\0';
        if (strlen(linha) > 0)
            pilha_push(topo, linha);
    }
    fclose(fp);
}

/* ================================================
   UTILITÁRIOS
   ================================================ */

void ler_string(const char *prompt, char *buf, int tam) {
    printf("%s", prompt);
    fgets(buf, tam, stdin);
    buf[strcspn(buf, "\n")] = '\0';
	
	int i;
	for(i = 0; buf[i] != '\0'; i++) {
	    if(buf[i] == ';') {
	        buf[i] = ' ';
	    }
	}
}

void bst_liberar(Livro *raiz) {
    if (raiz == NULL) return;
    bst_liberar(raiz->esq);
    bst_liberar(raiz->dir);
    free(raiz);
}

void pilha_liberar(Historico *topo) {
    while (topo != NULL) {
        Historico *tmp = topo;
        topo = topo->prox;
        free(tmp);
    }
}
void limpar_buffer() {
    int c;
    /* "Enquanto o que for lido não for o Enter (\n) nem o fim do arquivo (EOF), continue jogando no lixo" */
    while ((c = getchar()) != '\n' && c != EOF); 
}

void pausar() {
    printf(COR_AMARELA "\nPressione ENTER para voltar ao menu..." COR_RESET);
    getchar();
}
/* ================================================
   MENU PRINCIPAL
   ================================================ */


int main(void) {
	setlocale(LC_ALL, LOCALE);
    Livro     *raiz = NULL;
    Historico *topo = NULL;

    carregar_livros(&raiz);
    carregar_historico(&topo);

    int opcao;
    char titulo[100], autor[100];

    do {
        system(CLEAR);
        printf("====================================\n");
        printf(COR_AMARELA "       CATÁLOGO DE LIVROS\n" COR_RESET);
        printf("====================================\n\n");
        printf("  [ 1 ] Cadastrar novo livro\n");
        printf("  [ 2 ] Buscar livro pelo titulo\n");
        printf("  [ 3 ] Remover livro\n");
        printf("  [ 4 ] Listar todos os livros\n");
        printf("  [ 5 ] Ver historico de buscas\n");
        printf(COR_VERMELHA "  [ 0 ] Sair do programa\n\n" COR_RESET);
        printf("Escolha uma opcao: ");
        
        scanf("%d", &opcao);
        limpar_buffer();
        
        system(CLEAR);
        
        switch (opcao) {

        case 1:
            printf(COR_AMARELA "\n====================================\n        CADASTRAR LIVRO \n====================================\n\n" COR_RESET);
            ler_string("Titulo: ", titulo, sizeof(titulo));
            ler_string("Autor : ", autor,  sizeof(autor));
            raiz = bst_inserir(raiz, titulo, autor);
            printf(COR_VERDE "\nSucesso: Livro \"%s\" cadastrado!\n" COR_RESET, titulo);
            pausar();
            break;

        case 2: {
            printf(COR_AMARELA "\n====================================\n        BUSCAR LIVRO \n====================================\n\n" COR_RESET);
            ler_string("Titulo: ", titulo, sizeof(titulo));
            Livro *l = bst_buscar(raiz, titulo);
            if (l != NULL) {
            	printf(COR_VERDE "\n====================================\n        ENCONTRADO \n====================================\n\n" COR_RESET);
                printf("Título: %s\nAutor: %s\n", l->titulo, l->autor);
                pilha_push(&topo, titulo);
            } else {
                printf(COR_VERMELHA "\nLivro \"%s\" nao encontrado.\n" COR_RESET, titulo);
            }
            pausar();
            break;
        }

        case 3:
            printf(COR_AMARELA "\n====================================\n        REMOVER LIVRO \n====================================\n\n" COR_RESET);
            ler_string("Titulo: ", titulo, sizeof(titulo));
            Livro *antes = bst_buscar(raiz, titulo);
            if (antes != NULL) {
                printf(COR_VERDE "\nSucesso: Livro \"%s\" removido!\n" COR_RESET, antes->titulo);
            }
            raiz = bst_remover(raiz, titulo);
            pausar();
            break;

        case 4: {
            int n = 0;
            printf(COR_AMARELA "\n===============================================================\n             TODOS OS LIVROS (Ordem Alfabetica) \n===============================================================\n\n" COR_RESET);
            bst_listar(raiz, &n);
            if (n == 0) printf(COR_VERMELHA "  Nenhum livro cadastrado.\n" COR_RESET);
            pausar();
            break;
        }

        case 5:
            printf(COR_AMARELA "\n====================================\n HISTORICO DE BUSCAS \n====================================\n\n" COR_RESET);
            pilha_listar(topo);
            pausar();
            break;

        case 0:
            printf(COR_VERDE "Salvando dados e saindo do sistema...\n" COR_RESET);
            break;

        default:
            printf(COR_VERMELHA "Opcao inválida. Tente novamente.\n" COR_RESET);
            pausar();
        }

    } while (opcao != 0);

    /* Salva tudo antes de sair */
    FILE *fl = fopen("livros.csv",    "w");
    FILE *fh = fopen("historico.txt", "w");
    if (fl) { salvar_livros(raiz, fl);    fclose(fl); }
    else      printf(COR_VERMELHA "Erro ao salvar livros.csv\n" COR_RESET);
    if (fh) { salvar_historico(topo, fh); fclose(fh); }
    else      printf(COR_VERMELHA "Erro ao salvar historico.txt\n" COR_RESET);

    bst_liberar(raiz);
    pilha_liberar(topo);
    return 0;
}