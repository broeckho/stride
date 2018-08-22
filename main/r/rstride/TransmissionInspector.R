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
explore_outbreaks <- function(project_dir)
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
   pdf(file.path(project_dir,'health_exploration.pdf'),10,7)
   par(mfrow=c(3,3))

  for(i_config in 1:nrow(input_opt_design))
  {
    flag_exp <- .rstride$get_equal_rows(project_summary,input_opt_design[i_config,])
    project_summary_exp <- project_summary[flag_exp,]

    max_pop_size <- max(project_summary$population_size)
    id_factor <- 10^ceiling(log10(max_pop_size))

    # explore all project experiments
    i_exp <- 1
    data_transm <- foreach(i_exp = 1:nrow(project_summary_exp),.combine='rbind') %do%
    {
      # load transmission data
      load(file.path(project_summary_exp$output_prefix[i_exp],'data_transmission.RData'))

      # add run index
      data_transm$run_index <- i_exp

      # set local_id of initial seeding as NA
      data_transm$local_id[data_transm$local_id==-1] <- NA

      # adjust ids
      data_transm$local_id        <- data_transm$local_id + (i_exp*id_factor)
      data_transm$new_infected_id <- data_transm$new_infected_id + (i_exp*id_factor)

      data_transm
    }

    # INCIDENCE
    tbl_transm <- table(data_transm$sim_day,data_transm$run_index)
    tbl_transm_matrix <- matrix(c(tbl_transm),nrow=nrow(tbl_transm))
    boxplot(t(tbl_transm_matrix),at=as.numeric(rownames(tbl_transm)),
            xlab='time (days)',ylab='new cases',
            main='new cases (total)')
    
    # add some key param to the legend
    if(nrow(input_opt_design)>0){
      legend('topleft',paste(colnames(input_opt_design),input_opt_design[i_config,]))
    }
    
    # LOCATION
    data_transm$cnt_location[data_transm$cnt_location == 'primary_community'] <- 'prim comm'
    data_transm$cnt_location[data_transm$cnt_location == 'secondary_community'] <- 'sec comm'
    
    # rename the cnt location for the "seed infected"
    data_transm$cnt_location [data_transm$cnt_location == -1] <- NA

    num_transm_events <- nrow(data_transm)
    barplot(table(data_transm$cnt_location)/num_transm_events,las=2,
            main='tranmission context',ylab='relative incidence')

    tbl_loc_transm <- table(data_transm$sim_day,data_transm$cnt_location)
    tbl_loc_transm[tbl_loc_transm==0] <- NA

    incidence_days      <- as.numeric(row.names(tbl_loc_transm))
    incidence_day_total <- rowSums(tbl_loc_transm,na.rm = T)
    
    plot(range(incidence_days),0:1,col=0,xlab='day',ylab='relative daily incidence',
         main='transmission context over time')
    for(i_loc in 1:ncol(tbl_loc_transm)){
      points(incidence_days, tbl_loc_transm[,i_loc]/incidence_day_total,col=i_loc,lwd=4,pch=19)
    }
    legend('topleft',c(colnames(tbl_loc_transm)),col=1:ncol(tbl_loc_transm),lwd=4,cex=0.8)

    # REPRODUCTION NUMBER
    # secundary cases per local_id
    tbl_infections <- table(data_transm$local_id)
    data_infectors <- data.frame(local_id  = as.numeric(names(tbl_infections)),
                                 sec_cases = as.numeric(tbl_infections))

    # day of infection: case
    infection_time <- data.frame(local_id      = data_transm$new_infected_id,
                                 infector_id   = data_transm$local_id,
                                 infection_day = data_transm$sim_day)

    # day of infection: infector
    infector_time  <- data.frame(infector_id            = data_transm$new_infected_id,
                                 infector_infection_day = data_transm$sim_day)

    # merge case and infector timings
    infection_time <- merge(infection_time,infector_time)

    # merge secundary cases with time of infection
    sec_transm <- merge(infection_time,data_infectors,all=T)
    sec_transm$sec_cases[is.na(sec_transm$sec_cases)] <- 0

    # plot
    plot_xlim <- range(c(0,sec_transm$infection_day),na.rm=T)
    boxplot(sec_cases ~ infection_day, data = sec_transm,outline = F,
            at=sort(unique(sec_transm$infection_day)), xlim=plot_xlim,
            xlab='day',ylab='secundary infections',
            main='reproduction number')

    ## SERIAL INTERVAL
    sec_transm$serial_interval <- sec_transm$infection_day - sec_transm$infector_infection_day

    boxplot(serial_interval ~ infection_day, data = sec_transm,outline = F,
            at=sort(unique(sec_transm$infection_day)), xlim=plot_xlim,
            xlab='day',ylab='serial interval [infection]',
            main='serial interval\n[infection]')
    
    boxplot(sec_transm$serial_interval, outline = F,
            ylab='serial interval [infection]',
            main='serial interval\n[infection]')

  ## OUTBREAKS
    data_outbreak <- data_transm
    data_outbreak$outbreak_id <- NA
    flag <- data_outbreak$sim_day == -1
    data_outbreak$outbreak_id[flag] <- seq(sum(flag))
    data_outbreak_all <- NULL

    while(is.null(data_outbreak_all) || nrow(data_outbreak_all) < nrow(data_transm)){
      d_source <- data_outbreak[!is.na(data_outbreak$outbreak_id),]
      d_sink   <- data_outbreak[is.na(data_outbreak$outbreak_id),-6]
      dim(d_source)
      dim(d_sink)
      d_source <- d_source[,c(2,6)]
      names(d_source)[1] <- 'local_id'
      data_outbreak <- merge(d_sink,d_source,all.x = TRUE)
      dim(data_outbreak)

      sum(is.na(data_outbreak$outbreak_id))
      data_outbreak_all <- rbind(data_outbreak_all,d_source)
    }

    names(data_outbreak_all)[1] <- 'new_infected_id'
    names(data_transm)
    data_outbreak_all <- merge(data_outbreak_all,data_transm)
    dim(data_outbreak_all)

    boxplot(as.numeric(table(data_outbreak_all$outbreak_id)),
            ylab='outbreak size',
            main='outbreak size')

    # count / day
    tbl_all <- table(data_outbreak_all$sim_day,data_outbreak_all$outbreak_id) # table

    tbl_all_inv <- tbl_all[nrow(tbl_all):1,] # take the inverse for the cummulative sum
    tbl_all_cum <- apply(tbl_all_inv,2,cumsum)

    outbreaks_over_time <- data.frame(day = as.numeric(rownames(tbl_all_cum)),
                                      count = rowSums(tbl_all_cum > 0))

    # plot the number of ongoing outbreaks over time
    plot(outbreaks_over_time$day,outbreaks_over_time$count,
         xlab='time (days)',ylab='count (oubtreaks)',
         main='number ongoing outbreaks over time')

    # plot new cases per outbreak over time
    tbl_all_matrix <- matrix(c(tbl_all),nrow=nrow(tbl_all))
    boxplot(t(tbl_all_matrix),at=as.numeric(rownames(tbl_all)),
            ylab='new cases/outbreak',
            main='new cases/outbreak',
            xlab='day',
            ylim=range(c(0,tbl_all+1)))
    legend('topleft',paste0('count = ',ncol(tbl_all)))

  }

  # close PDF stream
  dev.off()

  # command line message
  .rstride$cli_print('OUTBREAK EXPLORATION COMPLETE')

} # function end


