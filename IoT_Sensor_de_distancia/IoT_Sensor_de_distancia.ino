#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "******" //Se define el nombre de la red
#define STAPSK  "******"    //Se define la contraseña de la red
#endif

/*Informacion de la red*/
WiFiServer server(80);                    //Se declara el numero de puerto
IPAddress local_IP(0, 0, 0, 0);   //Se declara la dirección IP estática al dispositivo
IPAddress gateway(0, 0, 0, 0);      //Se declara la dirección de puerta de enlace
IPAddress subnet(255, 255, 0, 0);         //Se declara la dirección máscara de subred

const char* ssid = STASSID;
const char* password = STAPSK;

/*Constantes de comuncacion del sensor ultrasonico  jsn-sr04t*/
const unsigned int TRIG_PIN=5;    //Se declara el pin de RX
const unsigned int ECHO_PIN=4;    //Se declara el pin de TX

const unsigned int BAUD_RATE=9600;    //Se declara el valor baud rate

void setup() {
  Serial.begin(BAUD_RATE);   //Se inicia la comunicación serial con el valor baud rate de 9600

  pinMode(TRIG_PIN, OUTPUT);    //Se define el modo del pin como señal de entrada
  pinMode(ECHO_PIN, INPUT);     //Se define el modo del pin como señal de salida
  
  if (!WiFi.config(local_IP, gateway, subnet)) {    //Se configura la red WiFi
    Serial.println("STA Failed to configure");
    }
  
  Serial.print(F("Connecting to "));    //Se imprime el nombre de la red
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);                 //Se configura la antena WiFi en modo de STATION para conectarse a un access point
  WiFi.begin(ssid, password);          //Inicia la conexión

  while (WiFi.status() != WL_CONNECTED) {   //Ciclo de intento de conexion a la red WiFi
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));      //Imprime mensaje de conexion a la red WiFi

  server.begin();                           //Inicia un servidor Web
  Serial.println(F("Server started"));      //

  Serial.println(WiFi.localIP());           //Se imprime la direccion IP
}

void loop() {
  WiFiClient client = server.available();   //Conexion de un nuevo cliente
  if (!client) {                            //Si no hay conexion de un nuevo cliente
    return;                                 //regresa al inicio del ciclo
  }

  /*Enviar la peticion del cliente*/
  Serial.println(F("new client"));
  client.setTimeout(5000);          //Conexion por 5000 milisegundos
  while (client.available()) {      //Leer la peticion del cliente
    client.read();
  }
  /*
   * Imprimir el mensaje, se imprime el codigo HTML.
   * La función distancia regresa el valor de la medición del sensor.
  */
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><meta http-equiv='Content-type' content='text/html; charset=utf-8'><title>IoT Medicion de distancia</title><style type='text/css' media='screen'>body {margin:0; padding:0; background-color: #333;} h2{text-align: center;font-size: 86px;font-family: Arial, Helvetica, sans-serif;color: #ffffffff;}.card{text-align: center;font-size: 48px;font-family: Arial, Helvetica, sans-serif;color: #ffffffff;margin: 2%;border: solid;border-radius: 30px;border-color: #33ff33ff;}p{text-align: center;font-size: 120px;font-family: Arial, Helvetica, sans-serif;color: #ffffffff; margin: 70px;}</style></head><body><h2>IoT Medicion de distancia</h2><div id='info' class='card'><span>-</span><br><label> distancia:</label><p>"));
  client.print(distancia());
  client.print(F(" cm</p><label>distancia minima: 30 cm</label><br><label>distancia máxima: 400 cm</label><br><br><span>-</span></div></body></html>"));

  Serial.println(F("Disconnecting from client"));
}

/*Esta función hace la lectura de las señales del sensor ultrasónico, regresa el valor de la medición en centimetros*/
int distancia() {
  digitalWrite(TRIG_PIN, LOW);    //señal en 0, para asegurar el estado del pulso
  delayMicroseconds(2);           //por 2 microsegundos
  digitalWrite(TRIG_PIN, HIGH);   //Señal en 1, inicia el pulso
  delayMicroseconds(10);          //por 10 microsegundos
  digitalWrite(TRIG_PIN, LOW);    //termina el pulso, señal en 0
  
  const unsigned long duration = pulseIn(ECHO_PIN, HIGH);   //lee el tiempo del pulso de recepción de la señal 
  int resultado = duration/29/2;                             //calcula la distancia en centímetros
  
  if(duration == 0){
    Serial.println("Alerta: no hay señal del sensor");
  }
  else{
    Serial.print("distancia:");
    Serial.print(resultado);
    Serial.println(" cm");
  }
  delay(100);
  return resultado;
}
