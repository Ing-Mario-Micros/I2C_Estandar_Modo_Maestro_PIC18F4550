/*
 * File:   Main18f_to_DS.c
 * Author: mario
 *
 * Created on 4 de marzo de 2023, 01:25 PM
 */



//Envió el cpodigo de I2C:

#include<xc.h>
#include "I2C.h"
#define _XTAL_FREQ 48000000      //Frecuencia de trabajo 48MHz
 /**Directivaas de configuración para el Oscilador**/
#pragma config FOSC = HSPLL_HS  //Configurar fuente de reloj por cristal con PLL
#pragma config PLLDIV = 5       //Dividir entrada de reloj en 5 - Para este caso 20/5=4MHz
#pragma config CPUDIV = OSC1_PLL2//Configurar el divisor de CPU en este caso de 2 - 96MHz/2=48MHz
/*La configuración antes usada depende del oscilador de cristal usado y de que
 *velocidad de trabajo se requiere para la CPU, en este caso al usar PLL se requiere
 *escalara el oscilador a 4MHz para tener 98MHz y luego de pasar por el divisor de cpu
 *se obtienen 48 MHz. [20MHz/5 = 4MHz]->[PLL=96MHz]->[96MHz/(2->CPUDIV) = 48MHz]
 */
/**Directivas de configuración del microcontrolador**/
#pragma config WDT=OFF          //Desactivación del perro guardian
#pragma config LVP=OFF          //Programación de Bajo voltaje Desactivada
#pragma config PBADEN = OFF     //Desacivación las funciones analogas del puerto B
/**Directivaas de configuración para el USB**/
#pragma config XINST = OFF
#pragma config USBDIV = 2
#pragma config VREGEN = ON


#define LED_CPU LATC1         //Pin del led de CPU asociado al funcionamiento del micro
#define LED_Test LATC2        //Pin del led de prueba asociado a tareas especificas
#define LED_Fault LATC0       //Pin del led de Error asociado a bloqueos en el micro





char Contador = 0;

void main(void){
  unsigned char aux=12;//Variable temporal para guardar dato de registro
  
  /*Configuracion del oscilador interno*/
  OSCCON=0b01110000; //Configuración reloj interno a 8MHz
  __delay_ms(1);
  
  /*configuración de pines para led*/
  TRISC=0;//Configuración del puerto C para LEDS como salida
  LATC=0; //Inicialización del puerto C de LEDS en cero
  
  /*Configuración del Modulo mssp en modo I2C*/
  Activar_I2C();
  
  //__delay_ms(100);
  while(1){
    __delay_ms(10);
      EnvioDato(0x10,1,Contador);//Envio de dato posicion 1 con valor variable 
      aux = LecturaDato(0x10,0); //Lectura del dato almacenado en la posición 0
      EnvioDato(0x10,0,0); //Encio de dato en posicion 0 con valor fijo de 0
      Contador ++; //Incremento de valor variable en 1
      if(Contador >= 10){//Reseteo de variable a 0 cuando llega a 10
          Contador=0;
      }
      if(aux==0){//Identifica si el valor almacenado en la posición 0 es igual a 0
          LED_Test=1;
      }
      else{
          LED_Test=0;
      }
    LED_CPU=1; //Led de funcionamiento On
    __delay_ms(1000);
    LED_CPU=0; //Led de funcionamiento Off
    __delay_ms(1000);
  }  
}
