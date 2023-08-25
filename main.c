#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO 16

struct familia
{
  char nome[TAMANHO];
  char parentesco[TAMANHO];
  int idade;
  struct familia *proximo;
};

char nome_arquivo[] = "familia.dat";

// Lê a entrada padrão, descarta a nova linha
char *entrada(void)
{
  static char buffer[TAMANHO];
  char *r;

  if (fgets(buffer, TAMANHO, stdin) == NULL)
  {
    fprintf(stderr, "Erro de entrada\n");
    exit(1);
  }

  buffer[strcspn(buffer, "\n")] = '\0'; // Remove a nova linha

  return buffer;
}

// Aloca uma nova estrutura
struct familia *alocar(void)
{
  struct familia *p;
  p = (struct familia *)malloc(sizeof(struct familia));

  if (p == NULL)
  {
    fprintf(stderr, "Incapaz de alocar memória\n");
    exit(1);
  }

  return p;
}

// Preenche a estrutura
void preencher(struct familia *s)
{
  printf("Nome: ");
  strcpy(s->nome, entrada());
  
  printf("Parentesco: ");
  strcpy(s->parentesco, entrada());

  printf("Idade: ");
  s->idade = atoi(entrada());

  putchar('\n');
}

// Exibe a lista
int exibir(struct familia *s)
{
  int contador = 0;

  if (s == NULL)
  {
    puts("Lista está vazia");
  }
  else
  {
    while (s != NULL)
    {
      printf("%d: %s, seu %s, tem %d anos\n",
             contador + 1, s->nome, s->parentesco, s->idade);

      s = s->proximo;
      contador++;
    }
  }

  putchar('\n');
  return contador;
}

// Salva a lista encadeada
void salvar(struct familia *s)
{
  FILE *fp;
  int registros = 0;

  if (s == NULL)
  {
    puts("Lista está vazia\n");
    return;
  }

  fp = fopen(nome_arquivo, "wb");
  if (fp == NULL)
  {
    fprintf(stderr, "Incapaz de criar %s\n\n", nome_arquivo);
    return;
  }

  while (s != NULL)
  {
    fwrite(s, sizeof(struct familia), 1, fp);
    s = s->proximo;
    registros++;
  }

  printf("%d registros salvos...\n", registros);
  fclose(fp);
  puts("Feito!\n");
}

// Lê a lista encadeada de um arquivo
struct familia *abrir(void)
{
  FILE *fp;
  struct familia *primeiro = NULL;
  struct familia *atual;
  struct familia *temp;
  int registros = 0;

  fp = fopen(nome_arquivo, "rb");
  if (fp == NULL)
  {
    fprintf(stderr, "Incapaz de ler de %s\n\n", nome_arquivo);
    return NULL;
  }

  while (!feof(fp))
  {
    temp = alocar();

    if (fread(temp, sizeof(struct familia), 1, fp) != 1)
    {
      free(temp);
      break;
    }

    temp->proximo = NULL;

    if (primeiro == NULL)
    {
      primeiro = temp;
      atual = primeiro;
    }
    else
    {
      atual->proximo = temp;
      atual = atual->proximo;
    }

    registros++;
  }

  fclose(fp);
  printf("%d registros lidos...\n", registros);
  puts("Feito!\n");

  return primeiro;
}

// Libera a memória alocada pela lista encadeada
void liberar(struct familia *s)
{
  struct familia *temp;

  while (s != NULL)
  {
    temp = s;
    s = s->proximo;
    free(temp);
  }
}

int main()
{
  struct familia *primeiro = NULL;
  struct familia *atual;
  struct familia *temp;
  int terminou = 0;
  char comando[TAMANHO];

  puts("Banco de dados da Família");

  while (!terminou)
  {
    atual = primeiro;

    puts("Menu:");
    puts("A - Adicionar registro");
    puts("D - Excluir registro");
    puts("L - Listar todos os registros");
    puts("O - Abrir");
    puts("S - Salvar");
    puts("Q - Sair");
    printf("Escolha: ");

    strcpy(comando, entrada());

    switch (comando[0])
    {
    case 'A':
    case 'a':
      temp = alocar();
      preencher(temp);

      if (primeiro == NULL)
      {
        primeiro = temp;
        atual = primeiro;
      }
      else
      {
        while (atual->proximo != NULL)
          atual = atual->proximo;

        atual->proximo = temp;
      }

      break;

    case 'D':
    case 'd':
      if (primeiro == NULL)
      {
        puts("Nada para excluir\n");
        break;
      }
      else
      {
        exibir(primeiro);
        printf("Remover registro (1 a %d): ", exibir(primeiro));
        int registro = atoi(entrada());

        if (registro < 1 || registro > exibir(primeiro))
        {
          puts("Valor fora do intervalo\n");
          break;
        }

        if (registro == 1)
        {
          temp = primeiro;
          primeiro = primeiro->proximo;
          free(temp);
          puts("Feito!\n");
        }
        else
        {
          atual = primeiro;
          struct familia *anterior = NULL;

          for (int i = 1; i < registro; i++)
          {
            anterior = atual;
            atual = atual->proximo;
          }

          anterior->proximo = atual->proximo;
          free(atual);
          puts("Excluído\n");
        }
      }

      break;

    case 'L':
    case 'l':
      exibir(primeiro);
      break;

    case 'O':
    case 'o':
      liberar(primeiro);
      primeiro = abrir();
      break;

    case 'S':
    case 's':
      salvar(primeiro);
      break;

    case 'Q':
    case 'q':
      terminou = 1;
      puts("Tchau!");
      break;

    default:
      puts("Comando não reconhecido");
    }
  }

  liberar(primeiro);
  return 0;
}
