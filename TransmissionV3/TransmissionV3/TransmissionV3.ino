#include <Wire.h>
#include "RTClib.h"

// === Pinos e constantes do ultrassom ===
const int trigPin = 5;
const int echoPin = 18;
#define SOUND_SPEED 0.034 // cm/us
#define LIMIAR_CM 20.0    // Distância para "achar algo"

// === PWM ===
const int pwmPin1 = 14;
const int pwmChannel1 = 0;
const int freq1 = 125000; // 125 kHz
const int resolution = 8;
const int dutyCycle = 127; // 50%
const int bitRate = 2000; // 2 kHz
const int bitDuration = 500; // microsegundos por bit (1/2000s)

// === RTC ===
RTC_DS1307 rtc;

// === Setup ===
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicializa PWM
  pinMode(pwmPin1, OUTPUT);
    ledcAttachChannel(pwmPin1, freq1, resolution, pwmChannel1);
    
  // Inicializa RTC
  if (!rtc.begin()) {
    Serial.println("RTC não encontrado!");
    while (1);
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC parado. Ajustando com horário da compilação.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// === Função de transmissão bit a bit ===
void transmitirMensagem(String msg) {
  for (int i = 0; i < msg.length(); i++) {
    char c = msg[i];
    for (int b = 7; b >= 0; b--) {
      bool bit = (c >> b) & 0x01;
      if (bit) {
      ledcWrite(pwmPin1, dutyCycle);
      Serial.print(bit);
    } else {
      ledcWrite(pwmPin1, 0);
      Serial.print(bit);
    }

    delayMicroseconds(bitDuration);
    }
  }
  ledcWrite(pwmPin1, 0); // desliga após envio
}

// === Loop principal ===
void loop() {
  // === Medição da distância ===
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distanceCm = duration * SOUND_SPEED / 2;

  Serial.print("Distância: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  // === Verifica se algo foi detectado ===
  if (distanceCm < LIMIAR_CM) {
    DateTime now = rtc.now();
    String horario = String(now.year()) + "-" + 
                     (now.month() < 10 ? "0" : "") + String(now.month()) + "-" +
                     (now.day() < 10 ? "0" : "") + String(now.day()) + " " +
                     (now.hour() < 10 ? "0" : "") + String(now.hour()) + ":" +
                     (now.minute() < 10 ? "0" : "") + String(now.minute()) + ":" +
                     (now.second() < 10 ? "0" : "") + String(now.second());
                     
    String mensagem = "Achei algo | " + horario;
    Serial.print("Transmitindo: ");
    Serial.println(mensagem);
    transmitirMensagem(mensagem);
  }

  // Espera 1 minuto
  delay(2000);
}
