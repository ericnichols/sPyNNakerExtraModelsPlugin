// sPyNNaker neural modelling includes
#include "neuron/spin-neuron-impl.h"

// Recurrent STDP includes
#include "recurrent_pre_stochastic_impl.h"
#include "recurrent_impl.h"

//---------------------------------------
// Globals
//---------------------------------------
// Exponential lookup-tables
uint16_t pre_exp_dist_lookup[STDP_FIXED_POINT_ONE];
uint16_t post_exp_dist_lookup[STDP_FIXED_POINT_ONE];

// Global plasticity parameter data
plasticity_trace_region_data_t plasticity_trace_region_data;

//---------------------------------------
// Functions
//---------------------------------------
uint32_t *plasticity_region_trace_filled(uint32_t* address, uint32_t flags)
{
  use(flags);

  log_info("plasticity_region_trace_filled: starting");
  log_info("\tRecurrent pre-calculated stochastic STDP rule");

  // Copy plasticity region data from address
  // **NOTE** this seems somewhat safer than relying on sizeof
  plasticity_trace_region_data.accumulator_depression_plus_one = (int32_t)address[0];
  plasticity_trace_region_data.accumulator_potentiation_minus_one = (int32_t)address[1];
  
  log_info("\tAccumulator depression=%d, Accumulator potentiation=%d", plasticity_trace_region_data.accumulator_depression_plus_one - 1, plasticity_trace_region_data.accumulator_potentiation_minus_one + 1);

  // Copy LUTs from following memory 
  // **HACK** these aren't actually int16_t-based but this function will still work fine
  address_t lut_address = copy_int16_lut(&address[2], STDP_FIXED_POINT_ONE, (int16_t*)&pre_exp_dist_lookup[0]);
  lut_address = copy_int16_lut(lut_address, STDP_FIXED_POINT_ONE, (int16_t*)&post_exp_dist_lookup[0]);

  log_info("plasticity_region_trace_filled: completed successfully");

  return lut_address;
}