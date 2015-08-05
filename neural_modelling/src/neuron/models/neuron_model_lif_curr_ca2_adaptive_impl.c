#include "neuron_model_lif_curr_ca2_adaptive_impl.h"

#include <debug.h>

//----------------------------------------------------------------------------
// Model from Liu, Y. H., & Wang, X. J. (2001). Spike-frequency adaptation of
// a generalized leaky integrate-and-fire model neuron. Journal of
// Computational Neuroscience, 10(1), 25-45. doi:10.1023/A:1008916026143
//----------------------------------------------------------------------------
// for general machine time steps
// defaults to 1ms time step i.e. 10 x 1/10ths of a msec
static uint32_t	refractory_time_update = 10;

// simple Leaky I&F ODE - discrete changes elsewhere -  assumes 1ms timestep?
static inline void _lif_neuron_closed_form(
        neuron_pointer_t neuron, REAL V_prev, input_t input_this_timestep) {

    REAL alpha = input_this_timestep * neuron->R_membrane + neuron->V_rest;
    REAL this_eTC = neuron->exp_TC;

    // update membrane voltage
    neuron->V_membrane = alpha - (this_eTC * (alpha - V_prev));
}

// ODE solver has just set neuron->V which is current state of membrane voltage
static inline void _neuron_discrete_changes(neuron_pointer_t neuron) {

    // reset membrane voltage
    neuron->V_membrane = neuron->V_reset;

    // reset refractory timer
    neuron->refract_timer  = neuron->T_refract;

    // Apply influx of calcium to trace
    neuron->I_Ca2 += neuron->I_alpha;
}


// setup function which needs to be called in main program before any neuron
// code executes
// MUST BE: minimum 100, then in 100usec steps...
void neuron_model_set_machine_timestep(timer_t microsecs) {

    const uint16_t time_step_divider = 100;

    // 10 for 1ms time step, 1 for 0.1ms time step which is minimum
    refractory_time_update = microsecs / time_step_divider;
}

bool neuron_model_state_update(input_t exc_input, input_t inh_input,
                               input_t external_bias, neuron_pointer_t neuron) {

    bool spike = false;
    REAL V_last = neuron->V_membrane;

    // countdown refractory timer
    neuron->refract_timer -= refractory_time_update;

    // Decay Ca2 trace
    neuron->I_Ca2 *= neuron->exp_TauCa;

    // If outside of the refractory period
    if (neuron->refract_timer <= 0) {

        // Get the input in nA
        input_t input_this_timestep = exc_input - inh_input
                                    - neuron->I_Ca2
                                    + external_bias + neuron->I_offset;

        _lif_neuron_closed_form(neuron, V_last, input_this_timestep);

        spike = REAL_COMPARE(neuron->V_membrane, >=, neuron->V_thresh);

        if (spike) {
            _neuron_discrete_changes(neuron);
        }
    }
    return spike;
}

state_t neuron_model_get_membrane_voltage(neuron_pointer_t neuron) {
    return neuron->V_membrane;
}


// printout of neuron definition and state variables
void neuron_model_print(restrict neuron_pointer_t neuron) {
    log_debug("V membrane    = %11.4k mv", neuron->V_membrane);
    log_debug("V thresh      = %11.4k mv", neuron->V_thresh);
    log_debug("V reset       = %11.4k mv", neuron->V_reset);
    log_debug("V rest        = %11.4k mv", neuron->V_rest);

    log_debug("I Ca2         = %11.4k nA", neuron->I_Ca2);
    log_debug("I Alpha       = %11.4k nA", neuron->I_alpha);

    log_debug("I offset      = %11.4k nA", neuron->I_offset);
    log_debug("R membrane    = %11.4k Mohm", neuron->R_membrane);

    log_debug("exp(-ms/(RC)) = %11.4k [.]", neuron->exp_TC);
    log_debug("exp(-ms/(TauCa)) = %11.4k [.]", neuron->exp_TauCa);

    log_debug("T refract     = %u microsecs", neuron->T_refract * 100);
}