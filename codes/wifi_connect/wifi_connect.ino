#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "Programmer Bose 2.4";
const char* password = "Pneucis@202128";

void setup() {
  Serial.begin(115200);
 

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}
void loop(){
  
}
