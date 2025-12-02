#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

WiFiServer tcpServer(23);          // Serveur TCP pour la communication série
WiFiClient tcpClient;

ESP8266WebServer webServer(80);    // Serveur HTTP

// Fonction appelée sur la racine du serveur web
void handleRoot() {
  String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>ESP8266 Web Server</title></head><body>";
  page += "<h1>Bienvenue sur TeensyBridge</h1>";
  page += "<p>Station connectée et serveur web actif.</p>";
  page += "</body></html>";
  webServer.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);
  
  // Création du point d’accès WiFi avec mot de passe
  WiFi.softAP("TeensyBridge", "12345678");

  // Démarrage serveur TCP
  tcpServer.begin();
  tcpServer.setNoDelay(true);
  
  // Démarrage serveur web HTTP
  webServer.on("/", handleRoot);  // Définit la page à servir sur "/"
  webServer.begin();

  Serial.println("WiFi AP started. Waiting for connections...");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());  // Affiche l'IP, normalement 192.168.4.1
}

void loop() {
  // Gérer les connexions serveur web
  webServer.handleClient();

  // Gérer la connexion TCP
  if (!tcpClient || !tcpClient.connected()) {
    WiFiClient newClient = tcpServer.available();
    if (newClient) {
      tcpClient = newClient;
      Serial.println("TCP Client connected");
    }
  }

  if (tcpClient && tcpClient.connected()) {
    // Du WiFi vers Teensy
    while (tcpClient.available()) {
      char c = tcpClient.read();
      Serial.write(c);
    }

    // Du Teensy vers WiFi
    while (Serial.available()) {
      char c = Serial.read();
      tcpClient.write(c);
    }
  }
}
