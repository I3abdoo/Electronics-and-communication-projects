#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Servo_ESP32.h>
Servo_ESP32 servo;

const char* ssid = "Oufa";
const char* password = "035904246@oufa";

WebServer server(80);

const int entryGatePin = 22;
const int car1Pin = 34;
const int car2Pin = 35;
const int car3Pin = 13;
const int car4Pin = 18;
const int servoPin = 4;


bool entryGateState = LOW;
bool car1State = HIGH;
bool car2State = HIGH;
bool car3State = HIGH;
bool car4State = HIGH;

void handleRoot() {
  String html = "<html><head><title>Smart Parking System</title></head><body>";
  html += "<h1>Smart Parking System</h1>";
  html += "<p>Entry gate: " + String(entryGateState == HIGH ? "OPEN" : "CLOSED") + "</p>";
  html += "<p>Car 1: " + String(car1State == LOW ? "VACANT" : "OCCUPIED") + "</p>";
  html += "<p>Car 2: " + String(car2State == LOW ? "VACANT" : "OCCUPIED") + "</p>";
  html += "<p>Car 3: " + String(car3State == LOW ? "VACANT" : "OCCUPIED") + "</p>";
  html += "<p>Car 4: " + String(car4State == LOW ? "VACANT" : "OCCUPIED") + "</p>";
  html += "<p><a href=\"/open\">Open gate</a> / <a href=\"/close\">Close gate</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOpen() {
  if (car1State == HIGH && car2State == HIGH && car3State == HIGH && car4State == HIGH) {
    String html = "<html><body><h1>NO PLACE</h1></body></html>";
    server.send(200, "text/html", html);
  } else {
    servo.write(90);
    entryGateState = HIGH;
    server.sendHeader("Location", "/");
    server.send(303);
  }
}

void handleClose() {
  servo.write(0);
  entryGateState = LOW;
  server.sendHeader("Location", "/");
  server.send(303);
}
void openDoor() {
  // Check if car is detected by the IR sensor
  if (digitalRead(entryGatePin) == LOW) {
    // Open the door if it's closed
    if (entryGateState == LOW) {
      handleOpen();
      delay(1000);
      handleClose();
    }
  }
}

void setup() {
  pinMode(entryGatePin, INPUT);
  pinMode(car1Pin, INPUT_PULLUP);
  pinMode(car2Pin, INPUT_PULLUP);
  pinMode(car3Pin, INPUT_PULLUP);
  pinMode(car4Pin, INPUT_PULLUP);
  
  servo.attach(servoPin);
  servo.write(0);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/open", HTTP_GET, handleOpen);
  server.on("/close", HTTP_GET, handleClose);
  
  server.begin();
}

void loop() {
  car1State = digitalRead(car1Pin) == LOW;
  car2State = digitalRead(car2Pin) == LOW;
  car3State = digitalRead(car3Pin) == LOW;
  car4State = digitalRead(car4Pin) == LOW;

if (entryGateState == HIGH && (car1State == HIGH && car2State == HIGH && car3State == HIGH && car4State == HIGH)) {
     servo.write(0);
     entryGateState = LOW;
  }
  openDoor();

  server.handleClient();
}