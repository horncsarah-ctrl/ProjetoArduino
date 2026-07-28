// DECLARAÇÃO DAS PORTAS
const int led1 = 2;
const int led2 = 3;
const int led3 = 4;
const int led4 = 5;
const int led5 = 6;
const int buzzer = 7;
const int trig = 8;
const int echo = 9; 
const int botaoIniciar = 10;
const int ldr = A0;
const int potenc = A1;

// DECLARANDO AS FASES DO JOGO
int faseAtual = 0;
const int faseInicio = 0; 
const int fase1 = 1;
const int fase2 = 2;
const int fase3 = 3;
const int vitoria = 4;

// VARIÁVEIS PARA O FUNCIONAMENTO DO JOGO
int distanciaMao = 0;
int objetivoDistancia = 0;
int objetivoPotenciometro = 0;
int unidades = 5;
int valorLDR = 0;
int valorPotenciometro = 0;
bool estavaNoVerde = false;
bool estavaNoNumeroCerto = false;
unsigned long tempoInicialNoVerde = 0;
unsigned long tempoInicioPotenciometro = 0;
int diferencaAlvoPotenciometro = 0;
int diferencaAlvo = 0;
int length = 33; // the number of notes
char notes[] = "i iyci ipoi iyci ipozgd zgr zgc tc "; // a space represents a rest
int beats[] = {2,1,3,2,2,2,1,3,2,2,2,1,3,2,2,2,1,3,2,2,1,1,5,4,1,1,5,4,1,1,5,4,1,1,4};
int tempo = 150;

// VARIÁVEIS DA MÚSICA SEM TRAVAR O JOGO
int notaAtual = 0;
unsigned long tempoInicioNota = 0;
unsigned long duracaoNotaAtual = 0;
bool notaTocando = false;

