#include <Arduino.h>

#define ledPin1 13
#define ledPin2 12
#define ledPin3 11
#define ledPin4 10
#define ledPin5 9

const int switchPin1 = 5;
const int switchPin2 = 4;
const int switchPin3 = 3;

bool switchState[3] = {false, false, false};
int leds[] = {ledPin1, ledPin2, ledPin3, ledPin4, ledPin5};

unsigned long previousMillis = 0;
unsigned long interval = 1000;
int currentState = 0;

// Variables para la secuencia de semáforo
int ledIndex = 0;
bool esperandoApagado = false;
unsigned long lastMillisSemaforo = 0;

// Función para la secuencia del semáforo
void secuenciaSemaforo() {
    unsigned long tiempoActual = millis();

    if (!switchState[1]) { // Si el switch se suelta, reiniciar y apagar todo
        ledIndex = 0;
        esperandoApagado = false;
        for (int i = 0; i < 5; i++) {
            digitalWrite(leds[i], LOW);
        }
        return;
    }

    if (!esperandoApagado) {
        if (ledIndex < 5 && tiempoActual - lastMillisSemaforo >= 1000) {
            lastMillisSemaforo = tiempoActual;
            digitalWrite(leds[ledIndex], HIGH);
            Serial.print("LED encendido en pin: ");
            Serial.println(leds[ledIndex]);
            ledIndex++;

            if (ledIndex == 5) { // Si todos los LEDs están encendidos, activar espera de 2 segundos
                esperandoApagado = true;
                lastMillisSemaforo = tiempoActual;
            }
        }
    } else {
        if (tiempoActual - lastMillisSemaforo >= 2000) {
            for (int i = 0; i < 5; i++) {
                digitalWrite(leds[i], LOW);
            }
            Serial.println("Se apagan todos");
            esperandoApagado = false;
            ledIndex = 0;
            lastMillisSemaforo = tiempoActual;
        }
    }
}

// Función de secuencia de encendido/apagado rápido
void secuencia() {
    static bool state = false;
    static unsigned long lastMillis = 0;

    if (millis() - lastMillis >= 500) {
        lastMillis = millis();
        state = !state;
        for (int i = 0; i < 5; i++) {
            digitalWrite(leds[i], state ? HIGH : LOW);
        }
    }
}

void setup() {
    pinMode(switchPin1, INPUT);
    pinMode(switchPin2, INPUT);
    pinMode(switchPin3, INPUT);

    for (int i = 0; i < 5; i++) {
        pinMode(leds[i], OUTPUT);
    }

    Serial.begin(9600);
}

void loop() {
    // Leer estado de los botones
    switchState[0] = digitalRead(switchPin1);
    switchState[1] = digitalRead(switchPin2);
    switchState[2] = digitalRead(switchPin3);

    if (switchState[0]) {
        currentState = 1;
    } else if (switchState[1]) {
        currentState = 2;
        ledIndex = 0;  // Resetear la secuencia cada vez que se presiona switch2
        esperandoApagado = false;
    } else if (switchState[2]) {
        currentState = 3;
    } else {
        currentState = 0;
    }

    // Ejecutar la acción correspondiente
    switch (currentState) {
        case 1: // Encender LED 1 al presionar switch 1
            Serial.println("---Led1 encendido---");
            digitalWrite(ledPin1, HIGH);
            delay(200);
            while (digitalRead(switchPin1)); // Espera sin bloquear
            digitalWrite(ledPin1, LOW);
            Serial.println("---Led1 apagado---");
            delay(200);
            break;

        case 2: // Secuencia de semáforo
            Serial.println("---Inicia Semaforo---");
            delay(200);
            while (digitalRead(switchPin2)) {
                secuenciaSemaforo();
            }
            Serial.println("---Se apaga Semaforo---");
            delay(200);
            for (int i = 0; i < 5; i++) {
                digitalWrite(leds[i], LOW);
            }
            break;

        case 3: // Secuencia de encendido/apagado
            Serial.println("---Inicia secuencia---");
            delay(200);
            while (digitalRead(switchPin3)) {
                secuencia();
            }
            Serial.println("///Finaliza secuencia///");
            delay(200);
            for (int i = 0; i < 5; i++) {
                digitalWrite(leds[i], LOW);
            }
            break;
    }
}
