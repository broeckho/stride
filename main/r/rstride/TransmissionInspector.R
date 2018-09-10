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
# EXPLORE TRANSMISSION EVENTS, OUTBREAKS GENERATION INTERVALS AND R0       ##
#############################################################################
inspect_transmission_data <- function(project_dir)
{
  # check if project_dir exists
  if(.rstride$dir_not_present(project_dir)){
    return(-1)
  }

  # load project summary
  project_summary      <- .rstride$load_project_summary(project_dir)

  # retrieve all variable model parameters
  input_opt_design     <- .rstride$get_variable_model_param(project_summary)
  
  # open PDF stream
  pdf(file.path(project_dir,'transmission_exploration.pdf'),10,7)
  
  i_config <- 1
  for(i_config in 1:nrow(input_opt_design)){

    # reset figure arrangements... and start new plot
    par(mfrow=c(3,3))
    
    # load the transmission output subset, corresponding the 'input_opt_design' row
    flag_exp            <- .rstride$get_equal_rows(project_summary,input_opt_design[i_config,])
    data_transm         <- .rstride$load_aggregated_output(project_dir,'data_transmission',project_summary$exp_id[flag_exp])
    num_runs_exp        <- sum(flag_exp)
    dim(data_transm)
    
    # INCIDENCE
    tbl_transm <- table(data_transm$sim_day,data_transm$exp_id)
    tbl_transm_matrix <- matrix(c(tbl_transm),nrow=nrow(tbl_transm),dimnames=list(rownames(tbl_transm)))
    if(nrow(tbl_transm_matrix)==0) tbl_transm_matrix <- matrix(0,nrow=1,ncol=1,dimnames=list(0))
    boxplot(t(tbl_transm_matrix),at=as.numeric(rownames(tbl_transm_matrix)),
            xlab='time (days)',ylab='new cases per run',
            main='new cases (total)')
    
    legend_info <- paste('num. runs',num_runs_exp)
    if(nrow(input_opt_design)>0){ # add some param info to the legend
      legend_info <- c(legend_info,paste(colnames(input_opt_design),input_opt_design[i_config,]))
    }
    legend('topleft',legend_info,cex=0.8)
    
    # LOCATION
    data_transm$cnt_location[data_transm$cnt_location == 'household'] <- 'HH'
    data_transm$cnt_location[data_transm$cnt_location == 'primary_community'] <- 'com wknd'
    data_transm$cnt_location[data_transm$cnt_location == 'secondary_community'] <- 'com week'
    
    # rename the cnt location for the "seed infected"
    num_transm_events <- nrow(data_transm)
    tbl_location <- table(data_transm$cnt_location)
    if(nrow(tbl_location)==0) tbl_location <- matrix(0,nrow=1,ncol=1,dimnames=list(0))
    barplot(tbl_location/num_transm_events,las=2,
            main='tranmission context',ylab='relative incidence',
            ylim=c(0,0.5))

    tbl_loc_transm  <- table(data_transm$sim_day,data_transm$cnt_location)
    if(nrow(tbl_loc_transm)==0) tbl_loc_transm <- matrix(0,nrow=1,ncol=1,dimnames=list(0))
    tbl_loc_transm[tbl_loc_transm==0] <- NA

    incidence_days      <- as.numeric(row.names(tbl_loc_transm))
    incidence_day_total <- rowSums(tbl_loc_transm,na.rm = T)
    
    plot(range(incidence_days),c(0,1.4),col=0,xlab='day',ylab='relative daily incidence',
         main='transmission context over time',yaxt='n')
    axis(2,seq(0,1,0.2),cex.axis=0.9)
   
    for(i_loc in 1:ncol(tbl_loc_transm)){
      points(incidence_days, tbl_loc_transm[,i_loc]/incidence_day_total,col=i_loc,lwd=4,pch=19)
    }
    if(!any(is.null(colnames(tbl_loc_transm)))) {
      legend('topleft',c(colnames(tbl_loc_transm)),col=1:ncol(tbl_loc_transm),pch=19,cex=0.8,ncol=5)
    }
   
    # REPRODUCTION NUMBER
    # secundary cases per local_id
    tbl_infections <- table(data_transm$infector_id)
    data_infectors <- data.frame(infector_id  = as.numeric(names(tbl_infections)),
                                 sec_cases    = as.numeric(tbl_infections))

    # day of infection: case
    infection_time <- data.frame(local_id      = data_transm$local_id,
                                 infector_id   = data_transm$infector_id,
                                 infection_day = data_transm$sim_day)

    # day of infection: infector
    infector_time  <- data.frame(infector_id            = data_transm$local_id,
                                 infector_infection_day = data_transm$sim_day)

    infection_time$infector_id[is.na(infection_time$infector_id)] <- 0
    infector_time$infector_id[is.na(infector_time$infector_id)] <- 0
    
    # merge case and infector timings
    if(nrow(data_infectors)==0){
      infection_time$sec_cases <- 0
      infection_time$infection_day <- -1
      infection_time$infector_infection_day <- -1
    } else{
      infection_time <- merge(infection_time,infector_time)
    }

    # merge secundary cases with time of infection
    sec_transm <- merge(infection_time,data_infectors,all=T)
    sec_transm$sec_cases[is.na(sec_transm$sec_cases)] <- 0

    # plot
    plot_xlim <- range(c(0,sec_transm$infection_day),na.rm=T)
    boxplot(sec_cases ~ infection_day, data = sec_transm,outline = F,
            at=sort(unique(sec_transm$infection_day)), xlim=plot_xlim,
            xlab='day',ylab='secundary infections',
            main='reproduction number',
            ylim=c(0,30))

    ## GENERATION INTERVAL
    # note: the generation interval is the time between the infection time of an infected person and the infection time of his or her infector.
    # reference: Kenah et al (2007)
    # remove the generation intervals counted form the initial infected seed infections
    #sec_transm$infector_infection_day[sec_transm$infector_infection_day == -1] <- NA
    sec_transm$generation_interval <- sec_transm$infection_day - sec_transm$infector_infection_day

    gen_interval <- sec_transm[!is.na(sec_transm$generation_interval),]
    gen_interval[gen_interval$infection_day==18,]
    if(nrow(gen_interval)==0) gen_interval <- data.frame(matrix(rep(0,6),nrow=1)); names(gen_interval) <- names(sec_transm)
    boxplot(generation_interval ~ infection_day, data = gen_interval,outline = F,
            at=sort(unique(gen_interval$infection_day)),xlim=plot_xlim,
            xlab='day',ylab='generation interval [infection]',
            main='generation interval\n[infection]')
    if(unique(project_summary$track_index_case[flag_exp]) == 'true'){
      text(0,pos=4,'TRACK INDEX CASE ON == NO TERTIARY CASES')
    }

    boxplot(gen_interval$generation_interval, outline = T,
            ylab='generation interval [infection]',
            main='generation interval\n[infection]')
    if(unique(project_summary$track_index_case[flag_exp]) == 'true'){
      text(0,pos=3,'TRACK INDEX CASE ON == NO TERTIARY CASES')
    }
    
  ## OUTBREAKS
    data_outbreak             <- data_transm
    data_outbreak$outbreak_id <- NA
    flag <- is.na(data_outbreak$sim_day)
    data_outbreak$outbreak_id[flag] <- seq(sum(flag))
    data_outbreak_all <- NULL

    while(is.null(data_outbreak_all) || nrow(data_outbreak_all) < nrow(data_transm)){
      d_source <- data_outbreak[!is.na(data_outbreak$outbreak_id),c('local_id','outbreak_id')]
      d_sink   <- data_outbreak[is.na(data_outbreak$outbreak_id),names(data_outbreak) != 'outbreak_id']
      dim(d_source)
      dim(d_sink)
      
      names(d_source)[1] <- 'infector_id'
      data_outbreak      <- merge(d_sink,d_source,all.x = TRUE)
      dim(data_outbreak)

      data_outbreak_all <- rbind(data_outbreak_all,d_source)
      
    }

    names(data_outbreak_all)[1] <- 'local_id'
    names(data_transm)
    data_outbreak_all <- merge(data_outbreak_all,data_transm)
    dim(data_outbreak_all)

    outbreak_size_data <- c(table(data_outbreak_all$outbreak_id))
    outbreak_size_cat <- cut(outbreak_size_data,breaks=c(1,2,5,10,50,max(c(100,outbreak_size_data))),right = F)
    barplot(table(outbreak_size_cat)/length(outbreak_size_cat),
            ylab='fraction',
            xlab='outbreak size',
            main='outbreak size',
            las=2,
            ylim=0:1)
   
    # add some info in the legend
    num_infected_seeds <- max(data_outbreak_all$outbreak_id) / max(data_outbreak_all$exp_id)
    legend('top',c(paste('num. runs',num_runs_exp),paste('outbreaks / run',num_infected_seeds)),cex=0.8)

    # count / day
    data_outbreak_all$sim_day[is.na(data_outbreak_all$sim_day)] <- 0
    tbl_all <- table(data_outbreak_all$sim_day,data_outbreak_all$outbreak_id) # table
    if(nrow(tbl_all)==0) {
      tbl_all     <- matrix(0,nrow=1,dimnames=list(0))
      tbl_all_cum <- matrix(0,nrow=1,dimnames=list(0))
    } else{
      tbl_all_inv <- tbl_all[nrow(tbl_all):1,] # take the inverse for the cummulative sum
      tbl_all_cum <- apply(tbl_all_inv,2,cumsum)
    }
   
    outbreaks_over_time <- data.frame(day = as.numeric(rownames(tbl_all_cum)),
                                      count = rowSums(tbl_all_cum > 0))
    

    # plot the number of ongoing outbreaks over time
    plot(outbreaks_over_time$day,outbreaks_over_time$count,
         ylim=c(0,max(data_outbreak_all$outbreak_id)),
         xlab='day of last infection',ylab='count (oubtreaks)',
         main='number ongoing outbreaks over time')
    
    # plot new cases per outbreak over time
    tbl_all_matrix <- matrix(c(tbl_all),nrow=nrow(tbl_all))
    boxplot(t(tbl_all_matrix),at=as.numeric(rownames(tbl_all)),
            ylab='new cases/outbreak',
            main='new cases / outbreak',
            xlab='day',
            ylim=range(c(0,tbl_all+1)))
    legend('topleft',paste0('num. outbreaks = ',max(data_outbreak_all$outbreak_id)),cex=0.8)

    # cases by age
    names(data_outbreak_all)
    hist(data_outbreak_all$part_age,0:99,right = F,
         freq = F,xlab='participant age',
         ylim = c(0,0.2))  
  
    data_outbreak_tmp <- data_outbreak_all
    data_outbreak_tmp$sim_day[is.na(data_outbreak_tmp$sim_day)] <- -10
    
    boxplot(part_age ~ outbreak_id, data=data_outbreak_tmp,xlab='outbreak id',ylab='age')  
    boxplot(part_age ~ sim_day, data=data_outbreak_tmp,
            at=sort(unique(data_outbreak_tmp$sim_day)),
            xlab='time (days)',ylab='age',
            cex=0.8)  
    
    data_case_age <- cut(data_outbreak_all$part_age,c(0,1,5,10,15,20,25,30,100),right=F)
    barplot(table(data_case_age)/length(data_case_age),
            las=2,xlab='age',ylab='Fraction',cex.names=0.8,
            ylim=c(0,0.3))
  

    
    
    
  } # end for-loop to vary the input_opt_design

  # close PDF stream
  dev.off()

  # command line message
  .rstride$cli_print('INSPECTION OF TRANSMISSION DATA COMPLETE')

} # function end



