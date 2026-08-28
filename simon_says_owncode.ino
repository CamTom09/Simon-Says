// Definiendo pines de leds
#define ledBlue 4
#define ledGreen 6
#define ledYellow 8
#define ledRed 10

// Definiendo botones
#define btnBlue 5
#define btnGreen 7
#define btnYellow 9
#define btnRed 11

#define pinBuzzer 3
const int ledsTones[4] = {262, 330, 392, 523};

// Listas para los colores y botones
int colori[] = {ledBlue, ledGreen, ledYellow, ledRed};
int btni[] = {btnBlue, btnGreen, btnYellow, btnRed};

// Nivel máximo
int maxLevel = 5;
// Secuencia para guardar qué leds deben prenderse
int sequence[5];
// Nivel en el que está el jugador
int level = 1;

bool shownSequence = false;

void setup() {
  for (int i = 0; i < 4; i++){
    pinMode(colori[i], OUTPUT);
    pinMode(btni[i], INPUT_PULLUP); // lee HIGH cuando está sin presionar
  }
  
  randomSeed(analogRead(A0)); // Lee el pin A0 como semilla (para aleatoriedad)
  
  generate_sequence();

  Serial.begin(9600);
}

// Loop principal
void loop() {
  
  // Mientras no se pasen el juego
  if (level <= maxLevel){
    show_sequence();
    players_turn();
  } else {
    finish_game();
    Serial.println("Papeaste al juego amigo, te amo");
  }
}

// Secuencia al pasar de nivel
void won_sequence(){
  delay(500);
  for (int i = 0; i < 4; i++){
    digitalWrite(colori[i], HIGH);
    tone(pinBuzzer, ledsTones[i]);
    delay(75);
    digitalWrite(colori[i], LOW);
    noTone(pinBuzzer);
    delay(75);
  }
  delay(500);
}
// Secuencia al perder
void lost_sequence(){
  delay(500);
  
  tone(pinBuzzer, 150);
  
  for (int i = 0; i < 4; i++){
    for (int i = 0; i < 4; i++){
      digitalWrite(colori[i], HIGH);
    }
    delay(150);
    for (int i = 0; i < 4; i++){
      digitalWrite(colori[i], LOW);
    }
    delay(150);
  }
  
  noTone(pinBuzzer);
  delay(500);
}

void finish_game(){
  delay(500);
  for (int i = 0; i < 15; i++){
    for (int i = 0; i < 2; i++)
    digitalWrite(colori[0], HIGH);
    digitalWrite(colori[2], HIGH);
    tone(pinBuzzer, ledsTones[0]);
    tone(pinBuzzer, ledsTones[2]);
    delay(200);
    digitalWrite(colori[0], LOW);
    digitalWrite(colori[2], LOW);
    noTone(pinBuzzer);
    delay(200);
    
    digitalWrite(colori[1], HIGH);
    digitalWrite(colori[3], HIGH);
    tone(pinBuzzer, ledsTones[1]);
    tone(pinBuzzer, ledsTones[3]);
    delay(200);
    digitalWrite(colori[1], LOW);
    digitalWrite(colori[3], LOW);
    noTone(pinBuzzer);
    delay(200);
  }
  generate_sequence();

  delay(2500);

  level = 1;
  
}

// Generar toda la secuencia al principip
void generate_sequence(){
  for (int i = 0; i < (maxLevel); i++){
    sequence[i] = random(4);
  }
}

// Mostrar la secuencia según el nivel
void show_sequence(){
  if (!shownSequence){
    for (int i = 0; i < level; i++){
    digitalWrite(colori[sequence[i]], HIGH);
    tone(pinBuzzer, ledsTones[sequence[i]]);
    delay(1000);
    digitalWrite(colori[sequence[i]], LOW);
    noTone(pinBuzzer);
    delay(500);
    }
  shownSequence = true;
  }
}

// Espera al turno del jugador
int wait_button() {
  while (true) { // Bucle infinito: "congela" el programa acá adentro
    
    for (int i = 0; i < 4; i++) { // Revisa rápido los 4 botones
      
      if (digitalRead(btni[i]) == LOW) { 
        
        delay(30); // 1. ANTI-REBOTE: Filtra el ruido eléctrico inicial

        // Prende el LED del botón tocado
        digitalWrite(colori[i], HIGH);
        tone(pinBuzzer, ledsTones[i]);

        // Mientras siga apretado, no hace nada
        while (digitalRead(btni[i]) == LOW) {
          // Bucle vacío a propósito
        }
        delay(30); // Anti-rebote al soltar

        // 4. APAGA EL LED
        digitalWrite(colori[i], LOW);
        noTone(pinBuzzer);

        // 5. DEVUELVE EL BOTÓN Y SALE DE LA FUNCIÓN
        return i; // Corta el 'while(true)' y devuelve el índice (0, 1, 2 o 3)
      }
    }
  }
}


// El turno del jugador (un poco más complicadijillo)
void players_turn(){
  Serial.println(level);
  bool lost = false;

  for (int step = 0; step < level; step++){
    int btnPressed = wait_button();
    //Serial.println(btni[btnPressed]);     // ¿Qué botón presionó? (muestra su pin)
    //Serial.println(btni[sequence[step]]) ;// ¿Qué botón es el de la secuencia? (muestra su pin)
    
    if (btni[btnPressed] == btni[sequence[step]]){
      continue;
    } else {
      lost = true;
      break;
    }
  }

  if (lost == true){
    lost_sequence();

    level = 1; // Castigo: vuelve al nivel 1
    shownSequence = false; // Bajamos la bandera para que muestre la secuencia otra vez
    delay(1000);
  } else{
    if (level == maxLevel){
      level++;
      shownSequence = false;
      delay(1000);
      return;
    } else {
      won_sequence();
      level++; // Premio: sube al siguiente nivel
      shownSequence = false; // Bajamos la bandera para que muestre la secuencia otra vez
      delay(1000); // Pausa de 1 segundo antes del próximo nivel
  	  }
    }
  
}