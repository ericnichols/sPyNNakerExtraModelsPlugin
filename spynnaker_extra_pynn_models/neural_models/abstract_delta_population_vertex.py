from spynnaker.pyNN.utilities.constants import POPULATION_BASED_REGIONS
from spynnaker.pyNN.utilities import utility_calls
from abc import ABCMeta
from six import add_metaclass
from abc import abstractmethod


@add_metaclass(ABCMeta)
class AbstractDeltaPopulationVertex(object):
    """
    This represents a pynn_population.py with two delta synapses
    """
    @abstractmethod
    def is_delta_vertex(self):
        """helper method for is_instance
        :return:
        """

    def get_n_synapse_parameters_per_synapse_type(self):
        # Delta synapses require no parameters
        return 0

    def get_n_synapse_types(self):

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
        **YUCK** Basically does nothing beside switch region
        """

        # Set the focus to the memory region 3 (synapse parameters):
        spec.switch_write_focus(
            region=POPULATION_BASED_REGIONS.SYNAPSE_PARAMS.value)

        spec.comment("\nWriting empty delta synapse parameters")
