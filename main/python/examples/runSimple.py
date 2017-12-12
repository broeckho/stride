from pystride.Simulation import Simulation

# Load run configuration
simulation = Simulation()
simulation.loadRunConfig("config/run_default.xml")

# Run the simulation
simulation.run()
