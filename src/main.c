/* === Headers files inclusions =============================================================== */

#include <stdbool.h>
#include <stddef.h>
#include "bsp.h"
#include "reloj.h"
#include "FreeRTOS.h"
#include "bsp.h"
#include "task.h"
#include "event_groups.h"

/* === Macros definitions ====================================================================== */

#define EVENT_KEY_ACCEPT_ON    (1 << 0)
#define EVENT_KEY_CANCEL_ON    (1 << 1)
#define EVENT_KEY_INCREMENT_ON (1 << 2)
#define EVENT_KEY_DECREMENT_ON (1 << 3)
#define EVENT_KEY_SET_TIME_ON  (1 << 4)
#define EVENT_KEY_SET_ALARM_ON (1 << 5)

#define EVENT_KEY_ACCEPT_OFF    (1 << 6)
#define EVENT_KEY_CANCEL_OFF    (1 << 7)
#define EVENT_KEY_INCREMENT_OFF (1 << 8)
#define EVENT_KEY_DECREMENT_OFF (1 << 9)
#define EVENT_KEY_SET_TIME_OFF  (1 << 10)
#define EVENT_KEY_SET_ALARM_OFF (1 << 11)

#define EVENT_WAIT_30s_DONE (1 << 12)

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

static board_t board;

static clock_t reloj;

static modo_t modo;

static bool alarma_sonando = false;

static bool reset_delay = false;

static uint8_t entrada[4];

EventGroupHandle_t app_events;

/* === Private function declarations =========================================================== */

static void AcceptProgTask(void * object);

static void CancelProgTask(void * object);

static void IncrementProgTask(void * object);

static void DecrementProgTask(void * object);

static void SetTimeProgTask(void * object);

static void SetAlarmProgTask(void * object);

static void TickTask(void * object);

static void DisplayRefreshTask(void * object);

static void KeyTask(void * object);

static void Wait30sTask(void * object);

void DisparoAlarma(clock_t reloj);

void CambiarModo(modo_t valor);

void IncrementarBCD(uint8_t numero[2], const uint8_t limite[2]);

void DecrementarBCD(uint8_t numero[2], const uint8_t limite[2]);

/* === Private variable definitions ============================================================ */

static const uint8_t LIMITE_MINUTOS[] = {5, 9};

static const uint8_t LIMITE_HORAS[] = {2, 3};

/* === Private function implementation ========================================================= */

void DisparoAlarma(clock_t reloj) {
    DigitalOutputActivate(board->buzzer[0]);
    DigitalOutputActivate(board->buzzer[1]);
    DigitalOutputActivate(board->buzzer[2]);
    DigitalOutputActivate(board->buzzer[3]);
    DigitalOutputActivate(board->buzzer[4]);
    DigitalOutputActivate(board->buzzer[5]);
    alarma_sonando = true;
}

void DesactivarAlarma(clock_t reloj) {
    DigitalOutputDeactivate(board->buzzer[0]);
    DigitalOutputDeactivate(board->buzzer[1]);
    DigitalOutputDeactivate(board->buzzer[2]);
    DigitalOutputDeactivate(board->buzzer[3]);
    DigitalOutputDeactivate(board->buzzer[4]);
    DigitalOutputDeactivate(board->buzzer[5]);
    alarma_sonando = false;
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

static void DisplayRefreshTask(void * object) {
    while (true) {
        DisplayRefresh(board->display);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void TickTask(void * object) {
    int current_tic_value;
    static const int half_sec = TICS_POR_SEC / 2;
    uint8_t hora[6];
    TickType_t last_value = xTaskGetTickCount();

    while (true) {
        current_tic_value = ClockTic(reloj);

        if (current_tic_value == half_sec || current_tic_value == 0) {
            if (modo <= MOSTRANDO_HORA) {
                ClockGetTime(reloj, hora, sizeof(hora));
                DisplayWriteBCD(board->display, hora, sizeof(hora));
                if (modo == MOSTRANDO_HORA)
                    DisplayToggleDot(board->display, 1);
            }
        }
        vTaskDelayUntil(&last_value, pdMS_TO_TICKS(1));
    }
}

static void KeyTask(void * object) {
    uint16_t last_state, current_state, changes, events;

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(150));

        current_state = 0;
        if (DigitalInputGetState(board->accept)) {
            current_state |= EVENT_KEY_ACCEPT_ON;
        };
        if (DigitalInputGetState(board->cancel)) {
            current_state |= EVENT_KEY_CANCEL_ON;
        };
        if (DigitalInputGetState(board->increment)) {
            current_state |= EVENT_KEY_INCREMENT_ON;
        };
        if (DigitalInputGetState(board->decrement)) {
            current_state |= EVENT_KEY_DECREMENT_ON;
        };
        if (DigitalInputGetState(board->set_time)) {
            current_state |= EVENT_KEY_SET_TIME_ON;
        };
        if (DigitalInputGetState(board->set_alarm)) {
            current_state |= EVENT_KEY_SET_ALARM_ON;
        };

        changes = current_state ^ last_state;
        last_state = current_state;
        events = (((changes << 6) & (!current_state << 6)) | (changes & current_state));

        xEventGroupSetBits(app_events, events);
    }
}

static void Wait30sTask(void * object) {
    uint32_t count;
    EventBits_t events;

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        events = xEventGroupGetBits(app_events);
        if ((modo <= MOSTRANDO_HORA) || reset_delay) {
            count = 0;
            reset_delay = false;
        } else {
            count++;
        }
        if (count == 30) {
            xEventGroupSetBits(app_events, (EVENT_WAIT_30s_DONE | events));
        }
    }
}

