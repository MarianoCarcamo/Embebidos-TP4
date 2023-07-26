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

/** \brief Brief description of the file
 **
 ** Full file description
 **
 ** \addtogroup name Module denomination
 ** \brief Brief description of the module
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include <stdbool.h>
#include "digital.h"
#include "chip.h"

/* === Macros definitions ====================================================================== */

#ifndef OUTPUT_INSTANCES
#define OUTPUT_INSTANCES 6
#endif

#ifndef INPUT_INSTANCES
#define INPUT_INSTANCES 6
#endif

/* === Private data type declarations ========================================================== */

// Estructura para almacenar el descriptor de una entrada digital
struct digital_input_s {
    uint8_t pin;     // Puerto GPIO de la entrada digital
    uint8_t port;    // Terminal del puerto GPIO de la entrada digital
    bool inverted;   // La entrada opera con logica invertida
    bool last_state; // Estado anterior de la entrada digital
    bool allocated;  // Bandera para indicar que el descriptor esta en uso
};

// Esctructura para almacenar el descriptor de una salida digital
struct digital_output_s {
    uint8_t pin;    // Puerto GPIO de la salida digital
    uint8_t port;   // Terminal del uerto GPIO de la salida digital
    bool allocated; // Bandera para indicar que el descriptor esta en uso
};

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

digital_input_t DigitalInputAllocated(void);

digital_output_t DigitalOutputAllocated(void);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

// Funcion para asignar un descriptor para crea una nueva entrada digital
digital_input_t DigitalInputAllocated(void) {
    digital_input_t input = NULL;

    static struct digital_input_s instances[INPUT_INSTANCES] = {0};

    for (int index = 0; index < INPUT_INSTANCES; index++) {
        if (!instances[index].allocated) {
            instances[index].allocated = true;
            input = &instances[index];
            break;
        }
    }
    return input;
}

// Funcion para asignar un descriptor para crea una nueva salida digital
digital_output_t DigitalOutputAllocated(void) {
    digital_output_t output = NULL;

    static struct digital_output_s instances[OUTPUT_INSTANCES] = {0};

    for (int index = 0; index < OUTPUT_INSTANCES; index++) {
        if (!instances[index].allocated) {
            instances[index].allocated = true;
            output = &instances[index];
            break;
        }
    }
    return output;
}

/* === Public function implementation ========================================================== */

digital_input_t DigitalInputCreate(uint8_t port, uint8_t pin, bool logic) {
    digital_input_t input = DigitalInputAllocated();

    if (input) {
        input->port = port;
        input->pin = pin;
        input->inverted = logic;
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, input->port, input->pin, false);
    }
    return input;
}

bool DigitalInputGetState(digital_input_t input) {
    if (input->inverted) {
        return Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, input->port, input->pin) == 0;
    } else {
        return Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, input->port, input->pin) != 0;
    }
}

bool DigitalInputHasChange(digital_input_t input) {
    bool current_state = DigitalInputGetState(input);
    bool has_changed = false;

    if (current_state == !(input->last_state))
        has_changed = true;
    input->last_state = current_state;
    return has_changed;
}

bool DigitalInputHasActivated(digital_input_t input) {
    bool current_state = DigitalInputGetState(input);
    bool has_activated = false;

    if (current_state == true && input->last_state == false)
        has_activated = true;
    input->last_state = current_state;
    return has_activated;
}

bool DigitalInputHasDeactivated(digital_input_t input) {
    bool current_state = DigitalInputGetState(input);
    bool has_deactivated = false;

    if (current_state == false && input->last_state == true)
        has_deactivated = true;
    input->last_state = current_state;
    return has_deactivated;
}

digital_output_t DigitalOutputCreate(uint8_t port, uint8_t pin) {
    digital_output_t output = DigitalOutputAllocated();

    if (output) {
        output->port = port;
        output->pin = pin;
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, output->port, output->pin, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, output->port, output->pin, true);
    }
    return output;
}

void DigitalOutputActivate(digital_output_t output) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, output->port, output->pin, true);
}

void DigitalOutputDeactivate(digital_output_t output) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, output->port, output->pin, false);
}

void DigitalOutputToggle(digital_output_t output) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, output->port, output->pin);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */