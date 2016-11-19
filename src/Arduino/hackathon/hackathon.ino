#include <SPI.h>
#include "SX1272.h"

// Define section
#define BAND868 //900, 433
#define MAX_NB_CHANNEL 9
#define STARTING_CHANNEL 10
#define ENDING_CHANNEL 18
uint8_t loraChannelIndex = 0;
uint32_t loraChannelArray[MAX_NB_CHANNEL] = {CH_10_868, CH_11_868, CH_12_868, CH_13_868, CH_14_868, CH_15_868, CH_16_868, CH_17_868, CH_18_868};
#define LORAMODE  1 //Mode
#define LORA_ADDR 6 //Self address
#define DEFAULT_DEST_ADDR 1201 //Gateway address

#define FLUSHOUTPUT               Serial.flush();

//Variables
int dest_addr = DEFAULT_DEST_ADDR;
char cmd[260] = "0";          // answer to RPi. Represents button state. 0 - free, 1 - reserved
const int SIGNAL_LED = 13;    // L-led on board
const int BUTTON_PIN = 2;     // button connected to pin 2
char sprintf_buf[100];
int msg_sn = 0;
bool radioON = false;
uint8_t loraMode = LORAMODE;
uint32_t loraChannel = loraChannelArray[loraChannelIndex];
char loraPower = 'x'; //innitial poser level, M (maximum), H (high), L (low)
uint8_t loraAddr = LORA_ADDR;
unsigned int inter_pkt_time = 10000; //Time between sending
unsigned int random_inter_pkt_time = 0;
long last_periodic_sendtime = 0;


// Configure LoRa tranciever
void startConfig() {

  int e;

  // Set transmission mode and print the result
  e = sx1272.setMode(loraMode);
  // Select frequency channel
  if (loraMode == 11) {
    e = sx1272.setChannel(CH_18_868);
  }
  else {
    e = sx1272.setChannel(loraChannel);
  }
  // Select output power (Max, High or Low)
  e = sx1272.setPower(loraPower);
  // get preamble length
  e = sx1272.getPreambleLength();
  // Set the node address and print the result
  sx1272._nodeAddress = loraAddr;
  e = 0;
}

void setup() {
  int e;

  //Add our code here
  Serial.begin(38400);
  // Power ON the module
  e = sx1272.ON();

  e = sx1272.getSyncWord();

  if (!e) {
    radioON = true;
    startConfig();
  }

  FLUSHOUTPUT;
  delay(1000);

}

void loop() {
  
  int e;

  if (radioON) {

    // Arduino раз в 10 секунд посылает состояние датчика
    if (inter_pkt_time)

      if (millis() - last_periodic_sendtime > (random_inter_pkt_time ? random_inter_pkt_time : inter_pkt_time)) {
        sx1272.CarrierSense();
        long startSend = millis();
        cmd[0] = digitalRead(BUTTON_PIN) + '0';                                             // write button state
        int id_place = 1; //id парковочного места и собственно ардуино
        char* str = "\$011";
        e = sx1272.sendPacketTimeout(dest_addr, (uint8_t*) str, strlen(str), 10000);   // send packet
        if (random_inter_pkt_time) {
          random_inter_pkt_time = random(1000, inter_pkt_time);
        }
        last_periodic_sendtime = millis();
      }

    e = 1;
    // trying to receive packet
    uint16_t w_timer = 1000;
    if (loraMode == 1)
      w_timer = 5000;
    e = sx1272.receivePacketTimeout(w_timer);

    // if packet was received?
    if (!e) {
      int a = 0, b = 0;
      uint8_t tmp_length;

      sx1272.getSNR();
      sx1272.getRSSIpacket();
      tmp_length = sx1272._payloadlength;
      if (tmp_length) {
        if ((char) sx1272.packet_received.data[0] == "0") {
          digitalWrite(SIGNAL_LED, LOW);
        } else {
          digitalWrite(SIGNAL_LED, HIGH);
        }
      }

    }

  }
}

