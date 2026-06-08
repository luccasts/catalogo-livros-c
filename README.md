# 📚 Catálogo de Livros - Sistema CLI em C

Sistema de linha de comando para gerenciamento de um catálogo de livros, desenvolvido em C puro (C11) como projeto final da disciplina de Estruturas de Dados.

---

## Descrição do problema

Bibliotecas e coleções pessoais de livros crescem rapidamente e se tornam difíceis de organizar e consultar. Este sistema resolve esse problema oferecendo um catálogo persistente onde o usuário pode **cadastrar**, **buscar**, **remover** e **listar** livros em ordem alfabética, além de consultar um **histórico das buscas realizadas** — tudo via menu interativo no terminal.

---

## Estruturas de dados utilizadas

### 🌳 Árvore Binária de Busca (BST) — catálogo de livros

Cada livro é um nó da BST, ordenado **alfabeticamente pelo título** (usando `strcasecmp` para comparação sem distinção de maiúsculas/minúsculas).

**Por que BST e não lista encadeada simples?**

A BST foi escolhida porque o problema central do catálogo é **busca e listagem ordenada**. Com ela:

- A **busca** por título tem custo médio **O(log n)**, contra O(n) de uma lista simples.
- A **listagem em ordem alfabética** é feita com uma travessia *in-order* (esquerda → raiz → direita), que percorre os nós exatamente na ordem crescente — sem custo extra de ordenação.
- A **inserção** já posiciona o livro no lugar correto da ordem alfabética automaticamente.

Usar uma lista encadeada exigiria percorrê-la inteira a cada busca e ordenação manual a cada inserção, o que é menos eficiente e semanticamente inadequado para um catálogo pesquisável.

> **Limitação conhecida:** a implementação não é balanceada (sem AVL ou Red-Black). Se os livros forem inseridos já em ordem alfabética, a árvore degenera em uma lista encadeada, e a busca volta a ser O(n). Isso é aceitável para o escopo deste projeto.

---

### 📋 Pilha encadeada — histórico de buscas

Cada busca bem-sucedida empilha o título pesquisado. A exibição do histórico mostra os títulos do **mais recente ao mais antigo** (comportamento LIFO).

**Por que Pilha e não um array fixo?**

A pilha é a estrutura semanticamente correta para um histórico de buscas por três razões:

1. **Semântica LIFO:** o item consultado mais recentemente aparece primeiro — exatamente o comportamento esperado de um histórico.
2. **Crescimento dinâmico:** a pilha encadeada não tem tamanho fixo. Um array exigiria um limite pré-definido ou realocação manual (`realloc`), enquanto a pilha cresce conforme necessário, alocando um nó por busca.
3. **Extensibilidade:** a operação de *desfazer a última busca* (`pop`) seria trivial de implementar com essa estrutura, sem nenhuma mudança na arquitetura.

---

## Funcionalidades do menu

```
====================================
       CATÁLOGO DE LIVROS
====================================

  [ 1 ] Cadastrar novo livro
  [ 2 ] Buscar livro pelo titulo
  [ 3 ] Remover livro
  [ 4 ] Listar todos os livros
  [ 5 ] Ver historico de buscas
  [ 0 ] Sair do programa
```

| Opção | Operação | Estrutura envolvida |
|-------|----------|-------------------|
| 1 | Cadastrar livro | Inserção na BST |
| 2 | Buscar por título | Busca na BST + Push na Pilha |
| 3 | Remover livro | Remoção na BST (3 casos) |
| 4 | Listar em ordem alfabética | Travessia in-order da BST |
| 5 | Ver histórico de buscas | Listagem da Pilha (LIFO) |
| 0 | Salvar e sair | Persistência em arquivo |

---

## Formato dos arquivos de persistência

O sistema utiliza **dois arquivos**, salvos automaticamente ao sair (opção 0):

### `livros.csv`

Armazena os livros em formato CSV com separador `;`. Um livro por linha:

```
Dom Casmurro;Machado de Assis
O Senhor dos Aneis;J.R.R. Tolkien
1984;George Orwell
```

**Formato:** `titulo;autor`

O separador `;` foi escolhido em vez de `,` porque títulos e nomes de autores frequentemente contêm vírgulas (ex: *"Tolkien, J.R.R."*), o que quebraria o parse se a vírgula fosse o delimitador.

Os livros são salvos em **pré-ordem** (raiz → esquerda → direita). Isso garante que, ao recarregar o arquivo e reinserir os livros, a árvore seja reconstruída com uma estrutura próxima à original — se fossem salvos em in-order (ordem alfabética) e reinseridos nessa mesma sequência, a árvore degeneraria em lista encadeada.

### `historico.txt`

Armazena os títulos buscados, um por linha, do mais antigo ao mais novo:

```
1984
Dom Casmurro
O Senhor dos Aneis
```

O histórico é salvo em ordem inversa à da pilha (do mais antigo ao mais novo) para que, ao recarregar com `pilha_push`, o topo da pilha corresponda à busca mais recente — reconstruindo o estado correto da pilha.

**Comportamento na primeira execução:** se os arquivos não existirem, `fopen` retorna `NULL` e o sistema inicia com catálogo e histórico vazios, sem erros.

---

## Como compilar e executar

### Linux / macOS

```bash
gcc -std=c11 -Wall -o catalogo main.c
./catalogo
```

### Windows (MinGW)

```bash
gcc -std=c11 -Wall -o catalogo.exe main.c
catalogo.exe
```

---

## Limitações conhecidas

| Limitação | Descrição |
|-----------|-----------|
| Árvore não balanceada | Inserções em ordem alfabética degeneradam a BST em O(n) |
| Separador no título/autor | Títulos ou autores contendo `;` corrompem o arquivo CSV |
| Histórico sem remoção | Não há operação de `pop` — o histórico só cresce |
| Tamanho fixo dos campos | Títulos e autores limitados a 99 caracteres |
| Busca exata | A busca por título precisa ser exata (sem busca parcial ou por autor) |

---

## Possíveis melhorias futuras

- Balanceamento da BST (AVL) para garantir O(log n) no pior caso
- Busca por autor ou por trecho do título
- Operação de desfazer última busca (pop do histórico)
- Validação de entradas com caracteres especiais e separador CSV
- Busca em múltiplos campos (título, autor, gênero)

---

## Autores
- Lucas Teixeira da Silva
- Maria Luíza Soares Viana
---
Projeto desenvolvido como avaliação final da disciplina de Estruturas de Dados — C11, bibliotecas padrão apenas.
