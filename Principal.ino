#include <U8glib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
U8GLIB_ST7920_128X64_1X u8g(6, 5, 4, 7);

#define SENHA_MESARIO 7836
#define LISTAR_TODOS_CANDIDATOS '1'
#define APURAR_RESULTADO_ELEICAO '2'
#define INICIAR_NOVA_ELEICAO '3'
#define SAIR_MODO_MESARIO '4'

typedef struct Candidato {
    int numero;
    char* partido;
    char* nome;
    int qtdeVotos;
} Candidato;

void limpaTela();
void listarCandidatos();
void imprimirCandidato(Candidato* candidato, bool exibirVotos, int posY);
bool validaModoMesario(int voto);
void realizaVotacao(int voto);
void inciarNovaEleicao();
void apurarResultadoEleicao();
void ligaBuzzer();

String voto = "";
String estadoVotacao = "Insira seu voto: ";
int valor_digitado = 0;
int pinosLinhas[] = {13, 12, 11, 10};
int pinosColunas[] = {9, 8, 3, 2};
char teclas[4][4] = { {'1', '2', '3', 'A'},
                      {'4', '5', '6', 'B'},
                      {'7', '8', '9', 'C'},
                      {'*', '0', '#', 'D'}};

Candidato candidatos[] = {
  {1, "PSDB", "Alckinm"  , 0},
  {2, "PT"  , "Lula"     , 0},
  {3, "PL"  , "Bolsonaro", 0},
  {4, "PSOL", "Boulos"   , 0},
  {9999, "" , "NULO"     , 0}
};

bool votoConfirmado = false;

void setup() {
  pinMode(A0,OUTPUT);
  //Liga todas as linhas 
  for(int nL = 0; nL <= 3; nL++){
    pinMode(pinosLinhas[nL], OUTPUT);
    digitalWrite(pinosLinhas[nL], HIGH);
  }

  //Ativa o evento de escuta das colunas
  for (int nC = 0; nC <= 3; nC++){
    pinMode(pinosColunas[nC], INPUT_PULLUP);
  }

  u8g.setFont(u8g_font_6x10);
  u8g.setContrast(100); //Ajusta nitidez
  u8g.setFontPosBaseline();  
  u8g.setDefaultForegroundColor();  
  u8g.setRot180();
  if (u8g.getMode()== U8G_MODE_R3G3B2){  
     u8g.setColorIndex(255);
  }
  else if (u8g.getMode()==U8G_MODE_GRAY2BIT){  
     u8g.setColorIndex(1);
  }
  else if (u8g.getMode()==U8G_MODE_BW){
     u8g.setColorIndex(1);
  }

  Serial.begin(9600); 
}


void loop() {
  
  if(lerVotoTeclado() == true){
    atualizaDisplay(estadoVotacao, voto);
  }
  
  //Ajusta constraste 
  u8g.setContrast(analogRead(A0) / 4); 
}

bool lerVotoTeclado(){
 for (int nL = 0; nL <= 3; nL++){
    digitalWrite(pinosLinhas[nL], LOW);

    for (int nC = 0; nC <= 3; nC++){
      if (digitalRead(pinosColunas[nC]) == LOW){
        if (teclas[nL][nC] == 'C' && voto != "") { // Tecla "C" pressionada
          processaVoto();
          return true; //Confirma Voto 
        } else { 
          voto += teclas[nL][nC]; //Concatena valores lidos
          false;
        }
        while(digitalRead(pinosColunas[nC]) == LOW) {} // Aguarda o botão ser solto
      }
    }
    digitalWrite(pinosLinhas[nL], HIGH);
  }
  delay(10);
}

void atualizaDisplay(String label, String digito){
  u8g.firstPage();
  do {
    if(label == "Voto confirmado: ") {
        u8g.drawStr(5, 5, label.c_str()); 
        u8g.drawStr(5, 21, digito.c_str());
        u8g.drawStr(5, 37, "Voto confirmado!"); 
    }else if (digito == "") {
      u8g.drawStr(5, 20, label.c_str()); 
    }else{
      u8g.drawStr(5, 20, label.c_str()); 
      u8g.drawStr(5, 35, digito.c_str());
    }
  } while (u8g.nextPage()); 
}

void processaVoto() {
  int votoInt = atoi(voto.c_str());
  bool modoMesario = validaModoMesario(votoInt);

  if(modoMesario == true){
    limpaTela();
    while(modoMesario){
      char opcaoMenu;
      telaMenuModoMesario();
      for (int nL = 0; nL <= 3; nL++){
        digitalWrite(pinosLinhas[nL], LOW);
        for (int nC = 0; nC <= 3; nC++){
          if (digitalRead(pinosColunas[nC]) == LOW){

            opcaoMenu = teclas[nL][nC]; 
            Serial.print("opcaoMenu: ");
            Serial.println(opcaoMenu); 

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
             
            while(digitalRead(pinosColunas[nC]) == LOW) {} // Aguarda o botão ser solto
          }
        }
        digitalWrite(pinosLinhas[nL], HIGH);
      }
      delay(10);
    }
  } else {
    realizaVotacao(votoInt);
    voto= "";
    estadoVotacao = "Insira seu voto:";
    atualizaDisplay(estadoVotacao, voto);
  }
}

