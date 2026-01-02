#include <stdio.h>
#include "control.h"
#include "plant.h"

static void log_tick(unsigned k, control_t* c) {
    printf("[tick] k=%u mode=%s set=%.2f y=%.3f u=%.3f u_sat=%u\n",
        k,
        (c->mode == MODE_OPEN) ? "OPEN" : "CLOSED",
        c->setpoint,
        c->meas,
        c->out,
        c->u_sat_count > 0);
}

int main(void) {

    control_t ctrl;
    plant_t plant;

    control_init(&ctrl);
    plant_init(&plant, 0.08f);

    /* INIT → IDLE */
    control_tick(&ctrl);

    unsigned k = 0;

    /* ===== TEST OPEN ===== */
    control_set_mode(&ctrl, MODE_OPEN);
    control_set_out(&ctrl, 0.7f);

    for (int i = 0; i < 30; i++) {
        ctrl.meas = plant_tick(&plant, ctrl.out);
        control_tick(&ctrl);
        log_tick(k++, &ctrl);
    }

    /* ===== TEST CLOSED ===== */
    control_set_mode(&ctrl, MODE_CLOSED);
    control_set_setpoint(&ctrl, 1.0f);

    for (int i = 0; i < 120; i++) {
        ctrl.meas = plant_tick(&plant, ctrl.out);
        control_tick(&ctrl);
        log_tick(k++, &ctrl);
    }

    /* ===== TEST STOP ===== */
    control_stop(&ctrl);
    control_tick(&ctrl);
    printf("[evt] STOP -> SAFE state=%d out=%.2f\n", ctrl.state, ctrl.out);

    /* ===== TEST WATCHDOG ===== */
    ctrl.state = ST_RUN;
    for (int i = 0; i < 10; i++) {
        ctrl.meas = plant_tick(&plant, ctrl.out);
        /* brak tick */
    }
    control_tick(&ctrl);
    printf("[wd] watchdog timeout -> state=%d err=%d\n", ctrl.state, ctrl.error);

    /* ===== TELEMETRIA ===== */
    printf("\n[telemetry]\n");
    printf("ticks=%u\n", ctrl.ticks);
    printf("wd_resets=%u\n", ctrl.wd_resets);
    printf("u_sat_count=%u\n", ctrl.u_sat_count);
    printf("mode_changes=%u\n", ctrl.mode_changes);

    return 0;
}
