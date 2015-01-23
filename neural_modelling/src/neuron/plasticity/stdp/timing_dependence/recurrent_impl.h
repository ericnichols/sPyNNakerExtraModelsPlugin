#ifndef RECURRENT_IMPL
#define RECURRENT_IMPL

// Standard includes
#include <stdbool.h>
#include <stdint.h>

// sPyNNaker neural modelling includes
#include "neuron/spin-neuron-impl.h"

// sPyNNaker plasticity common includes
#include "neuron/plasticity/common/maths.h"
#include "neuron/plasticity/common/runtime_log.h"

//---------------------------------------
// Macros
//---------------------------------------
// Synapse states
#define STATE_IDLE      0
#define STATE_PRE_OPEN  1
#define STATE_POST_OPEN 2

//---------------------------------------
// Externals
//---------------------------------------
extern plasticity_trace_region_data_t plasticity_trace_region_data;

//---------------------------------------
// Timing dependence functions
//---------------------------------------
static inline post_trace_t timing_get_initial_post_trace()
{
  return (post_trace_t){};
}
//---------------------------------------
static inline post_trace_t timing_add_post_spike(uint32_t last_time, post_trace_t last_trace)
{
  use(&last_time);
  use(&last_trace);

  plastic_runtime_log_info("\tdelta_time=%u", time - last_time);

  // Return new pre- synaptic event with decayed trace values with energy for new spike added
  return (post_trace_t){};
}
//---------------------------------------
static inline pre_trace_t timing_add_pre_spike(uint32_t last_time, pre_trace_t last_trace)
{
  use(&last_time);
  use(&last_trace);

  plastic_runtime_log_info("\tdelta_time=%u", time - last_time);

  return (pre_trace_t){};
}
//---------------------------------------
static inline update_state_t timing_apply_pre_spike(uint32_t time, pre_trace_t trace,
  uint32_t last_pre_time, pre_trace_t last_pre_trace,
  uint32_t last_post_time, post_trace_t last_post_trace,
  update_state_t previous_state)
{
  use(&trace);
  use(&last_pre_trace);
  use(&last_post_trace);

  // If we're idle, transition to pre-open state
  if(previous_state.state == STATE_IDLE)
  {
    plastic_runtime_log_info("\tOpening pre-window");
    previous_state.state = STATE_PRE_OPEN;
    previous_state = calculate_pre_window(previous_state);
  }
  // If we're in pre-open state
  else if(previous_state.state == STATE_PRE_OPEN)
  {
    // Get time of event relative to last pre-synaptic event
    uint32_t time_since_last_pre = time - last_pre_time;

    plastic_runtime_log_info("\tTime_since_last_pre_event=%u", time_since_last_pre);

    // If pre-window is still open
    if(IN_PRE_WINDOW(time_since_last_pre, previous_state))
    {
      plastic_runtime_log_info("\t\tClosing pre-window");
      previous_state.state = STATE_IDLE;
    }
    // Otherwise, leave state alone (essentially re-opening window)
    else
    {
      plastic_runtime_log_info("\t\tRe-opening pre-window");
      previous_state = calculate_pre_window(previous_state);
    }
  }
  // Otherwise, if we're in post-open
  else if(previous_state.state == STATE_POST_OPEN)
  {
    // Get time of event relative to last post-synaptic event
    uint32_t time_since_last_post = time - last_post_time;

    plastic_runtime_log_info("\tTime_since_last_post_event=%u", time_since_last_post);

    // If pre-synaptic spike occured at the same time, ignore it
    if(time_since_last_post == 0)
    {
      plastic_runtime_log_info("\t\tIgnoring coinciding spikes");

      // Transition back to idle
      previous_state.state = STATE_IDLE;
    }
    // Otherwise, if post-window is still open
    else if(IN_POST_WINDOW(time_since_last_post, previous_state))
    {
      // If accumulator's not going to hit depression limit, decrement it
      if(previous_state.accumulator > plasticity_trace_region_data.accumulator_depression_plus_one)
      {
        previous_state.accumulator--;
        plastic_runtime_log_info("\t\tDecrementing accumulator=%d", previous_state.accumulator);
      }
      // Otherwise, reset accumulator and apply depression
      else
      {
        plastic_runtime_log_info("\t\tApplying depression");

        previous_state.accumulator = 0;
        previous_state.weight_state = weight_apply_depression(previous_state.weight_state, STDP_FIXED_POINT_ONE);
      }

      // Transition back to idle
      previous_state.state = STATE_IDLE;
    }
    // Otherwise, if post-window has closed, skip idle state and go straight to pre-open
    else
    {
      plastic_runtime_log_info("\t\tPost-window closed - Opening pre-window");
      previous_state.state = STATE_PRE_OPEN;
      previous_state = calculate_pre_window(previous_state);
    }
  }
  else
  {
    plastic_runtime_log_info("\tInvalid state %u", previous_state.state);
  }

  return previous_state;
}
//---------------------------------------
static inline update_state_t timing_apply_post_spike(uint32_t time, post_trace_t trace,
  uint32_t last_pre_time, pre_trace_t last_pre_trace,
  uint32_t last_post_time, post_trace_t last_post_trace,
  update_state_t previous_state)
{
  use(&trace);
  use(&last_pre_trace);
  use(&last_post_trace);

  // If we're idle, transition to post-open state
  if(previous_state.state == STATE_IDLE)
  {
    plastic_runtime_log_info("\tOpening post-window");
    previous_state.state = STATE_POST_OPEN;
    previous_state = calculate_post_window(previous_state);
  }
  // If we're in post-open state
  else if(previous_state.state == STATE_POST_OPEN)
  {
    // Get time of event relative to last post-synaptic event
    uint32_t time_since_last_post = time - last_post_time;

    plastic_runtime_log_info("\tTime_since_last_post_event=%u", time_since_last_post);

    // If post window's still open
    if(IN_POST_WINDOW(time_since_last_post, previous_state))
    {
      plastic_runtime_log_info("\t\tClosing post-window");

      previous_state.state = STATE_IDLE;
    }
    // Otherwise, leave state alone (essentially re-opening window)
    else
    {
      plastic_runtime_log_info("\t\tRe-opening post-window");
      previous_state = calculate_post_window(previous_state);
    }
  }
  // Otherwise, if we're in pre-open
  else if(previous_state.state == STATE_PRE_OPEN)
  {
    // Get time of event relative to last pre-synaptic event
    uint32_t time_since_last_pre = time - last_pre_time;

    plastic_runtime_log_info("\tTime_since_last_pre_event=%u", time_since_last_pre);

    // If post-synaptic spike occured at the same time, ignore it
    if(time_since_last_pre == 0)
    {
      plastic_runtime_log_info("\t\tIgnoring coinciding spikes");

      // Transition back to idle
      previous_state.state = STATE_IDLE;
    }
    // Otherwise, if pre-window's still open
    else if(IN_PRE_WINDOW(time_since_last_pre, previous_state))
    {
      // If accumulator's not going to hit potentiation limit, increment it
      if(previous_state.accumulator < plasticity_trace_region_data.accumulator_potentiation_minus_one)
      {
        previous_state.accumulator++;
        plastic_runtime_log_info("\t\tIncrementing accumulator=%d", previous_state.accumulator);
      }
      // Otherwise, reset accumulator and apply potentiation
      else
      {
        plastic_runtime_log_info("\t\tApplying potentiation");

        previous_state.accumulator = 0;
        previous_state.weight_state = weight_apply_potentiation(previous_state.weight_state, STDP_FIXED_POINT_ONE);
      }

      // Transition back to idle
      previous_state.state = STATE_IDLE;
    }
    // Otherwise, if post-window has closed, skip idle state and go straight to pre-open
    else
    {
      plastic_runtime_log_info("\t\tPre-window closed - Opening post-window");
      previous_state.state = STATE_POST_OPEN;
      previous_state = calculate_post_window(previous_state);
    }
  }
  else
  {
    plastic_runtime_log_info("\tInvalid state %u", previous_state.state);
  }

  return previous_state;
}

#endif	// RECURRENT_IMPL
