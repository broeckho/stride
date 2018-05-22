import sys
import itertools

import pystride
from pystride.Event import EventType
from pystride.PyController import PyController

def printInfected(simulator, event):
    day = event.timestep
    print("Infected at day {}: {}".format(day, simulator.GetPopulation().GetInfectedCount()))
    sys.stdout.flush()

# Set the workspace (default = .)
pystride.workspace = "experiment"

# Configure simulation
controller = PyController("../config/run_default.xml")
controller.runConfig.setParameter("num_days", 2)

# Create forks with varying parameters
for r0, ir in list(itertools.product([10.0, 25.0], [0.4, 0.01])):
    # Create fork
    fork = controller.fork("r0_" + str(r0) + "-ir_" + str(ir))
    fork.runConfig.setParameter("r0", r0)
    fork.runConfig.setParameter("immunity_rate", ir)
    fork.registerCallback(printInfected, EventType.Stepped)

# Run all forks
controller.controlForks()
