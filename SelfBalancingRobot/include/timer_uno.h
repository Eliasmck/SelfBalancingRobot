#include <math.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

uint16_t prescalerValue_T16 = 0;

uint16_t prescalerValue_T8 = 0;

/* Configuracion para timer 16 bits*/
int confPrescaler_T16(float T);
int confModo_T16(uint8_t modo);
int confModoSalidas_T16(uint8_t outA, uint8_t outB);
int interrupciones_T16(uint8_t InputCapt, uint8_t OutputCaptA, uint8_t OutputCaptB, uint8_t Overflow);
void setDutyA16(float dutyA);
void setDutyB16(float dutyB);
void setTP16(float T);

/* Configuracion para timer 8 bits*/
int confPrescaler_T8(float T);
int confModo_T8(uint8_t modo);
int confModoSalidas_T8(uint8_t outA, uint8_t outB);
int interrupciones_T8(uint8_t OutputCaptA, uint8_t OutputCaptB, uint8_t Overflow);
void setDutyA8(float dutyA);
void setDutyB8(float dutyB);

/**
* CSn2 CSn1 CSn0
*  0    0    1    clkI/O/1 (No prescaling      (1<<CSn0)
*  0    1    0    clkI/O/8 (From prescaler)    (2<<CSn0)
*  0    1    1    clkI/O /64 (From prescaler)  (3<<CSn0)
*  1    0    0    clkI/O/256 (From prescaler)  (4<<CSn0)
*  1    0    1    clkI/O/1024 (From prescaler) (5<<CSn0)
*/

int confPrescaler_T16(float T) {
/**
*   Funcion que setear el prescalaer para un timer de 16bits dado
*   @param T: tiempo en ms que se quiere obtener
*   @returns 0/1 : 0 si todo funciono 1 si algo anda mal
*/
  T = (float)(T/1000.0);
  float aux = (float)(pow(2, 16)/F_CPU); //Maximo Tiempo en s que se puede llegar
  if      (T <= aux)      prescalerValue_T16 = 1;
  else if (T <= 8 * aux)  prescalerValue_T16 = 8;
  else if (T <= 64 * aux)  prescalerValue_T16 = 64;
  else if (T <= 256 * aux)  prescalerValue_T16 = 256;
  else if (T <= 1024 * aux)  prescalerValue_T16 = 1024;
  else if (T > 1024 * aux) return 1; //Se puede probar si anda con return 2 para codificar el error
  TCCR1B &=~ (7<<CS10); //Coloco en 0 todo el registro TCCRXB (Reset de la configuracion)

  switch (prescalerValue_T16) {
    case 0: //timer off
          TCCR1B &=~ (7<<CS10); //Coloco en 0 todo el registro TCCRXB
          break;
    case 1:
          TCCR1B |= (1<<CS10);
          break;
    case 8:
          TCCR1B |= (2<<CS10);
          break;
    case 64:
          TCCR1B |= (3<<CS10);
          break;
    case 256:
          TCCR1B |= (4<<CS10);
          break;
    case 1024:
          TCCR1B |= (5<<CS10);
          break;
    default:
          return 1;
  }
return 0;
}

int confPrescaler_T8(float T) {
/**
*   Funcion que setear el prescalaer para un timer de 16bits dado
*   @param T: tiempo en ms que se quiere obtener
*   @returns 0/1 : 0 si todo funciono 1 si algo anda mal
*/
  T = (float)(T/1000.0);
  float aux = (float)(pow(2, 8)/F_CPU); //Maximo Tiempo en s que se puede llegar
  if      (T <= aux)      prescalerValue_T8 = 1;
  else if (T <= 8 * aux)  prescalerValue_T8 = 8;
  else if (T <= 64 * aux)  prescalerValue_T8 = 64;
  else if (T <= 256 * aux)  prescalerValue_T8 = 256;
  else if (T <= 1024 * aux)  prescalerValue_T8 = 1024;
  else if (T > 1024 * aux) return 1; //Se puede probar si anda con return 2 para codificar el error
  TCCR0B &=~ (7<<CS00); //Coloco en 0 todo el registro TCCRXB (Reset de la configuracion)

  switch (prescalerValue_T8) {
    case 0: //timer off
          TCCR0B &=~ (7<<CS00); //Coloco en 0 todo el registro TCCRXB
          break;
    case 1:
          TCCR0B |= (1<<CS00);
          break;
    case 8:
          TCCR0B |= (2<<CS00);
          break;
    case 64:
          TCCR0B |= (3<<CS00);
          break;
    case 256:
          TCCR0B |= (4<<CS00);
          break;
    case 1024:
          TCCR0B |= (5<<CS00);
          break;
    default:
          return 1;
  }
return 0;
}

