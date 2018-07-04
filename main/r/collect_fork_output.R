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

require(XML)

if(0==1) # for debugging
{
  #setwd('..')
  project_dir <- './sim_output/20180503_203527/'
  remove_exp_dir <- FALSE
  collect_fork_ouptut(project_dir,FALSE)
}

#############################################################################
# FUNCTION TO PLOT SOCIAL CONTACT MATRICES AND COUNTS                      ##
#############################################################################
collect_fork_ouptut <- function(project_dir,remove_exp_dir = FALSE)
{
  
  project_dir_files <- dir(project_dir)
  exp_dir <- dir(project_dir)
  exp_dir <- exp_dir[!(grepl('data',exp_dir) | grepl('pdf',exp_dir) | grepl('csv',exp_dir))]
  
  summary_out <- NA
  cases_time <- NA
  
  
  i_file <- 1
  if(length(exp_dir)>0)
  for(i_file in 1:length(exp_dir))
  {
    # load config_file
    config_xml    <- xmlParse(file.path(project_dir,exp_dir[i_file],'config.xml'))
    config_list   <- xmlToList(config_xml)

    # load disease transmission probability
    disease_data    <- read.table(file.path(project_dir,exp_dir[i_file],'disease.csv'),sep=',',header=T)
    transmission_probability <- disease_data$value[disease_data$parameter_name == 'transmission_rate']
    
    # load transmission file
    transmission_data <- read.table(file.path(project_dir,exp_dir[i_file],'transmissions.csv'),sep=',',header=T)
    dim(transmission_data)
    
    # make factor => to include all locations in the table
    transmission_data$cnt_location <- (factor(transmission_data$cnt_location,
                                              levels = c('-1', 'household', 'primary_community', 'school', 'secondary_community', 'work')))
    
    # count transmissions
    summary_transmission_loc <- table(transmission_data$cnt_location)
    names(summary_transmission_loc)[1] <- 'seed'
    names(summary_transmission_loc) <- paste0('infected_',names(summary_transmission_loc))
    infected_total        <- sum(summary_transmission_loc)
    infected_transmission <- infected_total - as.numeric(summary_transmission_loc[1])
    summary_infected      <- c(infected_total=infected_total,infected_transmission=infected_transmission,summary_transmission_loc)
    
    # count transmissions over time
    new_cases_time <- hist(transmission_data$sim_day,0:config_list$num_days,plot=FALSE)
    
    # load participant file
    if(file.exists(file.path(project_dir,exp_dir[i_file],'participants.csv')))
    {
      participant_data <- read.table(file.path(project_dir,exp_dir[i_file],'participants.csv'),sep=',',header=T)
      
      
      part_summary <- c(part_rate_susceptible = sum(participant_data$is_suceptible == 'true'),
                        part_rate_infected = sum(participant_data$is_infected == 'true'),
                        part_rate_recovered = sum(participant_data$is_recovered == 'true'),
                        part_rate_immune = sum(participant_data$is_immune == 'true')) / nrow(participant_data)
    } else {
      part_summary <- c(participants_susceptible = NA,
                        part_rate_infected = NA,
                        part_rate_recovered = NA,
                        part_rate_immune = NA)
    }
    
    
    part_summary <- round(part_summary,digits=4)
    
    
    
    # create summmary file
    col_prefix <- which(names(config_list) == 'output_prefix')
    tmp_summary <- c(output_prefix=config_list$output_prefix,
                     summary_infected,
                     config_list[-col_prefix],
                     transmission_param_probability = transmission_probability,
                     part_summary)
    
    tmp_summary <- unlist(tmp_summary)
    
    if(i_file == 1){
      
      # create summary data.frame
      summary_out <- data.frame(matrix(NA,nrow=length(exp_dir),ncol=length(tmp_summary)))
      names(summary_out) <- names(tmp_summary)
      
      # create cases_time data.frame
      cases_time <- matrix(NA,nrow=length(exp_dir),ncol=as.numeric(config_list$num_days))
    }
    
    summary_out[i_file,] <- tmp_summary
    cases_time[i_file,]  <- new_cases_time$counts
   
    if(remove_exp_dir)
    {
      unlink(file.path(project_dir,exp_dir[i_file]), recursive = TRUE)
    }  
    
  } # end for(exp)
  
  write.table(summary_out,file=file.path(project_dir,'summary.csv'),sep=',',dec='.',row.names=F)
  write.table(cases_time,file=file.path(project_dir,'cases_time.csv'),sep=',',dec='.',row.names=F,col.names=F)
  
  print(paste('SUMMARY AND CASES FILE COMPLETE for:', project_dir))
  if(remove_exp_dir)
  {
    print('INDIVIDUAL RUN OUTPUT REMOVED')
  } 
  
} # end function


################################################
## COMMAND LINE FUNCTIONALITY                 ##
################################################
#!/usr/bin/env Rscript
args = commandArgs(trailingOnly=TRUE)
if (length(args)!=0) {
  if (length(args)==1) {
    args[2] = FALSE
  }
  collect_fork_ouptut(args[1],args[2])
} 

