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
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#############################################################################
#
# RESULTS AND TIMINGS FROM STRIDE SIMULATIONS
#
#############################################################################

plot_results <- function(data_tag){
  
  
  ######################
  ## GET DATA       ##
  ######################
  
  data	   <- read.table(paste(data_tag,'_summary.csv',sep=''),header=TRUE,sep=",",stringsAsFactors=F)
  data_log <- read.table(paste(data_tag,'_cases.csv',sep=''),header=FALSE,sep=",")
  
  if(dim(data_log)[2]>1)
  {
    
  ######################
  ## START PDF STREAM	##
  ######################
  pdf(file=paste0(data_tag,'_results.pdf'))
  
  
  ######################
  ## PLOT OVER TIME  	##
  ######################
  
  num_days <- dim(data_log)[2]
  plot(c(1,num_days),c(min(data_log),max(data_log)),col=0,xlab='time (days)',ylab='cases',main='TOTAL CASES')
  for(i in 1:dim(data_log)[1])
  {
    lines(as.double(data_log[i,]))  
  }
  
  daily_cases <- data_log[,2:(num_days)] - data_log[,1:(num_days-1)]
  plot(c(1,num_days),c(min(daily_cases),max(daily_cases)),col=0,xlab='time (days)',ylab='daily cases',main='DAILY CASES')
  for(i in 1:dim(daily_cases)[1])
  {
    lines(as.double(daily_cases[i,]))  
  }
  
  attack_rate <- daily_cases / data$pop_size
  plot(c(1,num_days),c(min(attack_rate),max(attack_rate)),col=0,xlab='time (days)',ylab='attack rate',main='ATTACK RATE')
  for(i in 1:dim(attack_rate)[1])
  {
    lines(as.double(attack_rate[i,]))  
  }
  
  ######################
  ## PLOT TIMINGS     ##
  ######################
  
  plot(data$AR, data$total_time,xlab='attack rate',ylab='total time (s)',main='TIMINGS')
  
  ######################
  ## SINGLE PLOTS     ##
  ######################
  
  # input
  opt_seeding_rate  <- sort(unique(data$seeding_rate))
  opt_R0            <- sort(unique(data$R0))
  opt_immunity_rate <- sort(unique(data$immunity_rate))
  
  plot_AR_lim       <- c(0,1.2)
  plot_time_lim     <- c(0,max(data$total_time)*1.2)
  
  if(length(opt_seeding_rate)>1)
  {
    plot_pch <- as.factor(data$R0)
    plot_col <- as.factor(data$immunity_rate)
    
    plot(data$seeding_rate,data$AR,ylim=plot_AR_lim,main='SEED INFECTED: AR',xlab='seeding rate',ylab='attack rate',pch=as.numeric(plot_pch),col=as.numeric(plot_col))
    if(length(opt_seeding_rate)>1)  {legend('topleft',paste('R0:',levels(plot_pch)),pch=1:length(levels(plot_pch)))              }
    if(length(opt_immunity_rate)>1) {legend('topright',paste('immunity rate:',levels(plot_col)),fill=1:length(opt_immunity_rate)) }
    
    plot(data$seeding_rate,data$total_time,ylim=plot_time_lim,main='SEED INFECTED: TIME',xlab='seeding rate',ylab='total time (s)',pch=as.numeric(plot_pch),col=as.numeric(plot_col))
    if(length(opt_seeding_rate)>1)  {legend('topleft',paste('R0:',levels(plot_pch)),pch=1:length(levels(plot_pch)))              }
    if(length(opt_immunity_rate)>1) {legend('topright',paste('immunity rate:',levels(plot_col)),fill=1:length(opt_immunity_rate)) }
  }
  
  if(length(opt_R0)>1)
  {
    plot_pch <- as.factor(data$seeding_rate)
    plot_col <- as.factor(data$immunity_rate)
    
    plot(data$R0,data$AR,ylim=plot_AR_lim,main='R0',xlab='R0: AR',ylab='attack rate',pch=as.numeric(plot_pch),col=as.numeric(plot_col))
    if(length(opt_seeding_rate)>1)  {legend('topleft',paste('seeding rate:',levels(plot_pch)),pch=1:length(levels(plot_pch)))    }
    if(length(opt_immunity_rate)>1) {legend('topright',paste('immunity rate:',levels(plot_col)),fill=1:length(levels(plot_col))) }
    
    plot(data$R0,data$total_time,ylim=plot_time_lim,main='R0: TIME',xlab='R0',ylab='total time (s)',pch=as.numeric(plot_pch),col=as.numeric(plot_col))
    if(length(opt_seeding_rate)>1)  {legend('topleft',paste('seeding rate:',levels(plot_pch)),pch=1:length(levels(plot_pch)))    }
    if(length(opt_immunity_rate)>1) {legend('topright',paste('immunity rate:',levels(plot_col)),fill=1:length(opt_immunity_rate)) }
  }
  
  if(length(opt_immunity_rate)>1)
  {
    plot_pch <- as.factor(data$R0)
    plot_col <- as.factor(data$seeding_rate)
    
    plot(data$immunity_rate,data$AR,ylim=plot_AR_lim,main='POPULATION IMMUNITY: AR',xlab='immunity rate',ylab='attack rate',pch=as.numeric(plot_pch),col=as.numeric(plot_col))
    if(length(opt_R0)>1)  {legend('topleft',paste('R0:',levels(plot_pch)),pch=1:length(levels(plot_pch)))                      }
    if(length(opt_seeding_rate)>1) {legend('topright',paste('seeding rate:',levels(plot_col)),fill=1:length(opt_seeding_rate)) }
    
    # attack rate in susceptible population
    plot(data$immunity_rate,data$AR/(1-data$immunity_rate),ylim=plot_AR_lim,main='POPULATION IMMUNITY: AR (2)',xlab='immunity rate',ylab='attack rate in susceptibles',pch=as.numeric(plot_pch),col=as.numeric(plot_col))
    if(length(opt_R0)>1)  {legend('topleft',paste('R0:',levels(plot_pch)),pch=1:length(levels(plot_pch)))                      }
    if(length(opt_seeding_rate)>1) {legend('topright',paste('seeding rate:',levels(plot_col)),fill=1:length(opt_seeding_rate)) }
    
    #time 
    plot(data$immunity_rate,data$total_time,ylim=plot_time_lim,main='POPULATION IMMUNITY: TIME',xlab='immunity rate',ylab='total time (s)',pch=as.numeric(plot_pch),col=as.numeric(plot_col))
    if(length(opt_R0)>1)  {legend('topleft',paste('R0:',levels(plot_pch)),pch=1:length(levels(plot_pch)))                      }
    if(length(opt_seeding_rate)>1) {legend('topright',paste('seeding rate:',levels(plot_col)),fill=1:length(opt_seeding_rate)) }
  }
  
  
  # check if there are transmission log files in the experiments directory
  # 1. get all files
  exp_files_all <- dir('experiments')
  # 2. remove other files from the list
  exp_files_all <- exp_files_all[grepl('transmissions.csv',exp_files_all)]
  
  # if transmission log files present
  if(length(exp_files_all)>0){
    
    i_exp <- 1
    data_transmission_all <- NULL
    outbreak_size_all <- matrix(0,nrow=100)
    
    exp_file <- exp_files_all[1]
    for(exp_file in exp_files_all)
    {
      
      # load transmission data
      data_transmission <- read.table(paste('experiments/',exp_file,sep=''),header=TRUE,sep=",")

      # rename columns
      names(data_transmission)[1:2] <- c('infector_id','infected_id')
      
      # set 'infector' of seeds to NA
      data_transmission$infector_id[data_transmission$infector_id==-1] <- NA
      
      # count secundary cases and summarise in data.frame: id and sec_cases
      tmp <- table(data_transmission$infector_id)
      infector_data <- data.frame(infected_id = names(tmp), sec_cases=as.double(tmp))

      # add infector data to the transmission data.frame (add NA sec_cases if missing)
      data_transmission <- merge(data_transmission,infector_data, all = TRUE)
      
      # set 'NA' secundary cases to 0
      data_transmission$sec_cases[is.na(data_transmission$sec_cases)] <- 0
      
      # add experiment tag
      data_transmission$exp_file <- exp_file
      
      # add data to summary variable
      data_transmission_all <- rbind(data_transmission_all,data_transmission)
      
      
      ## OUTBREAKS
      ## primary cases
      data_transmission$outbreak <- 0
      outbreak_id <- 1
      
      flag <- is.na(data_transmission$infector_id)
      num_outbreaks <- sum(flag)
      data_transmission$outbreak[flag] <- 1:num_outbreaks
      for(outbreak_id in 1:num_outbreaks)
      {
        infctrs <- data_transmission$infected_id[data_transmission$outbreak == outbreak_id]
        num_infctrs_prev <- 0
        
        while(length(infctrs) > num_infctrs_prev)
        {
          num_infctrs_prev <- length(infctrs)
          case_flag <- data_transmission$infector_id %in% infctrs
          sum(case_flag)
          data_transmission$outbreak[case_flag] <- outbreak_id
          
          infctrs <- data_transmission$infected_id[data_transmission$outbreak == outbreak_id]
        }
      }
      
      outbreak_size_count <- table(table(data_transmission$outbreak))
      outbreak_size_level <- as.numeric(names(outbreak_size_count))
      
      print(outbreak_size_level)
      print(outbreak_size_level)
      
      outbreak_size_all[outbreak_size_level] <- outbreak_size_all[outbreak_size_level] + outbreak_size_count
  
    }
    
    # plot secundary cases over time
    boxplot(data_transmission_all$sec_cases ~ data_transmission_all$sim_day, xlab='time infection (days)',ylab='secundary cases / infected')
  
    plot(outbreak_size_all)
    barplot((outbreak_size_all),beside = T,xlab='outbreak size',ylab='count')
    axis(1)  
    
    plot(outbreak_size_all)
    barplot(outbreak_size_all/sum(outbreak_size_all),beside = T,xlab='outbreak size',ylab='frequency',ylim=0:1)
    axis(1)  
    

  }
    

  
  ######################
  ## CLOSE PDF STREAM ##
  ######################
  dev.off()
  
  } # end if(data_log)[2]>1)
  
} # end function
