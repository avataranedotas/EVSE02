#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

//para fazer o hard_restart
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#define pino_leitura_amperes 34 //34 por causa do wifi
#define pino_LED 2

const char *ssid = "EVSEServer";
const char *password = "zzzzzzzz";

int amperes = 6;
int amperes_ant = 0;
int contasegundos = 0;

char buffer[100];

unsigned long timer1, timer2, timer3, timer4, timer5, timer6, timer7, timer8 = 0;
unsigned long delayStart = 0;

void hard_restart() {
  esp_task_wdt_init(1,true);
  esp_task_wdt_add(NULL);
  while(true);
}




void setup()
{

  int aux_temp;
  // put your setup code here, to run once:

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(pino_leitura_amperes, INPUT_PULLDOWN);
  digitalWrite(pino_leitura_amperes, false);

  pinMode(pino_LED, OUTPUT);
  digitalWrite(pino_LED, false);

  /*
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  */

  //esperar um pouco e piscar rapido

  while (millis() < 1000)
  {

    aux_temp = millis() % 100;

    if (aux_temp < 50)
      digitalWrite(pino_LED, false);
    else
      digitalWrite(pino_LED, true);
  }
}

void loop()
{

  int leit;
  float temp_f;

  sprintf(buffer, "http://192.168.1.1/%d", amperes);

  leit = analogRead(pino_leitura_amperes);
  //Serial.println(leit);

  temp_f = leit / 125.0;

  temp_f = temp_f - 12.0;

  temp_f = temp_f * 1.36;

  temp_f = temp_f + 6.0;

  amperes = int(temp_f);

  if ((amperes < 6) & (amperes >= 3))
    amperes = 6;
  if ((amperes < 3))
    amperes = 0;

  //debug
  //amperes = 9;

  //ciclo 1s

  if (millis() - timer1 > 1000)
  {
    Serial.print(millis());
    Serial.print("  Amperes a enviar:");
    Serial.println(amperes);

    //liga led
    digitalWrite(pino_LED, false);
    while ((millis() % 150)!=0) {};
    
    //desliga led
    digitalWrite(pino_LED, true);
    while ((millis() % 150)!=149) {};

    contasegundos = contasegundos + 1;

    if (contasegundos > 300) {
      hard_restart();

    }

    //se Wifi estiver ligado
    if ((WiFi.status() == WL_CONNECTED) /*& (amperes!=amperes_ant)*/)
    { //Check the current connection status

      //liga led
      digitalWrite(pino_LED, false);
      HTTPClient http;

      http.begin(buffer);        //Specify the URL
      int httpCode = http.GET(); //Make the request

      if (httpCode > 0)
      { //Check for the returning code

        String payload = http.getString();
        Serial.print("Enviado:");
        Serial.println(amperes);
        Serial.print("Resultado: ");
        Serial.println(httpCode); //200 = OK
        Serial.println(amperes);
        //Serial.println(payload);
      }

      else
      {
        Serial.println("Error on HTTP request");
      }

      http.end(); //Free the resources
      //desliga led
      digitalWrite(pino_LED, true);

    
    }


 //se Wifi não estiver ligado
    if ((WiFi.status() != WL_CONNECTED) /*& (amperes!=amperes_ant)*/)
    { //Check the current connection status
      Serial.print ("\nProblema com Wifi:");
      Serial.println (WiFi.status());
      Serial.print("Resultado:");
      if (WiFi.status()==WL_CONNECT_FAILED) Serial.println("Connect Failed\n");
      if (WiFi.status()==WL_CONNECTION_LOST) Serial.println("Connection Lost\n");
      if (WiFi.status()==WL_DISCONNECTED) Serial.println("Disconnected\n");
      if (WiFi.status()==WL_NO_SSID_AVAIL) Serial.println("No SSID Available\n");
      if (WiFi.status()==WL_IDLE_STATUS) Serial.println("Idle Status\n");
      if (WiFi.status()==WL_NO_SHIELD) Serial.println("No Shield\n");

      //WiFi.disconnect(true,false);

      while ((millis() % 2001)!=0) {};
      while ((millis() % 1999)!=0) {};

      WiFi.reconnect();





    }




      timer1 = millis();
  }

}