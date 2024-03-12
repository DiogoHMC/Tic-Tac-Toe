#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Node {

  int index;
  char valor[2];

  struct Node *prox;
  struct Node *ant;

} Node;

typedef struct Jogador {

  char nome[50];
  int pontuacao;
} Jogador;

// cores
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_GOLD "\x1b[38;5;220m"
#define ANSI_COLOR_SILVER "\x1b[38;5;7m"
#define ANSI_COLOR_COPPER "\x1b[38;5;166m"

void CreateNode(Node **head, Node **tail, int index);
void imprimirTabuleiro(Node *head, Node *tail, Jogador Player1, Jogador Player2,
                       int empate);

void imprimirInverso(Node *head, Node *tail);
int switchPlayerOrder(int Player);
int PosicaoOcupada(Node *head, int index);

void FindIndex(Node **head, Node **tail, int index, int valor);
int ganhadorDaPartida(Node *head, Node *tail);

void TicTacToeCompleto(Jogador Player1, Jogador Player2);
void FuncaoJogar();
void liberarLista(Node **head, Node **tail);

void exibirVencedores();
void FuncaoRanking(const char *nome, int pontuacao, int printar);
void bubblesortNoArquivo();

int main() {

  int escolhaMenu;
  FILE *arquivoRanking = fopen("ranking.txt", "r");

  if (arquivoRanking == NULL) {

    arquivoRanking = fopen("ranking.txt", "w");

    if (arquivoRanking == NULL) {

      printf("Erro ao criar o arquivo ranking.txt\n");
      return 1;
    }

    fclose(arquivoRanking);
  }

  else {

    fclose(arquivoRanking);
  }

  while (1) {
    printf(ANSI_COLOR_GOLD "==== Menu ====\n" ANSI_COLOR_RESET);
    printf("1. Jogar\n");
    printf("2. Ranking\n");
    printf("3. Sair\n");
    printf(ANSI_COLOR_GOLD "==============\n" ANSI_COLOR_RESET);
    printf("Escolha uma opção: ");
    scanf("%d", &escolhaMenu);
    getchar();

    switch (escolhaMenu) {
    case 1:

      FuncaoJogar();
      break;

    case 2:

      bubblesortNoArquivo();
      system("clear");
      exibirVencedores();
      break;

    case 3:

      return 0;

    default:

      printf("Opção inválida.\n");
    }
  }

  return 0;
}

void FuncaoJogar() {

  system("clear");

  Jogador Player1;
  Jogador Player2;

  do {

    printf("Digite um nick para o jogador 1: ");

    fgets(Player1.nome, sizeof(Player1.nome), stdin);

    Player1.nome[strcspn(Player1.nome, "\n")] = '\0';

    printf("Digite um nick para o jogador 2: ");

    fgets(Player2.nome, sizeof(Player2.nome), stdin);
    Player2.nome[strcspn(Player2.nome, "\n")] = '\0';

    if (strcmp(Player1.nome, Player2.nome) == 0) {

      printf("Os nomes não podem ser iguais. Tente novamente.\n");
    }

  } while (strcmp(Player1.nome, Player2.nome) == 0);

  system("clear");

  TicTacToeCompleto(Player1, Player2);
}

