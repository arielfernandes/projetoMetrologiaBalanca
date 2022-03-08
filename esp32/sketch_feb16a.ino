//Programa: comunicação MQTT com ESP32
//Autor: Pedro Bertoleti

#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

#define DT_PIN 18
#define SCK_PIN 19
//float offset              = 0; //variável para guardar o valor bruto de offset

HX711 balanca;
float calibration_factor = 432087.0588235294;                                                        /* Fator de calibração para ajuste da célula */
float peso;     

char buf[7]               = {0};


#define TOPICO_SUBSCRIBE "INCB_ESP32_recebe_informacao"

#define TOPICO_PUBLISH   "INCB_ESP32_envia_informacao"

#define ID_MQTT  "INCB_Cliente_MQTT"


const char* ssid = "MY SSID"; /* Add your router's SSID */
const char* password = "PASSWORD"; /*Add the password */

const char* BROKER_MQTT = "broker.hivemq.com";


int BROKER_PORT = 1883;



WiFiClient espClient;
PubSubClient MQTT(espClient);

//Prototypes
void init_serial(void);
void init_wifi(void);
void init_mqtt(void);
void reconnect_wifi(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verifica_conexoes_wifi_mqtt(void);

void setup()
{
  init_scale();
  init_serial();
  init_wifi();

  init_mqtt();
  

}

void init_wifi(void)
{
  delay(10);

  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");
  reconnect_wifi();
}

void init_scale() {
  balanca.begin(DT_PIN, SCK_PIN);
  balanca.set_scale();                                                                      /* seta escala */
  balanca.tare(); 
  long zero_factor = balanca.read_average();

}


void init_serial()
{
  Serial.begin(115200);
}


void init_mqtt(void)
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);

  MQTT.setCallback(mqtt_callback);
}


void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
  String msg;

  //obtem a string do payload recebido
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }
  Serial.print("[MQTT] Mensagem recebida: ");
  Serial.println(msg);
}


void reconnect_mqtt(void)
{
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
    }
  }
}


void reconnect_wifi()
{
  /* se já está conectado a rede WI-FI, nada é feito.
     Caso contrário, são efetuadas tentativas de conexão */
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}


void verifica_conexoes_wifi_mqtt(void)
{
  /* se não há conexão com o WiFI, a conexão é refeita */
  reconnect_wifi();
  /* se não há conexão com o Broker, a conexão é refeita */
  if (!MQTT.connected())
    reconnect_mqtt();
}

void loop()
{


  balanca.set_scale(calibration_factor);                                                    /* a balanca está em função do fator de calibração */

  Serial.print("Peso: ");                                                                   /* Printa "Peso:" na COM */
  peso = balanca.get_units(), 10;                                                           /* imprime peso  */
  if (peso < 0)                                                                            /* se a unidade for menor que 0 será considerado 0 */
  {
    peso = 0.00;                                                                            /* Para o caso do peso ser negativo, o valor apresentado será 0  */
  }                                

  
  memset(buf,0,7);
  dtostrf(peso,sizeof(peso),3,buf);

                                                 
  Serial.print(peso);                                                                       /* Printa o peso na serial  */
  Serial.print(" kg");                                                                      /* Printa "kg" na serial   */
  //Serial.print(" Fator de calibração: ");                                                   /* Printa "Fator de calibração:" na serial */
  Serial.print(calibration_factor);                                                         /* Printa o fator de calibração na serial */
  Serial.println();                                                                         /* Pula linha no serial */
  delay(500);     

  /* garante funcionamento das conexões WiFi e ao broker MQTT */
  verifica_conexoes_wifi_mqtt();
  /* Envia frase ao broker MQTT */
  MQTT.publish(TOPICO_PUBLISH, buf);

  /* keep-alive da comunicação com broker MQTT */
  MQTT.loop();
  if(!buf){
    verifica_conexoes_wifi_mqtt();
  }
  /* Agurda 1 segundo para próximo envio */
  delay(1000);
}
