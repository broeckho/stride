import pystride
from pystride.Event import EventType
from pystride.PyController import PyController

def vaccinate(simulator, event):
    if event.timestep == 20:
        pop = simulator.GetPopulation()
        for pIndex in range(pop.size()):
            if pop[pIndex].GetHealth().IsSusceptible():
                pop[pIndex].GetHealth().SetImmune()

def check(simulator, event):
    if event.timestep == 20:
        check.value = getattr(check, 'value', 0)
        check.value = sim.GetPopulation().GetInfectedCount()
    if event.timestep > 20:
        assert(sim.GetPopulation().GetInfectedCount() == check.value), "Infection count still changing on day {}".format(event.timestep)

# Configure simulation
sim = PyController(config_path="../config/run_default.xml")
# Register callbacks
sim.registerCallback(vaccinate, EventType.Stepped)
sim.registerCallback(check, EventType.Stepped)

sim.run()
