#include "neuron_model_lt_curr_impl.h"

#include <debug.h>

// simple Leaky I&F ODE - discrete changes elsewhere -  assumes 1ms timestep?
static inline void _lt_neuron_closed_form(
        neuron_pointer_t neuron, REAL V_prev, input_t input_this_timestep) {

    REAL alpha = input_this_timestep * neuron->R_membrane + neuron->V_rest;
    REAL this_eTC = neuron->exp_TC;

    // update membrane voltage
    neuron->V_membrane = alpha - (this_eTC * (alpha - V_prev));
}

// setup function which needs to be called in main program before any neuron
// code executes
// MUST BE: minimum 100, then in 100usec steps...
void neuron_model_set_machine_timestep(timer_t microsecs) {
    use(microsecs);
}

bool neuron_model_state_update(input_t exc_input, input_t inh_input,
                               input_t external_bias, neuron_pointer_t neuron) {

    REAL V_last = neuron->V_membrane;

    // Get the input in nA
    input_t input_this_timestep = exc_input - inh_input
                                + external_bias + neuron->I_offset;
    
    // Perform closed-form update
    _lt_neuron_closed_form(neuron, V_last, input_this_timestep);

    return REAL_COMPARE(neuron->V_membrane, >=, neuron->V_thresh);
}

REAL neuron_model_get_graded_potential(neuron_pointer_t neuron) {
    // Subtract resting from membrane potential and scale
    REAL graded_potential = (neuron->V_membrane - neuron->V_rest) * neuron->graded_potential_scale;
    
    // Clamp graded potential at one and return
    return REAL_COMPARE(graded_potential, <, ONE) ? graded_potential : ONE;
}

state_t neuron_model_get_membrane_voltage(neuron_pointer_t neuron) {
    return neuron->V_membrane;
}


// printout of neuron definition and state variables
void neuron_model_print(restrict neuron_pointer_t neuron) {
    log_debug("V membrane    = %11.4k mv", neuron->V_membrane);
    log_debug("V thresh      = %11.4k mv", neuron->V_thresh);
    log_debug("V rest        = %11.4k mv", neuron->V_rest);

    log_debug("I offset      = %11.4k nA", neuron->I_offset);
    log_debug("R membrane    = %11.4k Mohm", neuron->R_membrane);

    log_debug("exp(-ms/(RC)) = %11.4k [.]", neuron->exp_TC);
}
