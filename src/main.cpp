// Source : http://www.iotsharing.com/2017/05/tcp-udp-ip-with-esp32.html
// Source : https://www.dfrobot.com/blog-948.html

#include <Arduino.h>
#include "secrets.h"
#include "Offset.h"
#include "WiFi.h"
#include <PubSubClient.h>
#include <NTPClient.h>

// Nom d'utilisateur et Mot de passe WiFi
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

const char *host = "10.20.60.74";
const int port = 3325;

// --Serveur MTTQ--
// const char server[] = {"cmaisonneuve.cloud.shiftr.io"};
const char server[] = {"broker.hivemq.com"};

// --Nom d'utilisateur--
// const char user[] = {"cmaisonneuve"};

// --Mot de passe--
// const char pass[] = {"FmIOfnkb8BOZaUzw"};
const char payload[] = {"200,125,156"};

// --Initiation objet Cilents WiFi et MQTT--
WiFiClient Client_wifi;
PubSubClient ClientMQTT(server, 1883, Client_wifi);

// --Initiation variable timer--
int lastNow = 0;

const char *str_inp1 = "/cm/biped/1";
const char *str_inp2 = "/cm/biped/2";
const char *str_inp3 = "/cm/biped/3";
const char *str_inp4 = "/cm/biped/4";
const char *str_inp5 = "/cm/biped/5";
const char *str_inp6 = "/cm/biped/6";

String translateEncryptionType(wifi_auth_mode_t encryptionType)
{

  switch (encryptionType)
  {
  case (WIFI_AUTH_OPEN):
    return "Open";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA_PSK";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2_PSK";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA_WPA2_PSK";
  case (WIFI_AUTH_WPA2_ENTERPRISE):
    return "WPA2_ENTERPRISE";
  default:
    return "UNKNOWN";
  }
  return "UNKNOWN";
}

// --Affichage des reseaux disponiles--
void scanNetworks()
{
  int numberOfNetworks = WiFi.scanNetworks();
  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);

  for (int i = 0; i < numberOfNetworks; i++)
  {
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));
    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");
  }
}

// --Connection au reseau choisi--
void connectToNetwork()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");
}

// --Fonction de retour de donne a l'abonnement
void callback(char *topic, byte *payload, unsigned int length)
{
  uint8_t LED = digitalRead (13);
  digitalWrite(13,!LED);
  float angle;
  angle = atof ((const char *)payload);
  uint32_t temps_haut = (angle + 90) * 0.32 + 51.2; 

  if (strcmp(str_inp1, topic) == 0)
  {
    //Serial.printf("Moteur 1\n\r");
     ledcWrite(1, temps_haut);
  }
  else if (strcmp(str_inp2, topic) == 0)
  {
    //Serial.printf("Moteur 2\n\r");
    ledcWrite(2, temps_haut);
  }
  else if (strcmp(str_inp3, topic) == 0)
  {
    //Serial.printf("Moteur 3\n\r");
    ledcWrite(3, temps_haut);
  }
  else if (strcmp(str_inp4, topic) == 0)
  {
    //Serial.printf("Moteur 4\n\r");
    ledcWrite(4, temps_haut);
  }
  else if (strcmp(str_inp5, topic) == 0)
  {
    //Serial.printf("Moteur 5\n\r");
    ledcWrite(5, temps_haut);
  }
  else if (strcmp(str_inp6, topic) == 0)
  {
    //Serial.printf("Moteur 6\n\r");
    ledcWrite(6, temps_haut);
  }

  //Serial.printf("%u\n\r", temps_haut);
  //Serial.printf("%4.2f\n\r", angle);
}

// --Initiation des composants necessaires--
void setup()
{
  // --Port serie--
  Serial.begin(115200);

  // --LED--
  pinMode(13, OUTPUT);
  // --Moteur1--
  pinMode(19, OUTPUT);
  // --Moteur2--
  pinMode(18, OUTPUT);
  // --Moteur3--
  pinMode(5, OUTPUT);
  // --Moteur4--
  pinMode(21, OUTPUT);
  // --Moteur5--
  pinMode(17, OUTPUT);
  // --Moteur6--
  pinMode(16, OUTPUT);

  // Configurer fonction du PWM
  ledcSetup(1, 50, 10);
  ledcSetup(2, 50, 10);
  ledcSetup(3, 50, 10);
  ledcSetup(4, 50, 10);
  ledcSetup(5, 50, 10);
  ledcSetup(6, 50, 10);

  // Attache le canal au GPIO pour etre controller
  ledcAttachPin(19, 1);
  ledcAttachPin(18, 2);
  ledcAttachPin(5, 3);
  ledcAttachPin(21, 4);
  ledcAttachPin(17, 5);
  ledcAttachPin(16, 6);

  // --Pointeur fonction de retour--
  ClientMQTT.setCallback(callback);

  // --Connection WiFi--
  scanNetworks();
  connectToNetwork();

  // --Affichage caracteristiques WiFi--
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  // --Connection au client MQTT--
  // ClientMQTT.connect("client-id-maisonneuve", user, pass);
  ClientMQTT.connect("GREGORY");

  // --Abonnement au sujet--
  ClientMQTT.subscribe("/cm/biped/#");
}

// --Boucle principale--
void loop()
{
  if (!ClientMQTT.connected())
  {
    Serial.printf("Client MQTT deconnecte\n\r");
  }
  ClientMQTT.loop();
  //Test offset ne pas touché
  /*ledcWrite(1, 80); //off +3.2
  ledcWrite(2, 76.8); 
  ledcWrite(3, 76.8);
  ledcWrite(4, 76.8);
  ledcWrite(5, 76.8); 
  ledcWrite(6, 78); //off +1.2 */
}
