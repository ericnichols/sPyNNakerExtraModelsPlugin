APP = $(notdir $(CURDIR))$(EXT)
EXTRA_SRC_DIR = $(abspath $(CURDIR)/../)
SOURCE_DIR = $(NEURAL_MODELLING_DIRS)/src/neuron
MODEL_OBJS = lif_cond_stoc.o
NEURON_MODEL_H = lif_cond_stoc.h
SYNAPSE_SHAPING_H = $(NEURAL_MODELLING_DIRS)/src/neuron/synapses/exponential_impl.h

CFLAGS=-I.
CFLAGS+= -I$(NEURAL_MODELLING_DIRS)/src

APP_OUTPUT_DIR = $(CURDIR)/../build

include $(NEURAL_MODELLING_DIRS)/src/neuron/builds/Makefile.common

The output is:

(spinenv)Roberts-MacBook-Pro:src robandrew$ make
make: *** No rule to make target `build/src/lif_cond_stoc.o', needed by `build/src.elf'.  Stop.