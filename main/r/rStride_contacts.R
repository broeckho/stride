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
# E.g.: path/to/stride-557 $ ./bin/rStride_contacts.R 
#
#############################################################################

rm(list=ls())

# temporary
setwd('/Users/lwillem/opt/stride-559+')

# load rStride
source('./bin/rstride/rStride.R')

##################################
## DESIGN OF EXPERIMENTS        ##
##################################

# uncomment the following line to inspect the config xml tags
#names(xmlToList('./config/run_default.xml'))

# set the number of realisations per configuration set
num_seeds  <- 1

# add parameters and values to combine in a full-factorial grid
exp_design <- expand.grid(contact_log_level = "All",
                          num_days = 1,
                          seeding_rate = 1e-5,
                          num_participants_survey = 3000,
                          start_date = c("2017-01-01","2017-01-02"),
                          rng_seed = 1:num_seeds,
                          stringsAsFactors = F)

# add a unique seed for each run
set.seed(125)
exp_design$rng_seed <- sample(1e4,nrow(exp_design))

##################################
## RUN rSTRIDE                  ##
##################################
project_dir <- run_rStride(exp_design)

#####################################################
## PARSE LOGFILES & EXPLORE SOCIAL CONTACTS        ##
#####################################################

#project_dir <- './sim_output/20180730_113633'

project_output           <- file.path(project_dir,dir(project_dir))
exp_dirs                 <- project_output[grepl('exp',project_output)]
project_summary_filename <- project_output[grepl('_summary.csv',project_output)]

# load summary
project_summary <- read.table(project_summary_filename,sep=',',header=T)

i_exp <- 1
for(i_exp in 1:nrow(project_summary)){
  
  # parse logfile
  parse_contact_logfile(project_summary$output_prefix[i_exp])
  
  # plot contacts
  plot_contacts(project_summary[i_exp,],'./data')
}




