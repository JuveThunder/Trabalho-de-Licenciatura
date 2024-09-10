#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Juve"
#define WIFI_PASSWORD "123456789900"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAybuUXvFw40aHel8x7zMRvZU4DwPQ4ebU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "tl-2024-a791d-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


bool signupOK = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK) {

    if (Serial.available() > 0) {
      String data = Serial.readStringUntil('\n');
      Serial.println(data);

      // Array para armazenar os 8 valores convertidos para float
      float values[8];

      // Variável para guardar a posição inicial de cada substring
      int start = 0;
      int comma;

      // Extrair 8 valores separados por vírgula
      for (int i = 0; i < 8; i++) {
        comma = data.indexOf(',', start);  // Encontra a próxima vírgula

        if (comma != -1) {
          values[i] = data.substring(start, comma).toFloat();  // Extrai o valor entre as vírgulas
          start = comma + 1;  // Move o ponto de início para a próxima posição
        } else {
          values[i] = data.substring(start).toFloat();  // Último valor, sem vírgula após
        }
      }

      // Enviar os valores ao Firebase
      Firebase.RTDB.setFloat(&fbdo, "/GeradorFV/Corrente", values[0]);
      Firebase.RTDB.setFloat(&fbdo, "/GeradorFV/Tensao", values[1]);
      Firebase.RTDB.setFloat(&fbdo, "/Controlador/Corrente", values[2]);
      Firebase.RTDB.setFloat(&fbdo, "/Controlador/Tensao", values[3]);
      Firebase.RTDB.setFloat(&fbdo, "/Bateria/Corrente", values[4]);
      Firebase.RTDB.setFloat(&fbdo, "/Bateria/Tensao", values[5]);
      Firebase.RTDB.setFloat(&fbdo, "/Inversor/Corrente", values[6]);
      Firebase.RTDB.setFloat(&fbdo, "/Inversor/Tensao", values[7]);
    }
  }
}
