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

#ifndef DIGITAL_H
#define DIGITAL_H

/** \brief Generacion de la clase digital
 **
 ** Genera la clase digital que permite determinar entradas y salidas digitales
 **
 ** \addtogroup name Module denomination
 ** \brief Brief description of the module
 ** @{ */

/* === Headers files inclusions ================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* === Cabecera C++ ============================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

//! Referencia a un descriptor para gestionar una salida digital 
typedef struct digital_output_s * digital_output_t;

//! Referencia a un descriptor para gestionar una entrada digital 
typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Metodo para crear una entrada digital
 * 
 * @param port Puerto GPIO que contiene a la entrada
 * @param pin Numero de terminal del puerto GPIO asignado a la entrada
 * @param logic "false" para indicar activo en alto / "true" para indicar activo en bajo
 * @return digital_output_t Puntero al descriptor de la entrada creada
 */

digital_input_t DigitalInputCreate(uint8_t port, uint8_t pin, bool logic);

/**
 * @brief Metodo para leer el estado de la entrada
 * 
 * @param input Puntero al descriptor de la salida 
 * @return true La entrada se encuentra activada
 * @return false La entrada se encuentra desactivada
 */

bool DigitalInputGetState(digital_input_t input);

/**
 * @brief Metodo para detectar un cambio en la entrada
 * 
 * @param input Puntero al descriptor de la entrada
 * @return true La entrada tuvo un cambio desde el ultimo llamado
 * @return false La entrada no tuvo cambio desde el ultimo llamado 
 */

bool DigitalInputHasChange(digital_input_t input);

/**
 * @brief Metodo para detectar el estado activo de la entrada 
 * 
 * @param input Puntero al descriptor de la entrada
 * @return true La entrada tuvo una activacion desde el ultimo llamado
 * @return false La entrada no tuvo una activacion desde el ultimo llamado 
 */

bool DigitalInputHasActivated(digital_input_t input);

/**
 * @brief Metodo para detectar el estado inactivo de la entrada 
 * 
 * @param input Puntero al descriptor de la entrada
 * @return true La entrada tuvo una desactivacion desde el ultimo llamado
 * @return false La entrada no tuvo una desactivacion desde el ultimo llamado 
 */

bool DigitalInputHasDeactivated(digital_input_t input);

/**
 * @brief Metodo para crear una salida digital
 * 
 * @param port Puerto GPIO que contiene a la salida
 * @param pin Numero de terminal del puerto GPIO asignado a la salida 
 * @return digital_output_t Puntero al descriptor de la salida creada
 */

digital_output_t DigitalOutputCreate(uint8_t port, uint8_t pin);

/**
 * @brief Metodo para prender una salida digital
 * 
 * @param output Puntero al descriptor de la salida 
 */

void DigitalOutputActivate(digital_output_t output);

/**
 * @brief Metodo para apagar una salida digital
 * 
 * @param output Puntero al descriptor de la salida
 */

void DigitalOutputDeactivate(digital_output_t output);

/**
 * @brief Metodo para invertir el estado de una salida digital 
 * 
 * @param output Puntero al descriptor de la salida
 */

void DigitalOutputToggle(digital_output_t output);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif

/** @} End of module definition for doxygen */

#endif /* DIGITAL_H */