int confModo_T16(uint8_t modo) {
/**
* Función para selección del modo de uso de timer
* @param modo: modo del timer a usar 0-15
* @returns 0/1 : 0 si todo funciono 1 si algo anda mal
*/
    TCCR1A &=~ (3<<WGM10);
    TCCR1B &=~ (3<<WGM12);
    switch (modo) {
      case 0:
            TCCR1A &=~ (3<<WGM10);
            TCCR1B &=~ (3<<WGM12);
            break;
      case 1:
            TCCR1A |= (1<<WGM10);
            break;
      case 2:
            TCCR1A |= (1<<WGM11);
            break;
      case 3:
            TCCR1A |= (3<<WGM10);
            break;
      case 4:
            TCCR1B |= (1<<WGM12);
            break;
      case 5:
            TCCR1A |= (1<<WGM10);
            TCCR1B |= (1<<WGM12);
            break;
      case 6:
            TCCR1A |= (1<<WGM11);
            TCCR1B |= (1<<WGM12);
            break;
      case 7:
            TCCR1A |= (3<<WGM10);
            TCCR1B |= (1<<WGM12);
            break;
      case 8:
            TCCR1B |= (1<<WGM13);
            break;
      case 9:
            TCCR1A |= (1<<WGM10);
            TCCR1B |= (1<<WGM13);
            break;
      case 10:
            TCCR1A |= (1<<WGM11);
            TCCR1B |= (1<<WGM13);
            break;
      case 11:
            TCCR1A |= (3<<WGM10);
            TCCR1B |= (1<<WGM13);
            break;
      case 12:
            TCCR1B |= (3<<WGM12);
            break;
      case 13:
            TCCR1A |= (1<<WGM10);
            TCCR1B |= (3<<WGM12);
            break;
      case 14:
            TCCR1A |= (1<<WGM11);
            TCCR1B |= (3<<WGM12);
            break;
      case 15:
            TCCR1A |= (3<<WGM10);
            TCCR1B |= (3<<WGM12);
            break;
      default :
            return 1;
    }
return 0;
}

int confModo_T8(uint8_t modo) {
/**
* Función para selección del modo de uso de timer
* @param modo: modo del timer a usar 0-15
* @returns 0/1 : 0 si todo funciono 1 si algo anda mal
*/
    TCCR0A &=~ (3<<WGM00);
    TCCR0B &=~ (1<<WGM02);
    switch (modo) {
      case 0:
            TCCR0A &=~ (3<<WGM00);
            TCCR0B &=~ (1<<WGM02);
            break;
      case 1:
            TCCR0A |= (1<<WGM00);
            break;
      case 2:
            TCCR0A |= (1<<WGM01);
            break;
      case 3:
            TCCR0A |= (3<<WGM00);
            break;
      case 4:
            TCCR0B |= (1<<WGM02);
            break;
      case 5:
            TCCR0A |= (1<<WGM00);
            TCCR0B |= (1<<WGM02);
            break;
      case 6:
            TCCR0A |= (1<<WGM01);
            TCCR0B |= (1<<WGM02);
            break;
      case 7:
            TCCR0A |= (3<<WGM00);
            TCCR0B |= (1<<WGM02);
            break;
      default :
            return 1;
    }
return 0;
}

int confModoSalidas_T16(uint8_t outA, uint8_t outB){
/**
* Nota: los pines asociados a la temporización que se deseen utilizar con algún modo de salida, deben ser declarados como salidas por código antes de habilitar el timer
* Función para selección del modo de las salidas del timer
* @param outA: modo salida A
* @param outB: modo salida B
* @returns 0/1 : 0 si todo funciono 1 si algo anda mal
*/
  TCCR1A &=~ (3<<COM1A0);
  TCCR1A &=~ (3<<COM1B0);
  switch (outA) {
    case 0: //OC1A off
          TCCR1A &=~ (3<<COM1A0);
          break;
    case 1: //OC1A toggle
          TCCR1A |= (1<<COM1A0);
          break;
    case 2: //OC1A clear
          TCCR1A |= (1<<COM1A1);
          break;
    case 3: //OC1A set
          TCCR1A |= (3<<COM1A0);
    default :
            return 1;
  }
  switch (outB) {
    case 0: //OC1B off
          TCCR1A &=~ (3<<COM1B0);
          break;
    case 1: //OC1B toggle
          TCCR1A |= (1<<COM1B0);
          break;
    case 2: //OC1B clear
          TCCR1A |= (1<<COM1B1);
          break;
    case 3: //OC1B set
          TCCR1A |= (3<<COM1B0);
    default :
            return 1;
  }
return 0;
}

