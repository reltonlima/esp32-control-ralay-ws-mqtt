#include <WiFi.h>
#include <PubSubClient.h>

// Configurações Wi-Fi
const char* ssid = "YOU_SSID";
const char* password = "YOUR_PASSWORD";

// Configurações MQTT
const char* mqtt_server = "YOUR_MQTT_BROKER_IP_OR_HOSTNAME";
// const int mqtt_port = 1883; // Porta padrão do MQTT - Altere se precisar
const char* topic = "esp32/relay/command";  // Tópico para receber comandos
const char* status_topic = "esp32/relay/status"; // Tópico para enviar status

WiFiClient espClient;
PubSubClient client(espClient);

// Configuração do relé
const int relayPin = 23;
bool relayState = false;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  // Controle do relé
  if (strcmp(message, "on") == 0) {
    digitalWrite(relayPin, HIGH);
    relayState = true;
    client.publish(status_topic, "Relay: ON");
  } else if (strcmp(message, "off") == 0) {
    digitalWrite(relayPin, LOW);
    relayState = false;
    client.publish(status_topic, "Relay: OFF");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    
    // Cria um ID de cliente randomico
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
      client.subscribe(topic);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}