#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// Pini pentru senzori și LED-uri
#define SENSOR_ENTRY_1 5
#define SENSOR_EXIT_1 6
#define LED_WHITE_1 24
#define SENSOR_ENTRY_2 36
#define SENSOR_EXIT_2 37
#define LED_WHITE_2 25
#define LED_GREEN 7
#define LED_YELLOW 8
#define LED_RED 9
#define POTENTIOMETER_GAS A0
#define POTENTIOMETER_SMOKE A1
#define PANIC_BUTTON 10

// Praguri pentru incidente
#define MAX_CARS_PER_LANE 3
#define GAS_THRESHOLD 700
#define SMOKE_THRESHOLD 700

// Variabile globale
volatile int carCountLane1 = 0;
volatile int carCountLane2 = 0;
volatile bool gasLeak = false;
volatile bool smokeDetected = false;
volatile bool panicMode = false;
SemaphoreHandle_t sem_carCountLane1;
SemaphoreHandle_t sem_carCountLane2;

// Funcție pentru debounce
bool debounce(int pin) {
    if (digitalRead(pin) == HIGH) {
        vTaskDelay(50 / portTICK_PERIOD_MS); // Debounce de 50 ms
        if (digitalRead(pin) == HIGH) {
            return true;
        }
    }
    return false;
}

