#include "LedControl.h"
#include <FrequencyTimer2.h>

#define btn0 A0
#define btn1 A1
#define btn2 A2
#define btn3 A3
#define btn4 A7
#define btn5 A6
#define pot A8

LedControl lc = LedControl(18,19,20);
int pins[16] = {5, 4, 3, 2, 14, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6}; //PINES DE SALIDA HACIA LA MATRIZ SIN DRIVER

//int cols[8] = {13, 12, 11, 10, 9, 8, 7, 6};
//int rows[8] = {5, 4, 3, 2, 14, 15, 16, 17};

int cols[8] = {2,11,10,16,8,15,4,5}; //PINES DE LAS COLUMNAS DE LA MATRIZ
int rows[8] = {17,3,6,14,13,7,12,9}; //PINES DE LAS FILAS DE LA MATRIZ

int block[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}};
    
int field[18][11] = {};
int stage[18][11] = {};


//coordenadas

int y = 0;
int x = 4;
int puntaje = 0;
bool gameover = false;
byte col = 0;

int bloques[7][4][4] = {
    {{0, 1, 0, 0},
     {0, 1, 0, 0},
     {0, 1, 0, 0},
     {0, 1, 0, 0}},
    {{0, 0, 0, 0},
     {0, 1, 1, 0},
     {0, 1, 0, 0},
     {0, 1, 0, 0}},
    {{0, 0, 1, 0},
     {0, 1, 1, 0},
     {0, 1, 0, 0},
     {0, 0, 0, 0}},
    {{0, 1, 0, 0},
     {0, 1, 1, 0},
     {0, 0, 1, 0},
     {0, 0, 0, 0}},
    {{0, 0, 0, 0},
     {0, 1, 0, 0},
     {1, 1, 1, 0},
     {0, 0, 0, 0}},
    {{0, 0, 0, 0},
     {0, 1, 1, 0},
     {0, 1, 1, 0},
     {0, 0, 0, 0}},
    {{0, 0, 0, 0},
     {0, 1, 1, 0},
     {0, 0, 1, 0},
     {0, 0, 1, 0}}
};

void setup() {
  Serial.begin(9600); 
  randomSeed(analogRead(0)); //Generador de numeros aleatorios
  lc.shutdown(0,false);
  lc.setIntensity(0,5);
  lc.clearDisplay(0);

  for (int i = 0; i < 16; i++) {
    pinMode(pins[i], OUTPUT);
  }
  for (int i = 0; i < 8; i++) {
    digitalWrite(cols[i], LOW);
    digitalWrite(rows[i], LOW);
  }
  
  pinMode(btn0,INPUT);
  pinMode(btn1,INPUT);
  pinMode(btn2,INPUT);
  pinMode(btn3,INPUT);
  pinMode(btn4,INPUT);
  pinMode(btn5,INPUT);

  /* FrequencyTimer2 es una libreria que se esta llamando constantemente
        es utilizada para pintar en la matriz sin driver
   */

  FrequencyTimer2::disable();
  FrequencyTimer2::setPeriod(2000);
  FrequencyTimer2::setOnOverflow(displayP); //displayP es el Metodo que pinta en la matriz, se le manda como parametro
  initGame();
}
int valor; //Captura el valor del potenciometro
int wait = 150; //guarda la velocidad del juego

void loop() {
  if(!gameover){
    gameLoop();
  }
  else{
    gameover = false;
    initGame();
  }
  velocidad();
  delay(wait); //velocidad
}

//Metodo que leee la entrada analogica del potenciometro para determinar la velocidad
void velocidad(){
  valor = analogRead(pot);
  Serial.println(valor);
  if(valor > 750){
    wait = 50;    
  }
  else if(valor > 500){
    wait = 100;
  }
  else if(valor > 250){
    wait = 200;
  }
  else{
    wait = 350;
  }
}

int gameOver()
{
  col = 8;
  FrequencyTimer2::setOnOverflow(displayP);
  for(int i = 0; i < 8; i++){
    lc.setRow(0,i,B11111111);
    delay(wait);
  }
  lc.clearDisplay(0);
  return 0;
}

