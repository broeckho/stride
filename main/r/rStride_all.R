#!/usr/bin/env Rscript
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
#
#  Copyright 2018, Willem L, Kuylen E & Broeckhove J
#############################################################################
#
# Call this script from the main project folder (containing bin, config, lib, ...)
# to get all relative data links right. 
#
# E.g.: path/to/stride $ ./bin/rStride_all.R 
#
#############################################################################

# Clear work environment
rm(list=ls())

# load rStride
source('./bin/rstride/rStride.R')

# list all rStride scripts
script_opt <- list.files('./bin',pattern='rStride_',full.names = T)

# remove current '_all' script
script_opt <- script_opt[!grepl('rStride_all',script_opt)]
#script_opt <- script_opt[grepl('rStride_expl',script_opt)]



# run all rStride scripts
for(script_i in script_opt){
  .rstride$cli_print('---------------------------------------')
  # print script name
  .rstride$cli_print('RUN',script_i)
  
  # run script
  system(script_i,ignore.stdout=FALSE)
  
  # print script name
  .rstride$cli_print('CLOSE',script_i)
  .rstride$cli_print('---------------------------------------')
  
  
}




