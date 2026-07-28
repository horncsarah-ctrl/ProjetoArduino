const int dezA = 8; 
const int dezB = 9; 
const int dezC = 10; 
const int dezD = 11; 
const int uniA = 4; 
const int uniB = 5; 
const int uniC = 6; 
const int uniD = 7; 
const int botao = 3;

int pontuacao_enquanto_assopra, pontuacao_maxima_assoprado = 0, recorde_geral; 
int dezenas, unidades; 
int f_assoprando = 0; // flag se tem alguém assoprando ou não
int modo_display = 0; // 0 = mostra o sopro atual | 1 = mostra o recorde geral 
int estadoBotao;
int estadoAnteriorBotao;
int count_reset = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(dezA, OUTPUT); 
  pinMode(dezB, OUTPUT); 
  pinMode(dezC, OUTPUT); 
  pinMode(dezD, OUTPUT); 
  pinMode(uniA, OUTPUT);
  pinMode(uniB, OUTPUT); 
  pinMode(uniC, OUTPUT); 
  pinMode(uniD, OUTPUT); 
  pinMode(botao, INPUT); 
  pinMode(2, OUTPUT);
}

void loop()
{
  digitalWrite(2, modo_display);
  estadoBotao = digitalRead(botao);
  pontuacao_enquanto_assopra = map(analogRead(A0), 0, 1023, 0, 99);
  
  if(pontuacao_enquanto_assopra <=10){ 
    f_assoprando = 0;
  } else {
    f_assoprando = 1;
  }

  if(count_reset) count_reset--;  

  if(pontuacao_enquanto_assopra > 0 && f_assoprando == 0){
    f_assoprando = 1;
    pontuacao_maxima_assoprado = 0;  
  }

  if (!count_reset){
    f_assoprando = 1;
    pontuacao_maxima_assoprado = 0;  
  }

  if (estadoBotao != estadoAnteriorBotao && estadoBotao == 1) {
    modo_display = !modo_display;
  } 

  estadoAnteriorBotao = estadoBotao;

  if (f_assoprando == 1){
    if (pontuacao_enquanto_assopra > pontuacao_maxima_assoprado ){
      pontuacao_maxima_assoprado = pontuacao_enquanto_assopra;
      count_reset = 10;
    }
    if (pontuacao_maxima_assoprado >  recorde_geral){
      recorde_geral = pontuacao_maxima_assoprado;
    }
  }

  if (modo_display == 1){
    dezenas = recorde_geral / 10;   
    unidades = recorde_geral % 10;
  } else{
    dezenas = pontuacao_maxima_assoprado / 10;   
    unidades = pontuacao_maxima_assoprado % 10;
    delay(200);
  }

  digitalWrite(dezA,bitRead(dezenas,0));
  digitalWrite(dezB,bitRead(dezenas,1));
  digitalWrite(dezC,bitRead(dezenas,2));
  digitalWrite(dezD,bitRead(dezenas,3));
  digitalWrite(uniA,bitRead(unidades,0));
  digitalWrite(uniB,bitRead(unidades,1));
  digitalWrite(uniC,bitRead(unidades,2));
  digitalWrite(uniD,bitRead(unidades,3));
  Serial.println(pontuacao_maxima_assoprado);
  delay(10);
} 