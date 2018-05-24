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
import os
import csv
import xml.etree.ElementTree as ET

import pystride
from pystride.Event import EventType

# Callback function to create a summary file with input and output details
def getSummary(simulator, event):
    # Get simulator details
    num_cases = simulator.GetPopulation().GetInfectedCount()
    population_size = simulator.GetPopulation().size()
    transmission_rate = simulator.GetTransmissionRate()
    output_prefix = simulator.GetConfigValue("run.output_prefix")

    # Parse the config file
    tree = ET.parse(os.path.join(output_prefix,"config.xml"))
    root = tree.getroot()

    # Add input and output values and names to a list
    header_list = ["output_prefix","population_size", "num_cases","transmission_rate"]
    values_list = [output_prefix,population_size, num_cases, transmission_rate]
    for child in root:
        header_list.append(child.tag)
        values_list.append(child.text)
    
    # Write input and output to summary file
    summary_filename = os.path.join(output_prefix,"summary.csv")
    with open(summary_filename, "w") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(header_list)
        writer.writerow(values_list)

# Callback function to create a summary file with input and output details
def getCases(simulator, event):
    # Get simulator details
    num_cases = simulator.GetPopulation().GetInfectedCount()
    time_step = event.timestep
    output_prefix = simulator.GetConfigValue("run.output_prefix")
    
    cases_filename = os.path.join(output_prefix,"cases.csv")
    with open(cases_filename, "a",newline="") as csvfile:
        writer = csv.writer(csvfile)
        if time_step == 0:
            writer.writerow([output_prefix])
        writer.writerow({num_cases})
        
        
        
        