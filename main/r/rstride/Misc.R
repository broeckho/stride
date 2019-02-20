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

#.rstride$set_wd()  #DEVELOPMENT: to set the work directory as the latest stride install dir 
#.rstride$load_pd() #DEVELOPMENT: to retrieve the latest project directory (project_dir)

# load R packages
library(XML,quietly = TRUE)
library(doParallel,quietly = TRUE)

if(!(exists('.rstride'))){
  .rstride <- new.env()
}

###############################
## COMMAND LINE MESSAGES     ##
###############################

.rstride$cli_print <- function(...,WARNING=F) {
  
  # get function arguments
  function_arguments <- as.list(match.call(expand.dots=FALSE))$...
  
  # get function-call environment (to retrieve variable from that environment) 
  pf <- parent.frame()
  
  # parse list => make character vector
  function_arguments <- foreach(i=1:length(function_arguments),.combine='cbind') %do%{
    eval(unlist(function_arguments[[i]]),envir = pf)
  }
  
  # add a space to each function arguments
  function_arguments <- paste(' ',function_arguments)
  
  # set text color: black (default) or red (warning)
  web_color_black <- '\033[0;30m'
  web_color_red   <- '\033[0;31m'
  text_color      <- ifelse(WARNING,web_color_red,web_color_black)
  
  # print time + arguments (without spaces)
  cli_out <- paste0(c('echo "',text_color, '[',format(Sys.time()),']',
                      function_arguments, web_color_black,'"'),collapse = '')
  system(cli_out)
}

# terminate rStride
.rstride$cli_abort <- function()
{
  .rstride$cli_print('!! TERMINATE rSTRIDE CONTROLLER !!',WARNING=T)
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
  
  # check if project_dir exists
  if(.rstride$dir_not_present(project_dir)){
    stop('PROJECT DIR NOT PRESENT')
  }
  
  # get the filename in the project dir that contains "summary.csv" (full path)
  project_summary_filename <- file.path(project_dir,dir(project_dir,pattern = 'summary.csv'))
  
  # read the csv file
  project_summary          <- read.table(project_summary_filename,sep=',',header=T,stringsAsFactors = F)
  
  # return the data.frame
  return(project_summary)
}

###############################
## OPEN PDF STREAM           ##
###############################

.rstride$create_pdf <- function(project_dir,file_name,width=7,height=7){
  
  # load project summary
  project_summary   <- .rstride$load_project_summary(project_dir)
  
  # get run_tag
  run_tag           <- unique(project_summary$run_tag)
  
  # get file name with path
  file_name_path    <- file.path(project_dir,paste0(run_tag,'_',file_name,'.pdf'))
  
  # open pdf stream
  pdf(file_name_path,width,height)
  
}

###############################
## CREATE EXPERIMENT TAG     ##
###############################

