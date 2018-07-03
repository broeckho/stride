#!/usr/bin/python

#############################################################################
#  This file is part of the Stride software.
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or any
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2018, Willem L, Kuylen E & Broeckhove J
#############################################################################

import sys
import itertools
import os

import pystride
from pystride.Event import EventType
from pystride.PyController import PyController

# callback functions
from outputUtil import getSummary, getCases, parseLogfile

# Set the workspace (default = .)
pystride.workspace = "sim_output"

# Configure simulation
controller = PyController(config_path = "./config/run_default.xml",data_dir = "./data")
controller.runConfig.setParameter("vaccine_profile", "None")
controller.runConfig.setParameter("num_days", 18)
controller.runConfig.setParameter("num_participants_survey", 10)
controller.runConfig.setParameter("contact_log_level", "Transmissions")
controller.runConfig.setParameter("track_index_case", "true")
controller.runConfig.setParameter("seeding_rate", "0.000002")
controller.runConfig.setParameter("output_summary", "true")


# Design of Experiement: parameter values
rng_opt = range(1)
r0_opt = [14]

# Create forks with varying parameters
f_id = 0 # fork id
for r0, rng_seed in list(itertools.product(r0_opt,rng_opt)):
    # Create fork
    fork = controller.fork("exp" + str(f_id))
    fork.runConfig.setParameter("rng_seed", f_id)
    fork.runConfig.setParameter("r0", r0)
    fork.registerCallback(getCases, EventType.Stepped)
    fork.registerCallback(getSummary, EventType.AtFinished)
    fork.registerCallback(parseLogfile, EventType.AtFinished)

    f_id = f_id + 1


# Run all forks
controller.controlForks()



