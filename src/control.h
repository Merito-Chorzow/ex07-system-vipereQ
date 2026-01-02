#pragma once

/* ===== TRYBY PRACY ===== */
typedef enum {
    MODE_OPEN = 0,
    MODE_CLOSED = 1
} control_mode_t;

/* ===== STANY FSM ===== */
typedef enum {
    ST_INIT = 0,
    ST_IDLE,
    ST_RUN,
    ST_SAFE,
    ST_FAULT
} control_state_t;

/* ===== KODY BŁĘDÓW ===== */
typedef enum {
    ERR_NONE = 0,
    ERR_WATCHDOG,
    ERR_LIMIT_Y
} control_error_t;

/* ===== REGULATOR PI ===== */
typedef struct {
    float kp;
    float ki;
    float integ;
} pi_t;

/* ===== GŁÓWNA STRUKTURA SYSTEMU ===== */
typedef struct {
    /* kontrakt / rejestry */
    float setpoint;
    float meas;
    float out;

    float u_min;
    float u_max;
    float y_min;
    float y_max;

    control_mode_t mode;
    control_state_t state;
    control_error_t error;

    /* regulator */
    pi_t pi;

    /* telemetria */
    unsigned ticks;
    unsigned wd_misses;
    unsigned u_sat_count;

} control_t;

/* ===== API ===== */
void control_init(control_t* c);
void control_set_mode(control_t* c, control_mode_t m);
void control_set_out(control_t* c, float u);
void control_set_setpoint(control_t* c, float sp);
void control_tick(control_t* c);
