/*
Sørens algoritme til oxygen-måleren.

Måske der bare skal tendes for strømmen via relæ - engang i timen
1. start sampling
2. forbind til GSM net
3. send data
4. send stopsignal til relæ

a. en esp med deepsleep? - starter timer
b. lader 


*/

#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Arduino.h>
//Oxygen måler
#define DO_PIN A1     //Oxygen måler
#define VREF 5000    //VREF (mv)
#define ADC_RES 1024 //ADC Resolution

//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
// #define CAL1_V (266) //mv
// #define CAL1_T (18)   //℃

// #define CAL2_V (1300) //mv
// #define CAL2_T (15)   //℃
#define CAL1_V (971) //mv
#define CAL1_T (16)   //℃

#define CAL2_V (751) //mv
#define CAL2_T (10)   //℃

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;

int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
}

// Tryk føler
const int pressureInput = A0; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //
const int pressureMax = 921.6; //
const int pressuretransducermax = 100; //psi value of transducer being used


float pressureValue = 0; //variable to store the value coming from the pressure transducer





//Temp føler
Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp;
int _timeout;
String _buffer;
String number = "+4560871252"; //-> change with your number
String Start="AT+CIPSTART=\"TCP\"";     // TCPIP start command
String url="";
String ip="\"api.thingspeak.com\"";        // server ip or url
int port=80;  
int randomNumber;
float ilt = 7.1;
float dyb = -8.2;

String Arsp, Grsp;
SoftwareSerial gsm(10, 11); // RX, TX

void setup() {

  Serial.begin(9600);
  Serial.println("Testing GSM gsm800L");
  while (!Serial);
  {
    delay(100);
  }
  Serial.println("Serial OK");
  gsm.begin(9600);
  while (!gsm);
  {
    delay(100);
  }
  Serial.println("GSM OK");
    // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu_temp = mpu.getTemperatureSensor();
  uint16_t ilt;
  for (size_t i = 0; i < 20; i++)
  {
      ADC_Raw = analogRead(DO_PIN);
  ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;


  ilt = ilt + readDO(ADC_Voltage, uint8_t(mpu_temp))/20.0;
  pressureValue = pressureValue + (pressureValue-pressureZero)/(pressureMax-pressureZero)/20; 
  delay(500);

  }
  

  delay(1000);
  connectGSM("AT+CIPSHUT","OK");//lukker en tidligere forbindelse
  connectGSM("AT","OK");
  connectGSM("AT+CFUN=1","OK"); //AT+CFUN AT command  sets the level of functionality in the MT 1:full funtionalitet, MT 2:
  connectGSM("AT+CGATT=1","OK");
  connectGSM("AT+CSTT=\"internet\",\"\",\"\"","OK");
  connectGSM("AT+CIICR","OK");
  // delay(1000);
  connectGSM("AT+CIFSR","10");
  // delay(1000);
  connectGSM("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80","OK");
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  String s = "GET /update?api_key=5CED5ZNII2FPBO02&field1="+String(ilt)+"&field2="+String(pressureValue)+"&field3="+ String(temp.temperature)+" HTTP/1.0\r\n\r\n";//skriver værdien 7.5 til 
 
  int len = s.length();
  Serial.print("message length: ");
  Serial.println(len);

  String sendCmd="AT+CIPSEND=";           // sending number of byte command
  sendCmd += len;                         //sendCmd = "AT+CIPSEND=60" forklaring: https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/TCP-IP_AT_Commands.html#at-cipsend-send-data-in-the-normal-transmission-mode-or-wi-fi-passthrough-mode
  connectGSM(sendCmd,">");                //
  connectGSM(s,"SAMEORIGIN");
  // delay(1000);
  connectGSM("AT+CIPSHUT","OK");

}

void loop() {
  // delay(3600000);
  
  // sensors_event_t a, g, temp;
  // mpu.getEvent(&a, &g, &temp);


  // randomNumber=random(50,70);
  // dyb = - randomNumber/10.0;
  // connectGSM("AT+CIPSHUT","OK");//lukker en tidligere forbindelse
  // connectGSM("AT","OK");
  // connectGSM("AT+CFUN=1","OK"); //AT+CFUN AT command  sets the level of functionality in the MT 1:full funtionalitet, MT 2:
  // connectGSM("AT+CGATT=1","OK");
  // connectGSM("AT+CSTT=\"internet\",\"\",\"\"","OK");
  // connectGSM("AT+CIICR","OK");
  // // delay(1000);
  // connectGSM("AT+CIFSR","10");
  // // delay(1000);
  // connectGSM("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80","OK");
  // String s = "GET /update?api_key=5CED5ZNII2FPBO02&field1="+String(ilt)+"&field2="+String(dyb)+"&field3="+ String(temp.temperature)+" HTTP/1.0\r\n\r\n";//skriver værdien 7.5 til 
 
  // int len = s.length();
  // Serial.print("message length: ");
  // Serial.println(len);

  // String sendCmd="AT+CIPSEND=";           // sending number of byte command
  // sendCmd += len;                         //sendCmd = "AT+CIPSEND=60" forklaring: https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/TCP-IP_AT_Commands.html#at-cipsend-send-data-in-the-normal-transmission-mode-or-wi-fi-passthrough-mode
  // connectGSM(sendCmd,">");                //
  // connectGSM(s,"SAMEORIGIN");
  // // delay(1000);
  // connectGSM("AT+CIPSHUT","OK");
  

  // if(gsm.available())
  // {
  //   Grsp = gsm.readString();
  //   Serial.println(Grsp);
  // }

  // if(Serial.available())
  // {
  //   Arsp = Serial.readString();
  //   gsm.println(Arsp);
  // }

}


void connectGSM (String cmd, char *res)
{
  while(true)
  {
    Serial.println(cmd);
    gsm.println(cmd);
    delay(100);
    while(gsm.available()>0)
    {
      if(gsm.find(res))
      {
    // Grsp = gsm.readString();
    // Serial.println(Grsp);
        delay(100);
        return;
      }
    }
    delay(100);
   }
 }

//  void SendSMSMessage()
// {
//   Serial.println ("Sending Message");
//   gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
//   delay(200);
//   //Serial.println ("Set SMS Number");
//   gsm.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
//   delay(200);
//   String SMS = "Hej, Hvordan går det? Hilsen fra Arduino";
//   gsm.println(SMS);
//   delay(100);
//   gsm.println((char)26);// ASCII code of CTRL+Z
//   delay(200);
//   _buffer = _readSerial();
// }

// void RecieveSMSMessage()
// {
//   Serial.println ("gsm800L Read an SMS");
//   gsm.println("AT+CMGF=1");
//   delay (200);
//   gsm.println("AT+CNMI=1,2,0,0,0"); // AT Command to receive a live SMS
//   delay(200);
//   Serial.write ("Unread Message done");
// }

String _readSerial() {
  _timeout = 0;
  while  (!gsm.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (gsm.available()) {
    return gsm.readString();
  }
}

void callNumber() {
  gsm.print (F("ATD"));
  gsm.print (number);
  gsm.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}

