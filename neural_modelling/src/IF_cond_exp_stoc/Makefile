APP = $(notdir $(CURDIR))$(EXT)
EXTRA_SRC_DIR = $(CURDIR)/../
SOURCE_DIR = $(NEURAL_MODELLING_DIRS)/src/neuron
MODEL_OBJS = lif_cond_stoc.o $(SOURCE_DIR)/static_synapses.o
NEURON_MODEL_H = lif_cond_stoc.h
SYNAPSE_SHAPING_H = $(NEURAL_MODELLING_DIRS)/src/neuron/synapses/exponential_impl.h

CFLAGS=-I.
CFLAGS+= -I$(NEURAL_MODELLING_DIRS)/src

APP_OUTPUT_DIR = $(CURDIR)/../../../spynnaker_extra_pynn_models/model_binaries

include $(NEURAL_MODELLING_DIRS)/src/neuron/builds/Makefile.common
