#include "SX1272.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h> 
#include  <signal.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

// Define section

#define BAND868 //900, 433
#define MAX_NB_CHANNEL 9
#define STARTING_CHANNEL 10
#define ENDING_CHANNEL 18
uint8_t loraChannelIndex=0;
uint32_t loraChannelArray[MAX_NB_CHANNEL]={CH_10_868,CH_11_868,CH_12_868,CH_13_868,CH_14_868,CH_15_868,CH_16_868,CH_17_868,CH_18_868};
#define LORAMODE  1 //Mode
#define LORA_ADDR 255 //Self address
#define DEFAULT_DEST_ADDR 6 //Gateway address

  #define PRINTLN                   printf("\n")
  #define PRINT_CSTSTR(fmt,param)   printf(fmt,param)
  #define PRINT_STR(fmt,param)      PRINT_CSTSTR(fmt,param)
  #define PRINT_VALUE(fmt,param)    PRINT_CSTSTR(fmt,param)
  #define FLUSHOUTPUT               fflush(stdout);



//Variales
int dest_addr=DEFAULT_DEST_ADDR;
char cmd[260]="****************";
char sprintf_buf[100];
int msg_sn=0;
bool radioON=false;
uint8_t loraMode=LORAMODE;
uint32_t loraChannel=loraChannelArray[loraChannelIndex];
char loraPower='x'; //innitial poser level, M (maximum), H (high), L (low)
uint8_t loraAddr=LORA_ADDR;
unsigned int inter_pkt_time=10000; //Time between sending
unsigned int random_inter_pkt_time=0;
long last_periodic_sendtime=0;
// packet size for periodic sending
uint8_t MSS=40;
int ST=0;

int Gi=0;
int book=0;
int zero=0;
void send_data(int iden,int s)
{
	//printf("start send func with iden= %d status = %d \n",iden,s);
    int i=0, e;
    int cmdValue;
    if (radioON) {



        if (inter_pkt_time) {

            if (millis() - last_periodic_sendtime > (random_inter_pkt_time ? random_inter_pkt_time : inter_pkt_time)) {
                PRINT_CSTSTR("%s", "Start to send data: ");
                PRINT_VALUE("%ld", millis());
                PRINTLN;
		
              
                sx1272.CarrierSense();
                long startSend = millis();
		char status[1];
		if(s==0){status[0]='0';}
		if(s==1){status[0]='1';}
		if(s==2){status[0]='2';}
		e = sx1272.sendPacketTimeout(iden,(uint8_t*)status , strlen(status), 1000);

               
                PRINT_CSTSTR("%s", "LoRa Sent in ");
                PRINT_VALUE("%ld", millis() - startSend);
                PRINTLN;
                PRINT_CSTSTR("%s", "Packet sent, state ");
                PRINT_VALUE("%d", e);
                PRINTLN;
                if (random_inter_pkt_time) {
                    random_inter_pkt_time = random() % inter_pkt_time + 1000;
                    PRINT_CSTSTR("%s", "next in ");
                    PRINT_VALUE("%ld", random_inter_pkt_time);
                    PRINTLN;
                }
                last_periodic_sendtime = millis();
            }
        }




    }
//printf("stop send func with iden= %d status = %d \n",iden,s);
}



int callback(void *, int, char **, char **);

