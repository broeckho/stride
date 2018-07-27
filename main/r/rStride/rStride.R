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

run_rStride <- function(design_of_experiment)
{
  
  # command line message
  print(paste('[',Sys.time(),'] START rSTRIDE'))
  
  ###############################
  ## HELP FUNCIONS             ##
  ###############################
  
  # convert a list into XML format
  listToXML <- function(node, sublist){
    for(i in 1:length(sublist)){
      child <- newXMLNode(names(sublist)[i], parent=node);
      
      if (typeof(sublist[[i]]) == "list"){
        listToXML(child, sublist[[i]])
      }
      else{
        xmlValue(child) <- sublist[[i]]
      }
    } 
  }
  
  # save config file
  save_config_xml <- function(list_config,output_prefix){
    root <- newXMLNode("run")
    listToXML(root, list_config)
    filename <- paste0(output_prefix,'.xml')
    saveXML(root,file=filename)
    #return
    return(filename)
  }
  
  ###############################
  ## PARALLEL SETUP            ##
  ###############################
  
  ## SETUP PARALLEL NODES
  # note: they will be removed after 280 seconds inactivity
  num_proc <- detectCores()
  cl <- makeCluster(num_proc,cores=num_proc, timeout = 280)
  registerDoParallel(cl)
  
  # get process id of first node (to produce a status bar)
  cl_pid <- clusterEvalQ(cl, { Sys.getpid() })[[1]]
  
  
  ###############################
  ## GENERAL OPTIONS           ##
  ###############################
  stride_bin <- './bin/stride'
  config_opt <- '-c'
  config_filename <- './config/run_default.xml'
  output_dir <- 'sim_output'
  
  ##################################
  ## GENERAL CONFIG MODIFICATIONS ##
  ##################################
  config_default                  <- xmlToList(config_filename)
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
  print(paste('[',Sys.time(),'] READY TO RUN ',nrow(design_of_experiment),' EXPERIMENTS'))
  
  # create run tag using the current time
  run_tag <- format(Sys.time(), format="%Y%m%d_%H%M%S")
  
  # create run directory
  run_dir <- file.path(output_dir,run_tag)
  # if it does not exist: create full path using the recursive option
  if(!file.exists(run_dir)){
    dir.create(run_dir, recursive =  TRUE)
  }
  
  # run all experiments (in parallel)
  par_out <- foreach(i_exp=1:nrow(design_of_experiment),.combine='rbind',.packages='XML',.verbose=FALSE) %dopar%
  {  
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
    config_exp_filename <- save_config_xml(config_exp,config_exp$output_prefix)
    
    # run stride (using the C++ CliController)
    system(paste(stride_bin,config_opt,paste0('../',config_exp_filename)),ignore.stdout=T)
  
    # load output summary
    summary_filename <- file.path(config_exp$output_prefix,'summary.csv')
    run_summary <- read.table(summary_filename,header=T,sep=',')
    
    # merge output with configurations
    config_df <- as.data.frame(config_exp)
    run_summary <- merge(run_summary,config_df)
  
    # clean output folder: remove local summary, configuration and stride_log file
    unlink(summary_filename,recursive = T)
    unlink(config_exp_filename,recursive = T)
    unlink(file.path(config_exp$output_prefix,'stride_log.txt'),recursive = T)
    
    # return experiment output
    return(run_summary)
  }
  
  # save total summary
  write.table(par_out,file=file.path(run_dir,paste0(run_tag,'_summary.csv')),sep=',',row.names=F)
  
  
  ###############################
  ## TERMINATE PARALLEL NODES  ##
  ###############################
  
  # close nodes
  if(exists('cl')){
    stopCluster(cl); rm(cl)
  }

  # command line message
  print(paste('[',Sys.time(),'] rSTRIDE FINISHED'))
  
} # end run_rStride function
