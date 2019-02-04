#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define Global Variables

//THESE TWO NEED TO BE SET AT THE BEGINNING OF THE MATCH
const char team = 'R';
const int num = 1;

int val;
char state[15];
char health[3];


// Pins with switch attached to it

//FORWARD IS DIRECTION
int forward = D1;
int backward = D2;

//FORWARD1 IS SPEED
int forward1 = D3;
int backward1 = D4;

//FORWARD2 IS WEAPON1
int forward2 = D5;
int backward2 = D6;

//FORWARD2 IS WEAPON2
int forward3 = D7;
int backward3 = D8;


// analog pin A0 has a potentiometer attached to it.
int poten = A0;
int switchit; 

//This is used to calculate the ADC value
char potenState1[1];
char potenState2[2];
char potenState3[3];
char potenState4[4];
char potenState[8];
char buff[8] = " ";


const char* ssid = "Mechatronics";// This is the wifi we are connecting too
const char* password = "YayFunFun";// This is the Password to said wifi

//THIS IS NOT MY IP
IPAddress ipSendto(192,168,1,23);

//Designate ports and packet sizes
unsigned int udpRemotePort = 3001;
unsigned int udplocalPort = 2390;

const int packetSize = 22;
char udpBuffer[packetSize];
char readValue[packetSize];
char sendThis[packetSize];
char packetBuffer[packetSize];

WiFiUDP UDPTestServer;

//function to determine the offset when we read the main packet
//that is being transmitted
int offset(){
  
  int offset = 0;
  if(team == 'B'){
    offset = offset + 11;
  }
  if(num == 1){
    offset = offset + 3;
  }
  else if(num == 2){
    offset = offset + 5;
  }
  else if(num == 3){
    offset = offset + 7;
  }
  else if(num == 4){
    offset = offset + 9;
  }
  
  return offset;
}

void handleGetPacket(){
  //Checking for Packet
  int c = UDPTestServer.parsePacket();
  //Reading Packet
  if(c) {
    UDPTestServer.read(packetBuffer,packetSize);
    String myData = "";
    for(int i = 0; i < packetSize; i++){
      myData += (char)packetBuffer[i];
    }
    
    int return_val = offset();
    
    health[0] = myData[return_val];
    health[1] = myData[return_val+1];
   
  }
}

void handleSendPacket(){
  
  
  strcpy(udpBuffer,state);
  Serial.println(udpBuffer);
  UDPTestServer.beginPacket(ipSendto,udpRemotePort);
  UDPTestServer.write(udpBuffer, sizeof(udpBuffer));
  UDPTestServer.endPacket();
}


void setup() {
  // put your setup code here, to run once:
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);
  WiFi.config(IPAddress(192, 168, 1, 35), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));// my ip, gateway, subnet

  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  UDPTestServer.begin(udplocalPort);
  Serial.print("Local port: ");
  Serial.println(UDPTestServer.localPort());


  // make the switch pins an input:
  pinMode(forward, INPUT);
  pinMode(backward,INPUT);

  pinMode(forward1, INPUT);
  pinMode(backward1,INPUT);
  
  pinMode(forward2, INPUT);
  pinMode(backward2,INPUT);
  
  pinMode(forward3, INPUT);
  pinMode(backward3,INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  //RETRIEVE PACKET EVERY LOOP
  handleGetPacket();

  //read the pin value
  val = analogRead(poten);

  switchit = 0;
  
  strcpy(state,"00000000000000");

  // read the button input pin:
  int forwardState = digitalRead(forward);
  int reverseState = digitalRead(backward);
  // read the button input pin:
  int forwardState1 = digitalRead(forward1);
  int reverseState1 = digitalRead(backward1); 
  // read the button input pin:
  int forwardState2 = digitalRead(forward2);
  int reverseState2 = digitalRead(backward2); 
  // read the button input pin:
  int forwardState3 = digitalRead(forward3);
  int reverseState3 = digitalRead(backward3); 
  
  if(forwardState == 1 && reverseState == 0){
    state[0] = '1';
    state[1] = '0';
  }
  else if(forwardState == 0 && reverseState == 1){
    state[0] = '0';
    state[1] = '1';
  }
  else if(forwardState == 1 && reverseState == 1 ){
    state[0] = '0';
    state[1] = '0';
  }

  if(forwardState1 == 1 && reverseState1 == 0){
    state[2] = '1';
    state[3] = '0';
  }
  else if(forwardState1 == 0 && reverseState1 == 1){
    state[2] = '0';
    state[3] = '1';
  }
  else if(forwardState1 == 1 && reverseState1 == 1 ){
    state[2] = '0';
    state[3] = '0';
  }

  switch(switchit) {
    case 1  :
      state[4] = '0';
      state[5] = '0';
      state[6] = '0';
      state[7] = potenState1[0];
      break;

    case 2  :
      state[4] = '0';
      state[5] = '0';
      state[6] = potenState2[0];
      state[7] = potenState2[1];
      break;

    case 3  :
      state[4] = '0';
      state[5] = potenState3[0];
      state[6] = potenState3[1];
      state[7] = potenState3[2];
      break;

    case 4  :
      state[4] = potenState4[0];
      state[5] = potenState4[1];
      state[6] = potenState4[2];
      state[7] = potenState4[3];
      break;
  }

  if(forwardState2 == 1 && reverseState2 == 0){
    state[8] = '1';
    state[9] = '0';
  }
  else if(forwardState2 == 0 && reverseState2 == 1){
    state[8] = '0';
    state[9] = '1';
  }
  else if(forwardState2 == 1 && reverseState2 == 1 ){
    state[8] = '0';
    state[9] = '0';
  }

  if(forwardState3 == 1 && reverseState3 == 0){
    state[10] = '1';
    state[11] = '0';
  }
  else if(forwardState3 == 0 && reverseState3 == 1){
    state[10] = '0';
    state[11] = '1';
  }
  else if(forwardState3 == 1 && reverseState3 == 1 ){
    state[10] = '0';
    state[11] = '0';
  }
  
  state[12] = health[0];
  state[13] = health[1];

  state[14] = ' \n';

  //SEND PACKET EVERY LOOP
  handleSendPacket();

  //DELAY FOR 10MS
  delay(10);

}