int ssnd(int iden) {
    Gi=iden;
    sqlite3 *db;
    sqlite3_stmt *res;
    
    int rc = sqlite3_open("places.db", &db);
    
    if (rc != SQLITE_OK) {
        
        printf( "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return -99;
    }
    	char *z;
	char *sqlSelect = new char[60];//malloc(60);
	sprintf(sqlSelect,"SELECT status FROM Place where identifier=%d;",iden);
    rc =sqlite3_exec(db, sqlSelect,callback, 0, NULL);    
    
    if (rc != SQLITE_OK) {
        
        printf( "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return -99;
    }    
   

    return -80;
}






int callback(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    printf("\n\n cal back\n");
    for (int i = 0; i < argc; i++) {

	
	int ns=-9;
        if((argv[i])[0]=='0')ns=0;
	if((argv[i])[0]=='1')ns=1;
	if((argv[i])[0]=='2')ns=2;
	
printf("old=%d    new=%d\n",ST,ns);
if(ns==1){book=1;}
if((book==1&&ST==0)){send_data(Gi,1); printf("sending 1");}




    }
    
    printf("\n");
    
    return 0;
}






void SelectAndSend(int iden)
{
	//printf("start select and send func with iden= %d \n",iden);
 	ssnd(iden); 
	//printf("stop select and send func with iden= %d \n",iden);
}


void UpdateDB(int identifier, int status)
{

	//printf("start update db func with iden= %d , status =%d\n",identifier,status);
    int rc;
    char *error;


    sqlite3 *db;
    rc = sqlite3_open("places.db", &db);

    
	 char *sqlUpdateTable=new char[60];//malloc(60);
	sprintf(sqlUpdateTable,"Update Place Set status = %d where identifier=%d;",status,identifier);
 printf("\n\n update %d \n\n",status);
    rc = sqlite3_exec(db, sqlUpdateTable, NULL, NULL, &error);

    sqlite3_close(db);

//printf("stop update db func with iden= %d , status =%d\n",identifier,status);
}
































//Configure LoRa tranciever
void startConfig() {

  int e;
    
  // Set transmission mode and print the result
  e = sx1272.setMode(loraMode);
  // Select frequency channel
  if (loraMode==11) {
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
  //e = sx1272.setNodeAddress(loraAddr);
  sx1272._nodeAddress=loraAddr;
  e=0;
}

void setup() {
  int e;
  
  //Add our code here
  Serial.begin(38400);
  // Power ON the module
  e = sx1272.ON();
  
  PRINT_CSTSTR("%s","^$**********Power ON: state ");
  PRINT_VALUE("%d", e);
  PRINTLN;

  e = sx1272.getSyncWord();

  if (!e) {
    PRINT_CSTSTR("%s","^$Default sync word: 0x");
    PRINT_VALUE("%X", sx1272._syncWord);
    PRINTLN;

  }    
  
  if (!e) {
    radioON=true;
    startConfig();
  }
  
  FLUSHOUTPUT;
  delay(1000);
printf("setup finished   333331\n\n\n\n");
}

void loop() {




  int i=0, e;
  int cmdValue;
    SelectAndSend(6);

  if (radioON) {
     
      e=1;
     
      uint16_t w_timer=1000;
      if (loraMode==1)
        w_timer=5000;

      e = sx1272.receivePacketTimeout(w_timer);
     
      if (!e) {
         int a=0, b=0;
         uint8_t tmp_length;

         sx1272.getSNR();
         sx1272.getRSSIpacket();

         tmp_length=sx1272._payloadlength;
         
         sprintf(sprintf_buf,"--- rxlora. dst=%d type=0x%.2X src=%d seq=%d len=%d SNR=%d RSSIpkt=%d BW=%d CR=4/%d SF=%d\n", 
                   sx1272.packet_received.dst,
                   sx1272.packet_received.type, 
                   sx1272.packet_received.src,
                   sx1272.packet_received.packnum,
                   tmp_length, 
                   sx1272._SNR,
                   sx1272._RSSIpacket,
                   (sx1272._bandwidth==BW_125)?125:((sx1272._bandwidth==BW_250)?250:500),
                   sx1272._codingRate+4,
                   sx1272._spreadingFactor);
                   
         PRINT_STR("%s",sprintf_buf);
       
         sprintf(sprintf_buf,"^p%d,%d,%d,%d,%d,%d,%d\n",
                   sx1272.packet_received.dst,
                   sx1272.packet_received.type,                   
                   sx1272.packet_received.src,
                   sx1272.packet_received.packnum, 
                   tmp_length,
                   sx1272._SNR,
                   sx1272._RSSIpacket);
                   
         PRINT_STR("%s",sprintf_buf);          

         
         sprintf(sprintf_buf,"^r%d,%d,%d\n", 
                   (sx1272._bandwidth==BW_125)?125:((sx1272._bandwidth==BW_250)?250:500),
                   sx1272._codingRate+4,
                   sx1272._spreadingFactor);
                   
         PRINT_STR("%s",sprintf_buf);


          int ide=sx1272.packet_received.src;
          int st=-9;
		  printf("\n\n");
		  printf("received data=%c\n",(char)sx1272.packet_received.data[0]);
         if((char)sx1272.packet_received.data[0]=='1')
         {
             st=1;
         }
          

  if((char)sx1272.packet_received.data[0]==(char)'2')
         {
             st=2;
         }


  if((char)sx1272.packet_received.data[0]=='0')
         {
             st=0;
         }



	printf("\n\n got %d",st);

	if(st==0&&ST==0){UpdateDB(ide, st);}
	if(st==1&&ST==0){UpdateDB(ide, st);ST=1;book=0;}
	if((st==2&&(ST==0||ST==1))){UpdateDB(ide, st);ST=2;book=0;}	
	if(st==0&&ST==2){UpdateDB(ide, st);ST=0;book=0;}
         // UpdateDB(ide, st);

      }

  }
}


int main (int argc, char *argv[]){

  setup();
 // hueta();
  while(1){
    loop();
  }
  
  return (0);
}








