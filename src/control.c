#include "control.h"

/* ===== INICJALIZACJA ===== */
void control_init(control_t* c) {
    c->setpoint = 0.0f;
    c->meas = 0.0f;
    c->out = 0.0f;

    c->u_min = -1.0f;
    c->u_max =  1.0f;
    c->y_min = -10.0f;
    c->y_max =  10.0f;

    c->mode = MODE_OPEN;
    c->state = ST_INIT;
    c->error = ERR_NONE;

    c->pi.kp = 1.0f;
    c->pi.ki = 0.1f;
    c->pi.integ = 0.0f;

    c->ticks = 0;
    c->wd_misses = 0;
    c->u_sat_count = 0;
}

/* ===== ZMIANA TRYBU ===== */
void control_set_mode(control_t* c, control_mode_t m) {
    c->mode = m;
    if (c->state == ST_IDLE) {
        c->state = ST_RUN;
    }
}

/* ===== SETPOINT ===== */
void control_set_setpoint(control_t* c, float sp) {
    c->setpoint = sp;
}

/* ===== OPEN: RĘCZNE U ===== */
void control_set_out(control_t* c, float u) {
    if (c->mode == MODE_OPEN && c->state == ST_RUN) {
        if (u < c->u_min) u = c->u_min;
        if (u > c->u_max) u = c->u_max;
        c->out = u;
    }
}

/* ===== REGULATOR PI ===== */
static float pi_tick(control_t* c) {
    float e = c->setpoint - c->meas;
    float u = c->pi.kp * e + c->pi.ki * c->pi.integ;

    if (u > c->u_max) {
        u = c->u_max;
        c->u_sat_count++;
    } else if (u < c->u_min) {
        u = c->u_min;
        c->u_sat_count++;
    } else {
        c->pi.integ += e;
    }
    return u;
}

/* ===== TICK SYSTEMU ===== */
void control_tick(control_t* c) {
    c->ticks++;

    switch (c->state) {

    case ST_INIT:
        c->out = 0.0f;
        c->state = ST_IDLE;
        break;

    case ST_IDLE:
        c->out = 0.0f;
        break;

    case ST_RUN:
        if (c->mode == MODE_OPEN) {
            /* OUT ustawiane ręcznie */
        } else if (c->mode == MODE_CLOSED) {
            c->out = pi_tick(c);
        }
        break;

    case ST_SAFE:
        c->out = 0.0f;
        break;

    case ST_FAULT:
        c->out = 0.0f;
        break;
    }
}