// Task pentru verificarea intrării pe sensul 1
void entryCheckTaskLane1(void *pvParameters) {
    while (1) {
        if (debounce(SENSOR_ENTRY_1) && !panicMode) { // Detectăm o mașină nouă la intrare
            if (xSemaphoreTake(sem_carCountLane1, portMAX_DELAY) == pdTRUE) {
                if (carCountLane1 < MAX_CARS_PER_LANE && !gasLeak && !smokeDetected) {
                    carCountLane1++;
                    Serial.print("Intrare detectată pe sensul 1. Număr mașini: ");
                    Serial.println(carCountLane1);
                    digitalWrite(LED_GREEN, HIGH); // Aprindem LED-ul verde pentru intrare
                    vTaskDelay(100 / portTICK_PERIOD_MS); // Puls scurt pe LED
                    digitalWrite(LED_GREEN, LOW);
                    digitalWrite(LED_WHITE_1, HIGH); // LED alb rămâne aprins
                }
                xSemaphoreGive(sem_carCountLane1);
            }
            vTaskDelay(5000 / portTICK_PERIOD_MS); // Timeout de 2 secundă între detectări
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Verificare periodică rapidă
    }
}

// Task pentru verificarea ieșirii pe sensul 1
void exitCheckTaskLane1(void *pvParameters) {
    while (1) {
        if (debounce(SENSOR_EXIT_1)) { // Detectăm o mașină la ieșire
            if (xSemaphoreTake(sem_carCountLane1, portMAX_DELAY) == pdTRUE) {
                if (carCountLane1 > 0) {
                    carCountLane1--;
                    Serial.print("Ieșire detectată pe sensul 1. Număr mașini: ");
                    Serial.println(carCountLane1);
                    digitalWrite(LED_RED, HIGH); // Aprindem LED-ul roșu pentru ieșire
                    vTaskDelay(100 / portTICK_PERIOD_MS); // Puls scurt pe LED
                    digitalWrite(LED_RED, LOW);
                    if (carCountLane1 == 0) {
                        digitalWrite(LED_WHITE_1, LOW); // Stingem LED-ul alb dacă nu mai sunt mașini
                    }
                }
                xSemaphoreGive(sem_carCountLane1);
            }
            vTaskDelay(5000 / portTICK_PERIOD_MS); // Timeout de 1 secundă între detectări
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Verificare periodică rapidă
    }
}

// Task pentru verificarea intrării pe sensul 2
void entryCheckTaskLane2(void *pvParameters) {
    while (1) {
        if (debounce(SENSOR_ENTRY_2) && !panicMode) { // Detectăm o mașină nouă la intrare
            if (xSemaphoreTake(sem_carCountLane2, portMAX_DELAY) == pdTRUE) {
                if (carCountLane2 < MAX_CARS_PER_LANE && !gasLeak && !smokeDetected) {
                    carCountLane2++;
                    Serial.print("Intrare detectată pe sensul 2. Număr mașini: ");
                    Serial.println(carCountLane2);
                    digitalWrite(LED_GREEN, HIGH); // Aprindem LED-ul verde pentru intrare
                    vTaskDelay(100 / portTICK_PERIOD_MS); // Puls scurt pe LED
                    digitalWrite(LED_GREEN, LOW);
                    digitalWrite(LED_WHITE_2, HIGH); // LED alb rămâne aprins
                }
                xSemaphoreGive(sem_carCountLane2);
            }
            vTaskDelay(5000 / portTICK_PERIOD_MS); // Timeout de 1 secundă între detectări
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Verificare periodică rapidă
    }
}

// Task pentru verificarea ieșirii pe sensul 2
void exitCheckTaskLane2(void *pvParameters) {
    while (1) {
        if (debounce(SENSOR_EXIT_2)) { // Detectăm o mașină la ieșire
            if (xSemaphoreTake(sem_carCountLane2, portMAX_DELAY) == pdTRUE) {
                if (carCountLane2 > 0) {
                    carCountLane2--;
                    Serial.print("Ieșire detectată pe sensul 2. Număr mașini: ");
                    Serial.println(carCountLane2);
                    digitalWrite(LED_RED, HIGH); // Aprindem LED-ul roșu pentru ieșire
                    vTaskDelay(100 / portTICK_PERIOD_MS); // Puls scurt pe LED
                    digitalWrite(LED_RED, LOW);
                    if (carCountLane2 == 0) {
                        digitalWrite(LED_WHITE_2, LOW); // Stingem LED-ul alb dacă nu mai sunt mașini
                    }
                }
                xSemaphoreGive(sem_carCountLane2);
            }
            vTaskDelay(5000 / portTICK_PERIOD_MS); // Timeout de 1 secundă între detectări
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Verificare periodică rapidă
    }
}

// Task pentru monitorizarea incidentelor
void incidentMonitoring(void *pvParameters) {
    while (1) {
        int gasValue = analogRead(POTENTIOMETER_GAS);
        int smokeValue = analogRead(POTENTIOMETER_SMOKE);

        gasLeak = gasValue > GAS_THRESHOLD;
        smokeDetected = smokeValue > SMOKE_THRESHOLD;

        if (gasLeak) {
            Serial.println("Scurgere de gaze detectată!");
        }
        if (smokeDetected) {
            Serial.println("Fum detectat!");
        }

        if (gasLeak || smokeDetected || carCountLane1 >= MAX_CARS_PER_LANE || carCountLane2 >= MAX_CARS_PER_LANE || panicMode) {
            digitalWrite(LED_YELLOW, HIGH); // Aprindem LED-ul galben pentru alertă
        } else {
            digitalWrite(LED_YELLOW, LOW); // Stingem LED-ul galben dacă nu mai este alertă
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Task pentru monitorizarea butonului de panică
void panicButtonTask(void *pvParameters) {
    pinMode(PANIC_BUTTON, INPUT_PULLUP);
    bool lastState = digitalRead(PANIC_BUTTON);

    while (1) {
        bool currentState = digitalRead(PANIC_BUTTON);

        if (currentState == LOW && lastState == HIGH) {
            vTaskDelay(50 / portTICK_PERIOD_MS); // Debounce
            if (digitalRead(PANIC_BUTTON) == LOW) {
                panicMode = !panicMode;
                if (panicMode) {
                    Serial.println("ALERTĂ: Buton de panică activat!");
                } else {
                    Serial.println("ALERTĂ: Buton de panică dezactivat!");
                }
            }
        }

        lastState = currentState;
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

// Task pentru afișare
void displayTask(void *pvParameters) {
    while (1) {
        Serial.print("\nNumăr mașini pe sensul 1: ");
        Serial.println(carCountLane1);
        Serial.print("Număr mașini pe sensul 2: ");
        Serial.println(carCountLane2);
        vTaskDelay(2500 / portTICK_PERIOD_MS);
    }
}

volatile bool case1Executed = false;
volatile bool case2Executed = false;
volatile bool case3Executed = false;
volatile bool case4Executed = false;


// CAZ1: 
// INTRARE MASINA PE SENS 1
// INTRARE MASINA PE SENS 1
void case1Task(void *pvParameters) {
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    if (!case1Executed) {
        case1Executed = true; // Marcăm task-ul ca executat
        vTaskDelay(1000 / portTICK_PERIOD_MS);
            if (carCountLane1 < MAX_CARS_PER_LANE && carCountLane2 < MAX_CARS_PER_LANE) {
                Serial.print("Mașină 1 intră pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1++;
                Serial.println(carCountLane1);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);


                Serial.print("Mașină 2 intră pe sensul 1. Număr mașini (sens 2): ");
                carCountLane1++;
                Serial.println(carCountLane1);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        Serial.print("Caz 1 completat cu succes.");

        vTaskDelete(NULL); // Terminăm task-ul
    }
}

// CAZ2: 
// INTRARE MASINA PE SENS 1
// INTRARE MASINA PE SENS 2
// INTRARE MASINA PE SENS 1
// INTRARE MASINA PE SENS 2
// NIVEL GAZ 650 (OK)
// IESIRE MASINA DE PE SENS 1
// IESIRE MASINA DE PE SENS 2
// IESIRE MASINA DE PE SENS 1
// IESIRE MASINA DE PE SENS 2
void case2Task(void *pvParameters) {
  vTaskDelay(3000 / portTICK_PERIOD_MS);
    if (!case2Executed) {
        case2Executed = true; // Marcăm task-ul ca executat
        {
            Serial.println("Simulare caz 1: Două mașini intră pe sensul 1, doua masini pe sensul 2, senzorul de gaz detecteaza nivel de gaz (sub treshhold) si apoi ies masinile de pe ambele sensuri.");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            if (carCountLane1 < MAX_CARS_PER_LANE && carCountLane2 < MAX_CARS_PER_LANE) {
                Serial.print("Mașină 1 intră pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1++;
                Serial.println(carCountLane1);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);


                Serial.print("Mașină 2 intră pe sensul 2. Număr mașini (sens 2): ");
                carCountLane2++;
                Serial.println(carCountLane2);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                Serial.print("Mașină 3 intră pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1++;
                Serial.println(carCountLane1);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);


                Serial.print("Mașină 4 intră pe sensul 2. Număr mașini (sens 2): ");
                carCountLane2++;
                Serial.println(carCountLane2);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

            // Creștem nivelul de gaz, dar sub pragul de alarmă
            int gasValue = 650; // Valoare ajustată pentru a nu porni alarma
            Serial.print("Nivel gaz: ");
            Serial.println(gasValue);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

                Serial.print("Mașină 1 iese de pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1--;
                Serial.println(carCountLane1);
                digitalWrite(LED_RED, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_RED, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);


                Serial.print("Mașină 2 iese de pe sensul 2. Număr mașini (sens 2): ");
                carCountLane2--;
                Serial.println(carCountLane2);
                digitalWrite(LED_RED, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_RED, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                Serial.print("Mașină 3 iese de pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1--;
                Serial.println(carCountLane1);
                digitalWrite(LED_RED, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_RED, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);


                Serial.print("Mașină 4 iese de pe sensul 2. Număr mașini (sens 2): ");
                carCountLane2--;
                Serial.println(carCountLane2);
                digitalWrite(LED_RED, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_RED, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        Serial.print("Caz 2 completat cu succes.");
        vTaskDelete(NULL); // Terminăm task-ul
    }
}
}


void case3Task(void *pvParameters) {
  vTaskDelay(3000 / portTICK_PERIOD_MS);
    if (!case3Executed) {
        case3Executed = true; // Marcăm task-ul ca executat
        if (xSemaphoreTake(sem_carCountLane1, portMAX_DELAY) == pdTRUE && xSemaphoreTake(sem_carCountLane2, portMAX_DELAY) == pdTRUE) {
            Serial.println("Simulare caz 3: Doua mașini intră pe sensul 1, senzorul de gaz detecteaza nivel de gaz (peste treshhold), urmand iesirea masinilor.");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            if (carCountLane1 < MAX_CARS_PER_LANE && carCountLane2 < MAX_CARS_PER_LANE) {
                Serial.print("Mașină 1 intră pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1++;
                Serial.println(carCountLane1);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);


                Serial.print("Mașină 2 intră pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1++;
                Serial.println(carCountLane1);
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                // Creștem nivelul de gaz, dar sub pragul de alarmă
                int gasValue = 950; // Valoare ajustată pentru a nu porni alarma
                Serial.print("Nivel gaz: ");
                Serial.println(gasValue);
                vTaskDelay(1000 / portTICK_PERIOD_MS);

                Serial.print("Mașină 3 intră pe sensul 1. Număr mașini (sens 1): ");
                if (gasValue > 700)
                  Serial.print("\nIntrare blocata: gaz detectat.");
                  else{
                    carCountLane1++;
                    Serial.println(carCountLane1);
                    digitalWrite(LED_GREEN, HIGH);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GREEN, LOW);
                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                  }

                Serial.print("Mașină 1 iese de pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1--;
                Serial.println(carCountLane1);
                digitalWrite(LED_RED, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_RED, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                Serial.print("Mașină 2 iese de pe sensul 1. Număr mașini (sens 1): ");
                carCountLane1--;
                Serial.println(carCountLane1);
                digitalWrite(LED_RED, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_RED, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

        } 
        Serial.print("Caz 3 completat cu succes.");
        vTaskDelete(NULL); // Terminăm task-ul
    }
}
}


void case4Task(void *pvParameters) {
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    if (!case4Executed) {
        case4Executed = true; // Marcăm task-ul ca executat
        vTaskDelay(1000 / portTICK_PERIOD_MS);
            if (carCountLane1 < MAX_CARS_PER_LANE && carCountLane2 < MAX_CARS_PER_LANE) {
                Serial.print("Mașină 1 intră pe sensul 1.");
                carCountLane1++;
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                digitalWrite(LED_YELLOW, HIGH);
                Serial.print("Se apasa butonul de panica.");
                vTaskDelay(3000 / portTICK_PERIOD_MS);

                Serial.print("Mașină 1 iese de pe sensul 1.");
                carCountLane1--;
                digitalWrite(LED_GREEN, HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_GREEN, LOW);
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                digitalWrite(LED_YELLOW, LOW);
                Serial.print("Se apasa butonul de panica.(pt revenire)"); 
                vTaskDelay(5000 / portTICK_PERIOD_MS);
        
        Serial.print("Caz 4 completat cu succes.");
        vTaskDelete(NULL); // Terminăm task-ul
    }
}
}


// Configurarea sistemului
void setup() {
    Serial.begin(9600);
    Serial.println("Sistem de monitorizare a tunelului pornit.");

    // Configurare pini
    pinMode(SENSOR_ENTRY_1, INPUT);
    pinMode(SENSOR_EXIT_1, INPUT);
    pinMode(LED_WHITE_1, OUTPUT);
    pinMode(SENSOR_ENTRY_2, INPUT);
    pinMode(SENSOR_EXIT_2, INPUT);
    pinMode(LED_WHITE_2, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(POTENTIOMETER_GAS, INPUT);
    pinMode(POTENTIOMETER_SMOKE, INPUT);

    sem_carCountLane1 = xSemaphoreCreateMutex();
    sem_carCountLane2 = xSemaphoreCreateMutex();

    xTaskCreate(entryCheckTaskLane1, "Entry Check Task Lane 1", 128, NULL, 1, NULL);
    xTaskCreate(exitCheckTaskLane1, "Exit Check Task Lane 1", 128, NULL, 1, NULL);
    xTaskCreate(entryCheckTaskLane2, "Entry Check Task Lane 2", 128, NULL, 1, NULL);
    xTaskCreate(exitCheckTaskLane2, "Exit Check Task Lane 2", 128, NULL, 1, NULL);
    xTaskCreate(incidentMonitoring, "Incident Monitoring", 128, NULL, 1, NULL);
    xTaskCreate(displayTask, "Display Task", 128, NULL, 1, NULL);
    xTaskCreate(panicButtonTask, "Panic Button Task", 128, NULL, 1, NULL);

    // Alege cazul dorit:
  //xTaskCreate(case1Task, "Case 1 Task", 128, NULL, 1, NULL);
  //xTaskCreate(case2Task, "Case 2 Task", 128, NULL, 1, NULL);
  //xTaskCreate(case3Task, "Case 3 Task", 128, NULL, 1, NULL);
  //xTaskCreate(case4Task, "Case 4 Task", 128, NULL, 1, NULL);

}

void loop() {
    // Gol, cazurile sunt simulate în setup
}
