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
  project_dir <- './sim_output/20180503_171154_all_0p9/'; plot_tag <- '_all'
  project_dir <- './sim_output/20180503_172007_close_0p9/' ; plot_tag <-'_close'
  
  project_dir <- './sim_output/20180503_195252_all_full/'; plot_tag <- '_all_full'
  project_dir <- './sim_output/20180503_200832_close_full/' ; plot_tag <-'_close_full'
  
  project_dir <- './sim_output/20180503_205833'; plot_tag <- '_seroprev'
  
  explore_immunity(project_dir,plot_tag)
}

#############################################################################
# FUNCTION TO PLOT SOCIAL CONTACT MATRICES AND COUNTS                      ##
#############################################################################
explore_immunity <- function(project_dir,plot_tag)
{
  
  all_data <- read.table(file = file.path(project_dir[1],'summary.csv'),header=T,stringsAsFactors = F,sep=',')
  pop_size <- 600000
  
  pdf(file.path(project_dir,paste0('plot_cases',plot_tag,'.pdf')),10,10)
  par(mfrow=c(2,3))
  boxplot(all_data$infected_transmission / pop_size ~ all_data$immunity_rate ,ylab='relative cases',xlab='immunity level')
  boxplot(all_data$infected_transmission / pop_size ~ all_data$immunity_rate,ylim=c(0,0.01),ylab='relative cases',xlab='immunity level')
  boxplot(all_data$infected_transmission / pop_size ~ all_data$immunity_rate,ylim=c(0,0.0005),ylab='relative cases',xlab='immunity level')
  
  boxplot(all_data$infected_transmission ~ all_data$immunity_rate,ylim=c(0,pop_size*0.01),ylab='cases',xlab='immunity level')
  
  boxplot(all_data$infected_transmission / all_data$infected_seed ~ all_data$immunity_rate,ylim=c(0,pop_size*0.002),ylab='mean outbreak size',xlab='immunity level')
  boxplot(all_data$infected_transmission / all_data$infected_seed ~ all_data$immunity_rate,ylim=c(0,pop_size*0.00002),ylab='mean outbreak size',xlab='immunity level')
  dev.off()
  
  ######################
  ## GET DATA       ##
  ######################
  
  exp_dir <- dir(project_dir)
  exp_dir <- exp_dir[!(grepl('data',exp_dir) | grepl('pdf',exp_dir) | grepl('csv',exp_dir))]

  cat_outbreak_size        <- c(0,1,5,10,50,100,Inf)
  levels_outbreak_size     <- levels(cut(0,cat_outbreak_size))
  cat_transmission_context <- c("secondary_community","work","primary_community","household","school")
  cat_secundary_infections <- 1:50
  
  log_outbreak_size        <- data.frame(matrix(0,nrow=length(exp_dir),ncol=length(levels_outbreak_size)))
  names(log_outbreak_size) <- levels_outbreak_size
  
  log_transmission_context <- data.frame(matrix(0,nrow=length(exp_dir),ncol=length(cat_transmission_context)))
  names(log_transmission_context) <- cat_transmission_context
  
  log_secundary_infections <- matrix(0,nrow=length(exp_dir),ncol=length(cat_secundary_infections))
  log_exp_prefix           <- matrix(0,nrow=length(exp_dir),ncol=2)
  colnames(log_exp_prefix) <- c('output_prefix','immunity_rate')
  all_data$infected_total
  
  i_file <- 1
  if(length(exp_dir)>0)
    for(i_file in 1:length(exp_dir))
    {

      print(i_file)
      
      all_data_flag <- grepl(paste0(exp_dir[i_file],'/'),all_data$output_prefix)
      log_exp_prefix[i_file,] <- c(all_data$output_prefix[all_data_flag],all_data$immunity_rate[all_data_flag])
  
      
      pdata    <- read.table(file.path(project_dir,exp_dir[i_file],'participants.csv'),sep=',',header=T)
      num_part <- nrow(pdata)
      imm_tbl  <- table(pdata$is_immune == 'true', pdata$part_age)
      suc_tbl  <- table(pdata$is_suceptible == 'true', pdata$part_age)

       plot(imm_tbl[2,] / colSums(imm_tbl),ylab='immune',xlab='age')
      # plot(suc_tbl[2,] / colSums(imm_tbl),ylab='susceptible',xlab='age')
      # 
      transm_data         <- read.table(file.path(project_dir,exp_dir[i_file],'transmissions.csv'),sep=',',header=T,stringsAsFactors = F)
      dim(transm_data)
      
      num_infections <- aggregate(transm_data$new_infected_id ~ transm_data$local_id, FUN = length)
      names(num_infections) <- c("local_id",'sec_infections')
      transm_data <- merge(num_infections,transm_data)
    
      # transmissions event per person  
      #hist(transm_data$sec_infections[transm_data$local_id!=-1],freq = F,ylab='rate',xlab='transmissions event / person')
      sec_inf_tbl <- table(transm_data$sec_infections[transm_data$local_id!=-1])
      log_secundary_infections[i_file,as.double(names(sec_inf_tbl))] <- sec_inf_tbl / sum(sec_inf_tbl)
     
      # location
      #barplot(table(transm_data$cnt_location[transm_data$local_id!=-1]),las=2,cex.names = 0.7)
      transm_context_tbl <- table(transm_data$cnt_location[transm_data$local_id!=-1])
      log_transmission_context[i_file,names(transm_context_tbl)] <- transm_context_tbl / sum(transm_context_tbl)
      
      # clusters
      ## OUTBREAKS
      if(nrow(transm_data) < 20000)
      {
        # book keeping
        transm_data$outbreak <- 0
        outbreak_id <- 1
        
        ## primary cases
        flag <- transm_data$local_id == -1
        num_outbreaks <- sum(flag)
        transm_data$outbreak[flag] <- 1:num_outbreaks
        
        outbreak_id <- 1
        for(outbreak_id in 1:num_outbreaks)
        {
          infctrs <- transm_data$new_infected_id[transm_data$outbreak == outbreak_id]
          num_infctrs_prev <- 0
          
          while(length(infctrs) > num_infctrs_prev)
          {
            num_infctrs_prev <- length(infctrs)
            case_flag <- transm_data$local_id %in% infctrs
            sum(case_flag)
            transm_data$outbreak[case_flag] <- outbreak_id
            
            infctrs <- transm_data$new_infected_id[transm_data$outbreak == outbreak_id]
          }
        }
        
        outbreak_size <- table(transm_data$outbreak)
        #par(mfrow=c(2,2))
        # hist(outbreak_size,50)
        # barplot(table(cut(outbreak_size,cat_outbreak_size)),las=2)
        outbreak_size_tbl <- table(cut(outbreak_size,cat_outbreak_size))
        log_outbreak_size[i_file,] <- outbreak_size_tbl
      } else {
        log_outbreak_size[i_file,] <- NA
      } # end outbreak size calculation
      
    } # end for loop
  
  log_exp_prefix
  log_transmission_context
  log_outbreak_size
  log_secundary_infections
  
  pdf(file.path(project_dir,paste0('plot_transmission',plot_tag,'.pdf')),10,10)
  
  par(mfrow=c(2,3))
  for(i in 1:ncol(log_transmission_context)){
    boxplot(log_transmission_context[,i] ~ log_exp_prefix[,2],
            xlab='immunity level',ylab='fraction',
            main=names(log_transmission_context)[i],
            ylim=c(0,0.5))
  }
  
  num_outbreaks_run <- sum(log_outbreak_size[1,])
  par(mfrow=c(2,2))
  opt_imm <- unique(log_exp_prefix[,2])
  for(i in 1:length(opt_imm)){
    boxplot(log_outbreak_size[log_exp_prefix[,2]==opt_imm[i],]/num_outbreaks_run,
            xlab='outbreak size',ylab='frequency',
            las=2,main=paste('immunity level',opt_imm[i]), 
            cex.axis=0.7,ylim=c(0,1)
            )
  
    boxplot(log_secundary_infections[log_exp_prefix[,2]==opt_imm[i],1:10],
            xlab='sec cases',ylab='frequency',
            las=1,main=paste('immunity level',opt_imm[i]), 
            cex.names=0.5,
            ylim=c(0,0.6)
            )
  }
  
  dev.off()
  
} # end function



################################################
## COMMAND LINE FUNCTIONALITY                 ##
################################################
#!/usr/bin/env Rscript
args = commandArgs(trailingOnly=TRUE)
if (length(args)!=0) {
  explore_immunity(args[1])
} 
