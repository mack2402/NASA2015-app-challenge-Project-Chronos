/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////  HID_unity.c                                                    ////
////                                                                 ////
////  by GESH                                                        ////
////                                                                 ////
////  08/10/2006                                                     ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN
#use delay(clock=48000000)

#DEFINE USB_HID_DEVICE  TRUE

#define USB_EP1_TX_ENABLE  USB_ENABLE_INTERRUPT   
#define USB_EP1_TX_SIZE 8

#define USB_EP1_RX_ENABLE  USB_ENABLE_INTERRUPT  
#define USB_EP1_RX_SIZE 8

#include <pic18_usb.h>
#include <.\include\usb_kbd_HID.h>               
#include <usb.c>                                 
#include <ctype.h>

#define LED1    PIN_E0
#define LED2    PIN_E1
#define LED3    PIN_E2

#define front PIN_B0    // 'w'
#define back PIN_B1     // 's'
#define item1 PIN_B2    // '1'
#define item2 PIN_B3    // '2'
#define resetdb PIN_B4  // 'r'


#define LED_ON  output_high
#define LED_OFF output_low

int8 connected;
int8 enumerated;
int8 rx_msg[USB_EP1_RX_SIZE];
int8 tx_msg[8]={2,0,0,0,0,0,0,0};

char NextChar='0';
int1 hay_dato=0;

int8 i;


void usb_debug_task(void) {

   enumerated=usb_enumerated();

   if(enumerated){
      LED_ON(LED1);
      LED_OFF(LED3);
   }
   else{
      LED_ON(LED3);
      LED_OFF(LED1);
   }
}


int8 char_2_usb_kbd_code(char c){

   int8 ic;

   if(isAlpha(c)){
      ic=c-'a'+4;
   }
   else{
      if(c=='0'){
        ic=39;
      }
      else{
        ic=c-'1'+30;
      }
   }
   return(ic);
}

void usb_keyboard_task(void) {

   static char Char_Tx;
   
   int key;

   key = input_b();
   
   if(key != 0){
   
   switch(key){
     case 1:
         NextChar = 'w';
     break;
     
     case 2:
         NextChar = 's';
     break;
     
     case 3:
         NextChar = 'a';
     break;
     
     case 4:
         NextChar = '1';
     break;
     
     case 8:
         NextChar = '2';
     break;
     
     case 16:
         NextChar = 'r';
     break;
     
     default:
         NextChar = '0';
   }
      tx_msg[3]=char_2_usb_kbd_code(NextChar);
      usb_put_packet(1,tx_msg,sizeof(tx_msg),USB_DTS_TOGGLE);
      
      LED_OFF(LED2);
      delay_ms(5);
      
      
   }
   else{

      tx_msg[3]=0;
      usb_put_packet(1,tx_msg,sizeof(tx_msg),USB_DTS_TOGGLE);
      delay_ms(5);

   }
}



void usb_rx_task(void){

   if (usb_kbhit(1)){
      usb_get_packet(1, rx_msg, sizeof(rx_msg));
   }
}


#int_ext
void ext_handler(void){

   if(hay_dato==0){
     hay_dato=1;
     LED_ON(LED2);
   }
}

void main() {

   hay_dato=1;

   delay_ms(500);

   LED_OFF(LED1);
   LED_OFF(LED2);
   LED_OFF(LED3);

   usb_init_cs();


   set_tris_b(0b00011111);
   output_bit(PIN_B7, 0);
   output_bit(PIN_B6, 0);
   output_bit(PIN_B5, 0);

   while (TRUE) {

      usb_task();
      usb_debug_task();

      if (usb_enumerated()) {
         usb_keyboard_task();
         usb_rx_task();
         delay_ms(5);
      }
   }
}

