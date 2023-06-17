/************************************************************************************************
Copyright (c) 2023, Mariano Carcamo marianocarcamo98@gmail.com
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
SPDX-License-Identifier: MIT
*************************************************************************************************/

#ifndef RELOJ_H
#define RELOJ_H

/** \brief Archivo publico de la libreria Reloj.h
 **
 ** Esta libreria permite la implementacion de relojes despertador. Utiliza vectores en formato BCD
 ** para el manejo del "tiempo".
 **
 ** \addtogroup name Module denomination
 ** \brief Brief description of the module
 ** @{ */

/* === Headers files inclusions ================================================================ */

#include <stdbool.h>
#include <stdint.h>

/* === Cabecera C++ ============================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/// @brief  Descriptor de la clase clock_s
typedef struct clock_s * clock_t;

/**
 * @brief Funcion prototipo para el disparo de la alarma
 *
 * Se provee el prototipo de funcion a la que se llamara cuando la alarma sea disparada por el reloj
 * @param reloj Descriptor del reloj al que se le asigna dicha funcion.
 */
typedef void (*funcion_disparo)(clock_t reloj);

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/// @brief Metodo de creacion de un objeto reloj.
/// @param tics_por_segundo Base de tiempos del reloj
/// @param funcion Evento de disparo de la alarma
/// @return Puntero a un objeto clock_s
clock_t ClockCreate(int tics_por_segundo, funcion_disparo funcion);

/**
 * @brief Metodo para realizar el avance del tiempo
 *
 * Esta funcion permite el avance de la hora mediante la llamada periodica. Por esta razon debe ser
 * llamada desde una rutina de interrupcion del tipo tic, como ser la provista por SysTic.
 *
 * @param reloj Descriptor del reloj.
 */
void ClockTic(clock_t reloj);

/**
 * @brief Metodo para obtener la hora actual.
 *
 * @param reloj Descriptor del reloj.
 * @param hora Vector donde se almacenara el valor de la hora actual.
 * @param size Tamanio del vector en bytes.
 * @return true La hora es valida.
 * @return false La hora no es valida / no fue configurada previamente de forma exitosa.
 */
bool ClockGetTime(clock_t reloj, uint8_t * hora, int size);

/**
 * @brief Metodo para configurar la hora actual
 *
 * @param reloj Descriptor del reloj.
 * @param hora Valor de hora en la que se quiere configurar el reloj.
 * @param size Tamanio del vector en bytes
 * @return true Se configuro exitosamente.
 * @return false No se configuro de forma exitosa.
 */
bool ClockSetTime(clock_t reloj, const uint8_t * hora, int size);

/**
 * @brief Metodo para revisar la alarma.
 *
 * @param reloj Descriptor del reloj.
 * @param hora Vector donde se almacenara el valor de la alarma.
 * @param size Tamanio del vector en bytes.
 * @return true La alarma se encuentra habilitada.
 * @return false La alarma se encuentra deshabilitada.
 */
bool ClockGetAlarm(clock_t reloj, uint8_t * hora, int size);

/**
 * @brief Metodo para configurar la alarma.
 *
 * @param reloj Descriptor del reloj.
 * @param hora Hora de seteo de la alarma.
 * @param size Tamanio del vector en bytes.
 * @return true Se configuro correctamente.
 * @return false No fue configurada previamente de forma exitosa.
 */
bool ClockSetAlarm(clock_t reloj, const uint8_t * hora, int size);

/**
 * @brief Metodo para cambiar el estado de habilitacion de la alarma
 *
 * @param reloj Descriptor de la alarma
 * @return true La alarma se encuentra habilitada
 * @return false La alarma se encuentra deshabilitada
 */
bool AlarmToggel(clock_t reloj);

/**
 * @brief Metodo para posponer la alarma
 *
 * @param reloj Descriptor del reloj.
 * @param min Cantidad de minutos a posponer.
 */
void AlarmSnooze(clock_t reloj, int min);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif

/** @} End of module definition for doxygen */

#endif /* RELOJ_H */