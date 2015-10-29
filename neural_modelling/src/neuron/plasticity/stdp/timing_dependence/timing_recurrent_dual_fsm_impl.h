#ifndef _TIMING_RECURRENT_DUAL_FSM_IMPL_H_
#define _TIMING_RECURRENT_DUAL_FSM_IMPL_H_

//---------------------------------------
// Typedefines
//---------------------------------------
typedef uint16_t post_trace_t;
typedef uint16_t pre_trace_t;

#include "../synapse_structure/synapse_structure_weight_accumulator_impl.h"

#include "neuron/plasticity/stdp/timing_dependence/timing.h"
#include "neuron/plasticity/stdp/weight_dependence/weight_one_term.h"

// Include debug header for log_info etc
#include <debug.h>

// Include generic plasticity maths functions
#include "neuron/plasticity/common/maths.h"
#include "neuron/plasticity/common/stdp_typedefs.h"
#include "random.h"

//#include "random_util.h"

typedef struct {
    int32_t accumulator_depression_plus_one;
    int32_t accumulator_potentiation_minus_one;
} plasticity_trace_region_data_t;

//---------------------------------------
// Externals
//---------------------------------------
extern uint16_t pre_exp_dist_lookup[STDP_FIXED_POINT_ONE];
extern uint16_t post_exp_dist_lookup[STDP_FIXED_POINT_ONE];
extern plasticity_trace_region_data_t plasticity_trace_region_data;

static uint32_t last_event_time;

extern uint32_t last_spike;

extern uint32_t recurrentSeed[4];

#define ACCUMULATOR_DECAY_SHIFT 11

//---------------------------------------
// Timing dependence inline functions
//---------------------------------------
static inline post_trace_t timing_get_initial_post_trace() {
    return -9999;
}

//---------------------------------------
static inline post_trace_t timing_add_post_spike(
        uint32_t time, uint32_t last_time, post_trace_t last_trace) {
    /*use(&time);
    use(&last_time);
    use(&last_trace);
    // Pick random number and use to draw from exponential distribution
    uint32_t random = mars_kiss64_seed(recurrentSeed) & (STDP_FIXED_POINT_ONE - 1);
    uint16_t window_length = post_exp_dist_lookup[random];
	io_printf(IO_BUF, "-  t: %d  wl: %d    ", time, window_length);
    log_debug("\t\tResetting post-window: random=%d, window_length=%u", random,
              window_length);
			  */
    // Return window length
    return 0; //window_length;
}

//---------------------------------------
static inline pre_trace_t timing_add_pre_spike(
        uint32_t time, uint32_t last_time, pre_trace_t last_trace) {
    use(&time);
    use(&last_time);
    use(&last_trace);

    last_event_time = last_time;

    // Pick random number and use to draw from exponential distribution
    uint32_t random = mars_kiss64_seed(recurrentSeed) & (STDP_FIXED_POINT_ONE - 1);
    uint16_t window_length = pre_exp_dist_lookup[random];
	//io_printf(IO_BUF, "%u\n", window_length);
	//io_printf(IO_BUF, "AddPre t: %d  wl: %d\n", time, window_length);
    log_debug("\t\tResetting pre-window: random=%d, window_length=%u", random,
              window_length);

    // Return window length
    return window_length;
}

//---------------------------------------
// This performs three functions:
// 1) Decay the accumulator value. Long periods with no spikes should cause the state to forget as this
//    will not correspond to a complete set of pattern repeats.
// 2) Set the flag for pre_waiting_post (we've got a pre-spike so now waiting for a post -pike)
// 3) Check if there was a post-spike window open at the time that this pre-spike was detected
//    in which case we decrement the accumulator and perhaps perform synaptic depression.

static inline update_state_t timing_apply_pre_spike(
        uint32_t time, pre_trace_t trace, uint32_t last_pre_time,
        pre_trace_t last_pre_trace, uint32_t last_post_time,
        post_trace_t last_post_trace, update_state_t previous_state) {
    use(&trace);
    use(&last_pre_time);
    use(&last_pre_trace);

	// Decay accum value so that long periods without spikes cause it to forget:
    uint32_t time_since_last_event = time - last_event_time;
    if (previous_state.accumulator > 0) {
        previous_state.accumulator -=
            time_since_last_event >> ACCUMULATOR_DECAY_SHIFT;
        if (previous_state.accumulator < 0) {
            previous_state.accumulator = 0;
        }
    } else if (previous_state.accumulator < 0) {
        previous_state.accumulator +=
            time_since_last_event >> ACCUMULATOR_DECAY_SHIFT;
        if (previous_state.accumulator > 0) {
            previous_state.accumulator = 0;
        }
    }

	// Set pre_waiting_post flag, priming the pre_waiting_post state machine. 
	// A subsequent post is then able to trigger an accum update...
	//io_printf(IO_BUF, "AppPre Waiting1=%u PreWinClsTime: %u\n",
    //                     previous_state.pre_waiting_post, trace);

	// Check if there was a post window open when this pre arrived and if so,
	// trigger an accum decrement (a step towards synaptic depression):

	if (time <= previous_state.longest_post_pre_window_closing_time) {
		// The pre-spike has occurred inside a post window.

        // Get time of event relative to last post-synaptic event
        uint32_t time_since_last_post = time - last_post_time;

        log_debug("\t\t\ttime_since_last_post:%u, post_window_length:%u",
                   time_since_last_post, last_post_trace);

            if (previous_state.accumulator >
                plasticity_trace_region_data.accumulator_depression_plus_one){

                // If accumulator's not going to hit depression limit,
                // decrement it
                previous_state.accumulator--;
                io_printf(IO_BUF, "- A=%d t= %d lastPostTime: %d\n",
                         previous_state.accumulator, time, last_post_time);
            } else {

                // Otherwise, reset accumulator and apply depression
                io_printf(IO_BUF, "DEP! A=%d t= %d lastPost=%d\n",
                        previous_state.accumulator, time, last_post_time);

                previous_state.accumulator = 0;
                previous_state.weight_state = weight_one_term_apply_depression(
                    previous_state.weight_state, STDP_FIXED_POINT_ONE);
            }
    }

	// Set the post window to be just before this pre-spike. This is the only way I've found to
	// reset it. It means that the first window length will be garbage.
	previous_state.longest_post_pre_window_closing_time = time - 1;
	previous_state.pre_waiting_post = true;
	//io_printf(IO_BUF, "window status: %u\n", previous_state.pre_waiting_post);

    return previous_state;
}

