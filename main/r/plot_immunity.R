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
#  Copyright 2017, Willem L, Kuylen E & Broeckhove J
#############################################################################
#
# VISUALIZE POPULATION IMMUNITY AND TRANSMISSION EVENTS
#
#############################################################################


rm(list=ls())

#opt_data_tag <- '15touch_debug_household_cocoon' ; data_tag <- opt_data_tag
# opt_data_tag <- paste('15touch_debug',c('_household','_primary_community'),sep = '')
# opt_data_tag <- paste('15touch_debug_current',c('','_hh'),sep = '')
#opt_data_tag <- paste('current',c('random','clustered','none'),sep = '_')
opt_data_tag <- paste('current_clustered',c('hh','prim','sec','work','school'),sep = '_')
#data_tag <- opt_data_tag[3]
opt_data_tag <- paste('current',c('none'),sep = '_')

plot_immunity <- function(data_tag)
{
  
  io_folder <- '../' 

  # rename logfile => otherwise output of new run will be added
  orig_logfile <- paste0(io_folder,data_tag,'_logfile.txt')
  new_logfile <- paste0(io_folder,data_tag,'_logfile_fromR.txt')
  
  if(file.exists(orig_logfile)){
    file.rename(orig_logfile,new_logfile)
  }  
  
  getPDF <- TRUE
  
  ######################
  ## GET DATA         ##
  ######################
  print(data_tag)
  
  data_summary	     <- read.table(paste0(io_folder,data_tag,'_summary.csv',sep=''),header=TRUE,sep=",",stringsAsFactors=F)
  data_cases         <- read.table(paste(io_folder,data_tag,'_cases.csv',sep=''),header=FALSE,sep=",")
  data_person	       <- read.table(paste(io_folder,data_tag,'_person.csv',sep=''),header=TRUE,sep=",",stringsAsFactors=F)
  data_logfile       <- read.table(new_logfile,header=FALSE,sep=" ",stringsAsFactors=F)
  
  names(data_logfile) <- c('tag','infector_id','infected_id','cluster','day')
  cluster_levels <- c("household","school","work","primary_community","secondary_community")
  cluster_levels_proportion <- c(7,5,5,2,5)
  
  data_logfile$cluster_cat <- factor(data_logfile$cluster,cluster_levels)
  data_logfile[data_logfile==-1] <- NA
  
  if(getPDF)
  pdf(paste0(io_folder,'plot_', data_tag,'_outbreaks.pdf'))
  
  names(data_person)
  #data_person <- data_person[data_person$age < 85,]
  
  popsize <- dim(data_person)[1]
  pop_age <- table(data_person$age)
  tmp_tbl_seroprev <- table(data_person$age,data_person$is_immune)
  colnames(tmp_tbl_seroprev)
  
  # copy tbl into matrix with immunity status 0 and 1
  # required for populations without any immunity...
  tbl_seroprev <- matrix(0,nrow=length(pop_age),ncol=2)
  tbl_seroprev[,as.double(colnames(tmp_tbl_seroprev))+1] <- tmp_tbl_seroprev
  
  # COMPARE WITH CURRENT PROFILE => EXTRA VACCINATION ?
  require(XML)
  data <- xmlParse('../data/disease_measles.xml')
  xml_data <- xmlToList(data)
  imm_subset <- xml_data[[7]]
  flag_reg    <- which(names(imm_subset) == 'current')
  age_imm_reg <- as.numeric(unlist(imm_subset[[flag_reg]]))
  
  age_imm_scenario <- tbl_seroprev[,2]/pop_age
  diff_imm_scenario <- age_imm_scenario-age_imm_reg

  par(mfrow=c(1,2))
  par(mar=c(5.1,4.1,2.1,4.1))
  num_ticks <- 4
  vacc_plot_ftr <- 5
  
  plot(matrix(tbl_seroprev[,2]/pop_age),main='age-specific immunity',xlab='age',ylab='seroprevalence',ylim=c(0,1),type='l')
  lines(age_imm_reg,col=2,lwd=4)
  lines((diff_imm_scenario)*vacc_plot_ftr,col=3)
  lines(matrix(tbl_seroprev[,2]/pop_age),lwd=2)
  axis(4,c(0:(num_ticks+1))*0.1*vacc_plot_ftr/(num_ticks+1),(0:(num_ticks+1))*0.1/(num_ticks+1),col.axis="green",col="green",cex.axis=0.8)
  abline(h=0.1,lty=3)
  abline(h=0.2,lty=3)
  abline(h=0.3,lty=3)
  mtext('Extra vaccination coverage', side=4, line=3, cex.lab=1, col="green")
  legend('right',c('extra vaccines',paste(round(sum(pop_age * (age_imm_scenario))- sum(pop_age * (age_imm_reg))))),cex=0.8)
  legend('top',c('current','scenario'),col=2:1,lty=1,cex=0.8,ncol = 2)
  
  ## INCIDENCE AND SECONDARY CASES
  
  all_infections <- table(data_person$age[data_person$is_recovered==T])
  all_infections_ages <- as.double(names(all_infections))
  all_incidence <- all_infections / pop_age[all_infections_ages+1]
  sum(all_infections)
  
  data_infections <- merge(data_logfile,data_person,by.x='infected_id',by.y='id')
  dim(data_infections)
  names(data_infections)
  all_infections        <- table(data_infections$age)
  seeded_infections     <- table(data_infections$age[is.na(data_infections$infector_id)])
  secundary_infections  <- table(data_infections$age[!is.na(data_infections$infector_id)])
  
  hist_breaks <- -1:99
  hist_ages   <- 0:99
  total_infections      <- hist(data_infections$age,hist_breaks,plot=F)$counts
  seeded_infections     <- hist(data_infections$age[is.na(data_infections$infector_id)],hist_breaks,plot=F)$counts
  secundary_infections  <- hist(data_infections$age[!is.na(data_infections$infector_id)],hist_breaks,plot=F)$counts
  
  total_incidence  <- total_infections / pop_age
  seeded_incidence <- seeded_infections / pop_age
  secundary_incidence <- secundary_infections / pop_age
  
  y_max <- max(total_incidence,0.01,na.rm=T)
  plot(hist_ages,total_incidence,main='age-specific incidence',xlab='age',ylab='recovered',ylim=c(0,y_max),type='l',yaxt='n')
  axis(2)
  lines(seeded_incidence,col=4)
  lines(hist_ages,total_incidence)
  
  plot_legend <- c(paste('prim. cases', sum(seeded_infections)),
                   paste('immunity', format(sum(data_person[,4]) / popsize,digits=4)),
                   paste('sec. cases', sum(secundary_infections)),
                   paste('sec. attack rate', format(sum(secundary_infections) / popsize,digits=2)),
                   paste('sec. cases <1y', format(secundary_infections[1],digits=0))
  )
  legend('topright',plot_legend,cex=0.8)
  legend('right',c('total incidence', 'infect. seeds'),col=c(1,4),lty=1,cex=0.8)
  
  par(mar=c(5.1,4.1,4.1,2.1))
  
  ## CLUSTER IMMUNITY AND INCIDENCE
  
  pop_file <- data_summary$pop_file
  data_pop <- read.table(file=paste0('../data/',pop_file),sep=',',header=T)  
  names(data_pop)
  opt_clusters <- c('household_id','school_id', 'work_id', 'primary_community','secundary_community')
  for(col_tag in opt_clusters)
  {
    cluster_size <- table(data_pop[,col_tag])
    num_clusters <- max(data_pop[,col_tag])#length(cluster_size)
    
    flag <- data_person$age > 0
    hh_num_total <- hist(data_pop[flag,col_tag],breaks=0:num_clusters,plot=F)$counts
    hh_num_immune <- hist(data_pop[flag & data_person$is_immune==1,col_tag],breaks=0:num_clusters,plot=F)$counts
    hh_num_recovered <- hist(data_pop[flag & data_person$is_recovered==1,col_tag],breaks=0:num_clusters,plot=F)$counts
    
    hist((hh_num_immune / hh_num_total)[hh_num_total > 1],xlab='immunity',main=paste(col_tag,'immunity\n(size>1)'))
    boxplot((hh_num_immune / hh_num_total)[hh_num_total > 1],main=paste(col_tag,'immunity\n(size>1)'),ylim=0:1)
  
  }
  
  ## OUTBREAKS
  
  cluster_levels <- c("household","school","work","primary_community","secondary_community")
  cluster_levels_proportion <- c(7,5,5,2,5)
  
  data_logfile$cluster_cat <- factor(data_logfile$cluster,cluster_levels)
  data_logfile[data_logfile==-1] <- NA
  
  infector <- table(data_logfile$infector_id)
  infector_id <- names(infector)
  
  barplot(table(infector),xlab='secundary cases / infector',ylab='count',main='sec. cases',ylim=c(0,2000))
  barplot(table(infector)/length(infector),xlab='secundary cases / infector',ylab='frequency',main='sec. cases',ylim=c(0,1))
  
  
  ## SECUNDARY CASES OVER TIME
  infector_data <- data.frame(infected_id=names(infector),sec_cases=as.double(infector))
  infector_data <- merge(infector_data,data_logfile[,c('infected_id','day')])
  
  par(mfrow=c(1,1))
  boxplot(infector_data$sec_cases ~ infector_data$day,xlab='time own infection (days)',ylab='secundary cases / infected')
  
  
  ## OUTBREAKS
  if(sum(infector) < 20000)
  {
    ## primary cases
    data_logfile$outbreak <- 0
    outbreak_id <- 1
  
    flag <- is.na(data_logfile$infector_id)
    num_outbreaks <- sum(flag)
    data_logfile$outbreak[flag] <- 1:num_outbreaks
    for(outbreak_id in 1:num_outbreaks)
    {
      infctrs <- data_logfile$infected_id[data_logfile$outbreak == outbreak_id]
      num_infctrs_prev <- 0
      
      while(length(infctrs) > num_infctrs_prev)
      {
        num_infctrs_prev <- length(infctrs)
        case_flag <- data_logfile$infector_id %in% infctrs
        sum(case_flag)
        data_logfile$outbreak[case_flag] <- outbreak_id
        
        infctrs <- data_logfile$infected_id[data_logfile$outbreak == outbreak_id]
      }
    }
    
    table(table(data_logfile$outbreak))
   
    ## OUTBREAK SIZE
    tmp <- table(table(data_logfile$outbreak))
    tmp_size <- as.double(names(tmp))
    tmp_frac <- as.numeric(tmp/max(data_logfile$outbreak))
    
    par(mfrow=c(1,2))
    plot_out <- matrix(ncol=max(tmp_size))
    # plot_out[tmp_size] <- tmp_frac
    plot_out[tmp_size] <- tmp
    plot_out <- as.numeric(plot_out)
    barplot(plot_out,xlab="outbreak size",main='outbreak size',ylab='fraction',ylim=c(0,20))
    axis(1)
    legend('topright',c('total number',paste(sum(plot_out,na.rm=T))))
    
    
    ## OUTBREAK SIZE SELECTION
    plot_out <- as.numeric(plot_out[2:min(40,max(tmp_size))])
    barplot(plot_out,xlab="outbreak size",main='outbreak size (2-50)',ylab='fraction',ylim=c(0,5))
    axis(1)
    legend('topright',c('total number',paste(sum(plot_out,na.rm=T))))
    
    plot_out <- matrix(ncol=max(tmp_size))
    plot_out[tmp_size] <- tmp_frac
    plot_out <- as.numeric(plot_out)
    barplot(plot_out,xlab="outbreak size",main='outbreak size\nRELATIVE',ylab='fraction',ylim=c(0,1))
    axis(1)
    
    ## OUTBREAK SIZE SELECTION
    plot_out <- as.numeric(plot_out[2:min(40,max(tmp_size))])
    barplot(plot_out,xlab="outbreak size",main='outbreak size (2-50)\nRELATIVE',ylab='fraction',ylim=c(0,0.25))
    axis(1)
    
    
    ## CLUSTER
    
    plot_cluster_context <- function(flag,tag='')
    {
      tbl_cluster <- table(data_logfile$cluster_cat[flag])
      tbl_cluster_proportion <- tbl_cluster/cluster_levels_proportion
      barplot(tbl_cluster/sum(tbl_cluster),las=1,hor=T,cex.names=0.8,xlab='freq.',ylim=c(6,0.5),main=paste('transmission context\nTOTAL',tag),xlim=c(0,0.5))
      barplot(tbl_cluster_proportion/sum(tbl_cluster_proportion),las=1,hor=T,cex.names=0.8,xlab='freq.',ylim=c(6,0.5),main=paste('transmission context\nDAILY',tag),xlim=c(0,0.5))
      
    }
    
    # ALL
    plot_cluster_context(data_logfile$outbreak>-1)
    
    ## specific selections
    outbreak_size <- table(data_logfile$outbreak)
    selection <- names(outbreak_size[outbreak_size >= 2 & outbreak_size < 4])
    plot_cluster_context(data_logfile$outbreak %in% selection, '(2 <= size < 4)')
    
    selection <- names(outbreak_size[outbreak_size >= 4 & outbreak_size < 10])
    plot_cluster_context(data_logfile$outbreak %in% selection, '(4 <= size < 10)')
    
    selection <- names(outbreak_size[outbreak_size >= 10 ])
    plot_cluster_context(data_logfile$outbreak %in% selection, '(10 <= size)')
    
    
    
    data_logfile_sorted <- data_logfile[order(data_logfile$outbreak),]
    all_infected_id <- data_logfile_sorted$infected_id
    num_part <- length(all_infected_id)
  
  par(mfrow=c(1,1))
  ## TRANSMISSION TREE
  if(num_part < 5000)
  {
      num_days <- 150
      
      plot(c(1,num_days),c(1,num_part),col=0,xlab='time (days)',ylab='individual',main='transmission tree')
      #data_logfile$infected_id[is.na(data_logfile$infected_id)] <- -1
      i<- 4
      for(i in 1:num_part)
      {
        
        i_day      <- data_logfile_sorted$day[i]
        
        if(is.na(data_logfile_sorted$infector_id[i]))
        {
          i_infector <- NA
          i_infector_day <- NA
          i_cluster <- 1
        } else {
          i_infector <- which(all_infected_id %in% data_logfile_sorted$infector_id[i])
          i_infector_day <- data_logfile_sorted$day[i_infector]
          i_cluster  <- as.numeric(data_logfile_sorted$cluster_cat[i])
        }
        points(i_day,i,pch=22,col=i_cluster,bg=i_cluster,cex=0.8)
        lines(c(i_infector_day,i_day),c(i_infector,i),type='l',pch=23,col=i_cluster,bg=i_cluster)
        
      }
      legend('bottomright',levels(data_logfile$cluster_cat),col=1:length(data_logfile$cluster_cat),lty=1)
  
    } # if num_part < 5000
  } # if sum(infected) < 20000
  
  
  hist(data_person$start_symptomatic-data_person$start_infectiousness,xlab='days infectious, not symptomatic',main='')
  
  
  # location over time
  names(data_logfile)
  cluster_time <- table(data_logfile$cluster,data_logfile$day)
  cluster_time[cluster_time==0] <- NA
  plot_days <- as.double(colnames(cluster_time))
  plot(c(0,max(plot_days)),c(0,max(cluster_time,na.rm = T)),col=0,xlab='days',ylab='cases')
  
  i <- 1
  for(i in 1:dim(cluster_time)[1])
  lines(plot_days,cluster_time[i,],col=i,type='b',pch=20)
  legend('topleft',rownames(cluster_time),col=1:i,pch=20)
  
  
  if(getPDF)
  dev.off()
  


  }

## RUN CODE
for(data_tag in opt_data_tag)
{
  plot_immunity(data_tag)
}



