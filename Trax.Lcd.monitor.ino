#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- SET 1: BLOQUES VERTICALES (Para Spotify EQ) ---
byte b1[] = {0,0,0,0,0,0,0,31};
byte b2[] = {0,0,0,0,0,0,31,31};
byte b3[] = {0,0,0,0,0,31,31,31};
byte b4[] = {0,0,0,0,31,31,31,31};
byte b5[] = {0,0,0,31,31,31,31,31};
byte b6[] = {0,0,31,31,31,31,31,31};
byte b7[] = {0,31,31,31,31,31,31,31};
byte b8[] = {31,31,31,31,31,31,31,31};

// --- SET 2: MICRO-PÍXELES HORIZONTALES (Para Monitor de Recursos) ---
byte p1[] = {16,16,16,16,16,16,16,16}; 
byte p2[] = {24,24,24,24,24,24,24,24}; 
byte p3[] = {28,28,28,28,28,28,28,28}; 
byte p4[] = {30,30,30,30,30,30,30,30}; 
byte p5[] = {31,31,31,31,31,31,31,31}; 

int modoActual = -1; // 0: Spotify, 1: Monitor
int datosAudio[16] = {0};
int cpuDestino = 0, ramDestino = 0;
float cpuActual = 0.0, ramActual = 0.0;
String datosEntrantes = "";

void cambiarSetCaracteres(int modo) {
  if (modo == 0) {
    lcd.createChar(1, b1); lcd.createChar(2, b2);
    lcd.createChar(3, b3); lcd.createChar(4, b4);
    lcd.createChar(5, b5); lcd.createChar(6, b6);
    lcd.createChar(7, b7); lcd.createChar(8, b8);
  } else {
    lcd.createChar(1, p1); lcd.createChar(2, p2);
    lcd.createChar(3, p3); lcd.createChar(4, p4);
    lcd.createChar(5, p5);
  }
}

void setup() {
  Serial.begin(115200); 
  Serial.setTimeout(2);
  
  lcd.init();
  lcd.backlight();
  
  lcd.clear();
  lcd.print("SYSTEM ONLINE");
  delay(1000);
  lcd.clear();
}

void loop() {
  // ---- LEER PUERTO SERIAL ----
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      if (datosEntrantes.startsWith("AUD:")) {
        if (modoActual != 0) {
          modoActual = 0;
          lcd.clear();
          cambiarSetCaracteres(0);
        }
        String valores = datosEntrantes.substring(4);
        for (int i = 0; i < 16; i++) {
          int indexComa = valores.indexOf(',');
          if (indexComa > 0) {
            datosAudio[i] = valores.substring(0, indexComa).toInt();
            valores = valores.substring(indexComa + 1);
          } else {
            datosAudio[i] = valores.toInt();
          }
        }
      } 
      else if (datosEntrantes.startsWith("SYS:")) {
        if (modoActual != 1) {
          modoActual = 1;
          lcd.clear();
          cambiarSetCaracteres(1);
        }
        int indexComa = datosEntrantes.indexOf(',');
        if (indexComa > 4) {
          cpuDestino = datosEntrantes.substring(4, indexComa).toInt();
          ramDestino = datosEntrantes.substring(indexComa + 1).toInt();
        }
      }
      datosEntrantes = "";
    } else {
      datosEntrantes += c;
    }
  }

  // ---- RENDERIZAR MODO ACTIVO ----
  if (modoActual == 0) {
    ejecutarSpotifyEqualizer();
  } else if (modoActual == 1) {
    ejecutarMonitorRecursos();
  }
}

void ejecutarSpotifyEqualizer() {
  static unsigned long ultimoEq = 0;
  if (millis() - ultimoEq > 50) {
    ultimoEq = millis();
    for (int col = 0; col < 16; col++) {
      int altura = map(datosAudio[col], 0, 100, 0, 16); 
      lcd.setCursor(col, 0);
      if (altura > 8) lcd.write(altura - 8); else lcd.print(" ");
      lcd.setCursor(col, 1);
      if (altura >= 8) lcd.write(8); else if (altura > 0) lcd.write(altura); else lcd.print(" ");
    }
  }
}

void ejecutarMonitorRecursos() {
  static unsigned long ultimoMonitor = 0;
  if (millis() - ultimoMonitor > 40) {
    ultimoMonitor = millis();
    float ruidoCPU = random(-150, 150) / 100.0;
    float ruidoRAM = random(-50, 50) / 100.0;
    cpuActual = (cpuActual * 0.8) + ((cpuDestino + ruidoCPU) * 0.2);
    ramActual = (ramActual * 0.8) + ((ramDestino + ruidoRAM) * 0.2);
    cpuActual = constrain(cpuActual, 0.0, 100.0);
    ramActual = constrain(ramActual, 0.0, 100.0);

    pintarFilaMonitor(0, "CPU", (int)cpuActual);
    pintarFilaMonitor(1, "RAM", (int)ramActual);
  }
}

void pintarFilaMonitor(int fila, String etiqueta, int porcentaje) {
  lcd.setCursor(0, fila); lcd.print(etiqueta + ":");
  lcd.setCursor(4, fila);
  String numTexto = String(porcentaje) + "%";
  while (numTexto.length() < 4) numTexto = " " + numTexto;
  lcd.print(numTexto);
  
  int pasosActivos = map(porcentaje, 0, 100, 0, 40); 
  for (int col = 0; col < 8; col++) {
    lcd.setCursor(8 + col, fila);
    int pasosEnCol = pasosActivos - (col * 5);
    if (pasosEnCol >= 5) lcd.write(5); 
    else if (pasosEnCol > 0) lcd.write(pasosEnCol); 
    else lcd.print(" ");
  }
}
