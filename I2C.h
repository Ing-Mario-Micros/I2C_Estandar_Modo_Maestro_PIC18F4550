/* 
 * File:   I2C.h
 * Author: mario
 *
 * Created on 8 de marzo de 2023, 06:38 AM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include<xc.h>
/*-------------------------- Función de Configuración I2C --------------------*/
void Activar_I2C(void);
/*------------------------ Funciones de ordenes de protocolo -----------------*/
void Start(void);
void Stop(void);
void Rstart(void);
/*--------------- Funciones para la comunicación I2C Estandar ----------------*/
void EnvioDato(unsigned char,unsigned char,unsigned char);
unsigned char LecturaDato(unsigned char,unsigned char);
unsigned char LecturaDatoA(unsigned char);
void EnvioDatoA(unsigned char Direccion, unsigned char Dato);
/*-------------------------- Metodos de Configuración I2C --------------------*/
void Activar_I2C(void){
    TRISB0=1;
    TRISB1=1;
    SSPCON1=0b00101000;
    SSPCON2=0b00000000;
    SSPSTAT=0b11000000;
    SSPADD=120;//100kHz Calculo necesario SSPADD=(Fosc/4)/FrecI2C
    PIR1=0;  //Inicializacion de las banderas del registro de interrupción PIR1 en 0

}
/*------------------------ Metodos de ordenes de protocolo -------------------*/
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
/*--------------- Metodos para la comunicación I2C Estandar ----------------*/
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

#endif	/* I2C_H */

