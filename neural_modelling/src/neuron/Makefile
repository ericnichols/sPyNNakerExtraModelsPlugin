MODELS = IF_cond_exp_stoc IF_curr_exp_stdp_recurrent_pre_stochastic_multiplicative IF_curr_exp_stdp_mad_recurrent_dual_fsm_multiplicative IF_curr_exp_stdp_mad_vogels_2011_additive LT_curr_delta
BUILD_DIRS := $(addprefix builds/, $(MODELS))

all: $(BUILD_DIRS)
	for d in $(BUILD_DIRS); do (cd $$d; "$(MAKE)") || exit $$?; done

clean: $(BUILD_DIRS)
	for d in $(BUILD_DIRS); do (cd $$d; "$(MAKE)" clean) || exit $$?; done
