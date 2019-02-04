#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



const char* ssid = "Mechatronics";
const char* password = "YayFunFun";

IPAddress ipSendto(192,168,1,23);

//Port Parameters
WiFiUDP UDPTestServer; 
unsigned int UDPPORT = 2390;
//arbitrary port? Is this the port where the light is coming in?
const int packetSize = 22;
// can be up to 65535
byte packetBuffer[packetSize];

int val;
char state[7] = " ";
char health[2] = " ";

const char team = 'R';
int num = 1;

int switchit; 

char potenState1[1];
char potenState2[2];

char potenState[8];
char buff[8] = " ";



/////// INITIALIZE VARIABLES FOR DRIVING 
int left1 = D3;
int left2 = D4;
int right1 = D1;
int right2 = D2;
int leftspeed = D6;
int rightspeed = D5;





// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

  WiFi.config(IPAddress(192, 168, 1, 35), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));

  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  //Waiting until Connected
  Serial.println("Wifi Connected");
  UDPTestServer.begin(UDPPORT);

  //Begin SPI communications
  SPI.begin();

  
  pinMode(D3,OUTPUT);
  digitalWrite(D3,HIGH);


  /////// INITIALIZE VARIABLES FOR DRIVING 
  pinMode(left1,OUTPUT);
  pinMode(right1,OUTPUT);
  pinMode(left2,OUTPUT);
  pinMode(right2,OUTPUT);
  pinMode(leftspeed,OUTPUT);
  pinMode(rightspeed,OUTPUT);


  
}




// the loop routine runs over and over again forever:
void loop() {
  String data = handleUDPServer();


  // F/R of main motors
  char forwardState = data[0];
  char reverseState = data[1];

  // Speed state of main motors
  char forwardState1 = data[2];
  char reverseState1 = data[3];

  // Direction ADC
  char ADC[4] = {data[4],data[5],data[6],data[7]};

  // weapon state 1
  char forwardState2 = data[8];
  char reverseState2 = data[9];

    // weapon state 2
  char forwardState3 = data[10];
  char reverseState3 = data[11];

  // health
  health[0] = data[12];
  health[1] = data[13];
  

    

  strcpy(state,"000000");

  
  if(forwardState2 == '1' && reverseState2 == '0'){
    state[0] = '1';
    state[1] = '0';
  }
  else if(forwardState2 == '0' && reverseState2 == '1'){
    state[0] = '0';
    state[1] = '1';
  }
  else if(forwardState2 == '1' && reverseState2 == '1' ){
    state[0] = '0';
    state[1] = '0';
  }

  if(forwardState3 == '1' && reverseState3 == '0'){
    state[2] = '1';
    state[3] = '0';
  }
  else if(forwardState3 == '0' && reverseState3 == '1'){
    state[2] = '0';
    state[3] = '1';
  }
  else if(forwardState3 == '1' && reverseState3 == '1' ){
    state[2] = '0';
    state[3] = '0';
  }

  
  state[4] = health[0];
  state[5] = health[1];

  state[6] = ' \n';

  
  Serial.println(state);

  byte c;

  SPI.beginTransaction(SPISettings(10000,MSBFIRST,SPI_MODE0));

  digitalWrite(D3,LOW);

  for(const char* p = state; c = *p; p++){
    Serial.println(*p);
    SPI.transfer(c);
   
  }

  digitalWrite(D3,HIGH);

  SPI.endTransaction();



  controlMotors(forwardState,reverseState,forwardState1,reverseState1,ADC);

  
  delay(10);
 
}


void controlMotors(char F,char R,char speedhigh,char speedlow,char direct[4]){

  int adc = atoi(direct);
  if(adc > 308 && adc < 708){
      //Go straight

      
      // Speed control
      if(speedhigh == '1' && speedlow == '0'){
        
        analogWrite(leftspeed,1023);
        analogWrite(rightspeed,1023);
        
      }
      else if(speedhigh == '0' && speedlow == '1'){
        analogWrite(leftspeed,1023/2);
        analogWrite(rightspeed,1023/2);
      }
      else if(speedhigh == '1' && speedlow == '1'){
        analogWrite(leftspeed,1023/4);
        analogWrite(rightspeed,1023/4);
        
      }
    }


    
    else if (adc > 709){

      // TURN RIGHT

      float percentTurnR = 1-((1023 - adc)/308);
      
      
      if(speedhigh == '1' && speedlow == '0'){
        analogWrite(leftspeed,(1023/2) * (1.0-percentTurnR));
        analogWrite(rightspeed,(1023/2) * (1.0+percentTurnR));
      }
      else if(speedhigh == '0' && speedlow == '1'){
        analogWrite(leftspeed,(1023/4) * (1.0-percentTurnR));
        analogWrite(rightspeed,(1023/4)* (1.0+percentTurnR));
      }
      else if(speedhigh == '1' && speedlow == '1'){
        analogWrite(leftspeed,(1023/8) * (1.0-percentTurnR));
        analogWrite(rightspeed,(1023/8) * (1.0+percentTurnR));
      }
    }

    
    else if (adc < 307){

      // to the left

      float percentTurnL = (308 - adc)/308;
      
      if(speedhigh == '1' && speedlow == '0'){
        analogWrite(leftspeed,(1023/2) * (1.0+percentTurnL));
        analogWrite(rightspeed,(1023/2) * (1.0-percentTurnL));
      }
      else if(speedhigh == '0' && speedlow == '1'){
        analogWrite(leftspeed,(1023/4) * (1.0+percentTurnL));
        analogWrite(rightspeed,(1023/4)* (1.0-percentTurnL));
      }
      else if(speedhigh == '1' && speedlow == '1'){
        analogWrite(leftspeed,(1023/8) * (1.0+percentTurnL));
        analogWrite(rightspeed,(1023/8) * (1.0-percentTurnL));
      }
    }

    // Direction control
    if(F == '1' && R == '0'){

      
      digitalWrite(left2,HIGH);
      digitalWrite(left1,LOW);
      digitalWrite(right2,HIGH);
      digitalWrite(right1,LOW);
      
      
    }
    else if(F == '0' && R == '1'){
      digitalWrite(left2,LOW);
      digitalWrite(left1,HIGH);
      digitalWrite(right2,LOW);
      digitalWrite(right1,HIGH);
      
    }
    else if(F == '1' && R == '1'){
      digitalWrite(left2,LOW);
      digitalWrite(left1,LOW);
      digitalWrite(right2,LOW);
      digitalWrite(right1,LOW);
      analogWrite(leftspeed,1);
      analogWrite(rightspeed,1 );
      
      
    }

    


   
}


String handleUDPServer(){
  
  //Checking for Packet
  int c = UDPTestServer.parsePacket();
  //Reading Packet
  if(c) {
    UDPTestServer.read(packetBuffer,packetSize);
    String myData = "";
    for(int i = 0; i < packetSize; i++){
      myData += (char)packetBuffer[i];
    }
    
    return myData;

  }

}


