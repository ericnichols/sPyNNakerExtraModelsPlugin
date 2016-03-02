from spynnaker_extra_pynn_models import model_binaries


def _init_module():
    import logging
    import os
    import spynnaker.pyNN

    # Register this path with SpyNNaker
    spynnaker.pyNN.register_binary_search_path(os.path.dirname(
        model_binaries.__file__))

_init_module()


from spynnaker_extra_pynn_models.neuron.builds.if_curr_delta \
    import IFCurrDelta as IF_curr_delta
from spynnaker_extra_pynn_models.neuron.builds.if_curr_exp_ca2_adaptive \
    import IFCurrExpCa2Adaptive as IF_curr_exp_ca2_adaptive
from spynnaker_extra_pynn_models.neuron.builds.if_cond_exp_stoc \
    import IFCondExpStoc as IF_cond_exp_stoc
from spynnaker_extra_pynn_models.neural_properties.synapse_dynamics\
    .dependences.recurrent_time_dependency\
    import RecurrentTimeDependency as RecurrentRule
from spynnaker_extra_pynn_models.neural_properties.synapse_dynamics\
    .dependences.vogels_2011_time_dependency\
    import Vogels2011Rule
from spynnaker.pyNN.models.neural_models.if_curr_target_exp \
    import IFCurrentTargetExponentialPopulation as IF_curr_target_exp
