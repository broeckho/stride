#############################################
#           Test Python environment         #
#############################################

import os
from shutil import rmtree

import pystride
from pystride.PyController import PyController

# Configure simulation
sim = PyController("../config/run_default.xml")
sim.runConfig.setParameter("output_prefix", "testSimple")
sim.runConfig.setParameter("use_install_dirs", "true")

print(sim.runConfig.getParameter("disease_config_file"))

# Clean up leftover of previous (failed) testrun
if os.path.isdir("testSimple"):
    rmtree("testSimple")

# Run the simulation
sim.run()

'''
# Check results
target_cases = 118925

# TODO callback function that returns # of infected + recovered at end of simulation
'''
'''
summary_file = os.path.join("testSimple", "summary.csv")
with open(summary_file) as csvfile:
    reader = csv.DictReader(csvfile)
    actualCases = int((next(reader))['num_cases'])
    # Accept a 10% margin of error
    assert (abs(targetCases - actualCases) <= (targetCases * 0.1)),"Expected value is {} - actual value is {}".format(targetCases, actualCases)
'''
