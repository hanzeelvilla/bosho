#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "D:\Hanzeel\FaCUMtad\Semestre 4\proyecto_esli\MQTT\MQTT.ino" //RUTA DEL ARCHIVO MQTT

//MOTORES
#define AIN1 15
#define AIN2 2
#define BIN1 4
#define BIN2 5

bool izquierdaActivo = false;
bool derechaActivo = false;
bool adelanteActivo = false;
bool atrasActivo = false;

//Sensor seguidor de línea
#define sensD 34
#define sensI 35

//ULTRASONICO
#define TRIGGER 33
#define ECHO 32
#define SOUND_SPEED 0.034
long duration;
bool ultraSonicoActivo = false; // Variable para el estado del sensor

//AUDIOSONORO
#define buzzer 23
#define ledD 19
#define ledI 21

//MQTT
 MQTT mqtt;
 long lastMsg = 0;

//ULTRASONICO
void evitar_obstaculos() {
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);

  duration = pulseIn(ECHO, HIGH);
  float distancia = duration * SOUND_SPEED/2;
  Serial.print("Distancia: ");
  Serial.println(distancia);
  
  if(distancia < 10) {
    atras(255);
    delay(1000);
    derecha(255);
    delay(1000);
  } else {
    adelante(255);
  }
}

//SEGUIDOR DE LÍNEA
void seguidor_linea(){
    int valorD = analogRead(sensD);
    int valorI = analogRead(sensI);

    //INTENTO CON VELOCIDAD A 240 
    if(valorD < 4000 && valorI < 4000){
      adelante(220);
      Serial.println("Adelante");
    }
    else if(valorD > 4000 && valorI < 4000){
      izquierda(210);
      itermitenteI();
      Serial.println("Izquierda");
    }
    else if(valorI > 4000 && valorD < 4000){
      derecha(210);
      itermitenteD();
      Serial.println("Derecha");
    }
  }

   void print_valores_seguidor(int valD, int valI){
    Serial.print("Derecha: ");
    Serial.println(valD);
    Serial.print("Izquierda: ");
    Serial.println(valI);
    delay(1000);
  }

//CANCIONES
void tusa(){
  tone(buzzer, 880);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, LOW);
  delay(300);
  noTone(500);
  tone(buzzer, 740);
  digitalWrite(ledI, LOW);
  digitalWrite(ledD, HIGH);
  delay(300);
  noTone(500);
  tone(buzzer, 659.25);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, LOW);
  delay(300);
  noTone(500);
  tone(buzzer, 587.33);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, HIGH);
  delay(700);
  digitalWrite(ledI, LOW);
  digitalWrite(ledD, LOW);
  noTone(1000);
  delay(500);

  tone(buzzer, 587.33);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, HIGH);
  delay(300);
  noTone(500);
  delay(250);

  tone(buzzer, 587.33);
  digitalWrite(ledI, LOW);
  digitalWrite(ledD, HIGH);
  delay(300);
  noTone(500);
  tone(buzzer, 659.25);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, LOW);
  delay(300);
  noTone(500);
  tone(buzzer, 740);
  digitalWrite(ledI, LOW);
  digitalWrite(ledD, HIGH);
  delay(300);
  noTone(500);
  tone(buzzer, 880);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, HIGH);
  delay(700);
  noTone(500);
  
  digitalWrite(ledI, LOW);
  digitalWrite(ledD, LOW);
  noTone(1000);
  delay(500);
  tone(buzzer, 880);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, HIGH);
  delay(700);
  noTone(500);
  digitalWrite(ledI, LOW);
  digitalWrite(ledD, LOW);
}

void autodestruccion(){
  for(int i=0; i<3; i++){
    tone(buzzer, 1000);
    digitalWrite(ledI, HIGH);
    digitalWrite(ledD, HIGH);
    delay(500);
    noTone(buzzer);
    digitalWrite(ledI, LOW);
    digitalWrite(ledD, LOW);
    delay(500);
  }
  
  for(int i=0; i<5; i++){
    tone(buzzer, 1000);
    digitalWrite(ledI, HIGH);
    digitalWrite(ledD, HIGH);
    delay(300);
    noTone(buzzer);
    digitalWrite(ledI, LOW);
    digitalWrite(ledD, LOW);
    delay(300);
  }

  for(int i=0; i<10; i++){
    tone(buzzer, 1000);
    digitalWrite(ledI, HIGH);
    digitalWrite(ledD, HIGH);
    delay(90);
    noTone(buzzer);
    digitalWrite(ledI, LOW);
    digitalWrite(ledD, LOW);
    delay(90);
  }

  tone(buzzer, 1000);
  digitalWrite(ledI, HIGH);
  digitalWrite(ledD, HIGH);
  delay(3000);
  noTone(buzzer);
  digitalWrite(ledI, LOW);
  digitalWrite(ledD, LOW);
  delay(2000);
}

