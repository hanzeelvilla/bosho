//Librería para convertir a JSON el mensaje recibido
#include <ArduinoJson.h>

/* Se incluye librería y se crea objeto para la configuración del cliente WiFi */
#include <WiFi.h>
  WiFiClient ESP32_WIFI;

/* Se incluye librería paa utilizar el protocolo de telemetría MQTT, con el objeto correspondiente */
#include "PubSubClient.h"
  PubSubClient client ( ESP32_WIFI );

/* Directivas de configuración de red y parámetros MQTT */
#define HOTSPOT_WIFI  "INFINITUM02A0"            /* Nombre del punto de acceso a conectarse */
#define HOTSPOT_PWD   "RACbe21akl"                       /* Contraseña de la red */
#define MQTT_SERVER   "test.mosquitto.org"     /* Dirección del Broker MQTT */
#define MQTT_PORT     1883                     /* Puerto del Broker MQTT */
#define TXTOPIC       "/TX_ELBOSHO"         /* Nombre del topic de publicación */
#define RXTOPIC       "/RX_ELBOSHO"         /* Nombre del topic de suscripción */

float latitud = 19.256878;
float longitud = -103.752708;

//VARIABLES EXTERNAS
extern bool ultraSonicoActivo; 
extern bool izquierdaActivo;
extern bool derechaActivo;
extern bool adelanteActivo;
extern bool atrasActivo;
extern bool claxonActivo; 

/* Clase para la gestión de la red WiFi y el protocolo MQTT */
class MQTT {
  
  public:
  /* MÉTODOS */
  public:
    void subscribe_MQTT(void);            /* Conecta al usuario como un suscriptor MQTT */
    void publish_MQTT(void);              /* Conecta al usuario como un pulicador MQTT */
    void reconnect_MQTT(void);            /* Realiza la reconexión al servidor MQTT */
    void setup_WiFi(void);                /* Configura lo necesario para conectarse a una red WiFi */ 
    void set_MQTT_server(void);           /* Establece la dirección del Broker MQTT a utilizar */
    void set_MQTT_callback(void);         /* Se configura la obtención de la información como suscriptor */
    static void callback(char*, byte* ,unsigned int); /* Método que obtiene la trama realizada en las suscripciones */
  };

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ set_MQTT_server ( void ) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*                      Función que configura el puerto y la dirección del Broker MQTT a utilizar.                     *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void MQTT::set_MQTT_server(void ){
  client.setServer(MQTT_SERVER, MQTT_PORT);            /* Función librería PubsubClient */
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ set_MQTT_callback ( void ) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*                      Función que configura la función que recibirá la trama proveniente de los datos              *
*                      mostrados en la suscripción .                                                                *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void MQTT::set_MQTT_callback(void) {
  client.setCallback (this -> callback);                 /* Función librería PubsubClient */
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ subscribe_MQTT ( void ) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*                      Función que se suscribe a un topic MQTT, puede tener múltiples suscripciones.                *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void MQTT::subscribe_MQTT(void) {
  client.subscribe(RXTOPIC);                           /* Función librería PubsubClient */
  //client.subscribe( RXTOPIC1 );                          /* Función librería PubsubClient */
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ publish_MQTT ( void ) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*                      Función que publica información hacia a un topic MQTT.                                       *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void MQTT::publish_MQTT ( void ) {
 Serial.println(F("Publicando información"));

  // Crear un documento JSON
  DynamicJsonDocument doc(1024);
  doc["lat"] = latitud;
  doc["lgt"] = longitud;

  // Convertir el documento JSON a una cadena
  char buffer[512];
  size_t n = serializeJson(doc, buffer);

  // Publicar la cadena JSON en el topic
  client.publish(TXTOPIC, buffer, n);
   
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ reconnect_MQTT ( void ) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*                      Función que se reeconecta al servidor MQTT como cliente, tanto para publicar como suscribirse*
*                      a un topic.                                                                                  *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void MQTT::reconnect_MQTT(void) {
  if ( ! client.connected ( ) ) {                         /* ¿Está el cliente desconectado? */

    while(!client.connected()) {                    /* Mientras que está desconectado */
      
      Serial.print ( F ( "Conectando nuevamente al servidor" ) );

      if (client.connect("Client_test")) {             /* Si el cliente se conecta, el cliente puede tener autentificación */
        Serial.println(F ("Cliente conectado"));
        subscribe_MQTT();                                 /* Suscribirse al topic */
      } 
      else {    
                                                  /* Si el cliente no se conecta */
        Serial.print( F ("Falló al conectarse, tipo de error: "));
        Serial.print(client.state());                  /* Mostrar el estado o error, e intentarlo nuevamente */
        Serial.println( F ("Intentando nuevamente en 5 segundos"));
        delay(5000);
        }
    }
  }
  client.loop ();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ setup_WiFi ( void ) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*                      Función que se configura y se conecta a una red WiFi                                         *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void MQTT::setup_WiFi(void) {
  delay (10);
  Serial.println ( F("Configurando WiFi: "));
  WiFi.begin (HOTSPOT_WIFI, HOTSPOT_PWD );               /* Iniciar intentos de conexión hacia la red WiFi */
  
  while (WiFi.status() != WL_CONNECTED) {            /* Mientras que no se haya conectado a la red especificada */
    
    delay(500);
    Serial.print( F("."));                                /* Mostrar un mensaje de espera */ 
    
  }
  //Serial.println ( "WiFi configurado con la IP: " );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ callback ( char* topic, byte* message, unsigned int length ) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*                      Función que obtiene los mensajes mandados hacia el topic al cual se está suscrito                                                      *
*                      NOTA: Los paráetros de está función están de acuerdo con los que vienen en la librería Pubsubclient                                    *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void MQTT::callback(char* topic, byte* message, unsigned int length) {
  Serial.print( F ("Ha llegado un mensaje de: "));
  Serial.println(topic);                             /* Topic al cual se está suscrito */
  
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];                    /* El mensaje proviene en bytes por lo que es necesario concatenarlo */
  }

  Serial.println( messageTemp );

  //TRANSFORMAR EL MENSAJE
  DynamicJsonDocument doc(1024);

  DeserializationError error = deserializeJson(doc, messageTemp);

  if (error) {
    Serial.print(F("Error parseando JSON: "));
    Serial.println(error.f_str());
    return;
  }

  //GUARDAR LA INFO DEL JSON
  bool ultraSonico = doc["ultraSonico"];
  bool izquierda = doc["izquierda"];
  bool derecha = doc["derecha"];
  bool adelante = doc["adelante"];
  bool atras = doc["atras"];
  bool claxon = doc["claxon"];
  
  //CONDICIONES
  if (izquierda) {
    izquierdaActivo = true;
    Serial.println("Izquierda");
  } 
  else {
    izquierdaActivo = false;
  }

  if (derecha) {
    derechaActivo = true;
    Serial.println("Derecha");
  } 
  else {
    derechaActivo = false;
  }

  if (adelante) {
    adelanteActivo = true;
    Serial.println("Adelante");
  } 
  else {
    adelanteActivo = false;
  }

  if (atras) {
    atrasActivo = true;
    Serial.println("Atras");
  } 
  else {
    atrasActivo = false;
  }

  if(claxon){
    claxonActivo = true;
    Serial.println("Claxon");
  }
  else{
    claxonActivo = false;
  }

}