import pystride
from pystride.PyController import PyController

# Configure simulation
sim = PyController(config_path="../config/run_default.xml")
# Run the simulation
sim.run()
