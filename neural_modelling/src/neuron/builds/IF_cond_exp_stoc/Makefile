APP = $(notdir $(CURDIR))
BUILD_DIR = build/
NEURON_MODEL = $(SOURCE_DIRS)/neuron/models/neuron_model_lif_impl.o
NEURON_MODEL_H = $(SOURCE_DIRS)/neuron/models/neuron_model_lif_impl.h
INPUT_TYPE_H = $(SOURCE_DIRS)/neuron/input_types/input_type_conductance.h
THRESHOLD_TYPE_H = $(EXTRA_SRC_DIR)/neuron/threshold_types/threshold_type_maass_stochastic.h
SYNAPSE_TYPE_H = $(SOURCE_DIRS)/neuron/synapse_types/synapse_types_exponential_impl.h
SYNAPSE_DYNAMICS = $(SOURCE_DIRS)/neuron/plasticity/synapse_dynamics_static_impl.o

include ../Makefile.common
