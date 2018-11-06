/*
  RECEIVED OSC MESSAGES:
  /led 1 : turn on the builtin LED AND a LED tied to pin 4 (D2 on the Wemos)
  /led 0 : turn off the builtin LED AND a LED tied to pin 4 (D2 on the Wemos)
  *: please not that on the Wemos, the builtin LED is inverted (i.e. on when off)

  SENT OSC MESSAGES:
  /button 0 : when pin 0 (D3 on the Wemos) when a button is pressed to GND
  /button 1 : when pin 0 (D3 on the Wemos)is high when a button is released to PULL_UP

*/

// INCLUDE ESP8266WiFi:
#include <ESP8266WiFi.h>

// Please change the following values with your network settings:
const char* ssid     = "PetitPet";
const char* password = "freedomostie";

IPAddress ip(192, 168, 25, 10);
IPAddress gateway(192, 168, 25, 1);
IPAddress subnet(255, 255, 255, 0);

// UDP
int udpReceivePort = 7777;
IPAddress udpTxIp = IPAddress(192, 168, 25, 125);
int udpTxPort = 7890;

// INCLUDE ESP8266 UDP
#include <WiFiUdp.h>
WiFiUDP udp;

// UDP BUFFERS
#define UDP_RX_BUFFER_MAX_SIZE 256
char udpRxBuffer[UDP_RX_BUFFER_MAX_SIZE];

#define UDP_TX_BUFFER_MAX_SIZE 256
char udpTxBuffer[UDP_TX_BUFFER_MAX_SIZE];

// OSC PARSER AND PACKER
#include <TinyOsc.h>
TinyOsc osc;

#include <Bounce2.h>
Bounce button;

//===========
//== SETUP ==
//===========
void setup() {

  // INITIATE SERIAL COMMUNICATION FOR DEBUGGING.
  Serial.begin(57600);

  Serial.println("***STARTING WIFI***");

  // BEGIN WIFI
  WiFi.config(ip , gateway , subnet );
  WiFi.begin(ssid, password);

  // WAIT UNTIL CONNECTED
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  }
  //

  // PRINT CONNECTION SETTINGS
  Serial.println();
  Serial.println("WiFi connected, IP address: ");
  Serial.println( WiFi.localIP() );
  

  udp.begin(udpReceivePort); // BEGIN LISTENING ON UDP PORT udpReceivePort

  // SETUP THE BUILTIN LED
  pinMode(LED_BUILTIN, OUTPUT);

  // SETUP THE OTHER LED
  pinMode(4, OUTPUT);

  // SETUP THE BUTTON ON PIN 0
  button.attach(0, INPUT_PULLUP);
  button.interval(5);

}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {


  // GET THE FORMAT AS A C STRING
  char * format = osc.getFormat();


  Serial.println("***OSC***");
  Serial.print("Address: ");
  Serial.println(osc.getAddress());
  Serial.print("Type tags: ");
  Serial.println(format);

  // IF THE ADDRESS IS /led AND THERE IS AN INTEGER ('i')
  if ( osc.fullMatch("/led") && format[0] == 'i' ) {
    int state = osc.getNextInt32();
    digitalWrite(LED_BUILTIN, state);
    digitalWrite(4, state);
  }

}


//==========
//== LOOP ==
//==========
void loop() {

  // CHECK FOR OSC MESSAGES OVER UDP
  // ===============================

  // CHECK IF AN UDP PACKET WAS RECEIVED:
  // udp.parsePacket() RETURNS ture IF IT AN UDP PACKET WAS RECEIVED
  if ( udp.parsePacket() ) {

    // COPY THE PACKET INTO A BUFFER THAT WILL THEN BE USED BY TinyOsc
    // udp.read() RETURNS THE NUMBER OF chars THAT WERE RECEIVED
    int packetSize = udp.read(udpRxBuffer, UDP_RX_BUFFER_MAX_SIZE);

    Serial.println("***UDP***");
    Serial.print("Received packet of size: ");
    Serial.println(packetSize);
    Serial.print("From: ");
    IPAddress remoteIp = udp.remoteIP();
    Serial.println(remoteIp);

    // PARSE THE OSC MESSAGES FROM THE BUFFER
    // <TinyOsc>.parse( buffer name, size of the data, callback function)
    // FOR EVERY PARSED OSC MESSAGE IN  udpRxBuffer THE receivedOscMessage WILL BE CALLED
    osc.parse( udpRxBuffer, packetSize, receivedOscMessage);

  }



  // UPDATE THE BUTTON
  button.update();

  if ( button.rose() ) {

    // SEND OSC MESSAGES OVER UDP
    // ===============================

    // <TinyOsc>.writeMessage( name of buffer to write to , the maximum size of the buffer , the address , the format string , data... )
    // THE FORMAT STRING MUST MATCH THE DATA
    // 'i':32-bit integer
    // IN THIS CASE, THE DATA IS 1 (integer)
    int udpTxBufferLength = osc.writeMessage( udpTxBuffer, UDP_TX_BUFFER_MAX_SIZE ,  "/button",  "i",   1 );

    // udpTxBuffer NOW CONTAINS THE OSC MESSAGE AND WE SEND IT OVER UDP
    udp.beginPacket( udpTxIp , udpTxPort );
    udp.write( udpTxBuffer ,  udpTxBufferLength );
    udp.endPacket();

  } else if ( button.fell() ) {

    // <TinyOsc>.writeMessage( name of buffer to write to , the maximum size of the buffer , the address , the format string , data... )
    // THE FORMAT STRING MUST MATCH THE DATA
    // 'i':32-bit integer
    // IN THIS CASE, THE DATA IS 0 (integer)
    int udpTxBufferLength = osc.writeMessage( udpTxBuffer, UDP_TX_BUFFER_MAX_SIZE ,  "/button",  "i",   0 );

    // udpTxBuffer NOW CONTAINS THE OSC MESSAGE AND WE SEND IT OVER UDP
    udp.beginPacket( udpTxIp , udpTxPort );
    udp.write( udpTxBuffer ,  udpTxBufferLength );
    udp.endPacket();
  }
}
