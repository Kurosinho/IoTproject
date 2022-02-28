#include <ZsutDhcp.h>           
#include <ZsutEthernet.h>       
#include <ZsutEthernetUdp.h>    
#include <ZsutFeatures.h>       


byte MAC[]={0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01}; 
#define PACKET_BUFFER_LENGTH        120
#define ID 1
#define REGISTER 1
#define REPORT 2
#define HUMIDIFIER 1

char packetBuffer[PACKET_BUFFER_LENGTH];
static int val = 100;

ZsutEthernetUDP Udp;

void setup() {

    Serial.begin(1);
    ZsutEthernet.begin(MAC);
    Udp.begin(4502);
    Serial.print("Humidifier is up and running...\n");
    char msg[1] = {0b01001001};
    Udp.beginPacket(ZsutIPAddress(10,0,2,15), 4501);
    Udp.write(msg, strlen(msg));
    Udp.endPacket();

}

void loop() {

    char state[50];
    int packetSize = Udp.parsePacket();
    int z3 = ZsutAnalog3Read();
    
    if (val != z3){
        val = z3;
        int req = Udp.read(packetBuffer, PACKET_BUFFER_LENGTH);
    
        sprintf(state, "Humidity: %d\n", z3);
        Serial.println(state);
        Serial.println("Sending data\n");
        
        packetBuffer[0] = (REPORT << 6) + (ID << 3) + (HUMIDIFIER);
        packetBuffer[1] = z3 & 0b11111111;

        Udp.beginPacket(ZsutIPAddress(10,0,2,15), 4501);
        Udp.write(packetBuffer, strlen(packetBuffer));
        Udp.endPacket();
        Udp.flush();
    }   
}