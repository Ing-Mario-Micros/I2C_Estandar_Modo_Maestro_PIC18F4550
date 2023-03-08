/*
 * File:   Main18f_to_DS.c
 * Author: mario
 *
 * Created on 4 de marzo de 2023, 01:25 PM
 */



//Envió el cpodigo de I2C:

#include<xc.h>
#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF          //Desactivación del perro guardian
#pragma config LVP=OFF                 //Programación de Bajo voltaje Desactivada
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
  unsigned char aux=12;
  int ax,ay,az,gx,gy,gz;
  OSCCON=0b01110000;
  __delay_ms(1);
  TRISD=0x00;
  LATD=0;
  TRISB=0xFF;
  TRISE=0;
  LATE=0;
  TRISC=0;
  LATC=0;
  SSPCON1=0b00101000;
  SSPCON2=0b00000000;
  SSPSTAT=0b11000000;
  SSPADD=19;
  PIR1=0;  
  //__delay_ms(100);
  while(1){
    __delay_ms(10);
      /*EnvioDato(0x0B,Contador,Contador);*/
      //LATD=LecturaDatoA(0x10);
      //EnvioDatoA(0x10,1);
      EnvioDato(0x10,1,Contador);
      aux = LecturaDato(0x10,0);
      EnvioDato(0x10,0,5);
      Contador ++;
      if(Contador >= 10){
          Contador=0;
      }
      if(aux==0){
          LATC0=1;
      }
      else{
          LATC0=0;
      }
    //EnvioDatoA(0x10,1);
    //Contador++;
    LATC2=1;
    __delay_ms(1000);
    LATC2=0;
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
  SSPBUF=Direccion; //& 0b11111110;
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
  while(RB2 == 1);
  __delay_ms(500);
  Start();
  LATE2=1;
  LATE1=1;
  
  while(RB2 == 1);
    __delay_ms(500);
  SSPBUF=Direccion | 0b00000001; //& 0b11111110;
  SSPIF=0;
  while(SSPIF==0);
  LATE2=0;
  
  while(RB2 == 1);
    __delay_ms(500);
  RCEN=1;
  SSPIF=0;
  while(SSPIF==0);
  LATE2=1;
  
  while(RB2 == 1);
    __delay_ms(500);
  Stop();
  LATE2=0;
  LATE1=0;
  return SSPBUF;  
}
void EnvioDatoA(unsigned char Direccion, unsigned char Dato){
    //while(RB2 == 1);
    //Envio de condicion de Start
    __delay_ms(500);
    Start();
    LATC0=1;
    LATC1=1;
    
    //while(RB2 == 1);
    //Envio de Dirección de Escritura
    __delay_ms(500);
    SSPBUF=Direccion; //& 0b11111110;
    SSPIF=0;
    while(SSPIF==0);
    LATC0=0;
    
    //while(RB2 == 1);
    //Envio de dato
    __delay_ms(500);
    SSPBUF=Dato;
    SSPIF=0;
    while(SSPIF==0);
    LATC0=1;
    
    //while(RB2 == 1);
    //Envio de dirección de Stop
    __delay_ms(500);
    SSPIF=0;
    Stop(); 
    LATC0=0;
    LATC1=0;
}