// Now do two things:
// 1) Generate the window size for this post spike and extend the window closure time
// if this is beyond the current value. This is used by a following pre-spike for depression
// 2) Check if there is currently a pre-window open and then check if the post-spike is within
//    it. If so:
//               a) increment the accumulator 
//               b) perform potentiation and reset accumulator if it has reached threshold
//               c) set the pre_found_post flag, equivalent to clearing the pore_waiting_post 
//                  state machine back to idle (later post spikes will not cause an accum increment
//                  until a new pre-spike has arrived).

//---------------------------------------
static inline update_state_t timing_apply_post_spike(
        uint32_t time, post_trace_t trace, uint32_t last_pre_time,
        pre_trace_t last_pre_trace, uint32_t last_post_time,
        post_trace_t last_post_trace, update_state_t previous_state) {
    use(&trace);
    use(&last_post_time);
    use(&last_post_trace);

	// Generate a windw size for this post-spike and extend the post window if it is
	// beyond the current value:
	uint32_t random = mars_kiss64_seed(recurrentSeed) & (STDP_FIXED_POINT_ONE - 1);
    uint16_t window_length = post_exp_dist_lookup[random];
	uint32_t this_window_close_time = time + window_length;
	
	// Check if this post-spike extends the open window:
	if (previous_state.longest_post_pre_window_closing_time < this_window_close_time)
		previous_state.longest_post_pre_window_closing_time = this_window_close_time;

	//io_printf(IO_BUF, "AppPost  t: %d  wl: %d\n", time, window_length);
	//io_printf(IO_BUF, "AppPost Waiting2=%u PostWinCls: %u\n",
    //                     previous_state.pre_waiting_post, 
	//					 previous_state.longest_post_pre_window_closing_time);

    log_debug("\t\tResetting post-window: random=%d, window_length=%u", random,
              window_length);
	// Now check if this post spike occurred in the open window created by the previous pre-spike:
    if (time > last_event_time) {
        last_event_time = time;
    }

    // Get time of event relative to last pre-synaptic event
    uint32_t time_since_last_pre = time - last_pre_time;
	//io_printf(IO_BUF, "Time since last pre: %u, window status: %u\n", time_since_last_pre, previous_state.pre_waiting_post);
    log_debug("\t\t\ttime_since_last_pre:%u, pre_window_length:%u",
              time_since_last_pre, last_pre_trace);

    // If spikes don't coincide
    if (previous_state.pre_waiting_post == true && time_since_last_pre > 0) {
		//io_printf(IO_BUF, "AppPost Waiting3=%u PostWinCls: %u\n",
        //                 previous_state.pre_waiting_post, 
		//				 previous_state.longest_post_pre_window_closing_time);
		previous_state.pre_waiting_post = false;
        //!!io_printf(IO_BUF, "Pre-Sp: %d, win close: %d\n", time_since_last_pre, last_pre_trace);
        // If this post-spike has arrived within the last pre window
        if (time_since_last_pre < last_pre_trace) {

            if (previous_state.accumulator< 
				    plasticity_trace_region_data.accumulator_potentiation_minus_one) {

                // If accumulator's not going to hit potentiation limit,
                // increment it
                previous_state.accumulator++;
                io_printf(IO_BUF, "+ A= %d t= %d, lastPreTime= %d\n",
                         previous_state.accumulator, time, last_pre_time);
            } else {

                // Otherwise, reset accumulator and apply potentiation
                io_printf(IO_BUF, "Pot! A= %d t= %d, lastPreTime= %d\n",
                          previous_state.accumulator, time, last_pre_time);
                previous_state.accumulator = 0;
                previous_state.weight_state =
                    weight_one_term_apply_potentiation(
                        previous_state.weight_state, STDP_FIXED_POINT_ONE);
            }
        }
    }

    return previous_state;
}

#endif  // _TIMING_RECURRENT_DUAL_FSM_IMPL_H_
