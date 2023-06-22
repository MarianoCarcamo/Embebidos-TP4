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

/** \brief Archivo de prueba para la libreria reloj
 **
 ** Archivo de soporte para un desarrollo basado en pruebas sobre la libreria de reloj
 **
 ** \addtogroup tdd TDD
 ** \brief TDD
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include <string.h>
#include "unity.h"
#include "reloj.h"

/* === Macros definitions ====================================================================== */

#define TICS_POR_SEGUNDO 5

#define SIMULAR_SEGUNDOS(VALOR)                                                                    \
    for (int count = 0; count < VALOR; count++) {                                                  \
        for (int indice = 0; indice < TICS_POR_SEGUNDO; indice++) {                                \
            ClockTic(reloj);                                                                       \
        }                                                                                          \
    }
/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

void ClockAlarmTriggered(clock_t reloj);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

static clock_t reloj;
static uint8_t hora[6];
static const uint8_t TIME_SET[] = {1, 2, 3, 4, 0, 0};
static bool alarma_testigo;
/* === Private function implementation ========================================================= */

void ClockAlarmTriggered(clock_t reloj) {
    alarma_testigo = true;
}

/* === Public function implementation ========================================================== */
void setUp(void) {
    alarma_testigo = false;
    reloj = ClockCreate(TICS_POR_SEGUNDO, ClockAlarmTriggered);
    ClockSetTime(reloj, TIME_SET, 4);
}

// Al inicializar el reloj esta en 00:00 y con hora invalida.
void test_reloj_arranca_con_hora_invalida(void) {
    static const uint8_t ESPERADO[] = {0, 0, 0, 0, 0, 0};
    memset(hora, 0xff, sizeof(hora)); // Se inicializa hora en 0xff para asegurar el pase a 0

    clock_t reloj = ClockCreate(TICS_POR_SEGUNDO, ClockAlarmTriggered);
    TEST_ASSERT_FALSE(ClockGetTime(reloj, hora, 6));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Al ajustar la hora el reloj queda en hora y es valido.
void test_ajustar_hora(void) {
    clock_t reloj = ClockCreate(TICS_POR_SEGUNDO, ClockAlarmTriggered);
    TEST_ASSERT_TRUE(ClockSetTime(reloj, TIME_SET, 4));
    TEST_ASSERT_TRUE(ClockGetTime(reloj, hora, 6));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(TIME_SET, hora, 6);
}

// Despues de n ciclos de reloj la hora avanza un segundo.
void test_avanza_unidad_segundos(void) {
    static const uint8_t ESPERADO[] = {1, 2, 3, 4, 0, 1};

    SIMULAR_SEGUNDOS(1);
    ClockGetTime(reloj, hora, 6);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Despues de n ciclos de reloj la hora avanza 10 segundos.
void test_avanza_decena_segundos(void) {
    static const uint8_t ESPERADO[] = {1, 2, 3, 4, 1, 0};

    SIMULAR_SEGUNDOS(10);
    ClockGetTime(reloj, hora, 6);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Despues de n ciclos de reloj la hora avanza 1 minuto.
void test_avanza_unidad_minutos(void) {
    static const uint8_t ESPERADO[] = {1, 2, 3, 5, 0, 0};

    SIMULAR_SEGUNDOS(1 * 60);
    ClockGetTime(reloj, hora, 6);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Despues de n ciclos de reloj la hora avanza 1 hora.
void test_avanza_unidad_hora(void) {
    static const uint8_t ESPERADO[] = {1, 3, 3, 4, 0, 0};

    SIMULAR_SEGUNDOS(1 * 60 * 60);
    ClockGetTime(reloj, hora, 6);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Despues de n ciclos de reloj la hora avanza 10 horas.
void test_avanza_decena_hora(void) {
    static const uint8_t ESPERADO[] = {2, 2, 3, 4, 0, 0};

    SIMULAR_SEGUNDOS(10 * 60 * 60);
    ClockGetTime(reloj, hora, 6);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Despues de n ciclos de reloj la hora avanza 1 dia.
void test_avanza_un_dia(void) {
    static const uint8_t ESPERADO[] = {1, 2, 3, 4, 0, 0};

    SIMULAR_SEGUNDOS(24 * 60 * 60);
    ClockGetTime(reloj, hora, 6);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Fijar la hora de la alarma y consultarla.
void test_ajustar_alarma(void) {
    static const uint8_t ESPERADO[] = {1, 3, 3, 4, 0, 0};

    TEST_ASSERT_FALSE(ClockGetAlarm(reloj, hora, 6));
    TEST_ASSERT_TRUE(ClockSetAlarm(reloj, ESPERADO, 4));
    TEST_ASSERT_TRUE(ClockGetAlarm(reloj, hora, 6));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, 6);
}

// Fijar la alarma y avanzar la hora para que suene.
void test_disparar_alarma(void) {
    static const uint8_t SET_ALARMA[] = {1, 3, 3, 4, 0, 0};

    ClockSetAlarm(reloj, SET_ALARMA, 4);
    TEST_ASSERT_FALSE(alarma_testigo);
    SIMULAR_SEGUNDOS(30 * 60);
    TEST_ASSERT_FALSE(alarma_testigo);
    SIMULAR_SEGUNDOS(30 * 60 + 1);
    TEST_ASSERT_TRUE(alarma_testigo);
}

// Fijar la alarma, deshabilitarla y avanzar el reloj para que no suene.
void test_no_disparar_alarma(void) {
    static const uint8_t SET_ALARMA[] = {1, 3, 3, 4, 0, 0};

    ClockSetAlarm(reloj, SET_ALARMA, 4);
    TEST_ASSERT_FALSE(AlarmToggel(reloj));
    SIMULAR_SEGUNDOS(1 * 60 * 60);
    TEST_ASSERT_FALSE(alarma_testigo);
}

// Hacer sonar la alarma y posponerla.
void test_posponer_alarma(void) {
    static const uint8_t SET_ALARMA[] = {1, 3, 3, 4, 0, 0};

    ClockSetAlarm(reloj, SET_ALARMA, 4);
    SIMULAR_SEGUNDOS(1 * 60 * 60 + 1);
    TEST_ASSERT_TRUE(alarma_testigo);
    AlarmSnooze(reloj, 5);
    alarma_testigo = false;
    SIMULAR_SEGUNDOS(4 * 60);
    TEST_ASSERT_FALSE(alarma_testigo);
    SIMULAR_SEGUNDOS(1 * 60);
    TEST_ASSERT_TRUE(alarma_testigo);
}

// Hacer sonar la alarma y cancelarla hasta el otro dia.
void test_alarma_paso_dia(void) {
    static const uint8_t SET_ALARMA[] = {1, 3, 3, 4, 0, 0};

    ClockSetAlarm(reloj, SET_ALARMA, 4);
    SIMULAR_SEGUNDOS(1 * 60 * 60);
    TEST_ASSERT_TRUE(alarma_testigo);
    alarma_testigo = false;
    SIMULAR_SEGUNDOS(24 * 60 * 60);
    TEST_ASSERT_TRUE(alarma_testigo);
}
/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