# create experiment tag
.rstride$create_exp_tag <- function(i_exp){
  return(paste0('exp',sprintf("%04s", i_exp)))
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
## MATRIX OPERATIONS         ##
###############################

.rstride$get_equal_rows <- function(f_matrix,f_vector){
  return(as.logical(colSums(t(f_matrix[,names(f_vector)]) == c(f_vector)) == length(f_vector)))
}


########################################
## AGGREGATE EXPERIMENT OUTPUT FILES  ##
########################################

.rstride$aggregate_exp_output <- function(project_dir){
  
  # load project summary
  project_summary      <- .rstride$load_project_summary(project_dir)
  
  # id increment factor
  max_pop_size <- max(project_summary$population_size)
  id_factor    <- 10^ceiling(log10(max_pop_size))
  
  # get output data types
  data_type_opt <- unique(dir(file.path(project_summary$output_prefix),pattern='.RData'))
  
  data_type <- data_type_opt[2]
  for(data_type in data_type_opt)
  {
    
    data_filenames <- dir(project_dir,pattern=data_type,recursive = T,full.names = T)
    
    # load all project experiments
    i_exp <- 2
    data_all <- foreach(i_exp = 1:nrow(project_summary),.combine='rbind') %do%
    {
      # get file name
      exp_file_name <- file.path(project_summary$output_prefix[i_exp],data_type)
      
      # check if output exists for the specified data_type
      if(file.exists(exp_file_name)){
        
        # load output data
        param_name  <- load(exp_file_name)
        
        # load data
        data_exp    <- get(param_name)
        
        # for prevalence data, check the number of days
        if(grepl('prevalence',exp_file_name)){
          
          # create full-size data frame to include the maximum number of days
          data_tmp        <- data.frame(matrix(NA,ncol=max(project_summary$num_days)+2)) # +1 for day 0 and +1 for exp_id
          names(data_tmp) <-  c(paste0('day',0:max(project_summary$num_days)),
                                'exp_id')
          
          # insert the experiment data
          data_tmp[names(data_exp)] <- data_exp
          
          # replace the experiment data by the newly constructed data.frame
          data_exp <- data_tmp
        }
        
        # add run index
        data_exp$exp_id <- project_summary$exp_id[i_exp]
        
        # remove the original data file
        unlink(exp_file_name)
        
        # return experiment data
        data_exp
      }
    }
    
    # save
    run_tag <- unique(project_summary$run_tag)
    save(data_all,file=file.path(project_dir,paste0(run_tag,'_',data_type)))
  }
}

.rstride$load_aggregated_output <- function(project_dir,file_type,exp_id_opt = NA){
  
  # load project summary
  project_summary <- .rstride$load_project_summary(project_dir)
  
  # get ouput filenames
  dir_files       <- dir(project_dir,full.names = TRUE)
  output_filename <- dir_files[grepl(file_type,dir_files)]
  
  # if the file does not exists, return NA
  if(length(output_filename)==0){
    return(NA)
  }
  
  # load output
  param_name          <- load(output_filename)
  
  # rename parameter
  data_out <- get(param_name)
  
  # selection?
  if(!any(is.na(exp_id_opt))){
    data_out <- data_out[data_out$exp_id %in% exp_id_opt,]
  }
  
  # make person ids unique over all experiment, using a person id increment factor
  max_pop_size <- max(project_summary$population_size)
  id_factor    <- 10^ceiling(log10(max_pop_size))
  col_names    <- c('local_id','infector_id')
  col_names    <- col_names[col_names %in% names(data_out)]
  data_out[,col_names] <- data_out[,col_names] + (data_out$exp_id*id_factor)
  
  # return
  return(data_out)
  
}


###############################
## DEFENSIVE PROGRAMMING     ##
###############################

.rstride$no_return_value <- function(){
  
  return(invisible())
}

.rstride$dir_not_present <- function(path_dir){
  
  # if directory does not exists, return TRUE (+warning)
  if(!file.exists(paste(path_dir))){
    .rstride$cli_print('[',paste(match.call(),collapse=' '),'] DIRECTORY NOT PRESENT:',path_dir,WARNING=T)
    return(TRUE)
  } 
  
  # else, return FALSE
  return(FALSE)
}

# check file presence
.rstride$data_files_exist <- function(design_of_experiment = exp_design){
  
  # get the unique file names
  file_names <- unique(c(design_of_experiment$age_contact_matrix_file,
                         design_of_experiment$disease_config_file,
                         design_of_experiment$holidays_file,
                         design_of_experiment$population_file))
  
  # add the path to the data folder
  data_dir <- './data'
  file_names <- file.path(data_dir,file_names)
  
  # check the existance of the files
  file_not_exist_bool   <- !file.exists(file_names)
  
  # if any file missing => return FALSE
  if(any(file_not_exist_bool)){
    .rstride$cli_print('DATA FILE(S) MISSING:', paste(file_names[file_not_exist_bool],collapse = ' '),WARNING=T)
    return(FALSE)
  }  
  
  # else => return TRUE
  return(TRUE)
}

# log level
# check file presence
.rstride$log_levels_exist <- function(design_of_experiment = exp_design){
  
  valid_levels <- design_of_experiment$contact_log_level %in% c('None','Transmissions','All')
  
  if(any(!valid_levels)){
    .rstride$cli_print('INVALID LOG LEVEL(S):', paste(design_of_experiment$contact_log_level[!valid_levels],collapse = ' '),WARNING=T)
    return(FALSE)
  }  
  
  # else => return TRUE
  return(TRUE)
  
}

# R0
.rstride$valid_r0_values <- function(design_of_experiment = exp_design){
  
  if(any(!is.null(design_of_experiment$r0)))
  {
    
    r0_max <- max(design_of_experiment$r0)
    
    for(disease_config_file in unique(design_of_experiment$disease_config_file)){
      
      # load disease config file
      config_disease    <- xmlToList(file.path('data',disease_config_file))
      
      # get R0 limit    
      fit_r0_limit <- as.numeric(config_disease$label$fit_r0_limit)
      
      # check
      if(r0_max > fit_r0_limit){
        .rstride$cli_print('INVALID R0 CONFIG VALUE(S):', paste(design_of_experiment$r0,collapse = ' '),paste0('(R0 LIMIT = ',fit_r0_limit,')') ,WARNING=T)
        return(FALSE)
      } 
    } # end for-loop
  }
  
  # else => return TRUE
  return(TRUE)
  
}

# immunity
.rstride$valid_immunity_profiles <- function(design_of_experiment = exp_design){
  
  immunity_profiles <- unique(c(design_of_experiment$immunity_profile,design_of_experiment$vaccine_profile))
  
  # get immunity profile names
  disease_immunity_profiles <- c('None','Random','AgeDependent','Cocoon')
  
  # check if given profile names are valid
  if(!all(immunity_profiles %in% disease_immunity_profiles)){
    .rstride$cli_print('INVALID IMMUNITY PROFILE(S):', paste(immunity_profiles,collapse = ' '),WARNING=T)
    return(FALSE)
  } # end if-compare
  
  # else => return TRUE
  return(TRUE)
  
}




###############################
## DEVELOPMENT FUNCTIONS     ##
###############################

# load last project_dir
.rstride$load_pd <- function(){
  
  # set most recent build as work directory
  .rstride$set_wd()
  
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
