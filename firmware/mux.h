/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#ifndef MUX_H_
#define MUX_H_

#include "app.h"

typedef enum {
    MUX_STATE_UNDEFINED = 0,
    MUX_STATE_OFF,
    MUX_STATE_ON,
    MUX_STATE_GLITCH,
    MUX_STATE_MAX
} MUX_STATE;

void mux_init(APP* app);
void mux_set_state(APP* app, MUX_STATE state);



#endif /* MUX_H_ */
