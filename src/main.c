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
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "bsp.h"
#include "reloj.h"
#include <stdbool.h>
#include <stddef.h>
#include "FreeRTOS.h"
#include "bsp.h"
#include "semphr.h"
#include "task.h"

/* === Macros definitions ====================================================================== */

#ifndef TICS_POR_SEC
#define TICS_POR_SEC 1000
#endif

#ifndef PERIODO_PARPADEO
#define PERIODO_PARPADEO 1000
#endif

#ifndef SNOOZE_MINUTOS
#define SNOOZE_MINUTOS 5
#endif

/* === Private data type declarations ========================================================== */

typedef enum {
    SIN_CONFIGURAR,
    MOSTRANDO_HORA,
    AJUSTANDO_MINUTOS,
    AJUSTANDO_HORA,
    AJUSTANDO_MINUTOS_ALARMA,
    AJUSTANDO_HORA_ALARMA
} modo_t;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

void DisparoAlarma(clock_t reloj);

void CambiarModo(modo_t valor);

void IncrementarBCD(uint8_t numero[2], const uint8_t limite[2]);

void DecrementarBCD(uint8_t numero[2], const uint8_t limite[2]);

/**
 * @brief Cuentar segundos si se cumple una determinada condicion en las entradas
 *
 * Esta funcion permite contar una determinada cantidad de segundos mientras las entradas estan en
 * un estado determinado, ya sea en On u Off.
 * Esta funcion utiliza una variable global llamada "sec_count_down", una vez llegada a cero
 * (realizando el decremento en el systic) cumpliendo con las condiciones indicadas, la funcion
 * retorna un "true", caso contrario, retorna "false".
 *
 * @param segundos Cantidad de segundos
 * @param estado Estado en el que evaluar las entradas
 * @param cantidad_entradas Numero de entradas a evaluar
 * @param input Vector con entradas
 * @return true En caso de haber cumplido el tiempo en las condiciones indicadas
 * @return false En caso de no haber cumplido el tiempo en las condiciones indicadas
 */

bool ContarSegundosMientras(int segundos, bool estado, int cantidad_entradas,
                            const digital_input_t input[]);

/* === Public variable definitions =============================================================
 */

static board_t board;

static clock_t reloj;

static modo_t modo;

static int current_tic_value;

static int sec_count_down = 0;

static bool alarma_sonando = false;

/* === Private variable definitions ============================================================ */

static const uint8_t LIMITE_MINUTOS[] = {5, 9};

static const uint8_t LIMITE_HORAS[] = {2, 3};

/* === Private function implementation ========================================================= */

void DisparoAlarma(clock_t reloj) {
    DigitalOutputActivate(board->buzzer);
    alarma_sonando = true;
}

void CambiarModo(modo_t valor) {
    modo = valor;

    switch (modo) {
    case SIN_CONFIGURAR:
        DisplayBlinkDigits(board->display, 0, 3, PERIODO_PARPADEO);
        DisplayTurnOnDot(board->display, 1);
        DisplayTurnOffDot(board->display, 0);
        DisplayTurnOffDot(board->display, 2);
        DisplayTurnOffDot(board->display, 3);
        break;
    case MOSTRANDO_HORA:
        DisplayBlinkDigits(board->display, 0, 3, 0);
        DisplayTurnOffDot(board->display, 0);
        DisplayTurnOffDot(board->display, 1);
        DisplayTurnOffDot(board->display, 2);
        DisplayTurnOffDot(board->display, 3);
        break;
    case AJUSTANDO_MINUTOS:
        DisplayBlinkDigits(board->display, 2, 3, PERIODO_PARPADEO);
        DisplayTurnOnDot(board->display, 1);
        DisplayTurnOffDot(board->display, 0);
        DisplayTurnOffDot(board->display, 2);
        DisplayTurnOffDot(board->display, 3);
        break;
    case AJUSTANDO_HORA:
        DisplayBlinkDigits(board->display, 0, 1, PERIODO_PARPADEO);
        DisplayTurnOnDot(board->display, 1);
        DisplayTurnOffDot(board->display, 0);
        DisplayTurnOffDot(board->display, 2);
        DisplayTurnOffDot(board->display, 3);
        break;
    case AJUSTANDO_MINUTOS_ALARMA:
        DisplayBlinkDigits(board->display, 2, 3, PERIODO_PARPADEO);
        DisplayTurnOnDot(board->display, 0);
        DisplayTurnOnDot(board->display, 1);
        DisplayTurnOnDot(board->display, 2);
        DisplayTurnOnDot(board->display, 3);
        break;
    case AJUSTANDO_HORA_ALARMA:
        DisplayBlinkDigits(board->display, 0, 1, PERIODO_PARPADEO);
        DisplayTurnOnDot(board->display, 0);
        DisplayTurnOnDot(board->display, 1);
        DisplayTurnOnDot(board->display, 2);
        DisplayTurnOnDot(board->display, 3);
        break;
    default:
        break;
    }
}

