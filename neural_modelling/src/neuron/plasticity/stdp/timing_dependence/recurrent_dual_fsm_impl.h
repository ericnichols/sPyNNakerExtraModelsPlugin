#ifndef RECURRENT_DUAL_FSM_IMPL
#define RECURRENT_DUAL_FSM_IMPL

// Standard includes
#include <stdbool.h>
#include <stdint.h>

// sPyNNaker neural modelling includes
#include "neuron/spin-neuron-impl.h"

// sPyNNaker plasticity common includes
#include "neuron/plasticity/common/maths.h"
#include "neuron/plasticity/common/runtime_log.h"

//---------------------------------------
// Structures
//---------------------------------------
typedef struct
{
  int32_t accumulator_depression_plus_one;
  int32_t accumulator_potentiation_minus_one;
} plasticity_trace_region_data_t;

// Plastic synapse contains normal 16-bit weight and an accumulator
typedef struct
{
  weight_t weight;
  int16_t accumulator;
} plastic_synapse_t;

// The update state is a weight state with 32-bit ARM-friendly versions of the accumulator and the state
typedef struct
{
  weight_state_t weight_state;

  int32_t accumulator;
} update_state_t;

//---------------------------------------
// Typedefines
//---------------------------------------
typedef uint16_t post_trace_t;
typedef uint16_t pre_trace_t;
typedef plastic_synapse_t final_state_t;

//---------------------------------------
// Externals
//---------------------------------------
extern uint16_t pre_exp_dist_lookup[STDP_FIXED_POINT_ONE];
extern uint16_t post_exp_dist_lookup[STDP_FIXED_POINT_ONE];
extern plasticity_trace_region_data_t plasticity_trace_region_data;

//---------------------------------------
// Helper functions
//---------------------------------------
static inline uint32_t mars_kiss_fixed_point()
{
  // **YUCK** copy and pasted rng to allow inlining and also to avoid horrific executable bloat
  static uint32_t x = 123456789, y = 234567891, z = 345678912, w = 456789123, c = 0; /* Seed variables */
  int32_t t;

  y ^= ( y << 5 ); y ^= ( y >> 7 ); y ^= ( y << 22 );
  t = z + w + c;
  z = w;
  c = t < 0;
  w =  t & 2147483647;
  x += 1411392427;

  uint32_t random = x + y + w;

  // **YUCK** mask out and return STDP_FIXED_POINT_ONE lowest bits
  return (random & (STDP_FIXED_POINT_ONE - 1));
}

//---------------------------------------
// Timing dependence inline functions
//---------------------------------------
static inline post_trace_t timing_get_initial_post_trace()
{
  return 0;
}
//---------------------------------------
static inline post_trace_t timing_add_post_spike(uint32_t last_time, post_trace_t last_trace)
{
  use(&last_time);
  use(&last_trace);

  // Pick random number and use to draw from exponential distribution
  uint32_t random = mars_kiss_fixed_point();
  uint16_t window_length = post_exp_dist_lookup[random];
  plastic_runtime_log_info("\t\tResetting post-window: random=%d, window_length=%u", random, window_length);
  
  // Return window length
  return window_length;
}
//---------------------------------------
static inline pre_trace_t timing_add_pre_spike(uint32_t last_time, pre_trace_t last_trace)
{
  use(&last_time);
  use(&last_trace);

  // Pick random number and use to draw from exponential distribution
  uint32_t random = mars_kiss_fixed_point();
  uint16_t window_length = pre_exp_dist_lookup[random];
  plastic_runtime_log_info("\t\tResetting pre-window: random=%d, window_length=%u", random, window_length);
  
  // Return window length
  return window_length;
}
//---------------------------------------
static inline update_state_t timing_apply_pre_spike(uint32_t time, pre_trace_t trace, 
  uint32_t last_pre_time, pre_trace_t last_pre_trace, 
  uint32_t last_post_time, post_trace_t last_post_trace, 
  update_state_t previous_state)
{
  use(&trace);
  use(&last_pre_time);
  use(&last_pre_trace);
  
  // Get time of event relative to last post-synaptic event
  uint32_t time_since_last_post = time - last_post_time;
  
  plastic_runtime_log_info("\t\t\ttime_since_last_post:%u, post_window_length:%u", time_since_last_post, last_post_trace);
  
  // If spikes don't coincide
  if(time_since_last_post > 0)
  {
    // If this pre-spike has arrived within the last post window
    if(time_since_last_post < last_post_trace)
    {
      // If accumulator's not going to hit depression limit, decrement it
      if(previous_state.accumulator > plasticity_trace_region_data.accumulator_depression_plus_one)
      {
        previous_state.accumulator--;
        plastic_runtime_log_info("\t\t\t\tDecrementing accumulator=%d", previous_state.accumulator);
      }
      // Otherwise, reset accumulator and apply depression
      else
      {
        plastic_runtime_log_info("\t\t\t\tApplying depression");

        previous_state.accumulator = 0;
        previous_state.weight_state = weight_apply_depression(previous_state.weight_state, STDP_FIXED_POINT_ONE);
      }
    }
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
  use(&last_post_time);
  use(&last_post_trace);
  
  // Get time of event relative to last pre-synaptic event
  uint32_t time_since_last_pre = time - last_pre_time;
  
  plastic_runtime_log_info("\t\t\ttime_since_last_pre:%u, pre_window_length:%u", time_since_last_pre, last_pre_trace);
  
  // If spikes don't coincide
  if(time_since_last_pre > 0)
  {
    // If this post-spike has arrived within the last pre window
    if(time_since_last_pre < last_pre_trace)
    {
      // If accumulator's not going to hit potentiation limit, increment it
      if(previous_state.accumulator < plasticity_trace_region_data.accumulator_potentiation_minus_one)
      {
        previous_state.accumulator++;
        plastic_runtime_log_info("\t\t\t\tIncrementing accumulator=%d", previous_state.accumulator);
      }
      // Otherwise, reset accumulator and apply potentiation
      else
      {
        plastic_runtime_log_info("\t\t\t\tApplying potentiation");

        previous_state.accumulator = 0;
        previous_state.weight_state = weight_apply_potentiation(previous_state.weight_state, STDP_FIXED_POINT_ONE);
      }
    }
  }
  
  return previous_state;
}

//---------------------------------------
// Synapse interface functions
//---------------------------------------
static inline update_state_t synapse_init(plastic_synapse_t synaptic_word, index_t synapse_type)
{
  // Create update state, using weight dependance to initialise the weight state
  // And copying other parameters from the synaptic word into 32-bit form
  update_state_t update_state;
  update_state.weight_state = weight_init(synaptic_word.weight, synapse_type);
  update_state.accumulator = (int32_t)synaptic_word.accumulator;
  return update_state;
}
//---------------------------------------
static inline final_state_t synapse_get_final(update_state_t state)
{
  // Get weight from state
  weight_t weight = weight_get_final(state.weight_state);

  // Build this into synaptic word along with updated accumulator
  return (final_state_t){ .weight = weight, .accumulator = state.accumulator };
}
//---------------------------------------
static inline weight_t synapse_get_final_weight(final_state_t final_state)
{
  return final_state.weight;
}
//---------------------------------------
static inline plastic_synapse_t synapse_get_final_synaptic_word(final_state_t final_state)
{
  return final_state;
}

#endif  // RECURRENT_DUAL_FSM_IMPL