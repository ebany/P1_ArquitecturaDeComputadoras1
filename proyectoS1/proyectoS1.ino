#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include "notas.h"
#include "LedControl.h"
#include "pitches.h" //add Equivalent frequency for musical note
#include "themes.h" //add Note vale and duration

typedef struct lista
{
  String nota;
  int duracion;
  lista* siguiente;
} LISTA;

int col1[8][2] = {0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0
                 };

int col2[8][2] = {0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0
                 };

int col3[8][2] = {0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0
                 };

int col4[8][2] = {0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0,
                  0, 0
                 };

int contadores[4] = {0, 0, 0, 0};

int valCol1[3] = {0, 0, 0};
int valCol2[3] = {0, 0, 0};
int valCol3[3] = {0, 0, 0};
int valCol4[3] = {0, 0, 0};

// Din, clk, cs
LedControl lc = LedControl(44, 45, 42, 9);

byte interrogacion[8] = {
  0b00000100,
  0b00000000,
  0b00000000,
  0b00000100,
  0b00000100,
  0b00000100,
  0b00000100,
  0b00000000
};

/*LCD*/
const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 48, d7 = 49;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String texto = "Ningun cumbion";
String seccion = "G 99 A!";
int sizeF1 = 0;
int sizeF2 = 0;
#define VELOCIDAD 400
String msj1 = "";
String msj2 = "";

/*BOTONES-TURNOS-PUNTEO*/
int actual1 = 0;
int anterior1 = 0;
int actual2 = 0;
int anterior2 = 0;
/*---48 Q1 | 49 Q0.50*/
int turnos = 0;
int puntos = 0;

/*TIEMPO*/
unsigned long tiempo = 0;
unsigned long t_actualizado = 0;
unsigned long t_actualizado2 = 0;
unsigned long t_actualizado3 = 0;
unsigned long t_actualizado4 = 0;
unsigned long t_actualizado5 = 0;
unsigned long t_actualizado6 = 0;

/*EXTRAS*/
int Fa1 = 0;
int FaExt1 = 1;
int Fa2 = 15;
int FaExt2 = 1;
int flagP = 0;

/*JUEGO*/
int activo = 0;
int dificultad = 1;   //1 = facil | 2 = intermedio | 3 = dificil
int extraPush = 0;

/*SONIDO NOTAS*/
#define SONIDO 300
int pausa = 0;
int iniciarTocar = 0;
LISTA* primero;

/*BLUETOOTH*/
SoftwareSerial bt(15, 14);