void TicTacToeCompleto(Jogador Player1, Jogador Player2) {

  int player = 1, choice;
  Node *head = NULL;
  Node *tail = NULL;

  for (int i = 1; i <= 9; i++) {
    CreateNode(&head, &tail, i);
  }

  int totalTabela = 0;
  bool empate = false;

  do {
    imprimirTabuleiro(head, tail, Player1, Player2, 0);
    int ganhador = ganhadorDaPartida(head, tail);

    if (ganhador != 0) {

      if (ganhador == 1) {
        system("clear");
        printf("\t%s ganhou!!\n\n", Player1.nome);
        imprimirTabuleiro(head, tail, Player1, Player2, 0);

        FuncaoRanking(Player1.nome, 3, true);
        bubblesortNoArquivo();

        printf("+3 pontos para %s\n", Player1.nome);

        sleep(3);
        system("clear");
      } else {
        system("clear");
        printf("\t%s ganhou!!\n\n", Player2.nome);

        imprimirTabuleiro(head, tail, Player1, Player2, 0);
        FuncaoRanking(Player2.nome, 3, true);
        bubblesortNoArquivo();

        printf("+3 pontos para %s\n", Player2.nome);

        sleep(3);
        system("clear");
      }

      break;
    }

    // coleta do dado
    if (player == 1) {
      printf("%s, digite um index:  ", Player1.nome);
      totalTabela++;
    } else {
      printf("%s, digite um index:  ", Player2.nome);
      totalTabela++;
    }

    scanf("%d", &choice);

    if (choice == 0) {
      break;
    }

    // jogada
    if (choice >= 1 && choice <= 9) {
      if (PosicaoOcupada(head, choice)) {
        printf("Posição já está ocupada. Tente novamente.\n");
        printf("Aguarde 2 segundos para próxima jogada\n");
        totalTabela--;
        sleep(2);
      } else {
        FindIndex(&head, &tail, choice, player);
        player = switchPlayerOrder(player);
      }
    } else {
      printf("Movimento inválido, tente novamente.\n");
      printf("Aguarde 2 segundos para próxima jogada\n");
      totalTabela--;
      sleep(2);
    }

    // Verifica se todas as posições estão preenchidas
    if (totalTabela == 9 && ganhador == 0) {
      empate = true;
      break;
    }
    system("clear");
  } while (!empate); // continue

  if (empate) {
    system("clear");
    printf("O jogo empatou!\n\n");
    imprimirTabuleiro(head, tail, Player1, Player2, 1);

    FuncaoRanking(Player1.nome, 1, true);
    FuncaoRanking(Player2.nome, 1, false);
    printf("+1 ponto para ambos os jogadores\n");

    bubblesortNoArquivo();
    sleep(3);
    system("clear");
  }

  liberarLista(&head, &tail);
}

void CreateNode(Node **head, Node **tail, int index) {

  Node *novo = (Node *)malloc(sizeof(Node));

  novo->index = index;
  novo->valor[0] = ' ';
  novo->valor[1] = '\0';

  if (*head == NULL) {

    *head = novo;
    *tail = novo;
    novo->ant = *head;
    novo->prox = *head;
  }

  else {

    novo->prox = *head;
    (*head)->ant = novo;
    novo->ant = *tail;
    (*tail)->prox = novo;
    *head = novo;
  }
}

void imprimirTabuleiro(Node *head, Node *tail, Jogador Player1, Jogador Player2,
                       int empate) {
  Node *atual = tail;

  printf("\t\tTic Tac Toe\n\n");
  printf("\t%s (X)  -  %s (O)\n\n\n", Player1.nome, Player2.nome);

  // Calcula a largura total da tabela
  int larguraTabela = 19;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (j < 2) {
        if (strcmp(atual->valor, " ") != 0) {
          // Adicionando cor ao texto
          if (empate && (strcmp(atual->valor, "X") == 0 ||
                         strcmp(atual->valor, "O") == 0)) {
            printf("\t" ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET "   |",
                   atual->valor[0]);
          } else if (strcmp(atual->valor, "X") == 0) {
            printf("\t" ANSI_COLOR_GREEN "%c" ANSI_COLOR_RESET "   |",
                   atual->valor[0]);
          } else {
            printf("\t" ANSI_COLOR_RED "%c" ANSI_COLOR_RESET "   |",
                   atual->valor[0]);
          }
        } else {
          printf("\t" ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET "   |",
                 atual->index);
        }
      } else {
        if (strcmp(atual->valor, " ") != 0) {
          // Adicionando cor ao texto
          if (empate && (strcmp(atual->valor, "X") == 0 ||
                         strcmp(atual->valor, "O") == 0)) {
            printf("\t" ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET " \n",
                   atual->valor[0]);
          } else if (strcmp(atual->valor, "X") == 0) {
            printf("\t" ANSI_COLOR_GREEN "%c" ANSI_COLOR_RESET " \n",
                   atual->valor[0]);
          } else {
            printf("\t" ANSI_COLOR_RED "%c" ANSI_COLOR_RESET " \n",
                   atual->valor[0]);
          }
        } else {
          printf("\t" ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET " \n",
                 atual->index);
        }
      }

      atual = atual->ant;
    }

    if (i < 2) {
      printf("\t----|-------|----\n");
    }
  }

  printf("\n");
}

