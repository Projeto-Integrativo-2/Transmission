#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>
#endif

RH_ASK driver(2000, 23, 22, 0); // ESP8266 or ESP32: do not use pin 11 or 2

void setup() {
  Serial.begin(9600);
  if (!driver.init()) {
    Serial.println("Falha ao iniciar o driver");
  }
}

void loop() {
  const char *msg = "DADO";
  uint8_t id = 0xAA;
  // Cria um pacote combinando as duas strings
  char pacote[20];  // Certifique-se de que tem espaço suficiente
  sprintf(pacote, "id:0x%X msg:%s", id, msg);

  // Envia número como string
  driver.send((uint8_t *)pacote, strlen(pacote));
  driver.waitPacketSent();
  Serial.print("Enviei o pacote: ");
  Serial.println(pacote);
  delay(100);
}
