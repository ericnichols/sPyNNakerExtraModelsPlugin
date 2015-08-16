#ifndef _NEURON_MODEL_LIF_CURR_CA2_ADAPTIVE_IMPL_H_
#define _NEURON_MODEL_LIF_CURR_CA2_ADAPTIVE_IMPL_H_

#include <neuron/models/neuron_model.h>

//----------------------------------------------------------------------------
// Model from Liu, Y. H., & Wang, X. J. (2001). Spike-frequency adaptation of
// a generalized leaky integrate-and-fire model neuron. Journal of
// Computational Neuroscience, 10(1), 25-45. doi:10.1023/A:1008916026143
//----------------------------------------------------------------------------
// only works properly for 1000, 700, 400 microsec timesteps
//#define CORRECT_FOR_REFRACTORY_GRANULARITY

/////////////////////////////////////////////////////////////
// definition for LIF neuron parameters
typedef struct neuron_t {

    // membrane voltage threshold at which neuron spikes [mV]
    REAL    V_thresh;

    // post-spike reset membrane voltage [mV]
    REAL    V_reset;

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

    // countdown to end of next refractory period  [timesteps]
    int32_t refract_timer;

    // refractory time of neuron [timesteps]
    int32_t T_refract;

    // Calcium current
    REAL    I_Ca2;

    // Influx of CA2 caused by each spike
    REAL    I_alpha;

    // exp ( -(machine time step in ms)/(TauCa) )
    REAL    exp_TauCa;

} neuron_t;

typedef struct global_neuron_params_t {
} global_neuron_params_t;

//! \function that converts the input into the real value to be used by the
//! neuron
//! \param[in] input the input buffer that needs converting for use by the
//! neuron
//! \return the converted input buffer which has been converted for use by the
//! neuron
static inline input_t neuron_model_convert_input(input_t input) {
    return input;
}

#endif // _NEURON_MODEL_LIF_CURR_CA2_ADAPTIVE_IMPL_H_

