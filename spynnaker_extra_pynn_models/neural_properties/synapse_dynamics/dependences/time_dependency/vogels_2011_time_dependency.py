from data_specification.enums.data_type import DataType

from spynnaker.pyNN.models.neural_properties.synapse_dynamics.abstract_rules.\
    abstract_time_dependency import AbstractTimeDependency
from spynnaker.pyNN.models.neural_properties.synapse_dynamics.\
    plastic_weight_synapse_row_io import PlasticWeightSynapseRowIo
from spynnaker.pyNN.models.neural_properties.synapse_dynamics\
    import plasticity_helpers

import hashlib
import logging
logger = logging.getLogger(__name__)

# Constants
LOOKUP_TAU_SIZE = 256
LOOKUP_TAU_SHIFT = 0

class Vogels2011Rule(AbstractTimeDependency):

    def __init__(self, alpha, tau=20.0):
        AbstractTimeDependency.__init__(self)
        
        self._alpha = alpha
        self._tau = tau

    def __eq__(self, other):
        if (other is None) or (not isinstance(other, Vogels2011Rule)):
            return False
        return ((self._tau == other._tau))

    def create_synapse_row_io(
            self, synaptic_row_header_words, dendritic_delay_fraction):
        return PlasticWeightSynapseRowIo(
            synaptic_row_header_words, dendritic_delay_fraction)

    def get_params_size_bytes(self):
        return 4 + (2 * LOOKUP_TAU_SIZE)

    def is_time_dependance_rule_part(self):
        return True

    def write_plastic_params(self, spec, machine_time_step, weight_scales,
                             global_weight_scale):

        # Check timestep is valid
        if machine_time_step != 1000:
            raise NotImplementedError("STDP LUT generation currently only "
                                      "supports 1ms timesteps")
        
        # Write alpha to spec
        fixed_point_alpha = plasticity_helpers.float_to_fixed(
            self._alpha, plasticity_helpers.STDP_FIXED_POINT_ONE)
        spec.write_value(data=fixed_point_alpha, data_type=DataType.INT32)
        
        # Write lookup table
        plasticity_helpers.write_exp_lut(spec, self.tau,
                                         LOOKUP_TAU_SIZE,
                                         LOOKUP_TAU_SHIFT)

    @property
    def num_terms(self):
        return 1

    @property
    def vertex_executable_suffix(self):
        return "vogels_2011"

    @property
    def pre_trace_size_bytes(self):
        # Trace entries consist of a single 16-bit number
        return 2

    @property
    def tau(self):
        return self._tau

    def get_component_magic_number_identifiers(self):
        """
        over ride from requires_component_magic_number.py
        :return: returns the magic number aossicated with this component
        """
        return hashlib.md5("timing_vogels_2011_impl").hexdigest()[:8]