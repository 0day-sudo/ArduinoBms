#include <Arduino.h>

#define NUM_CELLS_PER_UNIT 2
#define NUM_UNITS          1

// Pin definierung
const int PIN_ZELLE1 = A1;
const int PIN_GESAMT = A2;
const int PIN_MAIN_SWITCH = 4; // D4 Steuert den Last-MOSFET
const int PIN_STATUS_LED = 13; // Onboard LED

// Konstanten fuer die Berechnungen und das Balancing 
const float V_REF = 5.0;         // Exakte Spannung am 5V Pin (bitte nachmessen!)
const float DIVIDER_FAKTOR = 2.0; // Da 10k/10k Teiler die Spannung halbiert
const float MIN_VOLT_ZELLE = 3.0; // Abschaltung bei 3.0V pro Zelle
const float MAX_VOLT_ZELLE = 4.2; // 100% bei 4.2V pro Zelle

struct CellData {
// Messwerte
  int voltage_mV;         // Aktuelle Gesamtzellspannung in Millivolt
  int voltage_Cell1;      // Zellspannung der ersten Zelle
  int voltage_Cell2;      // Zellspannung der zweiten Zelle
  int temperature_C;      // Aktuelle Zelltemperatur in Grad Celsius
  // Status & Steuerung
  byte internal_ID;       // Die interne Kanal-ID im Monitor-IC
  bool is_balancing;      // TRUE, wenn der Balancierwiderstand dieser Zelle aktiv ist
};

// Array in dehnen alle Units gespecihert sind
CellData Units[NUM_UNITS];

void read_Data_for_one_unit() {
  for (int i = 0; i < NUM_UNITS; i++) {
    // 1. MESSUNG (Durchschnitt aus 20 Werten für Stabilität)
    float rawA1 = 0;
    float rawA2 = 0;
    for(int i = 0; i < 20; i++) {
      rawA1 += analogRead(PIN_ZELLE1);
      rawA2 += analogRead(PIN_GESAMT);
      delay(2);
    }
    rawA1 /= 20.0;
    rawA2 /= 20.0;

    // 2. UMRECHNUNG IN VOLT
    // Spannung von Zelle 1 (direkt oder via Schutzwiderstand)
    float vZelle1 = (rawA1 * V_REF) / 1024.0;
    
    // Gesamtspannung (Wert am Pin * 2, wegen Teiler)
    float vGesamt = ((rawA2 * V_REF) / 1024.0) * DIVIDER_FAKTOR;
    
    // Zelle 2 berechnen (Differenz)
    float vZelle2 = vGesamt - vZelle1;

    Units[i].voltage_mV = vGesamt;
    Units[i].voltage_Cell1 = vZelle1;
    Units[i].voltage_Cell2 = vZelle2;

    Units[i].temperature_c = 25;
    Units[i].internal_ID = i;
    Units[i].is_balancing = false;
  
  }
}

void print_Data() {
  for (int i = 0; i < NUM_UNITS; i++) {
    Serial.print("Unit ["); Serial.print(Units[i].internal_ID); Serial.print("] ");
    Serial.print("Z1: "); Serial.print(Units[i].voltage_Cell1); Serial.print("V | ");
    Serial.print("Z2: "); Serial.print(Units[i].voltage_Cell2); Serial.print("V | ");
    Serial.print("Gesamt: "); Serial.print(Units[i].voltage_Total); Serial.println("V");
  }
}

void setup() {
  // wichtig fuer das debugging
  Serial.begin(115200);

  while(!Serial); // warten bis der Monitor offen ist
  
  Serial.print("Initialisierung des BMS ...\n");

  pinMode(PIN_MAIN_SWITCH, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);


}

// Die Hauptschleife läuft unendlich.
void loop() {
  // 1. Daten einlesen
  read_Data_for_one_unit();

  // 2. Daten ausgeben
  print_Data();

  // 3. Status-LED blinken lassen (ohne delay, damit Messung schnell bleibt)
  digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));

  // 4. Kurze Pause bis zur nächsten Messung
  delay(1000);
}