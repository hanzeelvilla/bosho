#define AIN1 4
#define AIN2 5
#define BIN1 7
#define BIN2 8

 void parar(){
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, LOW);
  } 

  void adelante(){
    digitalWrite(AIN1, HIGH); 
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, HIGH); 
    digitalWrite(BIN2, LOW);
  }

  void atras(){
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, HIGH);
  }

  void derecha(){
    digitalWrite(AIN1, HIGH); 
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, LOW);
  }

  void izquierda(){
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, HIGH); 
    digitalWrite(BIN2, LOW);
  }

void setup() {
  Serial.begin(9600);
  
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
}
 
void loop() {
}
