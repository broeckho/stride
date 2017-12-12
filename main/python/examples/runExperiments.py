import itertools

import pystride
from pystride.Simulation import Simulation

# Set the workspace (default = .)
pystride.workspace = "simulations"

# Configure simulation
simulation = Simulation()
simulation.loadRunConfig("config/run_default.xml")
simulation.runConfig.setParameter("num_days", 10)

# Create forks with varying parameters
for b0, ir in list(itertools.product([14, 10, 5], [0.2, 0.4, 0.6])):
    # Create fork
    fork = simulation.fork("b0_" + str(b0) + "-ir_" + str(ir))
    fork.diseaseConfig.setParameter("transmission/b0", b0)
    fork.runConfig.setParameter("immunity_rate", ir)

    fork.runConfig.setParameter("r0", 15)

# Run all forks
simulation.runForks()
