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
  // mpu_temp->printSensorDetails();

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

  String s = "GET /update?api_key=5CED5ZNII2FPBO02&field1="+String(ilt)+"&field2="+String(dyb)+"&field3="+ String(temp.temperature)+" HTTP/1.0\r\n\r\n";//skriver værdien 7.5 til 
 
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
  delay(3600000);
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  randomNumber=random(50,100);
  ilt = randomNumber/10.0;
  randomNumber=random(50,70);
  dyb = - randomNumber/10.0;
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
  String s = "GET /update?api_key=5CED5ZNII2FPBO02&field1="+String(ilt)+"&field2="+String(dyb)+"&field3="+ String(temp.temperature)+" HTTP/1.0\r\n\r\n";//skriver værdien 7.5 til 
 
  int len = s.length();
  Serial.print("message length: ");
  Serial.println(len);

  String sendCmd="AT+CIPSEND=";           // sending number of byte command
  sendCmd += len;                         //sendCmd = "AT+CIPSEND=60" forklaring: https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/TCP-IP_AT_Commands.html#at-cipsend-send-data-in-the-normal-transmission-mode-or-wi-fi-passthrough-mode
  connectGSM(sendCmd,">");                //
  connectGSM(s,"SAMEORIGIN");
  // delay(1000);
  connectGSM("AT+CIPSHUT","OK");
  

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

