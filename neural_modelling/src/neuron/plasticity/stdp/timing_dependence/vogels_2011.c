// sPyNNaker neural modelling includes
#include "neuron/spin-neuron-impl.h"

// Vogels 2011 includes
#include "vogels_2011_impl.h"

//---------------------------------------
// Globals
//---------------------------------------
// Exponential lookup-tables
int16_t tau_lookup[TAU_SIZE];

// Global plasticity parameter data
plasticity_trace_region_data_t plasticity_trace_region_data;

//---------------------------------------
// Functions
//---------------------------------------
uint32_t *plasticity_region_trace_filled(uint32_t* address, uint32_t flags)
{
  use(flags);

  log_info("plasticity_region_trace_filled: starting");
  log_info("\tVogels 2011 timing rule");
  
  // Copy parameters
  plasticity_trace_region_data.alpha = (int32_t)address[0];
  
  // Copy LUTs from following memory
  address_t lut_address = copy_int16_lut(&address[1], TAU_SIZE, &tau_lookup[0]);

  log_info("plasticity_region_trace_filled: completed successfully");

  return lut_address;
}