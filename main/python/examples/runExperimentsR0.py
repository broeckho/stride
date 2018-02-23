##########################################################
# Script to obtain number of cases for various R0 values #
##########################################################

import csv
import os
import time

import pystride
from pystride.Simulation import Simulation

t_start = time.perf_counter()

# Configure simulation
simulation = Simulation()
simulation.loadRunConfig("../config/run_default.xml")
simulation.runConfig.setParameter("output_prefix", "R0Runs")

# Create forks with different values for R0
r0_values = [0, 4, 8, 12, 16]

for r0 in r0_values:
    # Create fork
    fork = simulation.fork("r0_" + str(r0))
    fork.runConfig.setParameter("r0", r0)

# Run all forks
simulation.runForks()

# Get attack rates
for r0 in r0_values:
    summary_file = os.path.join(pystride.workspace, "R0Runs", "r0_" + str(r0), "summary.csv")
    with open(summary_file) as csvfile:
        reader = csv.DictReader(csvfile)
        print("R0 = " + str(r0) + ": Num cases = " + str(next(reader)['num_cases']))

t_end = time.perf_counter()
print("Total time elapsed: " + str(round(t_end - t_start)) + " seconds")
