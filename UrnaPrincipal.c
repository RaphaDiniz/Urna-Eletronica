#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SENHA_MESARIO 7836
#define LISTAR_TODOS_CANDIDATOS 1
#define APURAR_RESULTADO_ELEICAO 2
#define INICIAR_NOVA_ELEICAO 3
#define SAIR_MODO_MESARIO 4

typedef struct Candidato {
    int numero;
    char* partido;
    char* nome;
    int qtdeVotos;
} Candidato;

void listarCandidatos();
void imprimirCandidato(Candidato* candidato, bool exibirVotos);
bool validaModoMesario(int voto);
void limpaTela();
void realizaVotacao(int voto);
void inciarNovaEleicao();
void apurarResultadoEleicao();

Candidato candidatos[] = {
  {1, "PSDB", "Geraldo Alckinm" , 0},
  {2, "PT"  , "Lula da Silva"   , 0},
  {3, "PL"  , "Jair Bolsonaro"  , 0},
  {4, "PSOL", "Maria do Rosário", 0},
  {9999, "" , "NULO"            , 0}
};

int main() {
  int voto;
  while (true) {
    printf("\n\n\n>>>> Informe seu voto: ");
    scanf("%d", &voto);

    bool modoMesario = validaModoMesario(voto);
    if(modoMesario == true){
      limpaTela();
      while(modoMesario){
        int opcaoMenu;
        printf("\n 1 - Listar todos os candidatos \n");
        printf(" 2 - Apurar resultado eleitoral \n");
        printf(" 3 - Iniciar nova eleicao \n");
        printf(" 4 - Sair do modo mesario \n");
        scanf("\n  %d", &opcaoMenu);

        limpaTela();
        if(opcaoMenu == LISTAR_TODOS_CANDIDATOS){
          listarCandidatos();
        }else if(opcaoMenu == APURAR_RESULTADO_ELEICAO){
          apurarResultadoEleicao();
        }else if(opcaoMenu == INICIAR_NOVA_ELEICAO){
          inciarNovaEleicao();
        }else if(opcaoMenu == SAIR_MODO_MESARIO){
          modoMesario = false;
        }
      }
    } else {
      realizaVotacao(voto);
    }
  }
  return 0;
}

void apurarResultadoEleicao() {
  int qtdeCandidatos = sizeof(candidatos) / sizeof(Candidato);
  Candidato* vencedores = malloc(qtdeCandidatos * sizeof(Candidato));
  int maiorQtdeVotos = -1;
  int qtdVencedores = 0;

  // percorre a lista de candidatos e verifica quem tem mais votos
  for (int i = 0; i < qtdeCandidatos - 1; i++) {
    int qtdeVotos = candidatos[i].qtdeVotos;
    if (qtdeVotos > maiorQtdeVotos) {
      qtdVencedores = 0;
      vencedores[qtdVencedores] = candidatos[i];
      qtdVencedores++;
      maiorQtdeVotos = qtdeVotos;
    } else if (qtdeVotos == maiorQtdeVotos) {
      vencedores[qtdVencedores] = candidatos[i];
      qtdVencedores++;
    }
  }

  limpaTela();
  if (qtdVencedores == 0) {
    printf("Eleição indefinida. Nenhum candidato vencedor.\n\n");
  }
  else if (qtdVencedores == 1) {
    printf("Candidato vencedor no 1o turno:\n\n");
    imprimirCandidato(&vencedores[0], true);
  } else {
    printf("Empate tecnico. Decisão em 2o turno:\n\n");
    for (int i = 0; i < qtdVencedores; i++) {
      imprimirCandidato(&vencedores[i], true);
    }
  }

  //Libera espaço alocado
  free(vencedores);
}

void inciarNovaEleicao() {
  int qtdeCandidatos = sizeof(candidatos) / sizeof(Candidato);
  limpaTela();
  for (int i = 0; i < qtdeCandidatos; i++) {
    candidatos[i].qtdeVotos = 0;
  }
  printf("\nNova eleicao iniciada\n");
}

void realizaVotacao(int voto) {
  bool votoRealizado = false;
  for (int i = 0; i < voto; i++) {
    if (candidatos[i].numero == voto) {
      limpaTela();
      printf("\nVoce votou em:\n");
      printf("-------------------\n");
      imprimirCandidato(&candidatos[i], false);
      printf("-------------------\n");
      candidatos[i].qtdeVotos++;
      printf("Voto confirmado!\n");
      votoRealizado = true;
    }
  }
  if (votoRealizado == false) {
    limpaTela();
    printf("Voto anulado!\n");
  }
}

void imprimirCandidato(Candidato* candidato, bool exibirVotos) {
  if(exibirVotos) {
    printf(">> %s -  ( %d ) - [ %s ] Votos: %d\n", candidato->nome, candidato->numero, candidato->partido,
           candidato->qtdeVotos);
  }else{
    printf(">> %s -  ( %d ) - [ %s ]\n", candidato->nome, candidato->numero, candidato->partido);
  }
}

void listarCandidatos() {
  int qtdeCandidatos = sizeof(candidatos) / sizeof(Candidato);
  printf("\nLista dos candidatos:\n");
  for (int i = 0; i < qtdeCandidatos; i++) {
    imprimirCandidato(&candidatos[i], true);
  }
}

void limpaTela() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}


bool validaModoMesario(int voto) {
  bool modoMesario = false;
  if (voto == SENHA_MESARIO){
    modoMesario = true;
  }
  return modoMesario;
}