void IncrementarBCD(uint8_t numero[2], const uint8_t limite[2]) {
    numero[1]++;
    if ((numero[0] >= limite[0]) && numero[1] > limite[1]) {
        numero[0] = 0;
        numero[1] = 0;
    }
    if (numero[1] > 9) {
        numero[1] = 0;
        numero[0]++;
    }
}

void DecrementarBCD(uint8_t numero[2], const uint8_t limite[2]) {
    numero[1]--;
    if (numero[1] > 9) {
        numero[1] = 9;
        numero[0]--;
    }
    if ((numero[0] >= limite[0]) && numero[1] >= limite[1]) {
        numero[0] = limite[0];
        numero[1] = limite[1];
    }
}

bool ContarSegundosMientras(int segundos, bool estado, int cantidad_entradas,
                            const digital_input_t input[]) {
    sec_count_down = segundos;
    bool condicion = estado;

    if (condicion) {
        while (sec_count_down > 0 && condicion) {
            for (int index = 0; index < cantidad_entradas; index++) {
                condicion = DigitalInputGetState(input[index]);
                if (!condicion) {
                    return false;
                }
            }
        }
    } else {
        while (sec_count_down > 0 && !condicion) {
            for (int index = 0; index < cantidad_entradas; index++) {
                condicion = DigitalInputGetState(input[index]);
                if (condicion) {
                    return false;
                }
            }
        }
    }
    return true;
}

/* === Public function implementation ========================================================= */

