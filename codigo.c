#include <18F87J50.h>
#fuses  NOWDT,PLL2,STVREN,NOXINST,NODEBUG,NOCPUDIV,PROTECT,INTRC,FCMEN,NOIESO,NOEASHFT,MCU,NOWAIT 
#use delay(clock=48M)                              //Reloj interno
#define fast_io(j,b,d)                             //Codigo generado por el compilador para entrada salida
#DEFINE USB_HID_DEVICE  TRUE                       //Modo HID del USB
#define USB_EP1_TX_ENABLE  USB_ENABLE_BULK         //End Point 1 para entrada (Ep1)
#define USB_EP1_TX_SIZE    8                       //Tamaño de bits de envio
#define USB_EP1_RX_ENABLE  USB_ENABLE_BULK         //End Point 1 para salida (EP1)
#define USB_EP1_RX_SIZE    8                       //Tamaño de bits de recepcion
#include <header.h>                                //Libreria para el ID del vendedor y PID
#include <pic18_usb.h>                             //Funciones de bajo nivel(hardware) para la serie PIC 18Fxx5x que serviran en usb.c
#include <usb.c>                                   //libreria para el manejo del usb

int Entrada[3] = {0},Envio[2] = {0};               //Array de 3 y de 2 posiciones con 8 bits 
int i=0;

void main(){                                       //Cuerpo del programa
   set_tris_j(0x00);                               //Puerto j como entrada
   set_tris_b(0x00);                               //Puerto b como entrada
   set_tris_d(0x02);                               //Pin D0 Como entrada
   output_j(0x00);
   output_b(0x00);
   usb_init();                                     //Inicializamos usb
   usb_task();                                     //Monitorea el estado de la coneccion conectandose y desconectandose automaticamente
   usb_wait_for_enumeration();                     //Espera infinitamente hasta que el dispositivo fue enumerado    
   while (TRUE){                                   //Ciclo infinito
      usb_task();                                  //Preparamos el periferico del usb
      if (usb_enumerated()){                       //Si ya lo detecto la compu
         if (usb_kbhit(1)){                        //Si ya hay un paquete para recibir
            usb_get_packet(1, Entrada, 3);         //Recibimos dato por canal 1, en la variable Entrada de 3 byte
            if(Entrada[0]==1){                     //Si el primer byte es 1
               output_high(PIN_B0);
            }else{                                 //Si no
               output_low(PIN_B0);
            }
            output_j(Entrada[1]);                  //Mandamos dato del segundo byte recibido
         }
         if(input_state(PIN_D1)==0){               //Si precionamos el push button
            i++;                                   //Incrementamos contador
         }
         if(i==255){                               //Si contador llega a 255
            i=0;                                   //Reinciamos contador
         }
         Envio[0]=i;                               //Mandamos valor del contador por el primer byte
         usb_put_packet(1, Envio, 2, USB_DTS_TOGGLE); //Enviamos el paquete de tamaño 2 (bytes) del EP1 al PC
      }
   }
}