void imprimirInverso(Node *head, Node *tail) {

  if (head != NULL) {

    do {

      printf("INDEX: %d\n", tail->index);
      printf("VALOR: %s\n", tail->valor);

      tail = tail->ant;

    } while (head != tail->prox);
  }
}

int switchPlayerOrder(int Player) {

  if (Player == 1) {

    return 2;
  }

  else if (Player == 2) {

    return 1;
  }

  else {

    return 1;
  }
}

void FindIndex(Node **head, Node **tail, int index, int valor) {

  Node *current = *head;

  while (current != NULL) {

    if (current->index == index) {

      if (current->valor[0] == ' ') {

        if (valor == 1) {

          current->valor[0] = 'X';
        }

        else {

          current->valor[0] = 'O';
        }

      }

      else {

        printf("Índice já está ocupado. Tente novamente.\n");
      }

      return;
    }

    current = current->prox;
  }
}

int ganhadorDaPartida(Node *head, Node *tail) {
  int vitoria = 0;

  // Vitorias possiveis
  int possibilidades[8][3] = {{1, 5, 9}, {1, 2, 3}, {1, 4, 7}, {2, 5, 8},
                              {3, 6, 9}, {3, 5, 7}, {4, 5, 6}, {7, 8, 9}};

  for (int i = 0; i < 8; i++) {
    int p1 = possibilidades[i][0];
    int p2 = possibilidades[i][1];
    int p3 = possibilidades[i][2];

    Node *node1 = head;
    Node *node2 = head;
    Node *node3 = head;

    while (node1->index != p1) {
      node1 = node1->prox;
    }

    while (node2->index != p2) {
      node2 = node2->prox;
    }

    while (node3->index != p3) {
      node3 = node3->prox;
    }

    if (node1->valor[0] == node2->valor[0] &&
        node2->valor[0] == node3->valor[0] && node1->valor[0] != ' ') {
      if (node1->valor[0] == 'X') {
        vitoria = 1;
        break;
      } else {
        vitoria = 2;
        break;
      }
    }
  }

  return vitoria;
}

void exibirVencedores() {

  FILE *arquivo = fopen("ranking.txt", "r");

  if (arquivo == NULL) {

    printf("Erro ao abrir o arquivo ranking.txt\n");
    return;
  }

  printf(ANSI_COLOR_GOLD
         "\n==== Lista de Vencedor do Evento ====\n" ANSI_COLOR_RESET);

  char jogadorNome[50];
  int jogadorPontuacao;
  bool primeiro = true;
  bool segundo = false;
  bool terceiro = false;

  // Lê as informações de cada jogador do arquivo
  while (fscanf(arquivo, "%s %d", jogadorNome, &jogadorPontuacao) != EOF) {

    if (primeiro) {
      printf(ANSI_COLOR_GOLD "Nome: %s, Pontuação: %d\n" ANSI_COLOR_RESET,
             jogadorNome, jogadorPontuacao);
      primeiro = false;
      segundo = true;
    }

    else if (segundo) {
      printf(ANSI_COLOR_SILVER "Nome: %s, Pontuação: %d\n" ANSI_COLOR_RESET,
             jogadorNome, jogadorPontuacao);
      segundo = false;
      terceiro = true;
    }

    else if (terceiro) {
      printf(ANSI_COLOR_COPPER "Nome: %s, Pontuação: %d\n" ANSI_COLOR_RESET,
             jogadorNome, jogadorPontuacao);
      terceiro = false;
    }

    else {

      // Exibe o nome e a pontuação do jogador
      printf(ANSI_COLOR_BLUE "Nome: %s, Pontuação: %d\n" ANSI_COLOR_RESET,
             jogadorNome, jogadorPontuacao);
    }
  }

  fclose(arquivo);
  printf(ANSI_COLOR_GOLD
         "======================================\n\n" ANSI_COLOR_RESET);

  int tecla;

  printf("Digite 4 para voltar: ");
  scanf("%d", &tecla);

  while (tecla != 4) {

    printf("Digite 4 para voltar: ");
    scanf("%d", &tecla);
  }

  system("clear");
}