static void AcceptProgTask(void * object) {
    while (true) {
        xEventGroupWaitBits(app_events, EVENT_KEY_ACCEPT_ON, true, false, portMAX_DELAY);
        reset_delay = true;
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
            DesactivarAlarma(reloj);
            AlarmSnooze(reloj, SNOOZE_MINUTOS);
        }
    }
}

static void CancelProgTask(void * object) {
    while (true) {
        xEventGroupWaitBits(app_events, EVENT_KEY_CANCEL_ON | EVENT_WAIT_30s_DONE, true, false,
                            portMAX_DELAY);

        if (!alarma_sonando) {
            if (modo == MOSTRANDO_HORA) {
                AlarmDeactivate(reloj);
                DisplayTurnOffDot(board->display, 3);
            } else if (ClockGetTime(reloj, entrada, sizeof(entrada)) && (modo != MOSTRANDO_HORA)) {
                CambiarModo(MOSTRANDO_HORA);
            } else {
                CambiarModo(SIN_CONFIGURAR);
            }
        } else {
            DesactivarAlarma(reloj);
        }
    }
}

static void IncrementProgTask(void * object) {
    while (true) {
        xEventGroupWaitBits(app_events, EVENT_KEY_INCREMENT_ON, true, false, portMAX_DELAY);
        reset_delay = true;
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
}

static void DecrementProgTask(void * object) {
    while (true) {
        xEventGroupWaitBits(app_events, EVENT_KEY_DECREMENT_ON, true, false, portMAX_DELAY);
        reset_delay = true;
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
}

static void SetTimeProgTask(void * object) {
    while (true) {
        xEventGroupWaitBits(app_events, EVENT_KEY_SET_TIME_ON, true, false, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(3000));
        reset_delay = true;
        if (DigitalInputGetState(board->set_time)) {
            CambiarModo(AJUSTANDO_MINUTOS);
            ClockGetTime(reloj, entrada, sizeof(entrada));
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }
    }
}

static void SetAlarmProgTask(void * object) {
    while (true) {
        xEventGroupWaitBits(app_events, EVENT_KEY_SET_ALARM_ON, true, false, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(3000));
        reset_delay = true;
        if (DigitalInputGetState(board->set_alarm)) {
            CambiarModo(AJUSTANDO_MINUTOS_ALARMA);
            ClockGetAlarm(reloj, entrada, sizeof(entrada));
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    reloj = ClockCreate(TICS_POR_SEC, DisparoAlarma);
    board = BoardCreate();

    SisTick_Init(TICS_POR_SEC);

    app_events = xEventGroupCreate();

    CambiarModo(SIN_CONFIGURAR);

    xTaskCreate(TickTask, "TickTask", 256, NULL, tskIDLE_PRIORITY + 3, NULL);

    xTaskCreate(DisplayRefreshTask, "RefreshTask", 256, NULL, tskIDLE_PRIORITY + 2, NULL);

    xTaskCreate(AcceptProgTask, "AcceptProgram", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(CancelProgTask, "CancelProgram", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(IncrementProgTask, "IncrementProgram", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(DecrementProgTask, "DecrementProgram", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(SetTimeProgTask, "SetTimeProgram", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(SetAlarmProgTask, "SetAlarmProgram", 256, NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(KeyTask, "KeyTask", 256, NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(Wait30sTask, "Delay30sTask", 256, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    while (true) {
    }

    return 0;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
