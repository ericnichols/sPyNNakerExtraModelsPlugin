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

# extra models imports
from spynnaker_extra_pynn_models.neural_models.\
    stocastic_intergrate_and_fire_component import \
    StocasticIntegrateAndFireComponent


class IFConductanceExponentialStochasticPopulation(
        ExponentialComponent, ConductanceComponent,
        StocasticIntegrateAndFireComponent, AbstractPopulationVertex):

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
        self._executable_constant = \
            IFConductanceExponentialStochasticPopulation.CORE_APP_IDENTIFIER

    @property
    def model_name(self):
        return "IF_cond_exp_stoc"

    @staticmethod
    def set_model_max_atoms_per_core(new_value):
        IFConductanceExponentialStochasticPopulation.\
            _model_based_max_atoms_per_core = new_value

    def get_cpu_usage_for_atoms(self, vertex_slice, graph):
        """
        Gets the CPU requirements for a range of atoms
        """
        return 781 * ((vertex_slice.hi_atom - vertex_slice.lo_atom) + 1)

    def get_parameters(self):
        """
        Generate Neuron Parameter data (region 2):
        """

        # typedef struct neuron_t {
        #
        #     // membrane voltage threshold at which neuron spikes [mV]
        #     REAL     V_thresh;
        #
        #     // post-spike reset membrane voltage [mV]
        #     REAL     V_reset;
        #
        #     // membrane resting voltage [mV]
        #     REAL     V_rest;
        #
        #     // membrane resistance [some multiplier of Ohms, TBD probably
        #     // MegaOhm]
        #     REAL     R_membrane;
        #
        #     // reversal voltage - Excitatory [mV]
        #     REAL     V_rev_E;
        #
        #     // reversal voltage - Inhibitory [mV]
        #     REAL     V_rev_I;
        #
        #     // membrane voltage [mV]
        #     REAL     V_membrane;
        #
        #     // offset current [nA] but take care because actually 'per
        #     // timestep charge'
        #     REAL     I_offset;
        #
        #     // 'fixed' computation parameter - time constant multiplier for
        #     // closed-form solution
        #     // exp( -(machine time step in ms)/(R * C) ) [.]
        #     REAL     exp_TC;
        #
        #     // [kHz!] only necessary if one wants to use ODE solver because
        #     //  allows
        #     // multiply and host double prec to calc
        #     // - UNSIGNED ACCUM & unsigned fract much slower
        #     REAL     one_over_tauRC;
        #
        #     // countdown to end of next refractory period [ms/10]
        #     // - 3 secs limit do we need more? Jan 2014
        #     int32_t  refract_timer;
        #
        #     // refractory time of neuron [ms/10]
        #     int32_t  T_refract;
        #
        # #ifdef SIMPLE_COMBINED_GRANULARITY
        #
        #     // store the 3 internal timestep to avoid granularity
        #     REAL     eTC[3];
        # #endif
        # #ifdef CORRECT_FOR_THRESHOLD_GRANULARITY
        #
        #     // which period previous spike happened to approximate threshold
        #     // crossing
        #     uint8_t prev_spike_code;
        #
        #     // store the 3 internal timestep to avoid granularity
        #     REAL     eTC[3];
        # #endif
        # #ifdef CORRECT_FOR_REFRACTORY_GRANULARITY
        #
        #     // approx corrections for release from refractory period
        #     uint8_t  ref_divisions[2];
        #
        #     // store the 3 internal timestep to avoid granularity
        #     REAL     eTC[3];
        # #endif
        #
        # } neuron_t;
        return [
            NeuronParameter(self._v_reset, DataType.S1615),
            NeuronParameter(self._v_rest, DataType.S1615),
            NeuronParameter(self.r_membrane(self._machine_time_step),
                            DataType.S1615),
            NeuronParameter(self._e_rev_E, DataType.S1615),
            NeuronParameter(self._e_rev_I, DataType.S1615),
            NeuronParameter(self._du_th_inv, DataType.S1615),
            NeuronParameter(self._tau_th_inv, DataType.S1615),
            NeuronParameter(self._theta, DataType.S1615),
            NeuronParameter(self._v_init, DataType.S1615),
            NeuronParameter(self.ioffset(self._machine_time_step),
                            DataType.S1615),
            NeuronParameter(self.exp_tc(self._machine_time_step),
                            DataType.S1615),
            NeuronParameter(self._one_over_tau_rc, DataType.S1615),
            NeuronParameter(self._refract_timer, DataType.UINT32),
            NeuronParameter(self._scaled_t_refract(), DataType.UINT32),
        ]

    def is_conductive(self):
        return True

    def is_exp_vertex(self):
        return True

    def is_integrate_and_fire_vertex(self):
        return True

    def is_population_vertex(self):
        return True

    def is_recordable(self):
        return True