//DIRECCIONES MOTORES
 void parar(){
    analogWrite(AIN1, 0); 
    analogWrite(AIN2, 0);
    analogWrite(BIN1, 0); 
    analogWrite(BIN2, 0);
  } 

  void adelante(int velocidad){
    analogWrite(AIN1, velocidad);  //-10
    analogWrite(AIN2, LOW);
    analogWrite(BIN1, velocidad); 
    analogWrite(BIN2, LOW);
  }

  void atras(int velocidad){
    analogWrite(AIN1, LOW); 
    analogWrite(AIN2, velocidad);
    analogWrite(BIN1, LOW); 
    analogWrite(BIN2, velocidad);
  }

  void derecha(int velocidad){
    analogWrite(AIN1, velocidad); 
    analogWrite(AIN2, LOW);
    analogWrite(BIN1, LOW); 
    analogWrite(BIN2, LOW);
  }

  void izquierda(int velocidad){
    analogWrite(AIN1, LOW); 
    analogWrite(AIN2, LOW);
    analogWrite(BIN1, velocidad); 
    analogWrite(BIN2, LOW);
  }

  void izquierda_atras(int velocidad){
    analogWrite(AIN1, LOW); 
    analogWrite(AIN2, LOW);
    analogWrite(BIN1, LOW); 
    analogWrite(BIN2, velocidad);
  }

  void diagonalIzquierda(int velocidad) {
  analogWrite(AIN1, velocidad); //-20
  analogWrite(AIN2, LOW);
  analogWrite(BIN1, velocidad); 
  analogWrite(BIN2, LOW);
}

void diagonalDerecha(int velocidad) {
  analogWrite(AIN1, velocidad); 
  analogWrite(AIN2, LOW);
  analogWrite(BIN1, velocidad); //-40
  analogWrite(BIN2, LOW);
}

void diagonalIzquierdaA(int velocidad) {
  analogWrite(AIN1, LOW); 
  analogWrite(AIN2, velocidad);
  analogWrite(BIN1, LOW); 
  analogWrite(BIN2, velocidad); //-20
}

void diagonalDerechaA(int velocidad) {
  analogWrite(AIN1, LOW); 
  analogWrite(AIN2, velocidad); //-60
  analogWrite(BIN1, LOW); 
  analogWrite(BIN2, velocidad);
}

  void probar_motores(int velocidad){
    adelante(velocidad);
    delay(1000);
    parar();
    delay(1000);
    atras(velocidad);
    delay(1000);
    parar();
    delay(1000);
    derecha(velocidad);
    delay(1000);
    parar();
    delay(1000);
    izquierda(velocidad);
    delay(1000);
    parar();
    delay(1000);
    parar();
    delay(1000);
  }

  void bailar(int velocidad){
    derecha(velocidad);
    izquierda_atras(velocidad);
  }

  void itermitenteD() {
    digitalWrite(ledD, HIGH);
    delay(200);
    digitalWrite(ledD, LOW);
  }

  void itermitenteI() {
    digitalWrite(ledI, HIGH);
    delay(200);
    digitalWrite(ledI, LOW);
  }

void setup() {
  Serial.begin(115200);
  
  mqtt.setup_WiFi();
  mqtt.set_MQTT_server();
  mqtt.set_MQTT_callback( );

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  pinMode(sensD, INPUT);
  pinMode(sensI, INPUT);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT); 

  pinMode(buzzer, OUTPUT);
  pinMode(ledI, OUTPUT);
  pinMode(ledD, OUTPUT);
}
 
void loop() {
  delay (10);
  mqtt.reconnect_MQTT();
  long now = millis();
    if (now - lastMsg > 5000) {
      lastMsg = now;
      
      mqtt.publish_MQTT();
    }

  if (adelanteActivo && izquierdaActivo) {
    diagonalDerecha(240);
  } 
  else if (adelanteActivo && derechaActivo) {
    diagonalIzquierda(240);
  } 
  else if (atrasActivo && izquierdaActivo) {
    diagonalIzquierdaA(240);
  } 
  else if (atrasActivo && derechaActivo) {
    diagonalDerechaA(240);
  } 
  else if (adelanteActivo) {
    adelante(240);
  } 
  else if (atrasActivo) {
    atras(220);
  } 
  else if (izquierdaActivo) {
    izquierda(240);
    itermitenteI();
  } 
  else if (derechaActivo) {
    derecha(220);
    itermitenteD();
  } 
  else {
    parar();
  }


  //delay(200);
  /*
  iniciarTrigger();
  distanceCm = calcularDistancia();
  Serial.println(distanceCm);
  */

  //print_valores_seguidor(valorD, valorI); 
  
  //seguidor_linea();
}