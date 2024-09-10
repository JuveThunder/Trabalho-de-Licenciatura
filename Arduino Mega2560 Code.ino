#include <ZMPT101B.h>
#include <Robojax_AllegroACS_Current_Sensor.h>
#include "ACS712.h"

#define SENSITIVITY 445.0f

const int VIN_TensaoGerador = A14;
const int VIN_TensaoControlador = A1;
const int VIN_TensaoBateria = A3;

// Instanciando os sensores de corrente
//ACS712 CorrenteGerador(ACS712_20A, A14);
//ACS712 CorrenteControlador(ACS712_20A, A0);
//ACS712 CorrenteBateria(ACS712_20A, A2);
//ACS712 CorrenteInversor(ACS712_20A, A4);

const int VIN = A14; // define the Arduino pin A0 as voltage input (V in)
const float VCC   = 5.04;// supply voltage
const int MODEL = 1;   // enter the model (see above list)
#include <Robojax_AllegroACS_Current_Sensor.h>

Robojax_AllegroACS_Current_Sensor robojax1(MODEL,A15);
Robojax_AllegroACS_Current_Sensor robojax2(MODEL,A0);
Robojax_AllegroACS_Current_Sensor robojax3(MODEL,A2);
//Robojax_AllegroACS_Current_Sensor robojax4(MODEL,A4);
ACS712 sensor(ACS712_20A, A4);

ZMPT101B voltageSensor(A5, 50.0);

void setup() {
    Serial.begin(115200); // Inicializa o monitor serial
//    Serial.println("Bancada de Sistema Fotovoltaico");
    voltageSensor.setSensitivity(SENSITIVITY);
    sensor.calibrate();
}

void loop() {
    int numReadings = 50;
    float sumISG = 0, sumISC = 0, sumISB = 0, sumISI = 0;
    float totalVoltageGerador = 0;
    float totalVoltageControlador = 0;
    float totalVoltageBateria = 0;
    float totalVoltageInversor = 0;

    // 50 leituras de sinal de tensão e corrente
    for (int i = 0; i < numReadings; i++) {
        totalVoltageGerador += analogRead(VIN_TensaoGerador) * (25.00 / 1023.00);
        totalVoltageControlador += analogRead(VIN_TensaoControlador) * (25.00 / 1023.00);
        totalVoltageBateria += analogRead(VIN_TensaoBateria) * (25.00 / 1023.00);
        totalVoltageInversor += voltageSensor.getRmsVoltage();

        sumISG += robojax1.getCurrentAverage(50);
        sumISC += robojax2.getCurrentAverage(50);
        sumISB += robojax3.getCurrentAverage(50);
//        sumISI += robojax4.getCurrentAverage(50);
        sumISI += sensor.getCurrentAC();
        delay(30);
    }

    // Calcula as médias
    float avgISG = sumISG / numReadings;
    float avgTSG = totalVoltageGerador / numReadings;
    float avgISC = sumISC / numReadings;
    float avgTSC = totalVoltageControlador / numReadings;
    float avgISB = sumISB / numReadings;
    float avgTSB = totalVoltageBateria / numReadings;
    float avgISI = sumISI / numReadings;
    float avgTSI = totalVoltageInversor / numReadings;

    if(avgTSI<50){
  avgTSI=0;
  }


      if(avgISG<0.5){
  avgISG=avgISG;
  
  }

        if(avgISG>0.5){
avgISG=avgISG+0.5;
  }

  
  if(avgISB<1.2){
  avgISB=0;
  }

  if(avgISI<0.15){
  avgISI=0;
  }

  
    // Envia os valores médios para a serial
//    Serial.println("Saída do Gerador:");
    Serial.print(avgISG);
        Serial.print(",");
//    Serial.print(" A,");
    Serial.print(avgTSG);
        Serial.print(",");
//    Serial.println(" V");

//    Serial.println("Saída do Controlador:");
    Serial.print(avgISC-0.4);
        Serial.print(",");
//    Serial.print(" A, ");
    Serial.print(avgTSC-0.4);
        Serial.print(",");
//    Serial.println(" V");

//    Serial.println("Saída da Bateria:");
    Serial.print(avgISB);
        Serial.print(",");
//    Serial.print(" A, ");
    Serial.print(avgTSB-0.3);
        Serial.print(",");
//    Serial.println(" V");

//    Serial.println("Saída do Inversor:");
    Serial.print(avgISI);
        Serial.print(",");
//    Serial.print(" A, ");
    Serial.print(avgTSI);
        Serial.println();
//    Serial.println(" V");

//    Serial.println("------------------------------------------------------------------------");

    // Espera dois segundos antes de iniciar um novo ciclo
    delay(1000);
}
