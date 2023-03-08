/*
 * File:   Main18f_to_DS.c
 * Author: mario
 *
 * Created on 4 de marzo de 2023, 01:25 PM
 */



//Envió el cpodigo de I2C:

#include<xc.h>
#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC   //Fuente de reloj con oscilador interno
#pragma config WDT=OFF          //Desactivación del perro guardian
#pragma config LVP=OFF          //Programación de Bajo voltaje Desactivada
#pragma config PBADEN = OFF     //Desacivación las funciones analogas del puerto B



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
  SSPADD=20;//100kHz Calculo necesario SSPADD=(Fosc/4)/FrecI2C
  PIR1=0;  //Inicializacion de las banderas del registro de interrupción PIR1 en 0
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
