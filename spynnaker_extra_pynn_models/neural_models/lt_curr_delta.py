"""
LTCurrentDeltaPopulation
"""
from spynnaker.pyNN.utilities.constants import POPULATION_BASED_REGIONS
from spynnaker.pyNN.models.abstract_models.abstract_population_vertex import \
    AbstractPopulationVertex
from spynnaker.pyNN.utilities import constants
from abstract_delta_population_vertex import AbstractDeltaPopulationVertex
from spynnaker.pyNN.models.abstract_models.abstract_model_components.\
    abstract_leaky_integrate_properties \
    import AbstractLeakyIntegrateProperties
from spynnaker.pyNN.models.neural_properties.neural_parameter \
    import NeuronParameter


from data_specification.enums.data_type import DataType


class LTCurrentDeltaPopulation(AbstractDeltaPopulationVertex,
                               AbstractLeakyIntegrateProperties,
                               AbstractPopulationVertex):
    """
    LTCurrentDeltaPopulation: model which represents a leaky intergate
    model which sends gradient potentials relative to it's membrane voltage
    """
    _model_based_max_atoms_per_core = 256

    # noinspection PyPep8Naming
    def __init__(self, n_neurons, machine_time_step, timescale_factor,
                 spikes_per_second, ring_buffer_sigma, graded_potential_scale, 
                 constraints=None, label=None, tau_m=20.0, cm=1.0, 
                 v_rest=-65.0, v_thresh=-50.0, i_offset=0, v_init=None):
        
        self._graded_potential_scale = graded_potential_scale
        self._v_thresh = v_thresh
        
        # Instantiate the parent classes
        AbstractLeakyIntegrateProperties.__init__(
            self, atoms=n_neurons, cm=cm, tau_m=tau_m, i_offset=i_offset,
            v_init=v_init, v_rest=v_rest)
        AbstractPopulationVertex.__init__(
            self, n_neurons=n_neurons, n_params=10, label=label,
            binary="LT_curr_delta.aplx", constraints=constraints,
            max_atoms_per_core=(LTCurrentDeltaPopulation
                                ._model_based_max_atoms_per_core),
            machine_time_step=machine_time_step,
            timescale_factor=timescale_factor,
            spikes_per_second=spikes_per_second,
            ring_buffer_sigma=ring_buffer_sigma)
    @property
    def model_name(self):
        """

        :return:
        """
        return "LT_curr_delta"

    @staticmethod
    def set_model_max_atoms_per_core(new_value):
        """

        :param new_value:
        :return:
        """
        LTCurrentDeltaPopulation.\
            _model_based_max_atoms_per_core = new_value

    def get_cpu_usage_for_atoms(self, vertex_slice, graph):
        """

        :param vertex_slice:
        :param graph:
        :return:
        """
        return 781 * ((vertex_slice.hi_atom - vertex_slice.lo_atom) + 1)

    def get_parameters(self):
        """
        Generate Neuron Parameter data (region 2):
        """
        # Get the parameters
        return [
            NeuronParameter(self._v_thresh, DataType.S1615),
            NeuronParameter(self._v_rest, DataType.S1615),
            NeuronParameter(self.r_membrane(self._machine_time_step),
                            DataType.S1615),
            NeuronParameter(self._v_init, DataType.S1615),
            NeuronParameter(self.ioffset(self._machine_time_step),
                            DataType.S1615),
            NeuronParameter(self.exp_tc(self._machine_time_step),
                            DataType.S1615),
            NeuronParameter(self._graded_potential_scale, DataType.S1615)]
    
    def is_population_vertex(self):
        """

        :return:
        """
        return True

    def is_leaky_integrate_vertex(self):
        """

        :return:
        """
        return True
    
    def is_delta_vertex(self):
        """

        :return:
        """
        return True

    def is_recordable(self):
        """

        :return:
        """
        return True