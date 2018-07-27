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
# E.g.: path/to/stride-557 $ ./lib/explore_rStride.R 
#
#############################################################################

# load rStride
source('./lib/rStride.R')

##################################
## DESIGN OF EXPERIMENTS        ##
##################################

num_seeds <- 2
exp_design <- expand.grid(r0       = 14:16,
                          num_days = c(20,30),
                          rng_seed = 1:num_seeds)

# add unique seed for each run
set.seed(125)
exp_design$rng_seed <- sample(1e4,nrow(exp_design))

##################################
## RUN rSTRIDE                  ##
##################################
run_rStride(exp_design)