void setup() {
  Serial.begin(9600); //Se inicia la comunicación serie
  Serial1.begin(9600);

  pinMode(22, INPUT);
  pinMode(23, INPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(0, interrogacion);
  lcd.home();
  setMensajeF1();
  setMensajeF2();
  /*-----------------------------MATRIZ*/
  lc.shutdown(0, false);
  lc.setIntensity(0, 9);
  lc.clearDisplay(0);
  /*bluetooth*/
  bt.begin(9600);
}

void loop() {

  tiempo = millis();
  /*BOTONES*/
  actual1 = digitalRead(22);
  actual2 = digitalRead(23);
  /*JAVA*/
  if (Serial.available() > 0) {
    if (turnos > 0)
      cancion();
    //leer();
    else {
      while (Serial.available() > 0) {
        Serial.read();
      }
    }
  }
  /*IMPRIMIR MATRIZ*/
  imprimirMatriz();
  /*BLUETOOTH --> PUNTEO | Y ERRONEO N CORRECTO*/
  if (Serial1.available() > 0) {
    if (turnos > 0)
      respuestaBT();
    else {
      String msj = "";
      msj.concat(puntos);
      msj.concat("|N");
      Serial1.print("#");
      Serial1.print(msj);
      while (Serial1.available() > 0) {
        Serial1.read();
      }
    }
  }
  /*ELIMINAR NOTA*/
  if (activo == 1) {
    if (tiempo > t_actualizado4 + pausa * 1) { //1.80
      noTone(8);
      t_actualizado4 = tiempo;
    }
  }
  /*if (iniciarTocar == 1 && primero != NULL) {
    if (tiempo > t_actualizado6 + primero->duracion * 1.80) {
      noTone(8);
      LISTA* temp = primero;
      primero = primero->siguiente;
      free(temp);
      tone(8, nota(primero->nota), primero->duracion);
      t_actualizado6 = tiempo;
    }
    }*/
  /*MATRIZ*/
  if (dificultad == 1) {
    if (tiempo > t_actualizado5 + 500) {
      bajarBloques();
      t_actualizado5 = tiempo;
    }
  } else if (dificultad == 2) {
    if (tiempo > t_actualizado5 + 200) {
      bajarBloques();
      t_actualizado5 = tiempo;
    }
  } else if (dificultad == 3) {
    if (tiempo > t_actualizado5 + 1) {
      bajarBloques();
      t_actualizado5 = tiempo;
    }
  }
  /*LCD*/
  imprimir_fila1();
  imprimir_fila2();
  /*BOTONES*/
  coin();
  //Play_MarioUW();
  anterior1 = actual1;
  anterior2 = actual2;
}

/*void leer() {
  String entrada = "";//Serial.readStringUntil("\n");
  while (Serial.available() > 0) {
    char caracter = Serial.read();
    if (caracter != '\n')
      entrada.concat(caracter);
  }
  //Serial.println(entrada);
  if (turnos > 0 && entrada[0] == 'Y') {              //INICIO CANCION
    activo = 1;
    dificultad = (int)entrada[1] - 48;
    texto = entrada.substring(2);
  } else if (turnos > 0 && entrada[0] == 'H') {       //CANCION FINALIZADA
    //Serial.println("Se Finalizo");
    activo = 0;
    iniciarTocar = 1;
    t_actualizado6 = tiempo;
    tone(8, nota(primero->nota), primero->duracion);
    //Serial.println("inicia");
  } else if (activo == 1) {               //RECIBIR NOTA
    /*OBTENCION DE DURACION Y NOTA
    int noteDuration = 0;
    String notaR = "";
    if (entrada[1] - 48 >= 0 && entrada[1] - 48 < 10) {
      noteDuration = 1000 / (((int)entrada[0] - 48) * 10 + (int)entrada[1] - 48 );
      notaR = entrada.substring(2);
    } else {
      noteDuration = 1000 / ((int)entrada[0] - 48 );
      notaR = entrada.substring(1);
    }
    pausa = noteDuration;//TIEMPO DE DURACION
    agregar(notaR, noteDuration);
    //tone(8, nota(notaR), noteDuration);
    //Serial.println("nota");
    //delay(400);
  }
  }*/

void agregar(String nota, int duracion) {
  //Serial.println(nota+"--"+duracion);
  if (primero != NULL) { //no esta vacia
    LISTA* temp = primero;
    while (temp->siguiente != NULL) {
      temp = temp->siguiente;
    }
    LISTA* nuevo = (LISTA*)malloc(sizeof(LISTA));
    nuevo->nota = nota;
    nuevo->duracion = duracion;
    nuevo->siguiente = NULL;
    temp->siguiente = nuevo;
    //Serial.println("no nulo");
  } else {              //esta vacia
    primero = (LISTA*)malloc(sizeof(LISTA));
    primero->nota = nota;
    primero->duracion = duracion;
    primero->siguiente = NULL;
    //Serial.println("nulo");
  }
}

void respuestaBT() {
  String msj = "";
  if (activo == 1) {
    switch (Serial1.read()) {
      case 'A':
        if (col1[6][0] != 0) {
          puntos += col1[6][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else if ( col1[7][0] != 0) {
          puntos += col1[7][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else {
          if (puntos > 0)
            puntos--;
          else
            puntos = 0;
          msj.concat(puntos);
          msj.concat("|Y");
          Serial1.print("#");
          Serial1.print(msj);
          //Serial.println("ERROR");
        }
        break;
      case 'B':
        if (col2[6][0] != 0) {
          puntos += col2[6][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else if ( col2[7][0] != 0) {
          puntos += col2[7][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else {
          if (puntos > 0)
            puntos--;
          else
            puntos = 0;
          msj.concat(puntos);
          msj.concat("|Y");
          Serial1.print("#");
          Serial1.print(msj);
          //Serial.println("ERROR");
        }
        break;
      case 'C':
        if (col3[6][0] != 0) {
          puntos += col3[6][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else if ( col3[7][0] != 0) {
          puntos += col3[7][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else {
          if (puntos > 0)
            puntos--;
          else
            puntos = 0;
          msj.concat(puntos);
          msj.concat("|Y");
          Serial1.print("#");
          Serial1.print(msj);
          //Serial.println("ERROR");
        }
        break;
      case 'D':
        if (col4[6][0] != 0) {
          puntos += col4[6][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else if ( col4[7][0] != 0) {
          puntos += col4[7][0];
          msj.concat(puntos);
          msj.concat("|N");
          Serial1.print("#");
          Serial1.print(msj);
        } else {
          if (puntos > 0)
            puntos--;
          else
            puntos = 0;
          msj.concat(puntos);
          msj.concat("|Y");
          Serial1.print("#");
          Serial1.print(msj);
          //Serial.println("ERROR");
        }
        break;
    }
  } else {
    msj.concat(puntos);
    msj.concat("|N");
    Serial1.print("#");
    Serial1.print(msj);
    while (Serial1.available() > 0) {
      Serial1.read();
    }
  }
}

void cancion() {
  String entrada = "";
  while (Serial.available() > 0) {
    char caracter = Serial.read();
    if (caracter != '\n')
      entrada.concat(caracter);
  }
  //Serial.println(entrada);
  //String entrada = Serial.readStringUntil('\n');
  if (turnos > 0 && entrada[0] == 'Y') {              //INICIO CANCION
    String msj = "";
    msj.concat(puntos);
    msj.concat("|N");
    Serial1.print("#");
    Serial1.print(msj);
    activo = 1;
    dificultad = (int)entrada[1] - 48;
    texto = entrada.substring(2);
  } else if (turnos > 0 && entrada[0] == 'H') {       //CANCION FINALIZADA
    //Serial.println("Se Finalizo");
    String msj = "";
    msj.concat(texto);
    msj.concat("$");
    msj.concat(puntos);
    //Serial1.print("#");
    Serial.print(msj);
    activo = 0;
    texto = "Ningun cumbion";
    puntos = 0;
    turnos--;
    extraPush = 0;          
  } else if (activo == 1) {               //RECIBIR NOTA
    /*OBTENCION DE DURACION Y NOTA*/
    int noteDuration = 0;
    String notaR = "";
    if (entrada.length() > 3 && entrada[2] - 48 >= 0 && entrada[2] - 48 < 10) {
      noteDuration = 1000 / (((int)entrada[0] - 48) * 100 + ((int)entrada[1] - 48 ) * 10 + (int)entrada[1] - 48);
      notaR = entrada.substring(3);
    } else if (entrada[1] - 48 >= 0 && entrada[1] - 48 < 10) {
      noteDuration = 1000 / (((int)entrada[0] - 48) * 10 + (int)entrada[1] - 48 );
      notaR = entrada.substring(2);
    } else {
      noteDuration = 1000 / ((int)entrada[0] - 48 );
      notaR = entrada.substring(1);
    }
    pausa = noteDuration;//TIEMPO DE DURACION
    tone(8, nota(notaR), noteDuration);
    //Serial.println("nota");
    //delay(400);
    t_actualizado4 = tiempo;
    if (dificultad == 1) {
      if (extraPush < 14)
        extraPush++;
      else {
        meter();
        extraPush = 0;
      }
    } else if (dificultad == 2) {
      if (extraPush < 10)
        extraPush++;
      else {
        meter();
        extraPush = 0;
      }
    } else {
      if (extraPush < 7)
        extraPush++;
      else {
        meter();
        extraPush = 0;
      }
    }
  }
}

void coin() {
  if (actual1 != anterior1 && actual1 == 1)
    turnos += 4;
  else if (actual2 != anterior2 && actual2 == 0)
    turnos += 2;
}

void setMensajeF1() {
  msj1 = "GRUPO 13 A! T-" + String(turnos) + " P" + String(puntos);
  sizeF1 = msj1.length();
}

void setMensajeF2() {
  msj2 = texto;
  sizeF2 = msj2.length();
}

void imprimir_fila1() {
  int tam_texto = msj1.length() + 4;
  if (Fa1 < 16 && tiempo > t_actualizado + VELOCIDAD) {
    String texto2 = msj1.substring(tam_texto - Fa1 - 4);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(texto2);
    t_actualizado = tiempo;
    Fa1++;
    setMensajeF1();
  } else if ( Fa1 < tam_texto - 3 && tiempo > t_actualizado + VELOCIDAD) {
    String texto1 = msj1.substring(tam_texto - Fa1 - 4);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(texto1);
    t_actualizado = tiempo;
    Fa1++;
    setMensajeF1();
  } else if (tiempo > t_actualizado + VELOCIDAD) {
    if (FaExt1 < 17) {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(FaExt1 - 1, 0);
      lcd.write((byte)0);
      lcd.setCursor(FaExt1, 0);
      lcd.print(msj1.substring(0, 16));
      if (FaExt1 != 16) {
        FaExt1++;
      } else {
        Fa1 = 0;
        FaExt1 = 1;
        setMensajeF1();
      }
      t_actualizado = tiempo;
    }
  }
  /*for (int a = 0; a < 16; a++) {
    String texto2 = msj1.substring(tam_texto - a - 4);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(texto2);
    delay(VELOCIDAD);
    }*/

  /*for (int a = 16; a < tam_texto - 3; a++) {
    String texto1 = msj1.substring(tam_texto - a - 4);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(texto1);
    delay(VELOCIDAD);
    }*/

  /*for (int a = 1; a < 16; a++) {
    lcd.clear();
    lcd.setCursor(a-1,0);
    lcd.write((byte)0);
    lcd.setCursor(a, 0);
    lcd.print(msj1.substring(0, 16));
    delay(VELOCIDAD);
    }*/
}

void imprimir_fila2() {
  int tam_texto = msj2.length() + 4;
  String texto2 = msj2.substring(0, 15);
  if (Fa2 > 0 && tiempo > t_actualizado2 + VELOCIDAD) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(Fa2, 1);
    lcd.print(texto2);
    Fa2--;
    t_actualizado2 = tiempo;
  } else if (FaExt2 <= tam_texto && tiempo > t_actualizado2 + VELOCIDAD) {
    String texto1 = msj2.substring(FaExt2 - 1);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(texto1);
    if (FaExt2 != tam_texto) {
      FaExt2++;
    } else {
      Fa2 = 15;
      FaExt2 = 1;
      setMensajeF2();
    }
    t_actualizado2 = tiempo;
  }
  /*for (int a = 15; a > 0; a--) {
    lcd.clear();
    lcd.setCursor(a, 1);
    lcd.print(texto2);
    delay(VELOCIDAD);
    }*/
  /*for (int a = 1; a <= tam_texto; a++) {
    String texto1 = msj2.substring(a - 1);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(texto1);
    delay(VELOCIDAD);
    }*/
}

void imprimirMatriz() {
  for (int a = 0; a < 8; a++) {
    for (int b = 0; b < 2; b++) {

      if (col1[a][b] == 0) {
        lc.setLed(0, a, b, false);
      } else {
        lc.setLed(0, a, b, true);
      }

      if (col2[a][b] == 0) {
        lc.setLed(0, a, b + 2, false);
      } else {
        lc.setLed(0, a, b + 2, true);
      }

      if (col3[a][b] == 0) {
        lc.setLed(0, a, b + 4, false);
      } else {
        lc.setLed(0, a, b + 4, true);
      }

      if (col4[a][b] == 0) {
        lc.setLed(0, a, b + 6, false);
      } else {
        lc.setLed(0, a, b + 6, true);
      }

    }
  }
}

void meter() {
  int randomColumna = random(0, 4);
  int randomSize = random(2, 5);
  switch (randomColumna) {
    case 0:
      for (int a = 0; a < 3; a++) {
        if (valCol1[a] == 0) { //espacio vacio
          valCol1[a] = randomSize;
          if (contadores[randomColumna] == 0)
            contadores[randomColumna] = randomSize;
          break;
        }
      }
      break;
    case 1:
      for (int a = 0; a < 3; a++) {
        if (valCol2[a] == 0) { //espacio vacio
          valCol2[a] = randomSize;
          if (contadores[randomColumna] == 0)
            contadores[randomColumna] = randomSize;
          break;
        }
      }
      break;
    case 2:
      for (int a = 0; a < 3; a++) {
        if (valCol3[a] == 0) { //espacio vacio
          valCol3[a] = randomSize;
          if (contadores[randomColumna] == 0)
            contadores[randomColumna] = randomSize;
          break;
        }
      }
      break;
    case 3:
      for (int a = 0; a < 3; a++) {
        if (valCol4[a] == 0) { //espacio vacio
          valCol4[a] = randomSize;
          if (contadores[randomColumna] == 0)
            contadores[randomColumna] = randomSize;
          break;
        }
      }
      break;
  }
}

void bajarBloques() {
  for (int a = 0; a < 4; a++) {
    switch (a) {
      case 0:
        /*CORRIMIENTO HACIA ABAJO*/
        for (int b = 7; b > 0; b--) {
          for (int c = 0; c < 2; c++) {
            col1[b][c] = col1[b - 1][c];
          }
        }
        /*METER NUEVO VALO*/
        if (valCol1[0] != 0) { //hay blqoue que meter
          if (contadores[a] != 0) { //METER NUMERO
            col1[0][0] = valCol1[0];
            col1[0][1] = valCol1[0];
            contadores[a]--;  //disminuyo en 1 el contador
          } else {
            /*CORRIMIENTO DE VALORES EN COLA*/
            for (int n = 0; n < 2; n++) {
              valCol1[n] = valCol1[n + 1];
            }
            valCol1[2] = 0;
            if (valCol1[0] != 0)
              contadores[a] = valCol1[0];
            else
              contadores[a] = 0;
            /*METER 0´S*/
            col1[0][0] = 0;
            col1[0][1] = 0;
          }
        } else {           //no hay bloque para meter. Meter 0´s
          col1[0][0] = 0;
          col1[0][1] = 0;
        }
        break;
      case 1:
        /*CORRIMIENTO HACIA ABAJO*/
        for (int b = 7; b > 0; b--) {
          for (int c = 0; c < 2; c++) {
            col2[b][c] = col2[b - 1][c];
          }
        }
        /*METER NUEVO VALOR*/
        if (valCol2[0] != 0) { //hay blqoue que meter
          if (contadores[a] != 0) { //METER NUMERO
            col2[0][0] = valCol2[0];
            col2[0][1] = valCol2[0];
            contadores[a]--;  //disminuyo en 1 el contador
          } else {
            /*CORRIMIENTO DE VALORES EN COLA*/
            for (int n = 0; n < 2; n++) {
              valCol2[n] = valCol2[n + 1];
            }
            valCol2[2] = 0;
            if (valCol2[0] != 0)
              contadores[a] = valCol2[0];
            else
              contadores[a] = 0;
            /*METER 0´S*/
            col2[0][0] = 0;
            col2[0][1] = 0;
          }
        } else {           //no hay bloque para meter. Meter 0´s
          col2[0][0] = 0;
          col2[0][1] = 0;
        }
        break;
      case 2:
        /*CORRIMIENTO HACIA ABAJO*/
        for (int b = 7; b > 0; b--) {
          for (int c = 0; c < 2; c++) {
            col3[b][c] = col3[b - 1][c];
          }
        }
        /*METER NUEVO VALOR*/
        if (valCol3[0] != 0) { //hay blqoue que meter
          if (contadores[a] != 0) { //METER NUMERO
            col3[0][0] = valCol3[0];
            col3[0][1] = valCol3[0];
            contadores[a]--;  //disminuyo en 1 el contador
          } else {
            /*CORRIMIENTO DE VALORES EN COLA*/
            for (int n = 0; n < 2; n++) {
              valCol3[n] = valCol3[n + 1];
            }
            valCol3[2] = 0;
            if (valCol3[0] != 0)
              contadores[a] = valCol3[0];
            else
              contadores[a] = 0;
            /*METER 0´S*/
            col3[0][0] = 0;
            col3[0][1] = 0;
          }
        } else {           //no hay bloque para meter. Meter 0´s
          col3[0][0] = 0;
          col3[0][1] = 0;
        }
        break;
      case 3:
        /*CORRIMIENTO HACIA ABAJO*/
        for (int b = 7; b > 0; b--) {
          for (int c = 0; c < 2; c++) {
            col4[b][c] = col4[b - 1][c];
          }
        }
        /*METER NUEVO VALOR*/
        if (valCol4[0] != 0) { //hay blqoue que meter
          if (contadores[a] != 0) { //METER NUMERO
            col4[0][0] = valCol4[0];
            col4[0][1] = valCol4[0];
            contadores[a]--;  //disminuyo en 1 el contador
          } else {
            /*CORRIMIENTO DE VALORES EN COLA*/
            for (int n = 0; n < 2; n++) {
              valCol4[n] = valCol4[n + 1];
            }
            valCol4[2] = 0;
            if (valCol4[0] != 0)
              contadores[a] = valCol4[0];
            else
              contadores[a] = 0;
            /*METER 0´S*/
            col4[0][0] = 0;
            col4[0][1] = 0;
          }
        } else {           //no hay bloque para meter. Meter 0´s
          col4[0][0] = 0;
          col4[0][1] = 0;
        }
        break;
    }
  }
}
