#include <LiquidCrystal.h>

const int ledVerde = 9;
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>

byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x68};

void callback(char *topic, byte * payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client("m14.cloudmqtt.com", 14064, callback, ethClient);
int const qtdVagas= 2;
long lastReconnectAttempt = 0;
int vagas[qtdVagas];
int contador = 0;


boolean reconnect() {
  Serial.println("reconectando...");
  if (client.connect("arduino", "arduino", "arduino")) {
    digitalWrite(ledVerde, HIGH);
    Serial.println("conectado");
    client.publish("teste", "hello world");
    client.subscribe("teste/#");
  }
  Serial.println("Conectado MQTT");
  return client.connected();
}
void callback(char* topic, byte * payload, unsigned int length) {

  int msg = atoi(payload);
  digitalWrite(ledVerde, LOW);
  delay(100);
  digitalWrite(ledVerde, HIGH);
  delay(100);
  digitalWrite(ledVerde, LOW);
  delay(100);
  digitalWrite(ledVerde, HIGH);
  //  Serial.println(msg);
  //  Serial.println(topic);

  vagas[topic[6] - '0' - 1] = msg;
  int livres = 0;
  int ocupadas = 0;
  
  for (contador = 0 ; contador <= qtdVagas ; contador++) {
    Serial.println("Numero vaga");
    Serial.println(topic[6]);
    Serial.println("Mensagem:");
    Serial.println(vagas[topic[6] - '0' - 1]);
    if (vagas[contador] == 1) {
      livres = livres + 1;
      Serial.println("livres:");
      Serial.println(livres);
    }
    else if (vagas[contador] == 0) {
      ocupadas = ocupadas + 1;
      Serial.println("ocupadas:");
      Serial.println(ocupadas);
    }
  }
  Serial.println("Numero vaga");

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(livres);
  delay(1000);

}

void setup() {
  pinMode(ledVerde, OUTPUT);
  Serial.begin(9600);
  Serial.println("iniciando...");
  Ethernet.begin(mac);
  delay(1500);
  lastReconnectAttempt = 0;

  if (client.connect("arduino", "arduino", "arduino"))
  {
    // Conecta no topic para receber mensagens
    client.subscribe("teste/#");
    digitalWrite(ledVerde, HIGH);
    Serial.println("Conectado MQTT");
    delay(50);
  }

  lcd.begin(16, 2);
  lcd.print("Vagas Livres:");
}

void loop() {

  if (!client.connected()) {
    digitalWrite(ledVerde, LOW);
    long now = millis();
    if ((now - lastReconnectAttempt) > 5000) {
      Serial.println("reconectando...");
      lastReconnectAttempt = now;

      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
  }
}


