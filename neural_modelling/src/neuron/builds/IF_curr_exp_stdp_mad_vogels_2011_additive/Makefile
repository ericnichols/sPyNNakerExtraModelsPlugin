APP = $(notdir $(CURDIR))
BUILD_DIR = build/
EXTRA_SOURCE_DIR = $(CURDIR)/../../../
SOURCE_DIRS = $(NEURAL_MODELLING_DIRS)/src/neuron
MODEL_OBJS = $(SOURCE_DIRS)/neuron/models/neuron_model_lif_curr_impl.o \
             $(SOURCE_DIRS)/neuron/plasticity/common/post_events.o \
             $(SOURCE_DIRS)/neuron/plasticity/stdp/synapse_dynamics_stdp_mad_impl.o \
             $(SOURCE_DIRS)/neuron/plasticity/common/maths.o \
             $(EXTRA_SOURCE_DIR)/neuron/plasticity/stdp/timing_dependence/vogels_2011.o \
             $(SOURCE_DIRS)/neuron/plasticity/stdp/weight_dependence/weight_additive_one_term_impl.o
NEURON_MODEL_H = $(SOURCE_DIRS)/neuron/models/neuron_model_lif_curr_impl.h
SYNAPSE_TYPE_H = $(SOURCE_DIRS)/neuron/synapse_types/synapse_types_exponential_impl.h
TIMING_DEPENDENCE_H = $(EXTRA_SOURCE_DIR)/neuron/plasticity/stdp/timing_dependence/vogels_2011_impl.h
WEIGHT_DEPENDENCE_H = $(SOURCE_DIRS)/neuron/plasticity/stdp/weight_dependence/weight_additive_one_term_impl.h
PLASTIC_SYNAPSE_STRUCTURE_H = $(SOURCE_DIRS)/neuron/plasticity/stdp/synapse_structure/synapse_structure_weight_impl.h
APPLICATION_MAGIC_NUMBER = 0xAC1

CFLAGS += -I$(NEURAL_MODELLING_DIRS)/src

APP_OUTPUT_DIR = $(CURDIR)/../../../../../spynnaker_extra_pynn_models/model_binaries

include $(SOURCE_DIRS)/builds/Makefile.common