#############################################################################
# ESTIMATE TRANSMISSION PROBABILITY PARAMETERS TO CALLIBRATE THE MODEL R0  ##
#############################################################################

callibrate_r0 <- function(project_dir)
{
  
  # check if project_dir exists
  if(.rstride$dir_not_present(project_dir)){
    return(-1)
  }
  
  # terminal message
  .rstride$cli_print('START TO CALLIBRATE R0')
  
  ###############################
  ## PARALLEL SETUP            ##
  ###############################
  .rstride$start_slaves()
  
  ##################################
  ## REPRODUCTION NUMBER          ##
  ##################################

  project_summary <- .rstride$load_project_summary(project_dir)
  
  i_exp <- 1
  # get all results (in parallel)
  sec_transm_all <- foreach(i_exp=1:nrow(project_summary),.combine='rbind',.verbose=FALSE) %dopar%
  {
    
    # load transmission data
    load(file.path(project_summary$output_prefix[i_exp],'data_transmission.RData'))
    
    # count secundary infections
    tbl_infections <- table(data_transm$local_id,exclude = -1)
    sec_transm <- data.frame(local_id = as.numeric(names(tbl_infections)),
                             sec_cases = as.numeric(tbl_infections))
    
    # add infection time
    infection_time <- data.frame(local_id = data_transm$new_infected_id,
                                 infection_day = data_transm$sim_day)
    
    sec_transm <- merge(infection_time,sec_transm,all=T)
    sec_transm$sec_cases[is.na(sec_transm$sec_cases)] <- 0
    
    sec_transm$r0 <- project_summary$r0[i_exp]
    sec_transm$transmission_rate <- project_summary$transmission_rate[i_exp]
    
    return(sec_transm)
  }
  
  dim(sec_transm_all)
  
  # select index cases
  flag <- sec_transm_all$infection_day == -1
  sec_transm_all <- sec_transm_all[flag,]
  dim(sec_transm_all)
  
  # plot(sec_transm_all$r0,sec_transm_all$sec_cases)
  # boxplot(sec_cases ~ r0, data = sec_transm_all)
  # boxplot(sec_cases ~ transmission_rate, data = sec_transm_all)
  # boxplot(r0 ~ transmission_rate, data = project_summary,add =T ,col=2,border=2)
  
  # FIT SECOND ORDER POLYNOMIAL
  temp <- data.frame(x=sec_transm_all$transmission_rate, y=sec_transm_all$sec_cases)
  mod <- summary(lm(y ~ 0 + x + I(x^2), data= temp))
  mod
  
  # Get parameters
  fit_b0 <- 0
  fit_b1 <- mod$coefficients[1,1]
  fit_b2 <- mod$coefficients[2,1]
  
  
  # R0 limit: to prevent complex root values                
  R0_limit_fit           <- -fit_b1^2/(4*fit_b2)
  transmission_limit_fit <- min(1,.rstride$f_poly_transm(floor(R0_limit_fit),fit_b0,fit_b1,fit_b2))
  
  # R0 limit: prevent complex roots and transmission rates >1
  R0_limit <- .rstride$f_poly_r0(transmission_limit_fit,fit_b0,fit_b1,fit_b2)
  
  # Reformat fitted values to plot
  R2_poly2 <- round(mod$r.squared,digits=4)
  
  poly_input   <- sort(temp$x)
  R0_poly_fit  <- .rstride$f_poly_r0(poly_input,fit_b0,fit_b1,fit_b2)
  
  # open pdf stream
  pdf(file.path(project_dir,'fit_r0.pdf'))
  
  boxplot(round(sec_transm_all$sec_cases,digits=3) ~ round(sec_transm_all$transmission_rate,digits=2), 
          xlab='transmission probability',ylab='secundary cases',
          at=sort(round(unique(sec_transm_all$transmission_rate),digits=3)),
          xlim=range(sec_transm_all$transmission_rate),
          boxwex=0.005)
  
  lines(poly_input,R0_poly_fit,type='l',col=3,lwd=4)
  leg_text_bis <- paste0(c(paste0('b',0:2,': '),'R^2: ','R0 lim: '),round(c(fit_b0,fit_b1,fit_b2,mod$r.squared,R0_limit),digits=2))
  legend('topleft',legend=leg_text_bis,cex=0.8,title='b0+b1*x+b2*x^2',fill=3)
  
  boxplot(sec_transm_all$sec_cases ~ sec_transm_all$r0,
          xlab='R0',ylab='secundary cases',
          at=sort(unique(sec_transm_all$r0)),
          boxwex=0.5)
  abline(0,1,col=2,lwd=2)
  legend('topleft',legend='x=y',cex=0.8,title='reference',col=2,lwd=2)
  
  
  ###############################
  ## EXPLORE DISEASE FILE       ##
  ###############################
  
  # get disease config filename
  disease_config_file     <- unique(project_summary$disease_config_file)
  
  # load disease config file
  config_disease    <- xmlToList(file.path('data',disease_config_file))
  
  par(mfrow=c(2,2))
  plot(seq_len(length(config_disease$time_asymptomatic))-1,config_disease$time_asymptomatic,xlab='days',ylab='probability',main='time_asymptomatic')
  plot(seq_len(length(config_disease$time_infectious))-1,config_disease$time_infectious,xlab='days',ylab='probability',main='time_infectious')
  plot(seq_len(length(config_disease$start_symptomatic))-1,config_disease$start_symptomatic,xlab='days',ylab='probability',main='start_symptomatic')
  plot(seq_len(length(config_disease$time_symptomatic))-1,config_disease$time_symptomatic,xlab='days',ylab='probability',main='time_symptomatic')
  par(mfrow=c(1,1))
  
  R0_poly_orig <- .rstride$f_poly_r0(poly_input,
                                     as.numeric(config_disease$transmission$b0),
                                     as.numeric(config_disease$transmission$b1),
                                     as.numeric( config_disease$transmission$b2))
  
  plot(poly_input,R0_poly_orig,type='l',lwd=2,xlab='transmission probability',ylab='secundary cases')
  lines(poly_input,R0_poly_fit,col=3,lwd=2)
  legend('topleft',c('original fit','new fit'),col=c(1,3),lwd=1)
  

  
  ####################################
  ## DISEASE HISTORY MEASLES        ##
  ####################################
  
  if(exists('config_disease$label$pathogen') && config_disease$label$pathogen ==  'measles')
  {
    # # REF:Lesler et al (2009), Lancet Infect Dis
    # # incubation period: lognormal distribution with median 12.5 and dispersion 1.23
    # lognormal distribution,
    days_opt       <- c(0:170)/10                                              # setup time horizon with a small day-interval     
    lnorm_dist     <- dlnorm(days_opt, meanlog = log(12.5), sdlog = log(1.23)) # get lognormal densities
    cum_lnorm_dist <- cumsum(lnorm_dist)/sum(lnorm_dist)                       # get cumulative density
    cum_lnorm_dist <- round(cum_lnorm_dist,digits=2)                           # round 
    cum_lnorm_dist <- data.frame(day=days_opt,prob=cum_lnorm_dist)             # add time horizon 
    
    plot(cum_lnorm_dist$day,cum_lnorm_dist$prob,xlab='day',ylab='probability',main='start symptoms\n(Lesler et al. 2009)')
    
    abline(v=12.5); abline(h=0.5) # median
    abline(h=0.25); abline(v=10.9) # 25% percentile
    abline(h=0.75); abline(v=14.4) # 75% percentile
    
    prob_symp <- cum_lnorm_dist$prob[cum_lnorm_dist$day %in% 0:17]                # store probability for discrete time steps
    prob_symp[1:9] <- 0                                                           # set probability for day 0-8 to "0"
    
    lines(0:17,prob_symp,lwd=4,col=2)
    legend('topleft',c('lesler et al. 2009','STRIDE'),col=1:2,lwd=2)
    if(any(as.numeric(config_disease$start_symptomatic[1:18]) != prob_symp)){
      # command line message
      .rstride$cli_print('"START SYMPTOMATIC" NOT CONFORM LESLER ET AL 2009.')
      
      config_disease$start_symptomatic[1:18] <- prob_symp
      
      # command line message
      .rstride$cli_print('UPDATED "START SYMPTOMATIC" ACCORDING LESLER ET AL 2009.')
    }
    
  }
  
  
  # close pdf stream
  dev.off()
  
  ####################################
  ## UPDATE DISEASE CONFIG FILE     ##
  ####################################
 
  # update transmission param
  config_disease$transmission$b0 <- fit_b0
  config_disease$transmission$b1 <- fit_b1
  config_disease$transmission$b2 <- fit_b2
  
  # add/update meta data
  num_infected_seeds          <- unique(project_summary$seeding_rate * project_summary$population_size)
  par_exp_design              <- .rstride$get_variable_model_param(project_summary) # changing parameters in the exp design
  total_num_index_cases       <- nrow(sec_transm_all)
  num_rng_seeds               <- length(unique(project_summary$rng_seed))
  dim_exp_design              <- nrow(expand.grid(par_exp_design))
  num_realisations            <- unique(table(project_summary[,colnames(par_exp_design)]))
  
  # if the disease config file has only one value (original state), set value as pathogen
  if(length(config_disease$label) == 1) {
    config_disease$label <- list(pathogen = config_disease$label)
  }
  
  # add the population and contact file, together with the current date  
  config_disease$label <- list(pathogen = config_disease$label$pathogen,
                               population_file         = unique(project_summary$population_file),
                               age_contact_matrix_file = unique(project_summary$age_contact_matrix_file),
                               r0                      = unique(project_summary$r0),
                               start_date              = unique(project_summary$start_date),
                               num_infected_seeds      = num_infected_seeds,
                               total_num_index_cases   = total_num_index_cases,
                               num_rng_seeds           = num_rng_seeds,
                               dim_exp_design          = dim_exp_design,
                               num_realisations        = num_realisations,
                               fit_r0_limit            = round(R0_limit,digits=2) 
                              ) 
  # update filename
  disease_config_update_file <- sub('.xml','_updated',disease_config_file)
  
  # save
  new_disease_config_filename <- .rstride$save_config_xml(config_disease,'disease',file.path(project_dir,disease_config_update_file))
  
  # terminal message
  .rstride$cli_print('NEW DISEASE CONFIG FILE', new_disease_config_filename)
  
  ###############################
  ## TERMINATE PARALLEL NODES  ##
  ###############################
  .rstride$end_slaves()
  
  # command line message
  .rstride$cli_print('R0 CALLIBRATION FINISHED')
  
}

##################################
## POLYNOMIAL HELP FUNCTIONS    ##
##################################

.rstride$f_poly        <- function(x,b0,b1,b2) {b0 + b1*x + b2*(x^2)}
.rstride$f_poly_r0     <- function(x,b0,b1,b2) {b0 + b1*x + b2*(x^2)}
.rstride$f_poly_transm <- function(r0,b0,b1,b2) 
{
  c = b0 - r0
  b = b1
  a = b2
  
  d <- b^2 - (4 * a * c)
  
  x1 <- (-b + sqrt(d)) / (2*a)
  x2 <- (-b - sqrt(d)) / (2*a)
  
  return(x1)
}

