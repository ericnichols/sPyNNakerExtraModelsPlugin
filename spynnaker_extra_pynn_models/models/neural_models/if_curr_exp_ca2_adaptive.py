"""
IFCurrentExponentialCa2AdaptivePopulation


Model from Liu, Y. H., & Wang, X. J. (2001). Spike-frequency adaptation of a
generalized leaky integrate-and-fire model neuron. Journal of Computational
Neuroscience, 10(1), 25-45. doi:10.1023/A:1008916026143
"""
import numpy

from spynnaker.pyNN.models.abstract_models.abstract_population_vertex import \
    AbstractPopulationVertex
from spynnaker.pyNN.models.abstract_models.abstract_model_components.\
    abstract_exp_population_vertex import AbstractExponentialPopulationVertex
from spynnaker.pyNN.models.abstract_models.abstract_model_components.\
    abstract_integrate_and_fire_properties \
    import AbstractIntegrateAndFireProperties
from spynnaker.pyNN.models.neural_properties.neural_parameter \
    import NeuronParameter
from spynnaker.pyNN.utilities import utility_calls

from data_specification.enums.data_type import DataType


class IFCurrentExponentialCa2AdaptivePopulation(
        AbstractExponentialPopulationVertex,
        AbstractIntegrateAndFireProperties,
        AbstractPopulationVertex):
    """
    IFCurrentExponentialCa2AdaptivePopulation: model which represents a
    leaky integrate and fire model with a exponential decay curve and based
    off current.
    """

    _model_based_max_atoms_per_core = 256

    # noinspection PyPep8Naming
    def __init__(self, n_neurons, machine_time_step, timescale_factor,
                 spikes_per_second, ring_buffer_sigma, constraints=None,
                 label=None, tau_m=20.0, cm=1.0, v_rest=-65.0, v_reset=-65.0,
                 v_thresh=-50.0, tau_syn_E=5.0, tau_syn_I=5.0, tau_refrac=0.1,
                 i_offset=0, tau_ca2=50.0, i_ca2=0.0, i_alpha=0.1,
                 v_init=None):
        # Instantiate the parent classes
        AbstractExponentialPopulationVertex.__init__(
            self, n_neurons=n_neurons, tau_syn_E=tau_syn_E,
            tau_syn_I=tau_syn_I, machine_time_step=machine_time_step)
        AbstractIntegrateAndFireProperties.__init__(
            self, atoms=n_neurons, cm=cm, tau_m=tau_m, i_offset=i_offset,
            v_init=v_init, v_reset=v_reset, v_rest=v_rest, v_thresh=v_thresh,
            tau_refrac=tau_refrac)
        AbstractPopulationVertex.__init__(
            self, n_neurons=n_neurons, n_params=12, label=label,
            binary="IF_curr_exp_ca2_adaptive.aplx", constraints=constraints,
            max_atoms_per_core=(IFCurrentExponentialCa2AdaptivePopulation
                                ._model_based_max_atoms_per_core),
            machine_time_step=machine_time_step,
            timescale_factor=timescale_factor,
            spikes_per_second=spikes_per_second,
            ring_buffer_sigma=ring_buffer_sigma)

        self._tau_ca2 = utility_calls.convert_param_to_numpy(
            tau_ca2, n_neurons)
        self._i_ca2 = utility_calls.convert_param_to_numpy(
            i_ca2, n_neurons)
        self._i_alpha = utility_calls.convert_param_to_numpy(
            i_alpha, n_neurons)

    def exp_tau_ca2(self, machine_time_step):
        return numpy.exp(float(-machine_time_step) / (1000.0 * self._tau_ca2))

    @property
    def model_name(self):
        """

        :return:
        """
        return "IF_curr_exp_ca2_adaptive"

    @staticmethod
    def set_model_max_atoms_per_core(new_value):
        """

        :param new_value:
        :return:
        """
        IFCurrentExponentialCa2AdaptivePopulation.\
            _model_based_max_atoms_per_core = new_value

    def get_cpu_usage_for_atoms(self, vertex_slice, graph):
        """

        :param vertex_slice:
        :param graph:
        :return:
        """
        return 781 * ((vertex_slice.hi_atom - vertex_slice.lo_atom) + 1)

    def get_parameters(self):
        return [

            # membrane voltage threshold at which neuron spikes [mV]
            # REAL    V_thresh;
            NeuronParameter(self._v_thresh, DataType.S1615),

            # post-spike reset membrane voltage [mV]
            # REAL    V_reset;
            NeuronParameter(self._v_reset, DataType.S1615),

            # membrane resting voltage [mV]
            # REAL    V_rest;
            NeuronParameter(self._v_rest, DataType.S1615),

            # membrane resistance
            # REAL    R_membrane;
            NeuronParameter(self._r_membrane, DataType.S1615),

            # membrane voltage [mV]
            # REAL    V_membrane;
            NeuronParameter(self._v_init, DataType.S1615),

            # offset current [nA]
            # REAL    I_offset;
            NeuronParameter(self.ioffset, DataType.S1615),

            # 'fixed' computation parameter - time constant multiplier for
            # closed-form solution
            # exp( -(machine time step in ms)/(R * C) ) [.]
            # REAL    exp_TC;
            NeuronParameter(self._exp_tc(self._machine_time_step),
                            DataType.S1615),

            # countdown to end of next refractory period  [timesteps]
            # int32_t refract_timer;
            NeuronParameter(self._refract_timer, DataType.INT32),

            # refractory time of neuron [timesteps]
            # int32_t T_refract
            NeuronParameter(self._tau_refract_timesteps(
                self._machine_time_step), DataType.INT32),

            # Calcium current
            # REAL    I_Ca2;
            NeuronParameter(self._i_ca2, DataType.S1615),

            # Influx of CA2 caused by each spike
            # REAL    I_alpha;
            NeuronParameter(self._i_alpha, DataType.S1615),

            # exp ( -(machine time step in ms)/(TauCa) )
            # REAL    exp_TauCa;
            NeuronParameter(self.exp_tau_ca2(self._machine_time_step),
                            DataType.S1615)
        ]

    def get_global_parameters(self):
        return []

    def is_population_vertex(self):
        """

        :return:
        """
        return True

    def is_integrate_and_fire_vertex(self):
        """

        :return:
        """
        return True

    def is_exp_vertex(self):
        """

        :return:
        """
        return True

    def is_recordable(self):
        """

        :return:
        """
        return True
