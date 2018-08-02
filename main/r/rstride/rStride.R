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
# 
# R controller for the Stride model
#
#############################################################################

# load R packages
library(XML,quietly = TRUE)
library(doParallel,quietly = TRUE)

# load general help functions
source('./bin/rstride/misc.R')

# load specific functions
source('./bin/rstride/ContactInspector.R')
source('./bin/rstride/LogParser.R')
source('./bin/rstride/TransmissionInspector.R')
source('./bin/rstride/SummaryInspector.R')


# Function to run rStride for a given design of experiment
run_rStride <- function(design_of_experiment = exp_design , dir_postfix = '',ignore_stride_stdout = TRUE)
{

  # command line message
  .rstride$cli_print('STARTING rSTRIDE CONTROLLER')
  
  ###############################
  ## PARALLEL SETUP            ##
  ###############################
  .rstride$start_slaves()
  
  
  ###############################
  ## GENERAL OPTIONS           ##
  ###############################
  stride_bin              <- './bin/stride'
  config_opt              <- '-c'
  config_default_filename <- './config/run_default.xml'
  output_dir              <- 'sim_output'
  
  ###############################
  ## RUN TAG AND DIRECTORY     ##
  ###############################
  
  # create run tag using the current time
  run_tag <- format(Sys.time(), format="%Y%m%d_%H%M%S")
  
  # add dir_postfix
  run_tag <- paste0(run_tag,dir_postfix)
  
  # create run directory
  run_dir <- file.path(output_dir,run_tag)
  # if it does not exist: create full path using the recursive option
  if(!file.exists(run_dir)){
    dir.create(run_dir, recursive =  TRUE)
  }
  
  # command line message
  .rstride$cli_print('PROJECT DIR',run_dir)
  
  ##################################
  ## GENERAL CONFIG MODIFICATIONS ##
  ##################################
  config_default                  <- xmlToList(config_default_filename)
  config_default$num_threads      <- 1
  config_default$vaccine_profile  <- 'None'
  config_default$vaccine_rate     <- 0
  config_default$immunity_profile <- 'None'
  config_default$immunity_rate    <- 0
  config_default$output_summary   <- 'true'
  
  ##################################
  ## RUN                          ##
  ##################################
  
  # command line message
  .rstride$cli_print('READY TO RUN',nrow(design_of_experiment),'EXPERIMENTS')
  
  # store local copy of slave1 pid
  pid_slave1 <- par_nodes_info$pid_slave1
  
  # run all experiments (in parallel)
  par_out <- foreach(i_exp=1:nrow(design_of_experiment),
                     .combine='rbind',
                     .packages='XML',
                     .verbose=FALSE) %dopar%
  {  

    # print progress (only the slave1)
    .rstride$print_progress(i_exp,nrow(design_of_experiment),pid_slave1)
   
    # create experiment tag
    exp_tag <- paste0('exp',sprintf("%04s", i_exp))
    
    # copy default param
    config_exp <-   config_default
    
    # add design parameters
    for(i_param in 1:ncol(design_of_experiment)){
      config_exp[names(design_of_experiment)[i_param]] <- design_of_experiment[i_exp,i_param]
    }  
  
    # update experiment output prefix
    config_exp$output_prefix <- file.path(run_dir,exp_tag)
    
    # create xml file
    config_exp_filename <- .rstride$save_config_xml(config_exp,'run',config_exp$output_prefix)
    
    # run stride (using the C++ CliController)
    system(paste(stride_bin,config_opt,paste0('../',config_exp_filename)),ignore.stdout=ignore_stride_stdout)
  
    # load output summary
    summary_filename <- file.path(config_exp$output_prefix,'summary.csv')
    run_summary      <- read.table(summary_filename,header=T,sep=',')
    
    # merge output summary with input param
    config_df   <- as.data.frame(config_exp)
    run_summary <- merge(run_summary,config_df)
  
    # parse contact_log (if present)
    contact_log_filename <- file.path(config_exp$output_prefix,'contact_log.txt')
    if(file.exists(contact_log_filename)){
      parse_contact_logfile(contact_log_filename)
    }
    
    # clean output folder: remove configuration, contact_log, summary and stride_log
    unlink(summary_filename,recursive = T)
    unlink(config_exp_filename,recursive = T)
    unlink(contact_log_filename,recursive = T)
    unlink(file.path(config_exp$output_prefix,'stride_log.txt'),recursive = T)
    
    # return experiment output summary
    return(run_summary)
  }
  
  # save overal summary
  write.table(par_out,file=file.path(run_dir,paste0(run_tag,'_summary.csv')),sep=',',row.names=F)
  
  
  ###############################
  ## TERMINATE PARALLEL NODES  ##
  ###############################
  .rstride$end_slaves()

  # command line message
  .rstride$cli_print('rSTRIDE CONTROLLER FINISHED')
  
  return(run_dir)
  
} # end run_rStride function