void bubblesortNoArquivo() {

  FILE *arquivo = fopen("ranking.txt", "r+");

  if (arquivo == NULL) {

    printf("Erro ao abrir o arquivo ranking.txt\n");
    return;
  }

  char jogadorNome[50];
  int jogadorPontuacao;

  // Matriz para armazenar nomes dos jogadores
  // Array para armazenar pontuações dos jogadores
  // Contador de jogadores lidos do arquivo
  char vencedores[100][50];
  int pontuacoes[100];
  int numJogadores = 0;

  int swapped;
  int i;

  // Lê os nomes e pontuações dos jogadores do arquivo
  while (fscanf(arquivo, "%s %d", jogadorNome, &jogadorPontuacao) != EOF) {

    strcpy(vencedores[numJogadores], jogadorNome);
    pontuacoes[numJogadores] = jogadorPontuacao;
    numJogadores++;
  }

  // Algoritmo de ordenação bubble sort
  do {

    swapped = 0;

    for (i = 0; i < numJogadores - 1; i++) {

      if (pontuacoes[i] < pontuacoes[i + 1]) {

        int tempPontuacao = pontuacoes[i];
        pontuacoes[i] = pontuacoes[i + 1];
        pontuacoes[i + 1] = tempPontuacao;

        char tempNome[50];
        strcpy(tempNome, vencedores[i]);
        strcpy(vencedores[i], vencedores[i + 1]);
        strcpy(vencedores[i + 1], tempNome);

        swapped = 1;
      }
    }

  } while (swapped);

  rewind(arquivo);

  // Escreve os nomes e pontuações ordenados de volta no arquivo
  for (i = 0; i < numJogadores; i++) {

    fprintf(arquivo, "%s %d\n", vencedores[i], pontuacoes[i]);
  }

  fclose(arquivo);
}

void FuncaoRanking(const char *nome, int pontuacao, int printar) {

  // Abre o arquivo "ranking.txt" para leitura
  // Abre um arquivo temporário para escrita

  FILE *arquivo = fopen("ranking.txt", "r");
  FILE *temp = fopen("temp.txt", "w");

  if (arquivo == NULL || temp == NULL) {

    printf("Erro ao abrir os arquivos ranking.txt\n");
    return;
  }

  char buffer[100];
  int encontrado = 0;

  // Lê cada linha do arquivo "ranking.txt"
  while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {

    char jogadorNome[50];
    int jogadorPontuacao;

    // Analisa o conteúdo da linha para extrair nome e pontuação
    if (sscanf(buffer, "%s %d", jogadorNome, &jogadorPontuacao) == 2) {

      // Verifica se o nome na linha atual coincide com o nome fornecido
      if (strcmp(jogadorNome, nome) == 0) {

        encontrado = 1;
        jogadorPontuacao += pontuacao;
        fprintf(temp, "%s %d\n", nome, jogadorPontuacao);
      }

      else {

        // Se o nome não coincide, escreve a linha atual no arquivo temporário
        fprintf(temp, "%s %d\n", jogadorNome, jogadorPontuacao);
      }
    }
  }

  fclose(arquivo);
  fclose(temp);

  remove("ranking.txt");
  rename("temp.txt", "ranking.txt");

  // Se o jogador não foi encontrado no arquivo, adiciona uma nova entrada
  if (!encontrado) {

    arquivo = fopen("ranking.txt", "a");

    if (arquivo == NULL) {

      printf("Erro ao abrir o arquivo para adicionar novo jogador\n");
      return;
    }

    fprintf(arquivo, "%s %d\n", nome, pontuacao);
    fclose(arquivo);
  }
  if (printar) {
    printf("Pontuação registrada com sucesso.\n");
  }
}

int PosicaoOcupada(Node *head, int index) {
  Node *current = head;

  while (current != NULL) {
    if (current->index == index) {
      if (current->valor[0] != ' ') {
        return 1;
      } else {
        return 0;
      }
    }
    current = current->prox;
  }

  return 1;
}

void liberarLista(Node **head, Node **tail) {
  if (*head == NULL) {

    return;
  }

  Node *atual = *head;
  Node *proximo;

  do {
    proximo = atual->prox;
    free(atual);
    atual = proximo;
  } while (atual != *head);

  *head = *tail = NULL;

  // printf("Free finalizado\n");
}
