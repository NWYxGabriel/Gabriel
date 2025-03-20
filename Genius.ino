#include <EEPROM.h>

#define NOTE_D4  294
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_A5  880

// Constantes e variáveis globais
int tons[4] = { NOTE_A5, NOTE_A4, NOTE_G4, NOTE_D4 };
int sequencia[100] = {};
int rodada_atual = 0;
int passo_atual_na_sequencia = 0;

const int ACERTOS_PARA_VENCER = 16;
#define ENDERECO_VITORIAS 0

int vitorias = 0;

int pinoAudio = 9;
int pinosLeds[4] = { 5, 4, 3, 2 };
int pinosBotoes[4] = { 10, 11, 12, 13 };

int botao_pressionado = 0;
bool perdeu_o_jogo = false;

// Configuração inicial do Arduino
void setup() {
  Serial.begin(9600);
  for (int i = 0; i <= 3; i++) {
    pinMode(pinosLeds[i], OUTPUT);
    pinMode(pinosBotoes[i], INPUT);
  }
  pinMode(pinoAudio, OUTPUT);
  randomSeed(analogRead(0));

  vitorias = lerVitorias();
  Serial.print("Vitórias registradas: ");
  Serial.println(vitorias);
}

// Loop principal do jogo
void loop() {
  if (perdeu_o_jogo) {
    resetarJogo();
  }
  
  if (rodada_atual == 0) {
    tocarSomDeInicio();
    delay(500);
  }

  proximaRodada();
  reproduzirSequencia();
  aguardarJogador();

  if (rodada_atual >= ACERTOS_PARA_VENCER) {
    venceuJogo();
  }

  delay(1000);
}

// Controle do jogo
void resetarJogo() {
  rodada_atual = 0;
  passo_atual_na_sequencia = 0;
  perdeu_o_jogo = false;
}

void proximaRodada() {
  int numero_sorteado = random(0, 4);
  sequencia[rodada_atual++] = numero_sorteado;
}

void reproduzirSequencia() {
  for (int i = 0; i < rodada_atual; i++) {
    tone(pinoAudio, tons[sequencia[i]]);
    digitalWrite(pinosLeds[sequencia[i]], HIGH);
    delay(500);
    noTone(pinoAudio);
    digitalWrite(pinosLeds[sequencia[i]], LOW);
    delay(100);
  }
  noTone(pinoAudio);
}

void aguardarJogador() {
  for (int i = 0; i < rodada_atual; i++) {
    aguardarJogada();
    verificarJogada();
    if (perdeu_o_jogo) {
      break;
    }
    passo_atual_na_sequencia++;
  }
  passo_atual_na_sequencia = 0;
}

// Interação do jogador
void aguardarJogada() {
  boolean jogada_efetuada = false;
  while (!jogada_efetuada) {
    for (int i = 0; i <= 3; i++) {
      if (digitalRead(pinosBotoes[i]) == HIGH) {
        botao_pressionado = i;
        tone(pinoAudio, tons[i]);
        digitalWrite(pinosLeds[i], HIGH);
        delay(300);
        digitalWrite(pinosLeds[i], LOW);
        noTone(pinoAudio);
        jogada_efetuada = true;
      }
    }
    delay(10);
  }
}

void verificarJogada() {
  if (sequencia[passo_atual_na_sequencia] != botao_pressionado) {
    perdeuJogo();
  }
}

// Feedback ao jogador
void perdeuJogo() {
  for (int i = 0; i <= 3; i++) {
    tone(pinoAudio, tons[i]);
    digitalWrite(pinosLeds[i], HIGH);
    delay(200);
    digitalWrite(pinosLeds[i], LOW);
    noTone(pinoAudio);
  }
  perdeu_o_jogo = true;
}

void venceuJogo() {
  vitorias++;
  salvarVitorias(vitorias);

  Serial.print("Vitória registrada! Total: ");
  Serial.println(vitorias);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j <= 3; j++) {
      digitalWrite(pinosLeds[j], HIGH);
    }
    tone(pinoAudio, NOTE_A5);
    delay(500);
    for (int j = 0; j <= 3; j++) {
      digitalWrite(pinosLeds[j], LOW);
    }
    noTone(pinoAudio);
    delay(500);
  }
  resetarJogo();
}

// Funções auxiliares
void tocarSomDeInicio() {
  tone(pinoAudio, tons[0]);
  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[i], HIGH);
  }
  delay(500);
  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[i], LOW);
  }
  noTone(pinoAudio);
}

void salvarVitorias(int vitorias) {
  EEPROM.write(ENDERECO_VITORIAS, vitorias);
}

int lerVitorias() {
  return EEPROM.read(ENDERECO_VITORIAS);
}
