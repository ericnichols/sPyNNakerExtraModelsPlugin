APP = $(notdir $(CURDIR))
BUILD_DIR = build/

MODEL_PATH = $(SOURCE_DIRS)/neuron/models/neuron_model_lif_curr_impl
SYNAPSE_TYPE_PATH = ../../synapse_types/synapse_types_exponential_impl
SYNAPSE_DYNAMICS_PATH =  $(SOURCE_DIRS)/neuron/plasticity/stdp/synapse_dynamics_stdp_impl

USES_STDP = TRUE
STDP_TIMING_PATH = $(SOURCE_DIRS)/neuron/plasticity/stdp/timing_dependence/timing_recurrent_pre_stochastic_impl
STDP_WEIGHT_PATH = $(SOURCE_DIRS)/neuron/plasticity/stdp/weight_dependence/weight_multiplicative_impl
PLASTIC_STRUCTURE_PATH = ../../plasticity/stdp/synapse_structure/synapse_structure_weight_state_accumulator_window_impl

include ../Makefile.common
