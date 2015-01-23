APP = $(notdir $(CURDIR))$(EXT)
EXTRA_SRC_DIR = $(CURDIR)/../../../
SOURCE_DIR = $(NEURAL_MODELLING_DIRS)/src/neuron
MODEL_OBJS = $(SOURCE_DIR)/models/lif_curr.o $(SOURCE_DIR)/plasticity/common/post_events.o $(SOURCE_DIR)/plasticity/stdp/stdp.o $(SOURCE_DIR)/plasticity/common/maths.o $(EXTRA_SOURCE_DIR)/neuron/plasticity/stdp/timing_dependence/recurrent_pre_stochastic.o $(SOURCE_DIR)/plasticity/stdp/weight_dependence/multiplicative.o
NEURON_MODEL_H = $(SOURCE_DIR)/models/lif_curr.h
SYNAPSE_SHAPING_H = $(SOURCE_DIR)/synapses/exponential_impl.h
STDP_TYPEDEFS_H = $(SOURCE_DIR)/plasticity/stdp/stdp_typedefs.h
TIMING_DEPENDENCE_H = $(EXTRA_SRC_DIR)/neuron/plasticity/stdp/timing_dependence/recurrent_pre_stochastic_impl.h
WEIGHT_DEPENDENCE_H = $(SOURCE_DIR)/plasticity/stdp/weight_dependence/multiplicative_impl.h

CFLAGS += -I$(NEURAL_MODELLING_DIRS)/src/

APP_OUTPUT_DIR = $(CURDIR)/../../../../../spynnaker_extra_pynn_models/model_binaries

include $(SOURCE_DIR)/builds/Makefile.common

$(BUILDDIR)//neuron/plasticity/stdp/timing_dependence/recurrent_pre_stochastic.o: $(EXTRA_SRC_DIR)/neuron/plasticity/stdp/timing_dependence/recurrent_pre_stochastic.c
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -include $(SYNAPSE_SHAPING_H) -include $(STDP_TYPEDEFS_H) -include $(WEIGHT_DEPENDENCE_H) -o $@ $<

$(BUILDDIR)/neuron/plasticity/stdp/weight_dependence/multiplicative.o: $(SOURCE_DIR)/plasticity/stdp/weight_dependence/multiplicative.c
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -include $(SYNAPSE_SHAPING_H) -include $(STDP_TYPEDEFS_H) -o $@ $<

$(BUILDDIR)/neuron/plasticity/stdp/stdp.o: $(SOURCE_DIR)/plasticity/stdp/stdp.c
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -include $(SYNAPSE_SHAPING_H) -include $(STDP_TYPEDEFS_H) -include $(WEIGHT_DEPENDENCE_H) -include $(TIMING_DEPENDENCE_H) -include $(EXTRA_SRC_DIR)/neuron/plasticity/stdp/timing_dependence/recurrent_impl.h -o $@ $<

$(BUILDDIR)/neuron/plasticity/common/post_events.o: $(SOURCE_DIR)/plasticity/common/post_events.c
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -include $(SYNAPSE_SHAPING_H) -include $(STDP_TYPEDEFS_H) -include $(WEIGHT_DEPENDENCE_H) -include $(TIMING_DEPENDENCE_H) -include $(EXTRA_SRC_DIR)/neuron/plasticity/stdp/timing_dependence/recurrent_impl.h -o $@ $<
