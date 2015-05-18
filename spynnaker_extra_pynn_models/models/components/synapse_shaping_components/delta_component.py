"""
delta component
"""

from spynnaker.pyNN.models.components.synapse_shape_components.\
    abstract_synapse_shape_component import \
    AbstractSynapseShapeComponent
from spynnaker.pyNN.utilities import constants
from spynnaker.pyNN.utilities import utility_calls

from abc import ABCMeta
from six import add_metaclass
from abc import abstractmethod
import hashlib


@add_metaclass(ABCMeta)
class DeltaComponent(AbstractSynapseShapeComponent):
    """
    delta component
    """

    def __init__(self):
        pass

    def get_synapse_shape_magic_number(self):
        """
        over ridden from AbstractSynapseShapeComponent
        :return:
        """
        return hashlib.md5("synapse_types_delta_impl").hexdigest()[:8]

    @abstractmethod
    def is_delta_shaped(self):
        """
        helper method for isinstance
        :return:
        """

    def get_n_synapse_parameters_per_synapse_type(self):
        """
        returns how mnay synpase params are needed for a delta shaping component
        :return:
        """
        # Delta synapses require no parameters
        return 0

    @staticmethod
    def get_n_synapse_types():
        """
        returns how many different types of synapses are supported by the delta
        shaping component
        :return:
        """

        # There are 2 synapse types (excitatory and inhibitory)
        return 2

    @staticmethod
    def get_n_synapse_type_bits():
        """
        Return the number of bits used to identify the synapse in the synaptic
        row
        """
        return 1

    def write_synapse_parameters(self, spec, subvertex, vertex_slice):
        """
        :param spec: the dsg writer
        :param subvertex: the parttiioned vertex to have its synapse paramters
        written
        :param vertex_slice: the slice of the partitionable vertex this
        partitioned vertex is to cover.
        """
        utility_calls.unused(subvertex)
        utility_calls.unused(vertex_slice)
        # Set the focus to the memory region 3 (synapse parameters):
        spec.switch_write_focus(
            region=constants.POPULATION_BASED_REGIONS.SYNAPSE_PARAMS.value)

        spec.comment("\nWriting empty delta synapse parameters")