int main(void) {
    uint8_t entrada[4];

    reloj = ClockCreate(TICS_POR_SEC, DisparoAlarma);
    board = BoardCreate();

    SisTick_Init(TICS_POR_SEC);
    CambiarModo(SIN_CONFIGURAR);

    while (true) {
        if (DigitalInputHasActivated(board->accept)) {
            if (!alarma_sonando) {
                if (modo == MOSTRANDO_HORA) {
                    AlarmActivate(reloj);
                    DisplayTurnOnDot(board->display, 3);
                } else if (modo == AJUSTANDO_MINUTOS) {
                    CambiarModo(AJUSTANDO_HORA);
                } else if (modo == AJUSTANDO_HORA) {
                    ClockSetTime(reloj, entrada, sizeof(entrada));
                    CambiarModo(MOSTRANDO_HORA);
                } else if (modo == AJUSTANDO_MINUTOS_ALARMA) {
                    CambiarModo(AJUSTANDO_HORA_ALARMA);
                } else if (modo == AJUSTANDO_HORA_ALARMA) {
                    ClockSetAlarm(reloj, entrada, sizeof(entrada));
                    CambiarModo(MOSTRANDO_HORA);
                    AlarmActivate(reloj);
                    DisplayTurnOnDot(board->display, 3);
                }
            } else {
                DigitalOutputDeactivate(board->buzzer);
                AlarmSnooze(reloj, SNOOZE_MINUTOS);
                alarma_sonando = false;
            }
        }

        if (DigitalInputHasActivated(board->cancel)) {
            if (!alarma_sonando) {
                if (modo == MOSTRANDO_HORA) {
                    AlarmDeactivate(reloj);
                    DisplayTurnOffDot(board->display, 3);
                } else if (ClockGetTime(reloj, entrada, sizeof(entrada)) &&
                           (modo != MOSTRANDO_HORA)) {
                    CambiarModo(MOSTRANDO_HORA);
                } else {
                    CambiarModo(SIN_CONFIGURAR);
                }
            } else {
                DigitalOutputDeactivate(board->buzzer);
                alarma_sonando = false;
            }
        }

        if (ContarSegundosMientras(3, true, 1, &(board->set_time))) {
            CambiarModo(AJUSTANDO_MINUTOS);
            ClockGetTime(reloj, entrada, sizeof(entrada));
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }

        if (ContarSegundosMientras(3, true, 1, &(board->set_alarm)) &&
            ClockGetTime(reloj, entrada, sizeof(entrada))) {

            CambiarModo(AJUSTANDO_MINUTOS_ALARMA);
            ClockGetAlarm(reloj, entrada, sizeof(entrada));
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }

        if (DigitalInputHasActivated(board->increment)) {
            if (modo == AJUSTANDO_MINUTOS) {
                IncrementarBCD(&entrada[2], LIMITE_MINUTOS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            } else if (modo == AJUSTANDO_HORA) {
                IncrementarBCD(entrada, LIMITE_HORAS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            } else if (modo == AJUSTANDO_MINUTOS_ALARMA) {
                IncrementarBCD(&entrada[2], LIMITE_MINUTOS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            } else if (modo == AJUSTANDO_HORA_ALARMA) {
                IncrementarBCD(entrada, LIMITE_HORAS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            }
        }

        if (DigitalInputHasActivated(board->decrement)) {
            if (modo == AJUSTANDO_MINUTOS) {
                DecrementarBCD(&entrada[2], LIMITE_MINUTOS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            } else if (modo == AJUSTANDO_HORA) {
                DecrementarBCD(entrada, LIMITE_HORAS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            } else if (modo == AJUSTANDO_MINUTOS_ALARMA) {
                DecrementarBCD(&entrada[2], LIMITE_MINUTOS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            } else if (modo == AJUSTANDO_HORA_ALARMA) {
                DecrementarBCD(entrada, LIMITE_HORAS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            }
        }

        if (ContarSegundosMientras(30, false, 6,
                                   (digital_input_t[]){board->accept, board->cancel,
                                                       board->set_time, board->set_alarm,
                                                       board->increment, board->decrement})) {
            if (modo > MOSTRANDO_HORA) {
                if (ClockGetTime(reloj, entrada, sizeof(entrada))) {
                    CambiarModo(MOSTRANDO_HORA);
                } else {
                    CambiarModo(SIN_CONFIGURAR);
                }
            }
        }

        for (int index = 0; index < 20; index++) {
            for (int delay = 0; delay < 25000; delay++) {
                __asm("NOP");
            }
        }
    }
}

void SysTick_Handler(void) {
    static const int half_sec = TICS_POR_SEC / 2;
    uint8_t hora[6];

    DisplayRefresh(board->display);
    current_tic_value = ClockTic(reloj);

    if (current_tic_value == half_sec || current_tic_value == 0) {
        if (modo <= MOSTRANDO_HORA) {
            ClockGetTime(reloj, hora, sizeof(hora));
            DisplayWriteBCD(board->display, hora, sizeof(hora));
            if (modo == MOSTRANDO_HORA)
                DisplayToggleDot(board->display, 1);
        }
        if ((current_tic_value == 0) && sec_count_down) {
            sec_count_down--;
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
