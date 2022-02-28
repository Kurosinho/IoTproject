#include <ZsutDhcp.h>          
#include <ZsutEthernet.h>       
#include <ZsutEthernetUdp.h>    
#include <ZsutFeatures.h>       
#include <Arduino.h>


byte MAC[]={0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01}; 

#define PACKET_BUFFER_LENGTH        120

#define REGISTER 1
#define COMMAND 3

#define AIR_CONDITIONER 3

#define ON 1
#define OFF 2

#define LED     ZSUT_PIN_D2

unsigned char packetBuffer[PACKET_BUFFER_LENGTH];

ZsutEthernetUDP Udp;

void setup() {
    
    Serial.begin(1);
    ZsutPinMode(LED, OUTPUT);
    ZsutEthernet.begin(MAC);
    Udp.begin(4504);
    Serial.print("Air Conditioner is up and running...\n");
    char msg[1] = {0b01011011};
    Udp.beginPacket(ZsutIPAddress(10,0,2,15), 4501);
    Udp.write(msg, strlen(msg));
    Udp.endPacket();

}

void loop() {
    int packetSize = Udp.parsePacket();
    
    if (packetSize > 0){
        int req = Udp.read(packetBuffer, PACKET_BUFFER_LENGTH);

        if (((packetBuffer[0] & 0b11000000) >> 6) == COMMAND){

            if (((packetBuffer[0] & 0b00001000) >> 3) == ON){
                ZsutDigitalWrite(LED, HIGH);
                Serial.print("Air conditioner is on!\n");
            }
            if (((packetBuffer[0] & 0b00010000) >> 3) == OFF){
                ZsutDigitalWrite(LED, LOW);
                Serial.print("Air conditioner is off!\n");
            }
        }
    }   
}