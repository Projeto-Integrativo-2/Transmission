#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>
#endif


RH_ASK driver(2000, 23, 22, 0); // Configuração do módulo ASK rx,tx

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  if (!driver.init()) {
    Serial.println("Falha ao iniciar o driver");
    
  }
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  uint8_t meuid = 0xBB;

  if (driver.recv(buf, &buflen)) {
    digitalWrite(2, HIGH);
    buf[buflen] = '\0';  // Garante que seja uma string válida

    // Converte buffer para String do Arduino
    String receivedStr = String((char*)buf);

    Serial.println(receivedStr);
    char intercom[20];  // Certifique-se de que tem espaço suficiente
    sprintf(intercom, "%s, meu id:0x%X", receivedStr.c_str(), meuid);
    // Envia número como string
    driver.send((uint8_t *)intercom, strlen(intercom));
    Serial.println(intercom);
    Serial.println("");
    driver.waitPacketSent();
  }
  else{
    digitalWrite(2, LOW);
  }
}
