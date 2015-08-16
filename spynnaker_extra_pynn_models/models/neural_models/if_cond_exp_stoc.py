"""
IFConductanceExponentialStochasticPopulation
"""
from spynnaker.pyNN.models.components.neuron_components.\
    abstract_population_vertex import \
    AbstractPopulationVertex
from data_specification.enums.data_type import DataType
from spynnaker.pyNN.models.components.\
    inputs_components.conductance_component import ConductanceComponent
from spynnaker.pyNN.models.neural_properties.neural_parameter \
    import NeuronParameter
from spynnaker.pyNN.models.components.\
    synapse_shape_components.exponential_component import ExponentialComponent
from spynnaker.pyNN.utilities import utility_calls

# extra models imports
from spynnaker_extra_pynn_models.models.components.model_component.\
    stocastic_intergrate_and_fire_component import \
    StocasticIntegrateAndFireComponent


class IFConductanceExponentialStochasticPopulation(
        ExponentialComponent, ConductanceComponent,
        StocasticIntegrateAndFireComponent, AbstractPopulationVertex):
    """
    a conductive exponental stochastic model.
    """

    _model_based_max_atoms_per_core = 60

    # noinspection PyPep8Naming
    def __init__(self, n_neurons, machine_time_step, timescale_factor,
                 spikes_per_second, ring_buffer_sigma, constraints=None,
                 label=None, tau_m=20., cm=1.0, e_rev_E=0.0, e_rev_I=-70.0,
                 v_rest=-65.0, v_reset=-65.0, tau_syn_E=5.0, tau_syn_I=5.0,
                 tau_refrac=0.1, v_thresh=-50.0, du_th=0.5, tau_th=20.0,
                 i_offset=0.0, v_init=-65.0):
        # Instantiate the parent classes
        ConductanceComponent.__init__(
            self, n_neurons, e_rev_E=e_rev_E, e_rev_I=e_rev_I)
        ExponentialComponent.__init__(
            self, n_neurons=n_neurons, tau_syn_E=tau_syn_E,
            tau_syn_I=tau_syn_I, machine_time_step=machine_time_step)
        StocasticIntegrateAndFireComponent.__init__(
            self, atoms=n_neurons, cm=cm, tau_m=tau_m, i_offset=i_offset,
            v_init=v_init, v_reset=v_reset, v_rest=v_rest, v_thresh=v_thresh,
            tau_refrac=tau_refrac, du_th=du_th, tau_th=tau_th, theta=v_thresh)

        AbstractPopulationVertex.__init__(
            self, n_neurons=n_neurons, n_params=14, label=label,
            max_atoms_per_core=
            IFConductanceExponentialStochasticPopulation.
            _model_based_max_atoms_per_core,
            binary="IF_cond_exp_stoc.aplx", constraints=constraints,
            machine_time_step=machine_time_step,
            timescale_factor=timescale_factor,
            spikes_per_second=spikes_per_second,
            ring_buffer_sigma=ring_buffer_sigma)

        self._theta = v_thresh
        self._du_th_inv = 1. / du_th
        self._tau_th_inv = 1. / tau_th

    @property
    def model_name(self):
        """
        human readable name for the model
        :return:
        """
        return "IF_cond_exp_stoc"

    @staticmethod
    def set_model_max_atoms_per_core(new_value):
        """
        sets the max atoms per core for this model type
        :param new_value: the new max atoms per core
        :return: none
        """
        IFConductanceExponentialStochasticPopulation.\
            _model_based_max_atoms_per_core = new_value

    def get_cpu_usage_for_atoms(self, vertex_slice, graph):
        """
        Gets the CPU requirements for a range of atoms
        :param vertex_slice: the slice of the parttiionabvle vertex that this
         partitioned vertex needs to implment
        :param graph: the partitionable graph
        """
        utility_calls.unused(vertex_slice)
        utility_calls.unused(graph)
        return 781 * ((vertex_slice.hi_atom - vertex_slice.lo_atom) + 1)

    def get_parameters(self):

        return [
            # membrane voltage threshold at which neuron spikes [mV]
            # REAL     V_thresh;
            NeuronParameter(self._v_thresh, DataType.S1615),

            # post-spike reset membrane voltage [mV]
            # REAL     V_reset;
            NeuronParameter(self._v_reset, DataType.S1615),

            # membrane resting voltage [mV]
            # REAL     V_rest;
            NeuronParameter(self._v_rest, DataType.S1615),

            # membrane resistance
            # REAL     R_membrane;
            NeuronParameter(self._r_membrane, DataType.S1615),

            # membrane voltage [mV]
            # REAL     V_membrane;
            NeuronParameter(self._v_init, DataType.S1615),

            # offset current [nA]
            # REAL     I_offset;
            NeuronParameter(self.ioffset, DataType.S1615),

            # 'fixed' computation parameter - time constant multiplier for
            # closed-form solution
            # exp( -(machine time step in ms)/(R * C) ) [.]
            # REAL     exp_TC;
            NeuronParameter(self._exp_tc(self._machine_time_step),
                            DataType.S1615),

            # countdown to end of next refractory period [timesteps]
            # int32_t  refract_timer;
            NeuronParameter(self._refract_timer, DataType.INT32),

            # refractory time of neuron [timesteps]
            # int32_t  T_refract;
            NeuronParameter(self._tau_refract_timesteps(
                self._machine_time_step), DataType.INT32),

            # sensitivity of soft threshold to membrane voltage [mV^(-1)]
            # (inverted in python code)
            NeuronParameter(self._du_th_inv, DataType.S1615),

            # time constant for soft threshold [ms^(-1)]
            # (inverted in python code)
            NeuronParameter(self._tau_th_inv, DataType.S1615),

            # soft threshold value [mV]
            NeuronParameter(self._theta, DataType.S1615)]

    def get_global_parameters(self):
        return [
            NeuronParameter(self._machine_time_step / 100.0, DataType.S1615)
        ]

    def is_conductance(self):
        """
        helper method for isinstance
        :return:
        """
        return True

    def is_exp_vertex(self):
        """
        helper method for isinstance
        :return:
        """
        return True

    def is_integrate_and_fire_vertex(self):
        """
        helper method for isinstance
        :return:
        """
        return True

    def is_population_vertex(self):
        """
        helper method for isinstance
        :return:
        """
        return True

    def is_recordable(self):
        """
        helper method for isinstance
        :return:
        """
        return True
