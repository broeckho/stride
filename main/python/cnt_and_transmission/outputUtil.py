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


# Callback function to parse the logfile
"""
    From logfile with all contacts or transmissions logged in the following format:
    [PART] local_id part_age part_gender
    [CONT] local_id part_age cnt_age cnt_home cnt_school cnt_work cnt_other sim_day
    [TRAN] local_id start_infection

    Create csv-files participants.csv, contacts.csv and transmissions.csv
    """
def parseLogfile(simulator, event):

    # get output prefix
    log_file_path = simulator.GetConfigValue("run.output_prefix")

    # setup ouput files
    participants_file = os.path.join(log_file_path, 'participants.csv')
    contacts_file     = os.path.join(log_file_path, 'contacts.csv')
    transmission_file = os.path.join(log_file_path, 'transmissions.csv')

    # Open csv files to write to.
    p = open(participants_file, 'w')
    c = open(contacts_file, 'w')
    t = open(transmission_file,'w')

    # Write headers for csv files
    p_fieldnames = ['local_id', 'part_age', 'part_gender', 'school_id', 'workplace_id']
    p_writer = csv.DictWriter(p, fieldnames=p_fieldnames)
    p_writer.writeheader()

    c_fieldnames = ['local_id', 'part_age', 'cnt_age', 'cnt_home', 'cnt_school', 'cnt_work', 'cnt_prim_comm', 'cnt_sec_comm', 'sim_day']
    c_writer = csv.DictWriter(c, fieldnames=c_fieldnames)
    c_writer.writeheader()

    t_fieldnames  = ['local_id', 'new_infected_id', 'cnt_location','sim_day']
    t_writer = csv.DictWriter(t, fieldnames=t_fieldnames)
    t_writer.writeheader()

    flag_p = 0
    flag_c = 0
    flag_t = 0

    f = open(os.path.join(log_file_path, 'contact_log.txt'),'r',10)
    for line in f:

        identifier = line[:6]
        line = line[7:]
        line = line.split()

        # participant details
        if identifier == "[PART]":
            flag_p = 1
            dic = {}
            for i in range(len(p_fieldnames)):
                value = line[i]
                dic[p_fieldnames[i]] = value
            p_writer.writerow(dic)
        # contact details
        if identifier == "[CONT]":
            flag_c = 1
            dic = {}
            for i in range(len(c_fieldnames)):
                value = line[i]
                dic[c_fieldnames[i]] = value
            c_writer.writerow(dic)

        # transmission details
        if identifier == "[TRAN]" or identifier == "[PRIM]":
            flag_t = 1
            dic = {}
            for i in range(len(t_fieldnames)):
                value = line[i]
                dic[t_fieldnames[i]] = value
            t_writer.writerow(dic)

    # if no log info on partipants, contacts or transmission present, delete that file
    if(flag_p==0):
        os.remove(participants_file)
    if(flag_c==0):
        os.remove(contacts_file)
    if(flag_t==0):
        os.remove(transmission_file)

    #os.remove(log_file_path+'_logfile.txt')
    f.close()
    p.close()
    c.close()
    t.close()
