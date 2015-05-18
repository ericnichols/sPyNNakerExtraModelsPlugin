from spynnaker_extra_pynn_models import model_binaries


def _init_module():
    import os
    import spynnaker.pyNN

    # Register this path with SpyNNaker
    spynnaker.pyNN.register_binary_search_path(os.path.dirname(
        model_binaries.__file__))

_init_module()

from spynnaker_extra_pynn_models.neural_properties.synapse_dynamics.dependences.time_dependency.recurrent_time_dependency \
    import RecurrentTimeDependency as RecurrentRule