void telaMenuModoMesario(){
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_5x8);
    u8g.drawStr(2, 20, "1-Ver candidatos");
    u8g.drawStr(2, 30, "2-Apurar resultado");
    u8g.drawStr(2, 40, "3-Iniciar nova eleicao");
    u8g.drawStr(2, 50, "4-Sair do modo mesario");
  } while (u8g.nextPage());
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
    atualizaDisplay("Indefinido. Nenhum vencedor", "");
  }
  else if (qtdVencedores == 1) {
    imprimirCandidato(&vencedores[0], true, "Vencedor no 1o turno", 20);
  } else {
    for (int i = 0; i < qtdVencedores; i++) {
      imprimirCandidato(&vencedores[i], true, "Decisão em 2o turno", 20);
    }
  }
  delay(4000);
  //Libera espaço alocado
  free(vencedores);
}

void inciarNovaEleicao() {
  int qtdeCandidatos = sizeof(candidatos) / sizeof(Candidato);
  limpaTela();
  for (int i = 0; i < qtdeCandidatos; i++) {
    candidatos[i].qtdeVotos = 0;
  }
  atualizaDisplay("Nova eleição inciada!", "");
}

void realizaVotacao(int voto) {
  bool votoRealizado = false;
  for (int i = 0; i < voto; i++) {
    if (candidatos[i].numero == voto) {
      limpaTela();
      imprimirConfirmacaoVoto(&candidatos[i]);
      candidatos[i].qtdeVotos++;
      votoRealizado = true;
      //Liga Buzzer
      ligaBuzzer();
    }
  }
  if (votoRealizado == false) {
    limpaTela();
    int qtdeCandidatos = sizeof(candidatos) / sizeof(Candidato);
    candidatos[qtdeCandidatos - 1 ].qtdeVotos++;
    atualizaDisplay("Voto anulado!", "");
  }
  delay(3000);
}

void ligaBuzzer(){
  digitalWrite(A0, HIGH);//liga o buzzer   
  delay(200); //fica com ele ligado durante 2000 milissegundos  
  digitalWrite(A0, LOW); //desliga o buzzer   
  delay(500);
  digitalWrite(A0, HIGH);//liga o buzzer   
  delay(100); //fica com ele ligado durante 2000 milissegundos  
  digitalWrite(A0, LOW); //desliga o buzzer   
  delay(300);
  digitalWrite(A0, HIGH);//liga o buzzer   
  delay(200); //fica com ele ligado durante 2000 milissegundos  
  digitalWrite(A0, LOW); //desliga o buzzer   
  delay(100);
}

void imprimirCandidato(Candidato* candidato, bool exibirVotos, String label, int posY) {
  char str[100];
  if(exibirVotos) {
   char str[100];
   snprintf(str, 100, "%s (%d) - %s - Votos:", candidato->nome, candidato->numero, candidato->partido, candidato->qtdeVotos);
  }else{
   char str[100];
   snprintf(str, 100, "%s (%d) - %s", candidato->nome, candidato->numero, candidato->partido);
  }

  u8g.firstPage();
  do {
      u8g.setFontPosBaseline();  
      u8g.setFontPosBottom();
      u8g.drawStr(5, posY, label.c_str()); 
      u8g.drawStr(5, posY + 10, str); 
  }while (u8g.nextPage()); 
}

void imprimirConfirmacaoVoto(Candidato* candidato) {
   char str[100];
   snprintf(str, 100, "%s (%d) - %s", candidato->nome, candidato->numero, candidato->partido);
  
  u8g.firstPage();
  do {
      u8g.drawStr(5, 20, "Voto confirmado!"); 
      u8g.drawStr(5, 30, str); 
  }while (u8g.nextPage()); 
}

void listarCandidatos(){
  u8g.firstPage();
  do {
    percorreCandidatos();
  } while (u8g.nextPage());
  delay(4000);
}

void percorreCandidatos() {
  int qtdeCandidatos = sizeof(candidatos) / sizeof(Candidato);
  int x = 3;
  int y = 5;
  char buffer[100];

  // Loop para percorrer a lista de candidatos
  for (int i = 0; i < qtdeCandidatos; i++) {
    sprintf(buffer, "%s (%d) - %s [%d]", candidatos[i].nome, candidatos[i].numero,
            candidatos[i].partido, candidatos[i].qtdeVotos);
    u8g.setFontPosBaseline();  
    u8g.setFontPosBottom();
    u8g.setFontPosTop();
    u8g.setFont(u8g_font_5x8);
    u8g.drawStr(x, y, buffer);
    y += 12;
  }
}

bool validaModoMesario(int voto) {
  bool modoMesario = false;
  if (voto == SENHA_MESARIO){
    modoMesario = true;
  }
  return modoMesario;
}

void limpaTela() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void exportar_candidatos() {
    int tamanho = sizeof(candidatos) / sizeof(Candidato);
    FILE *arquivo = fopen("candidatos.txt", "w"); // abre o arquivo para escrita
    if (arquivo == NULL) { // verifica se houve erro ao abrir o arquivo
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    // escreve os dados dos candidatos no arquivo
    for (int i = 0; i < tamanho; i++) {
        fprintf(arquivo, "Nome: %s\n", candidatos[i].nome);
        fprintf(arquivo, "Partido: %s\n", candidatos[i].partido);
        fprintf(arquivo, "Número: %d\n", candidatos[i].numero);
        fprintf(arquivo, "\n");
    }

    fclose(arquivo); // fecha o arquivo
    printf("Dados dos candidatos exportados com sucesso para o arquivo candidatos.txt.\n");
}