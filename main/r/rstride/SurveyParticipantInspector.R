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
# EXPLORE PARTICIPANT DATA                                                 ##
#############################################################################

inspect_participant_data <- function(project_dir)
{
  
  # load project summary
  project_summary <- .rstride$load_project_summary(project_dir)
  
  # if no participants are surveyed, stop
  if(any(project_summary$num_participants_survey==0)){
    # command line message
    .rstride$cli_print('NO PARTICIPANT DATA AVAILABLE')
    return(.rstride$no_return_value())
  }
  
  # person id increment factor, to obtain unique ids in each experiment
  max_pop_size <- max(project_summary$population_size)
  id_factor <- 10^ceiling(log10(max_pop_size))
  
  
  # retrieve all variable model parameters
  input_opt_design        <- .rstride$get_variable_model_param(project_summary)
  
  # open pdf stream
  .rstride$create_pdf(project_dir,'survey_participant_inspection',10,7)
  par(mfrow=c(2,4))
  
  i_config <- 1
  for(i_config in 1:nrow(input_opt_design))
  {
    # select the participant output subset, corresponding the 'input_opt_design' row
    flag_exp            <- .rstride$get_equal_rows(project_summary,input_opt_design[i_config,])
    data_part           <- .rstride$load_aggregated_output(project_dir,'data_participants',project_summary$exp_id[flag_exp])
    num_runs_exp        <- sum(flag_exp)
    
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
    
    plot(1:30,colMeans(all_inf),ylab='population fraction',xlab='day',type='b',lwd=3,col=2)
    points(1:30,colMeans(all_symp),lwd=3,col=4,type='b')
    legend('topright',c('infectious','symptomatic'),col=c(2,4),lwd=4,cex=0.8)
    abline(v=6:9,lty=3)
    
    f_data <- data_part$start_infectiousness; f_main <- 'debug'
    plot_cum_distr <- function(f_data,f_main){
      tbl_data <- table(f_data)/length(f_data)
      tbl_data_cumm <- cumsum(tbl_data)
      plot(tbl_data,xlim=c(0,20),ylim=0:1,xlab='day',ylab='frequency',main=f_main)
      lines(as.numeric(names(tbl_data_cumm)),tbl_data_cumm,col=4,lwd=2,type='b')
      
      legend_position <- 'topleft'
      if(max(as.numeric(names(tbl_data)))<10) {legend_position <- 'topright'}
      legend(legend_position,c('per day','cummulative'),col=c(1,4),lwd=2,cex=0.8)
    }
    
    plot_cum_distr(data_part$start_infectiousness,f_main='start_infectiousness')
    plot_cum_distr(data_part$end_infectiousness-data_part$start_infectiousness,f_main='days infectious')
    plot_cum_distr(f_data=data_part$start_symptomatic-data_part$start_infectiousness,f_main='days infectious \n& not symptomatic')
    plot_cum_distr(data_part$start_symptomatic,f_main='start_symptomatic')
    plot_cum_distr(data_part$end_symptomatic-data_part$start_symptomatic,f_main='days symptomatic')
    
    
    ## POPULATION
    population_age <- as.data.frame(table(part_age = data_part$part_age))
    
    ## POPULATION IMMUNITY
    data_part$is_immune   <- data_part$is_immune == "TRUE"
    
    immune_age <- data.frame(table(is_immune = data_part$is_immune, part_age = data_part$part_age),stringsAsFactors = F)
    immune_age$part_age <- as.numeric(levels(immune_age$part_age)[(immune_age$part_age)])
    names(immune_age)  
    flag <- immune_age$is_immune == FALSE
    plot(immune_age$part_age[flag],
         immune_age$Freq[flag]/population_age$Freq,
         xlab='age',
         ylab='fraction susceptible',
         main='population susceptibility',
         pch=19, lwd=3
    )
    
    names(data_part)
    
    ## SCHOOLING
    school_age <- data.frame(table(school_enrolled = data_part$school_id != 0,part_age = data_part$part_age))
    school_age$part_age <- as.numeric(levels(school_age$part_age)[(school_age$part_age)])
    flag <- school_age$school_enrolled == TRUE
    plot(school_age$part_age[flag],
         school_age$Freq[flag]/population_age$Freq,
         xlab='age',
         ylab='population fraction',
         main='population enrolled in school',
         pch=1, lwd=3, xlim=c(0,30)
    )
    abline(v=c(0,3,6,12,18,25)-0.5)
    text(x=0,y=0.02,'kindergarten',srt=90,pos=4)
    text(x=3,y=0.02,'pre-school',srt=90,pos=4)
    text(x=7,y=0.02,'primary school',srt=90,pos=4)
    text(x=13,y=0.02,'secundary school',srt=90,pos=4)
    text(x=20,y=0.02,'tertiary school',srt=90,pos=4)
    
  }
  
  # close PDF stream
  dev.off()
  
  # command line message
  .rstride$cli_print('INSPECTION OF PARTICIPANT DATA COMPLETE')
  
} # function end
