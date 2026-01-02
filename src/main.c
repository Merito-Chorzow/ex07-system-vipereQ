#include <stdio.h>
#include "control.h"
#include "plant.h"

int main(void) {

    control_t ctrl;
    plant_t plant;

    control_init(&ctrl);
    plant_init(&plant, 0.05f);

    /* INIT → IDLE */
    control_tick(&ctrl);

    /* OPEN MODE */
    control_set_mode(&ctrl, MODE_OPEN);
    control_set_out(&ctrl, 0.5f);

    for (int i = 0; i < 50; i++) {
        ctrl.meas = plant_tick(&plant, ctrl.out);
        control_tick(&ctrl);
        printf("OPEN y=%.3f u=%.3f\n", ctrl.meas, ctrl.out);
    }

    /* CLOSED MODE */
    control_set_mode(&ctrl, MODE_CLOSED);
    control_set_setpoint(&ctrl, 1.0f);

    for (int i = 0; i < 100; i++) {
        ctrl.meas = plant_tick(&plant, ctrl.out);
        control_tick(&ctrl);
        printf("CLOSED y=%.3f u=%.3f\n", ctrl.meas, ctrl.out);
    }

    return 0;
}