void setup() {
  Serial.begin(9600); 

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(ldr, INPUT);
  pinMode(potenc, INPUT); 
  pinMode(botaoIniciar, INPUT); 

  randomSeed(analogRead(A3));
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(buzzer, HIGH);
    delayMicroseconds(tone);
    digitalWrite(buzzer, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = {'p','o','i','u','y','t','c','r','d','e','f','w','g','a','z','b','q', 'C', 'D','E','F','l','G','A','k','B'};
  int tones[] = {2864,2702,2551,2273,2146,2024,1915,1805,1700,1519,1432,1351,1275,1136,1073,1014,956,919,850,760,716,676,638,568,536,507};  
  
  for (int i = 0; i < 26; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

// Nova versão da música usando millis() para não travar as fases
void gerenciarMusicaDeFundo() {
  // Só toca música se o jogo já tiver começado (Fase 1, 2 ou 3)
  if (faseAtual != fase1 && faseAtual != fase2 && faseAtual != fase3) {
    notaTocando = false;
    return; 
  }

  if (!notaTocando) {
    duracaoNotaAtual = beats[notaAtual] * tempo;
    tempoInicioNota = millis();
    notaTocando = true;

    if (notes[notaAtual] != ' ') {
      // Toca a nota atual (como playTone usa delay interno, o som sai correto)
      playNote(notes[notaAtual], duracaoNotaAtual);
    }
  }

  // Verifica se o tempo da nota atual (ou do silêncio) já acabou
  if (millis() - tempoInicioNota >= duracaoNotaAtual) {
    notaTocando = false;
    notaAtual++; // Passa para a próxima nota
    
    // Se a música acabar, recomeça do início (Loop)
    if (notaAtual >= length) {
      notaAtual = 0;
    }
  }
}

// Fase 1 = O sensor ultrassonico
int faseLeituraMao() { 
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH, 30000); 
  return (duracao * 0.0343) / 2; 
}

void objetivoDistanciaMao(){
  objetivoDistancia = random(8, 23); 
}

void fase1DistanciaDaMao() {
  distanciaMao = faseLeituraMao();
  diferencaAlvo = distanciaMao - objetivoDistancia;

  digitalWrite (led1, LOW);
  digitalWrite (led2, LOW);
  digitalWrite (led3, LOW);
  digitalWrite (led4, LOW);
  digitalWrite (led5, LOW);

  if (diferencaAlvo < -12.0){
    digitalWrite(led1, HIGH);
    estavaNoVerde = false;
  }
  else if (diferencaAlvo >= -12.0 && diferencaAlvo <= -3.1){
    digitalWrite(led2, HIGH);
    estavaNoVerde = false;
  }
  else if (diferencaAlvo >= -3.0 && diferencaAlvo <= 3.0){
    digitalWrite(led3, HIGH);
    
    if (estavaNoVerde == false){
      estavaNoVerde = true;
      tempoInicialNoVerde = millis();
    }
    else {
      unsigned long tempoPassado = millis() - tempoInicialNoVerde;
      
      if (tempoPassado >= 2000) { 
        for (int i = 0; i < 5; i++){
          digitalWrite(led3, LOW);
          delay(100);
          digitalWrite(led3, HIGH);
          delay(100);
        }
        objetivoPosicaoPotenciometro(); 
        faseAtual = fase2;
        estavaNoVerde = false;
      }
    }
  }
  else if (diferencaAlvo >= 3.1 && diferencaAlvo <= 12.0) {
    digitalWrite(led4, HIGH);
    estavaNoVerde = false;
  }
  else {
    digitalWrite (led5, HIGH);
    estavaNoVerde = false;
  }
}

// Fase 2 = O Fotorresistor (LDR)
void fase2Iluminacao(){
  valorLDR = analogRead(ldr);
  Serial.println(valorLDR);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led4, LOW);

  if (valorLDR <= 550) {
    for (int i = 0; i < 5; i++){
      digitalWrite(led3, LOW);
      delay(100);
      digitalWrite(led3, HIGH);
      delay(100);
    }
    faseAtual = fase3; 
  }
  else {
    digitalWrite(led5, HIGH);
    digitalWrite(led3, LOW); 
  }
}

// Fase 3 = O Potenciometro 
void objetivoPosicaoPotenciometro(){  
  objetivoPotenciometro = random(100, 1000); 
}

void faseLeituraPotenciometro(){
  valorPotenciometro = analogRead (potenc);
}

void fase3PosicaoPotenciometro(){
  faseLeituraPotenciometro();
  diferencaAlvoPotenciometro = valorPotenciometro - objetivoPotenciometro;

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);

  if (diferencaAlvoPotenciometro < -250){
    digitalWrite(led1, HIGH);
    estavaNoNumeroCerto = false;
  }
  else if (diferencaAlvoPotenciometro >= -250 && diferencaAlvoPotenciometro <= -31) {
    digitalWrite(led2, HIGH);
    estavaNoNumeroCerto = false; 
  }
  else if (diferencaAlvoPotenciometro >= -30 && diferencaAlvoPotenciometro <= 30) { 
    digitalWrite(led3, HIGH);
    
    if (estavaNoNumeroCerto == false) {
      estavaNoNumeroCerto = true;
      tempoInicioPotenciometro  = millis();
    } 
    else {
      unsigned long tempoPassadoPotenciometro = millis() - tempoInicioPotenciometro;
 
      if (tempoPassadoPotenciometro >= 2000) { 
         for(int i = 0; i < 5; i++){ 
          digitalWrite(led3, LOW); 
          delay(100); 
          digitalWrite(led3, HIGH); 
          delay(100); 
        }
        faseAtual = vitoria; 
        estavaNoNumeroCerto = false;
      }
    }
  }
  else if (diferencaAlvoPotenciometro >= 31 && diferencaAlvoPotenciometro <= 250) { 
    digitalWrite(led4, HIGH);
    estavaNoNumeroCerto = false;
  }
  else {
    digitalWrite(led5, HIGH);
    estavaNoNumeroCerto = false;
  }
}

//Fazendo o jogo funcionar
void loop() {
  // CHAMA A MÚSICA CONSTANTEMENTE NO LOOP
  gerenciarMusicaDeFundo();

  switch (faseAtual) {
    
    case faseInicio:
      notaAtual = 0; // Reinicia a música para começar do início na próxima partida
      Serial.println("Aguardando pressionar o botão para iniciar...");
      
      if (digitalRead(botaoIniciar) == HIGH) {
        Serial.println("Botão pressionado! Sorteando objetivos...");
        
        objetivoDistanciaMao();
        Serial.print("Alvo da Fase 1 (Distancia): ");
        Serial.println(objetivoDistancia);

        delay(1000); 
        faseAtual = fase1; 
      }
      break;

    case fase1:
      fase1DistanciaDaMao(); 
      break;
    
    case fase2:
      Serial.println("Fase 2 em andamento ...");
      fase2Iluminacao(); 
      break;
      
    case fase3:
      Serial.println("Fase 3 em andamento ...");
      fase3PosicaoPotenciometro(); 
      break;
      
    case vitoria:
      Serial.println("Você Venceu! Piscando os LEDs...");

      for (int i = 0; i < 5; i++) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
        digitalWrite(led4, HIGH);
        digitalWrite(led5, HIGH);
        delay(300); 

        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        digitalWrite(led4, LOW);
        digitalWrite(led5, LOW);
        delay(300); 
      }

      Serial.println("Fim da comemoração. Reiniciando o jogo...");
      delay(2000); 
      
      faseAtual = faseInicio; 
      break; 
  }
}