void gameLoop()
{
  userInput();
  displayM();
  spawnBlock();  
  seElimina();
}

//Metodo que pinta la matriz con driver
void displayM()
{
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            //TODO: pintar matriz
            if(field[i][j+1]==0){
              lc.setLed(0,j,i,false);
            }
            else{
              lc.setLed(0,j,i,true);
            }
        }
    }
    if (gameover)
    {
        gameOver();
    }
}

//Metodo que pinta la matriz sin driver
void displayP(){
  digitalWrite(cols[col], LOW);
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (field[col + 8][row + 1] == 1) {
      digitalWrite(rows[row], LOW);
    }
    else {
      digitalWrite(rows[row], HIGH);
    }
  }
  digitalWrite(cols[col], HIGH);
}

void initGame()
{
    for (size_t i = 0; i <= 16; i++)
    {
        for (size_t j = 0; j <= 9; j++)
        {
            if ((j == 0) || (j == 9) || (i == 16))
            {
                field[i][j] = stage[i][j] = 9;
            }
            else
            {
                field[i][j] = stage[i][j] = 0;
            }
        }
    }
    crearBlocks();
    displayM();
}

//Leee los botones del juego
void userInput()
{
    if (digitalRead(btn1) == HIGH)
    {
        if (!esChoque(x + 1, y))
            moveBlock(x + 1, y);
    }
    if (digitalRead(btn2) == HIGH)
    {
        if (!esChoque(x - 1, y))
            moveBlock(x - 1, y);
    }
    if (digitalRead(btn3) == HIGH)
    {
        if (!esChoque(x, y + 1))
            moveBlock(x, y + 1);
    }
    if (digitalRead(btn4) == HIGH)
    {
        rotarBloque();
    }
}

bool crearBlocks()
{
    x = 4;
    y = 0;
    int tipo = random(7);
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            block[i][j] = 0;
            block[i][j] = bloques[tipo][i][j];
        }
    }
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            field[i][j + 4] = stage[i][j + 4] + block[i][j];
            if (field[i][j + 4] > 1)
            {
                gameover = true;
                return true;
            }
        }
    }
    return false;
}

void moveBlock(int x2, int y2)
{
    //Remover bloque
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            field[y + i][x + j] -= block[i][j];
        }
    }
    //actualizar coordenadas
    x = x2;
    y = y2;
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            field[y + i][x + j] += block[i][j];
        }
    }
    displayM();
}

void choque()
{
    for (size_t i = 0; i < 17; i++)
    {
        for (size_t j = 0; j < 10; j++)
        {
            stage[i][j] = field[i][j];
        }
    }
}

bool esChoque(int x2, int y2)
{
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            if (block[i][j] && stage[y2 + i][x2 + j] != 0)
            {
                return true;
            }
        }
    }
    return false;
}

bool rotarBloque()
{
    int tmp[4][4] = {};
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            tmp[i][j] = block[i][j];
        }
    }
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            block[i][j] = tmp[3 - j][i];
        }
    }
    if (esChoque(x, y))
    {
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                block[i][j] = tmp[i][j];
            }
        }
        return true;
    }
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            field[y + i][x + j] -= tmp[i][j];
            field[y + i][x + j] += block[i][j];
        }
    }
    displayM();
    return false;
}

void spawnBlock()
{
    if (!esChoque(x, y + 1))
    {
        moveBlock(x, y + 1);
    }
    else
    {
        choque();
        crearBlocks();
        displayM();
    }
}

void seElimina()
{
    for (size_t i = 0; i < 17; i++)
    {
        size_t j = 1;
        if (lineaLlena(i, j))
        {
            eliminarFila(i);
            puntaje++;
        }
    }
}

void eliminarFila(size_t x)
{
    for (size_t i = x; i > 1; i--)
    {
        for (size_t j = 8; j > 0; j--)
        {
            field[i][j] = field[i - 1][j];
        }
    }
}

bool lineaLlena(size_t x, size_t y)
{
    if (y == 9)
    {
        return true;
    }
    if (field[x][y] == 1)
    {
        return lineaLlena(x, y + 1);
    }
    else
    {
        return false;
    }
}