int confModoSalidas_T8(uint8_t outA, uint8_t outB){
/**
* Nota: los pines asociados a la temporización que se deseen utilizar con algún modo de salida, deben ser declarados como salidas por código antes de habilitar el timer
* Función para selección del modo de las salidas del timer
* @param outA: modo salida A
* @param outB: modo salida B
* @returns 0/1 : 0 si todo funciono 1 si algo anda mal
*/
  TCCR0A &=~ (3<<COM0A0);
  TCCR0A &=~ (3<<COM0B0);
  switch (outA) {
    case 0: //OC1A off
          TCCR0A &=~ (3<<COM0A0);
          break;
    case 1: //OC1A toggle
          TCCR0A |= (1<<COM0A0);
          break;
    case 2: //OC1A clear
          TCCR0A |= (1<<COM0A1);
          break;
    case 3: //OC1A set
          TCCR0A |= (3<<COM0A0);
    default :
            return 1;
  }
  switch (outB) {
    case 0: //OC1B off
          TCCR0A &=~ (3<<COM0B0);
          break;
    case 1: //OC1B toggle
          TCCR0A |= (1<<COM0B0);
          break;
    case 2: //OC1B clear
          TCCR0A |= (1<<COM0B1);
          break;
    case 3: //OC1B set
          TCCR0A |= (3<<COM0B0);
    default :
            return 1;
  }
return 0;
}

int interrupciones_T16(uint8_t InputCapt, uint8_t OutputCaptA, uint8_t OutputCaptB, uint8_t Overflow){
/*
* función para la habilitación opcional de las distintas fuentes de interrupción disponibles
* @param InputCapt: activa interrupciones por InputCapt
* @param OutputCaptA: activa interrupción por
* @param OutputCaptB: activa interrupción probar
* @parm Overflow: activa interrupción por Overflow
* @returns 0/1: si todo marcha bien/mal
*/
    if (InputCapt) {
      TIFR1 &=~ (1<<ICF1); //apaga flag
      TIMSK1 |= (1<<ICIE1); //habilita interrupcion
    }
    if (OutputCaptA) {
      TIFR1 &=~ (1<<OCF1A); //apaga flag
      TIMSK1 |= (1<<OCIE1A); //habilita interrupcion
    }
    if (OutputCaptB) {
      TIFR1 &=~ (1<<OCF1B); //apaga flag
      TIMSK1 |= (1<<OCIE1B); //habilita interrupcion
    }
    if (Overflow) {
      TIFR1 &=~ (1<<TOV1); //apaga flag
      TIMSK1 |= (1<<TOIE1); //habilita interrupcion
    }
return 0;
}

int interrupciones_T8(uint8_t OutputCaptA, uint8_t OutputCaptB, uint8_t Overflow){
/*
* función para la habilitación opcional de las distintas fuentes de interrupción disponibles
* @param InputCapt: activa interrupciones por InputCapt
* @param OutputCaptA: activa interrupción por
* @param OutputCaptB: activa interrupción probar
* @parm Overflow: activa interrupción por Overflow
* @returns 0/1: si todo marcha bien/mal
*/
    if (OutputCaptA) {
      TIFR0 &=~ (1<<OCF0A); //apaga flag
      TIMSK0 |= (1<<OCIE0A); //habilita interrupcion
    }
    if (OutputCaptB) {
      TIFR0 &=~ (1<<OCF0B); //apaga flag
      TIMSK0 |= (1<<OCIE0B); //habilita interrupcion
    }
    if (Overflow) {
      TIFR0 &=~ (1<<TOV0); //apaga flag
      TIMSK0 |= (1<<TOIE0); //habilita interrupcion
    }
return 0;
}

void setDutyA16(float dutyA){
  /* dutyA en ms
  * funciones  específicas  actualiza el valor del duty cicle A
  */
  OCR1A = (uint16_t)((dutyA/1000)*(F_CPU/prescalerValue_T16)+1); //Numero hasta el cual cuenta para llegar al
                                                               // tiempo del periodo de trabajo A del PWM
}

void setDutyA8(float dutyA){
  /* dutyA en ms
  * funciones  específicas  actualiza el valor del duty cicle A
  */
  float aux = (dutyA/1000)*(F_CPU/prescalerValue_T8)+1;
  OCR0A = (uint8_t) (aux); //Numero hasta el cual cuenta para llegar al
                                                               // tiempo del periodo de trabajo A
}

void setDutyB16(float dutyB){
  /* dutyB en ms
  * funciones  específicas  actualiza el valor del duty cicle B
  */
  OCR1B = (uint16_t)((dutyB/1000)*(F_CPU/prescalerValue_T16)+1); //Numero hasta el cual cuenta para llegar al
                                                              // tiempo del periodo de trabajo B del PWM
}

void setDutyB8(float dutyB){
  /* dutyB en ms
  * funciones  específicas  actualiza el valor del duty cicle B
  */
  float aux = (dutyB/1000)*(F_CPU/prescalerValue_T8)+1;
  OCR0B = (uint8_t) (aux); //Numero hasta el cual cuenta para llegar al
                                                              // tiempo del periodo de trabajo B
}

void setTP16(float T) {
  /* T en ms
  * funciones  específicas  para PWM 14 que setea el valor del periodo base del pwm
  */

      ICR1 = (uint16_t)((T/1000)*(F_CPU/prescalerValue_T16)+1); //Numero hasta el cual cuenta para llegar al
                                                              // tiempo del periodo base del PWM
}