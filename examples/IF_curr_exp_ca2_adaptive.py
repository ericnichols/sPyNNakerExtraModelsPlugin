import math
import numpy
import pylab

import spynnaker.pyNN as sim
import spynnaker_extra_pynn_models as q

dt = 0.1
N = 300
T = 250

sim.setup(timestep=dt, min_delay=1.0, max_delay=4.0)

cell = sim.Population(N, q.IF_curr_exp_ca2_adaptive,
                      {"tau_m":20.0, "cm":0.5,
                       "v_rest":-70.0, "v_reset":-60.0, "v_thresh":-54.0,
                       "i_alpha":0.1, "tau_ca2":50.0})


spike_source = sim.Population(N, sim.SpikeSourcePoisson, {"rate": 2500.0})

sim.Projection(spike_source, cell, sim.OneToOneConnector(weights=0.1, delays=0.1), target="excitatory")

cell.record()
cell.record_gsyn()

sim.run(T)

spike_times = cell.getSpikes(compatible_output=True)
ca2 = cell.get_gsyn(compatible_output=True)

# Split into list of spike times for each neuron
neuron_spikes = [spike_times[spike_times[:,0] == n,1] for n in range(N)]

# Calculate isis and pair these with bin index of first spike time in pair
neuron_binned_isis = numpy.hstack([
    numpy.vstack(
        (t[1:] - t[:-1],
         numpy.digitize(t[:-1], numpy.arange(T))))
        for t in neuron_spikes])

time_binned_isis = [neuron_binned_isis[0, neuron_binned_isis[1] == t] for t in range(T)]
mean_isis = {t: numpy.average(i)
             for (t, i) in enumerate(time_binned_isis) if len(i) > 0}

isi_variance = {t: math.sqrt(numpy.sum(numpy.power(time_binned_isis[t] - mean_isi, 2))) / mean_isi
                for (t, mean_isi) in mean_isis.iteritems()}

# Take average CA2 level across all neurons
average_ca2 = numpy.average(numpy.reshape(ca2[:,2], (N, int(T / dt))), axis=0)

# Plot
fig, axes = pylab.subplots(4, sharex=True)

axes[0].scatter(spike_times[:,1], spike_times[:,0],  s=2)
axes[0].set_ylabel("Neuron ID")

axes[1].scatter(list(mean_isis.iterkeys()), [1000.0 / i for i in mean_isis.itervalues()], s=2)
axes[1].set_ylabel("Firing rate/Hz")

axes[2].scatter(numpy.arange(0.0, T, dt), average_ca2, s=2)
axes[2].set_ylabel("CA2")
axes[2].set_ylim((0.0, numpy.amax(average_ca2) * 1.25))

axes[3].scatter(list(isi_variance.iterkeys()), list(isi_variance.itervalues()), s=2)
axes[3].set_ylabel("Coefficient of variance")

axes[3].set_xlim((0.0, T))
axes[3].set_xlabel("Time/ms")

sim.end()
pylab.show()

