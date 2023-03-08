/*
 * File:   Main18f_to_DS.c
 * Author: mario
 *
 * Created on 4 de marzo de 2023, 01:25 PM
 */



//Envió el cpodigo de I2C:

#include<xc.h>
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


#define LED_CPU LATC0         //Pin del led de CPU asociado al funcionamiento del micro
#define LED_Test LATC1        //Pin del led de prueba asociado a tareas especificas
#define LED_Fault LATC2       //Pin del led de Error asociado a bloqueos en el micro



void Start(void);
void Stop(void);
void Rstart(void);
void EnvioDato(unsigned char,unsigned char,unsigned char);
unsigned char LecturaDato(unsigned char,unsigned char);
unsigned char LecturaDatoA(unsigned char);
void EnvioDatoA(unsigned char Direccion, unsigned char Dato);
char Contador = 0;

void main(void){
  unsigned char aux=12;//Variable temporal para guardar dato de registro
  OSCCON=0b01110000; //Configuración reloj interno a 8MHz
  __delay_ms(1);
  
  TRISC=0;//Configuración del puerto C para LEDS como salida
  LATC=0; //Inicialización del puerto C de LEDS en cero
  
  TRISB0=1;
  TRISB1=1;
  SSPCON1=0b00101000;
  SSPCON2=0b00000000;
  SSPSTAT=0b11000000;
  SSPADD=120;//100kHz Calculo necesario SSPADD=(Fosc/4)/FrecI2C
  PIR1=0;  //Inicializacion de las banderas del registro de interrupción PIR1 en 0
  //__delay_ms(100);
  while(1){
    __delay_ms(10);
      EnvioDato(0x10,1,Contador);//Envio de dato posicion 1 con valor variable 
      aux = LecturaDato(0x10,0); //Lectura del dato almacenado en la posición 0
      EnvioDato(0x10,0,8); //Encio de dato en posicion 0 con valor fijo de 0
      Contador ++; //Incremento de valor variable en 1
      if(Contador >= 10){//Reseteo de variable a 0 cuando llega a 10
          Contador=0;
      }
      if(aux==0){//Identifica si el valor almacenado en la posición 0 es igual a 0
          LATC0=1;
      }
      else{
          LATC0=0;
      }
    LATC2=1; //Led de funcionamiento On
    __delay_ms(1000);
    LATC2=0; //Led de funcionamiento Off
    __delay_ms(1000);
  }  
}
void Start(void){    
  SEN=1;
  while(SEN==1);  
}
void Stop(void){
  PEN=1;
  while(PEN==1);  
}
void Rstart(void){
  RSEN=1;
  while(RSEN==1);  
}
void EnvioDato(unsigned char Direccion, unsigned char Registro, unsigned char Dato){
  Start();
  
  SSPBUF=Direccion & 0b11111110;
  SSPIF=0;
  while(SSPIF==0);
  
  SSPBUF=Registro;
  SSPIF=0;
  while(SSPIF==0);
  
  SSPBUF=Dato;
  SSPIF=0;
  while(SSPIF==0);
  
  SSPIF=0;
  Stop();  
}
unsigned char LecturaDato(unsigned char Direccion, unsigned char Registro){
  Start();
  
  SSPBUF=Direccion & 0b11111110;
  SSPIF=0;
  while(SSPIF==0);
  
  SSPBUF=Registro;
  SSPIF=0;
  while(SSPIF==0);
  
  Rstart();
  
  SSPBUF=Direccion | 0b00000001;
  SSPIF=0;
  while(SSPIF==0);
  
  RCEN=1;
  SSPIF=0;
  while(SSPIF==0);
  
  SSPIF = 0;
  ACKDT = 1;
  ACKEN = 1;
  while(SSPIF==0);
  
  
  Stop();
  return SSPBUF;  
}
unsigned char LecturaDatoA(unsigned char Direccion){
  
  Start();
  LATE2=1;
  LATE1=1;
  
  
  SSPBUF=Direccion | 0b00000001; //& 0b11111110;
  SSPIF=0;
  while(SSPIF==0);
  LATE2=0;
  
  
  RCEN=1;
  SSPIF=0;
  while(SSPIF==0);
  LATE2=1;
  
  
  Stop();
  LATE2=0;
  LATE1=0;
  return SSPBUF;  
}
void EnvioDatoA(unsigned char Direccion, unsigned char Dato){
    
    //Envio de condicion de Start
    Start();
    LATC0=1;
    LATC1=1;
    
    //Envio de Dirección de Escritura
    SSPBUF=Direccion; //& 0b11111110;
    SSPIF=0;
    while(SSPIF==0);
    LATC0=0;
    
    //Envio de dato
    SSPBUF=Dato;
    SSPIF=0;
    while(SSPIF==0);
    LATC0=1;
    
    //Envio de dirección de Stop
    SSPIF=0;
    Stop(); 
    LATC0=0;
    LATC1=0;
}
