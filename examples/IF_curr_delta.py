"""
A single IF neuron with exponential, current-based synapses, fed by two
spike sources.

Run as:

$ python IF_curr_exp.py <simulator>

where <simulator> is 'neuron', 'nest', etc

Andrew Davison, UNIC, CNRS
September 2006

$Id$
"""

import pylab

import spynnaker.pyNN as sim
import spynnaker_extra_pynn_models as q

sim.setup(timestep=1.0, min_delay=1.0, max_delay=4.0)

delta_cell = sim.Population(1, q.IF_curr_delta,{'i_offset' :   0.1,  'tau_refrac' : 3.0,
                                     'v_thresh' : -51.0,  'v_reset'    : -70.0})

exp_cell = sim.Population(1, sim.IF_curr_exp,{'i_offset' :   0.1,  'tau_refrac' : 3.0,
                                     'v_thresh' : -51.0,  'v_reset'    : -70.0,
                                     'tau_syn_E': 5.0, 'tau_syn_I': 5.0})


spike_sourceE = sim.Population(1, sim.SpikeSourceArray, {'spike_times': [float(i) for i in range(5,105,10)]})
spike_sourceI = sim.Population(1, sim.SpikeSourceArray, {'spike_times': [float(i) for i in range(155,255,10)]})
 
sim.Projection(spike_sourceE, exp_cell, sim.OneToOneConnector(weights=1.5, delays=2.0), target='excitatory')
sim.Projection(spike_sourceI, exp_cell, sim.OneToOneConnector(weights=-1.5, delays=4.0), target='inhibitory')
sim.Projection(spike_sourceE, delta_cell, sim.OneToOneConnector(weights=1.5, delays=2.0), target='excitatory')
sim.Projection(spike_sourceI, delta_cell, sim.OneToOneConnector(weights=-1.5, delays=4.0), target='inhibitory')

delta_cell.record_gsyn()
exp_cell.record_gsyn()
delta_cell.record_v()
exp_cell.record_v()
delta_cell.record()
exp_cell.record()

sim.run(200.0)

v_delta = delta_cell.get_v()
i_delta = delta_cell.get_gsyn()
v_exp = exp_cell.get_v()
i_exp = exp_cell.get_gsyn()

# Plot
fig, axis = pylab.subplots(2)

axis[0].plot(v_delta[:,1], v_delta[:,2], label="Delta")
axis[0].plot(v_exp[:,1], v_exp[:,2], label="Exponential")
axis[0].set_title("Voltage")
axis[0].legend()

axis[1].plot(i_delta[:,1], i_delta[:,2], label="Delta")
axis[1].plot(i_exp[:,1], i_exp[:,2], label="Exponential")
axis[1].set_title("Current")
axis[1].legend()

sim.end()
pylab.show()
