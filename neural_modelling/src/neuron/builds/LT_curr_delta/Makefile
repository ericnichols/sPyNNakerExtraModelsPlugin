APP = $(notdir $(CURDIR))
BUILD_DIR = build/
MODEL_OBJS = $(EXTRA_SRC_DIR)/neuron/models/neuron_model_lt_curr_impl.o $(SOURCE_DIRS)/neuron/plasticity/synapse_dynamics_static_impl.o
NEURON_MODEL_H = ../../models/neuron_model_lt_curr_impl.h
SYNAPSE_TYPE_H = $(SOURCE_DIRS)/neuron/synapse_types/synapse_types_delta_impl.h

CFLAGS += -DNEURON_GRADIENT_POTENTIAL

include ../Makefile.common
