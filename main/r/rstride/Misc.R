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
# HELP FUNCTIONS FOR rSTRIDE PRE- AND POST-PROCESSING                       
#
#############################################################################

#.rstride$set_wd()  #DEVELOPMENT: to set the last install dir as work directory 
#.rstride$load_pd() #DEVELOPMENT: to retrieve the last project_dir

# load R packages
library(XML,quietly = TRUE)
library(doParallel,quietly = TRUE)

if(!(exists('.rstride'))){
  .rstride <- new.env()
}

###############################
## COMMAND LINE MESSAGES     ##
###############################

.rstride$cli_print <- function(...) {

  # get function arguments
  function_arguments <- as.list(match.call(expand.dots=FALSE))$...
  
  # get function-call environment (to retrieve variable from that environment) 
  pf <- parent.frame()

  # parse list => make character vector
  function_arguments <- foreach(i=1:length(function_arguments),.combine='cbind') %do%{
    eval(unlist(function_arguments[[i]]),envir = pf)
  }
 
  # print time + arguments
  cli_out <- paste(c('echo', '[',format(Sys.time()),']',function_arguments),collapse = ' ')
  system(cli_out)
  
}

###############################
## PARALLEL START & END      ##
###############################

.rstride$start_slaves <- function(.env = .RstrideEnv)
{
  ## SETUP PARALLEL NODES
  # note: they will be removed after 280 seconds inactivity
  num_proc <- detectCores()
  par_cluster   <- makeForkCluster(num_proc, cores=num_proc, timeout = 280) 
  registerDoParallel(par_cluster)
  
  # store the process id (pid) of the first slave
  pid_slave1 <- clusterEvalQ(par_cluster, { Sys.getpid() })[[1]]
  
  # CREATE GLOBAL VARIABLE
  par_nodes_info <<- list(par_cluster = par_cluster,
                          pid_slave1 = pid_slave1)

  }

.rstride$end_slaves <- function()
{
  ## CLOSE NODES AND NODE INFO
  if(exists('par_nodes_info')){
    stopCluster(par_nodes_info$par_cluster); 
    rm(par_nodes_info,envir = .GlobalEnv) # REMOVE GLOBAL VARIABLE
  }
  
}

.rstride$print_progress <- function(i_current,i_total,pid_slave1){
  
   if(Sys.getpid() == pid_slave1){
     .rstride$cli_print('RUNNING...',i_current,'/',i_total)
   }
  
}


###############################
## PROJECT SUMMARY           ##
###############################

.rstride$load_project_summary <- function(project_dir){
 
  # get the filename in the project dir that contains "summary.csv" (full path)
  project_summary_filename <- file.path(project_dir,dir(project_dir,pattern = 'summary.csv'))
  
  # read the csv file
  project_summary          <- read.table(project_summary_filename,sep=',',header=T,stringsAsFactors = F)
  
  # return the data.frame
  return(project_summary)
}


###############################
## XML FUNCTIONS             ##
###############################

# Convert a list into XML format [RECURSIVE FUNCTION]
# counterpart of "XML::xmlToList" function
.rstride$listToXML <- function(node, sublist){
  
  for(i in 1:length(sublist)){
    child <- newXMLNode(names(sublist)[i], parent=node);
    
    if (typeof(sublist[[i]]) == "list"){
      .rstride$listToXML(child, sublist[[i]])
    }
    else{
      xmlValue(child) <- paste(sublist[[i]],collapse= ';')
    }
  } 
}

# list_config <- config_disease
# root_name <- 'disease'
# output_prefix <- 'sim_output'
# Save a list in XML format with given root node
.rstride$save_config_xml <- function(list_config,root_name,output_prefix){
  
  # setup XML doc (to add prefix)
  xml_doc = newXMLDoc()
  
  # setup XML root
  root <- newXMLNode(root_name, doc = xml_doc)
  
  # add list info
  .rstride$listToXML(root, list_config)
  
  # create filename
  filename <- paste0(output_prefix,'.xml')
  
  # xml prefix
  xml_prefix <- paste0(' This file is part of the Stride software [', format(Sys.time()), ']')
  
  # save as XML,
  # note: if we use an XMLdoc to include prefix, the line break dissapears...
  # fix: http://r.789695.n4.nabble.com/saveXML-prefix-argument-td4678407.html
  cat( saveXML( xml_doc, indent = TRUE, prefix = newXMLCommentNode(xml_prefix)),  file = filename) 
  
  # return the filename
  return(filename)
}

###############################
## DEVELOPING FUNCTIONS      ##
###############################

# load last project_dir
.rstride$load_pd <- function(){

  # default output dir
  output_dir              <- 'sim_output'
  
  # load directory content (non recursive)
  sim_dirs <- dir(output_dir) 
  
  # create project_dir (global)
  project_dir <<- file.path(output_dir,sim_dirs[length(sim_dirs)])
  
  # terminal message
  cat('SET PROJECT DIR TO ', project_dir)
  
}

# set most recent stride install directory as work directory 
#.rstride$set_wd()
.rstride$set_wd <- function(){

  # default install directory
  install_dir              <- system('echo $HOME/opt',intern=T)
  
  # load directory content (non recursive)
  stride_dirs <- dir(install_dir,pattern = 'stride-')
  
  # select last directory
  last_stride_dir <- stride_dirs[length(stride_dirs)]
  
  # set work directory
  setwd(file.path(install_dir,last_stride_dir))

  # terminal message
  cat('NEW WORK DIRECTORY ',file.path(install_dir,last_stride_dir))

}

