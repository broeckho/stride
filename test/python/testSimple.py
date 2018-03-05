#############################################
#           Test Python environment         #
#############################################
import csv
import os
from shutil import rmtree

import pystride
from pystride.Simulation import Simulation

# Configure simulation
simulation = Simulation()
simulation.loadRunConfig("../config/run_default.xml")
simulation.runConfig.setParameter("output_prefix", "testSimple")
simulation.runConfig.setParameter("use_install_dirs", "true")

# Clean up leftover of previous failed  testrun
if os.path.isdir("testSimple"):
    rmtree("testSimple")

# Run the simulation
simulation.run()
# Check output
targetCases = 106958
summary_file = os.path.join("testSimple", "summary.csv")
with open(summary_file) as csvfile:
    reader = csv.DictReader(csvfile)
    actualCases = int((next(reader))['num_cases'])
    # Accept a 10% margin of error
    assert (abs(targetCases - actualCases) <= (targetCases * 0.1)),"Expected value is {} - actual value is {}".format(targetCases, actualCases)
    
# Clean up
rmtree("testSimple")
