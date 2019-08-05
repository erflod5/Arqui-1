#include <LedControl.h>
#include <FrequencyTimer2.h>
#include "letras.h"

LedControl lc = LedControl(18, 19, 20);
#define numCaracteres (17)
#define btn1 A0
#define btn2 A1
#define btn3 A2
#define btn4 A3
#define btn5 A7

//VARIABLES Y CONSTANTES PARA EL MENSAJE INICIAL
byte col = 0;

byte leds[8][16];

int pins[16] = {5, 4, 3, 2, 14, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6};

//int cols[8] = {13, 12, 11, 10, 9, 8, 7, 6};
//int rows[8] = {5, 4, 3, 2, 14, 15, 16, 17};

int cols[8] = {14,11,10,4,8,3,16,17};
int rows[8] = {5,15,6,2,13,7,12,9};

byte Caracter[numCaracteres][8][8] = {
  SPACE, A, R, Q, U, I, UNO, LINE, G, R, U, P, O, LINE, UNO, CINCO, B
};

int contCaracter = 0;
int contRow = 0;

//CONTROLES DEL BOTON
int estado = 0;
int buttonState = 0;     // current state of the button
int lastButtonState = 0; // previous state of the button
int startPressed = 0;    // the time button was pressed
int endPressed = 0;      // the time button was released
int timeHold = 0;        // the time button was hold
int timeReleased = 0;    // the time button was released

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
  lc.clearDisplay(0);
  for (int i = 0; i < 16; i++) {
    pinMode(pins[i], OUTPUT);
  }
  for (int i = 0; i < 8; i++) {
    digitalWrite(cols[i], LOW);
    digitalWrite(rows[i], LOW);
  }
  clearLeds();
  FrequencyTimer2::disable();
  FrequencyTimer2::setPeriod(1000);
  FrequencyTimer2::setOnOverflow(display);
  setFirst(contCaracter);

  //BOTON
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(btn3, INPUT);
  pinMode(btn4, INPUT);
  pinMode(btn5, INPUT);
  pinMode(21, OUTPUT);

}

void loop() {
  pulsar();
  mensajeInicio(100);
}

void mensajeInicio(int del) {
  if (!estado) {
    FrequencyTimer2::setOnOverflow(display);
    changeChar();
    desplazar(contCaracter, del);
  }
  else {

  }
}

void changeChar() {
  if (++contRow == 8) {
    contCaracter = ++contCaracter % numCaracteres;
    contRow = 0;
  }
}

void pulsar() {
  buttonState = digitalRead(btn1); // read the button input
  if (buttonState != lastButtonState) { // button state changed
    updateState();
  }
  lastButtonState = buttonState;        // save state for next loop
}

void updateState() {
  if (buttonState == HIGH) {
    startPressed = millis();
    timeReleased = startPressed - endPressed;
  }
  else {
    endPressed = millis();
    timeHold = endPressed - startPressed;
    if (timeHold < 1000) {
      /*AQUI SE VERIFICA SI ESTA JUGANDO Y SE PONE EL JUEGO EN PAUSA*/
      if (estado) {
        digitalWrite(21, HIGH);
      }
    }
    else {
      digitalWrite(21, LOW);
      estado = !estado;
      if (estado) {
        clear();
        FrequencyTimer2::setOnOverflow(0);
        display();
        contCaracter = 0;
        setFirst(contCaracter);
      }
    }
  }
}

void clearLeds() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      leds[i][j] = 0;
    }
  }
}

void setFirst(int contCaracter) {
  for (int i = 0; i < 8; i++) {
    for (int j = 8; j < 16; j++) {
      leds[i][j] = Caracter[contCaracter][i][j - 8];
    }
  }
}

void desplazar(int contCaracter, int del) {
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 8; j++) {
      leds[j][i] = leds[j][i + 1];
    }
  }
  for (int j = 0; j < 8; j++) {
    leds[j][15] = Caracter[contCaracter][j][contRow];
  }
  displayDriver();
  delay(del);
}

void displayDriver() {
  for (int col = 0; col < 8; col++) {
    for (int row = 0; row < 8; row++) {
      if (leds[col][15 - row] == 1) {
        lc.setLed(0, 7 - col, row, true);
      }
      else {
        lc.setLed(0, 7 - col, row, false);
      }
    }
  }
}

void display() {
  digitalWrite(rows[7 - col], HIGH);
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (leds[col][7 - row] == 1) {
      digitalWrite(cols[row], HIGH);
    }
    else {
      digitalWrite(cols[row], LOW);
    }
  }
  digitalWrite(rows[7 - col], LOW);
}

void clear() {
  clearLeds();
  for (int col = 0; col < 8; col++) {
    for (int row = 0; row < 8; row++) {
      lc.setLed(0, row, col, false);
    }
  }
}

//TETRIS
