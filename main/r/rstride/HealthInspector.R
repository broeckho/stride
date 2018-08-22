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

#############################################################################
# EXPLORE DISEASE HISTORY                                                  ##
#############################################################################

explore_disease_history <- function(project_dir)
{
  
  # check if project_dir exists
  if(.rstride$dir_not_present(project_dir)){
    return(-1)
  }
  
  # load project summary
  project_summary <- .rstride$load_project_summary(project_dir)
  
  # retrieve all variable model parameters
  input_opt_design        <- .rstride$get_variable_model_param(project_summary)
  
  # open PDF stream
  pdf(file.path(project_dir,'health_exploration.pdf'),10,7)
  par(mfrow=c(2,3))
  
  for(i_config in 1:nrow(input_opt_design))
  {
    flag_exp <- .rstride$get_equal_rows(project_summary,input_opt_design[i_config,])
    project_summary_exp <- project_summary[flag_exp,]
    
    max_pop_size <- max(project_summary$population_size)
    id_factor <- 10^ceiling(log10(max_pop_size))
    
    # explore all project experiments
    i_exp <- 1
    data_part <- foreach(i_exp = 1:nrow(project_summary_exp),.combine='rbind') %do%
    {
      # load participant data
      load(file.path(project_summary$output_prefix[i_exp],'data_participants.RData'))
      
      # add run index
      data_part$run_index <- i_exp
      
      # adjust ids
      data_part$local_id        <- data_part$local_id + (i_exp*id_factor)
      
      data_part
    }
   
    num_part <- nrow(data_part)
    freq_start_inf  <- table(data_part$start_infectiousness) / num_part
    freq_start_symp <- table(data_part$start_symptomatic)    / num_part
    data_part[1,]
    all_inf <- matrix(0,num_part,30)
    all_symp <- matrix(0,num_part,30)
    for(i in 1:num_part){
      all_inf[i,data_part$start_infectiousness[i]:data_part$end_infectiousness[i]] <- 1
      all_symp[i,data_part$start_symptomatic[i]:data_part$end_symptomatic[i]] <- 1
    }
    
    plot(1:30,colMeans(all_inf),ylab='fraction',xlab='day',type='b',lwd=3,col=2)
    points(1:30,colMeans(all_symp),lwd=3,col=4,type='b')
    legend('topright',c('infectious','symptomatic'),col=c(2,4),lwd=4,cex=0.8)
    abline(v=6:9,lty=3)
  
    f_data <- data_part$start_infectiousness
    plot_cum_distr <- function(f_data,f_main){
      tbl_data <- table(f_data)/length(f_data)
      tbl_data_cumm <- cumsum(tbl_data)
      plot(tbl_data,xlim=c(0,20),ylim=0:1,xlab='day',ylab='frequency',main=f_main)
      lines(as.numeric(names(tbl_data_cumm)),tbl_data_cumm,col=4,lwd=2,type='b')
      legend('topleft',c('per day','cummulative'),col=c(1,4),lwd=2)
    }
    
    plot_cum_distr(data_part$start_infectiousness,f_main='start_infectiousness')
    plot_cum_distr(data_part$end_infectiousness-data_part$start_infectiousness,f_main='days infectious')
    plot_cum_distr(f_data=data_part$start_symptomatic-data_part$start_infectiousness,f_main='days infectious \n& not symptomatic')
    plot_cum_distr(data_part$start_symptomatic,f_main='start_symptomatic')
    plot_cum_distr(data_part$end_symptomatic-data_part$start_symptomatic,f_main='days symptomatic')
    
  }
  
  # close PDF stream
  dev.off()
  
  # command line message
  .rstride$cli_print('HEALTH EXPLORATION COMPLETE')
  
} # function end
