#ifndef RECURRENT_PRE_STOCHASTIC_IMPL
#define RECURRENT_PRE_STOCHASTIC_IMPL

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
// Helper macros for checking windows
#define IN_PRE_WINDOW(time_since_last_event, previous_state) in_window(time_since_last_event, previous_state.window_length)
#define IN_POST_WINDOW(time_since_last_event, previous_state) in_window(time_since_last_event, previous_state.window_length)

//---------------------------------------
// Structures
//---------------------------------------
// Plastic synapse contains normal 16-bit weight, a small state machine and an accumulator
typedef struct plastic_synapse_t
{
  unsigned int weight         : 16;
  int accumulator             : 4;
  unsigned int state          : 2;
  unsigned int window_length  : 10;
} plastic_synapse_t;

// The update state is a weight state with 32-bit ARM-friendly versions of the accumulator and the state
typedef struct update_state_t
{
  weight_state_t weight_state;

  int32_t accumulator;
  int32_t state;

  uint32_t window_length;
} update_state_t;

typedef struct
{
  int32_t accumulator_depression_plus_one;
  int32_t accumulator_potentiation_minus_one;
} plasticity_trace_region_data_t;

typedef plastic_synapse_t final_state_t;

typedef struct post_trace_t
{
} post_trace_t;

typedef struct pre_trace_t
{
} pre_trace_t;

//---------------------------------------
// Externals
//---------------------------------------
extern uint16_t pre_exp_dist_lookup[STDP_FIXED_POINT_ONE];
extern uint16_t post_exp_dist_lookup[STDP_FIXED_POINT_ONE];

//---------------------------------------
// Helper functions
//---------------------------------------
static inline int32_t mars_kiss_fixed_point()
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
  return (int32_t)(random & (STDP_FIXED_POINT_ONE - 1));
}
//---------------------------------------
static inline bool in_window(uint32_t time_since_last_event, const uint32_t window_length)
{
  return (time_since_last_event < window_length);
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
  update_state.state = (uint32_t)synaptic_word.state;
  update_state.window_length = (uint32_t)synaptic_word.window_length;
  return update_state;
}
//---------------------------------------
static inline final_state_t synapse_get_final(update_state_t state)
{
  // Get weight from state
  weight_t weight = weight_get_final(state.weight_state);

  // Build this into synaptic word along with updated accumulator and state
  return (final_state_t){ .weight = weight, .accumulator = (int)state.accumulator, .state = (unsigned int)state.state, .window_length = (unsigned int)state.window_length };
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

//---------------------------------------
// Recurrent trace rule interface functions
//--------------------------------------
static inline update_state_t calculate_pre_window(update_state_t previous_state)
{
  // Pick random number and use to draw from exponential distribution
  int32_t random = mars_kiss_fixed_point();
  previous_state.window_length = pre_exp_dist_lookup[random];
  plastic_runtime_log_info("\t\tRandom=%d, Exp dist=%u", random, previous_state.window_length);

  return previous_state;
}
//---------------------------------------
static inline update_state_t calculate_post_window(update_state_t previous_state)
{
   // Pick random number and use to draw from exponential distribution
  int32_t random = mars_kiss_fixed_point();
  previous_state.window_length = post_exp_dist_lookup[random];
  plastic_runtime_log_info("\t\tRandom=%d, Exp dist=%u", random, previous_state.window_length);

  return previous_state;
}

#endif  // RECURRENT_PRE_STOCHASTIC_IMPL
