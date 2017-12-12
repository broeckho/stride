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
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#############################################################################

"""
Create an age-based contact matrix in XML-format.

Author: Elise Kuylen and Lander Willem (2017)
"""

import sys
import csv
import xml.etree.cElementTree as ET

def createFromCSV(household_file, work_file, school_file, prim_community_file, sec_community_file, postfix="", directory="."):
    """
        Create contact matrices from files with diary-study results.
        """
    root = ET.Element("matrices")
    cluster_types = ["household", "primary_community", "work", "school", "secondary_community"]
    for cluster_type in cluster_types:
        cluster_root = ET.SubElement(root, cluster_type)
        cluster_file = None
        if cluster_type == "household":
            cluster_file = household_file
        elif cluster_type == "work":
            cluster_file = work_file
        elif cluster_type == "school":
            cluster_file = school_file
        elif cluster_type == "primary_community":
            cluster_file = prim_community_file
        else:
            cluster_file = sec_community_file
    
        with open(cluster_file, 'r') as f:
            part_age = 0
            
            reader = csv.DictReader(f, delimiter=';')
            for row in reader:
                # make participant element
                participant = ET.SubElement(cluster_root, "participant")
                ET.SubElement(participant, "age").text = str(part_age)
                contacts = ET.SubElement(participant, "contacts")
                for cnt_age in range(0, 81):
                    # make contact element
                    contact = ET.SubElement(contacts, "contact")
                    ET.SubElement(contact, "age").text = str(cnt_age)
                    # get number of contacts
                    key = "age" + str(cnt_age)
                    rate = float(row[key].replace(',','.'))
                    
                    ET.SubElement(contact, "rate").text = str(rate)
                part_age += 1

    tree = ET.ElementTree(root)
    output_file = directory + "/contact_matrix_" + postfix + ".xml";
    tree.write(output_file)
    print "Complete: " + output_file ;


def main(argv):
    print ""
    if len(argv) == 5:
        print "Creating contact matrix from CSV files using default name and directory."
        createFromCSV(argv[0], argv[1], argv[2], argv[3], argv[4])
    elif len(argv) == 6:
        print "Creating contact matrix from CSV files using default directory."
        createFromCSV(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5])
    elif len(argv) == 7:
        print "Creating contact matrix from CSV files."
        createFromCSV(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6])
    else:
        print "!! ERROR: Program requires 5 input files: household, work, school, primary_community, secondary_community, POSTFIX, DIRECTORY"
    print ""
if __name__ == "__main__":
    main(sys.argv[1:])
