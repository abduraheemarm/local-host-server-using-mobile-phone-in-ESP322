#include <WiFi.h>
#include <WebServer.h>

// 1. Replace with your actual network credentials
const char* ssid     = "realme C21Y";
const char* password = "12345678";

// GPIO 2 is the built-in LED on most ESP32 boards.
const int ledPin = 2; 

// Create a web server object on port 80
WebServer server(80);

// HTML, CSS, and JS code for the smooth slider interface
String getHTMLPage() {
  String page = "<!DOCTYPE html><html>";
  page += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  page += "<style>html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}";
  page += ".button { display: inline-block; padding: 15px 40px; font-size: 24px; margin: 10px; cursor: pointer; text-align: center; text-decoration: none; outline: none; color: #fff; background-color: #4CAF50; border: none; border-radius: 15px; box-shadow: 0 9px #999;}";
  page += ".button:active { background-color: #3e8e41; box-shadow: 0 5px #666; transform: translateY(4px); }";
  page += ".button2 {background-color: #f44336;}"; 
  page += ".slider { width: 80%; max-width: 300px; height: 20px; margin: 20px auto; display: block; }";
  page += "</style>";
  
  // JavaScript to send background commands without reloading the page
  page += "<script>";
  page += "function changeBrightness(val) { fetch('/led/brightness?value=' + val); }";
  page += "function turnOn() { document.getElementById('slide').value = 255; fetch('/led/on'); }";
  page += "function turnOff() { document.getElementById('slide').value = 0; fetch('/led/off'); }";
  page += "</script></head>";
  
  page += "<body><h1>ESP32 Dimmer Web Server</h1>";
  page += "<p><button class=\"button\" onclick=\"turnOn()\">TURN ON</button></p>";
  page += "<p><button class=\"button button2\" onclick=\"turnOff()\">TURN OFF</button></p>";
  page += "<h3>Fade / Brightness Control</h3>";
  page += "<p><input type=\"range\" min=\"0\" max=\"255\" value=\"0\" class=\"slider\" id=\"slide\" onchange=\"changeBrightness(this.value)\"></p>";
  page += "</body></html>";
  return page;
}

// Handler functions
void handleRoot() {
  server.send(200, "text/html", getHTMLPage());
}

void handleLEDOn() {
  analogWrite(ledPin, 255); // Full brightness
  server.send(200, "text/plain", "ON");
}

void handleLEDOff() {
  analogWrite(ledPin, 0); // Turned off
  server.send(200, "text/plain", "OFF");
}

void handleBrightness() {
  if (server.hasArg("value")) {
    int brightness = server.arg("value").toInt();
    analogWrite(ledPin, brightness); // Set brightness between 0 and 255
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing Value");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Set up pin for PWM output
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0); 

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Define server endpoints
  server.on("/", handleRoot);
  server.on("/led/on", handleLEDOn);
  server.on("/led/off", handleLEDOff);
  server.on("/led/brightness", handleBrightness); // New endpoint for slider

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}