#ifndef _THRESHOLD_TYPE_STOCHASTIC_H_
#define _THRESHOLD_TYPE_STOCHASTIC_H_

#include <neuron/threshold_types/threshold_type.h>

#define PROB_SATURATION 0.8k

typedef struct threshold_type_t {

    // sensitivity of soft threshold to membrane voltage [mV^(-1)]
    // (inverted in python code)
    REAL     du_th_inv;

    // time constant for soft threshold [ms^(-1)]
    // (inverted in python code)
    REAL     tau_th_inv;

    // soft threshold value  [mV]
    REAL     theta;

    //
    REAL     machine_time_step_ms_div_10;

} threshold_type_t;

static inline bool threshold_type_is_above_threshold(state_t value,
                        threshold_type_pointer_t threshold_type) {

    return REAL_COMPARE(value, >=, threshold_type->threshold_value);
}

#endif // _THRESHOLD_TYPE_STOCHASTIC_H_
