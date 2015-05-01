#ifndef _NEURON_MODEL_LT_CURR_IMPL_H_
#define _NEURON_MODEL_LT_CURR_IMPL_H_

#include <neuron/models/neuron_model.h>

// only works properly for 1000, 700, 400 microsec timesteps
//#define CORRECT_FOR_REFRACTORY_GRANULARITY

//#define CORRECT_FOR_THRESHOLD_GRANULARITY

// 9% slower than standard but inevitably more accurate(?) might over-compensate
//#define SIMPLE_COMBINED_GRANULARITY

/////////////////////////////////////////////////////////////
// definition for LT neuron parameters
typedef struct neuron_t {

    // membrane voltage threshold above which neuron generates output [mV]
    REAL    V_thresh;

    // membrane resting voltage [mV]
    REAL    V_rest;

    // membrane resistance [some multiplier of Ohms, TBD probably MegaOhm]
    REAL    R_membrane;

    // membrane voltage [mV]
    REAL    V_membrane;

    // offset current [nA] but take care because actually 'per timestep charge'
    REAL    I_offset;

    // 'fixed' computation parameter - time constant multiplier for
    // closed-form solution
    // exp( -(machine time step in ms)/(R * C) ) [.]
    REAL    exp_TC;
    
    // scaling factor to apply to (V_membrane - V_rest) to generate gradient potential
    REAL    graded_potential_scale;
} neuron_t;

//! \function that converts the input into the real value to be used by the
//! neuron
//! \param[in] input the input buffer that needs converting for use by the
//! neuron
//! \return the converted input buffer which has been converted for use by the
//! neuron
static inline input_t neuron_model_convert_input(input_t input) {
    return input;
}

#endif // _NEURON_MODEL_LT_CURR_IMPL_H_

