/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 ** Los botones de accept, increment y decrement imprimen en la pantalla 1 2 3 4
 ** Los botones de set_alarm, set_time y cancel borran la pantalla
 ** Este programa sirve para verificar el funcionamiento completo del Hardware
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "bsp.h"
#include "reloj.h"
#include <stdbool.h>
#include <stddef.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

void DisparoAlarma(clock_t reloj);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

static board_t board;

static clock_t reloj;

/* === Private function implementation ========================================================= */

void DisparoAlarma(clock_t reloj) {
}

/* === Public function implementation ========================================================= */

int main(void) {
    uint8_t hora[6];

    reloj = ClockCreate(10, DisparoAlarma);
    board = BoardCreate();

    SisTick_Init(600);

    while (true) {

        ClockGetTime(reloj, hora, sizeof(hora));
        __asm volatile("cpsid i");
        DisplayWriteBCD(board->display, hora, sizeof(hora));
        __asm volatile("cpsie i");
    }
}

void SysTick_Handler(void) {
    DisplayRefresh(board->display);
    ClockTic(reloj);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
