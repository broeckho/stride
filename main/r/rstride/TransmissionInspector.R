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
# EXPLORE TRANSMISSION EVENTS AND THE (BASIC) REPRODUCTION NUMBER          ##
#############################################################################

explore_transmission <- function(project_dir)
{
  
  # load project summary
  project_summary <- .rstride$load_project_summary(project_dir)
  
  # retrieve all variable model parameters
  input_opt     <- .rstride$get_variable_model_param(project_summary)
  
  # open PDF stream
  pdf(file.path(project_dir,'transmission_exploration.pdf'))
  par(mfrow=c(2,2))
  
  # explore all project experiments
  i_exp <- 1
  for(i_exp in 1:nrow(project_summary))
  {
    # load transmission data
    load(file.path(project_summary$output_prefix[i_exp],'data_transmission.RData'))
    
    names(data_transm)
    data_transm$cnt_location[data_transm$cnt_location == 'primary_community'] <- 'prim comm'
    data_transm$cnt_location[data_transm$cnt_location == 'secondary_community'] <- 'sec comm'
    
    # rename the cnt location for the "seed infected"
    data_transm$cnt_location [data_transm$cnt_location == -1] <- NA
    
    # INCIDENCE
    tbl_transm <- table(data_transm$sim_day)
    incidence <- data.frame(day = as.numeric(names(tbl_transm)),
                            count = as.numeric(tbl_transm))
    plot(incidence$day,incidence$count,xlim=c(0,project_summary$num_days[i_exp]),
         xlab='time',ylab='incidence')
    
    # add some key param to the legend
    legend('topleft',paste(names(input_opt),project_summary[i_exp,names(input_opt)]))
    
    # LOCATION
    num_transm_events <- nrow(data_transm)
    barplot(table(data_transm$cnt_location)/num_transm_events,las=2,
            main='tranmission context',ylab='relative incidence')
    
    tbl_loc_transm <- table(data_transm$sim_day,data_transm$cnt_location)
    tbl_loc_transm[tbl_loc_transm==0] <- NA
    
    incidence_days      <- as.numeric(row.names(tbl_loc_transm))
    incidence_day_total <- rowSums(tbl_loc_transm,na.rm = T)
    
    plot(c(0,project_summary$num_days[i_exp]*1.5),0:1,col=0,xlab='day',ylab='relative daily incidence')
    for(i_loc in 1:ncol(tbl_loc_transm)){
      lines(incidence_days, tbl_loc_transm[,i_loc]/incidence_day_total,col=i_loc,lwd=4)
    }
    legend('topright',c(colnames(tbl_loc_transm)),col=1:ncol(tbl_loc_transm),lwd=4)
    
    
    # REPRODUCTION NUMBER
    tbl_infections <- table(data_transm$local_id)
    sec_transm <- data.frame(local_id = as.numeric(names(tbl_infections)),
                             sec_cases = as.numeric(tbl_infections))
    
    infection_time <- data.frame(local_id = data_transm$new_infected_id,
                                 infection_day = data_transm$sim_day)
    
    sec_transm <- merge(infection_time,sec_transm,all=T)
    sec_transm$sec_cases[is.na(sec_transm$sec_cases)] <- 0
    boxplot(sec_cases ~ infection_day, data = sec_transm,outline = F)
    
  }
  
  # close PDF stream
  dev.off()
  
  # command line message
  .rstride$cli_print('EXPLORE TRANSMISSION FINISHED')
  
} # function end


#############################################################################
# ESTIMATE TRANSMISSION PROBABILITY PARAMETERS TO CALLIBRATE THE MODEL R0  ##
#############################################################################

callibrate_r0 <- function(project_dir)
{
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
  flag <- sec_transm_all$infection_day == 0
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
  
  # R0 limit
  R0_limit <- -fit_b1^2/(4*fit_b2) 

  # Reformat fitted values to plot
  R2_poly2 <- round(mod$r.squared,digits=4)
  
  poly_input   <- sort(temp$x)
  R0_poly_fit  <- .rstride$f_poly_r0(poly_input,fit_b0,fit_b1,fit_b2)
  
  # open pdf stream
  pdf(file.path(project_dir,'fit_r0.pdf'))
  
  boxplot(round(sec_transm_all$sec_cases,digits=3) ~ sec_transm_all$transmission_rate, 
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
  plot(unlist(config_disease$start_infectiousness),xlab='days',ylab='probability',main='start_infectiousness')
  plot(unlist(config_disease$time_infectious),xlab='days',ylab='probability',main='time_infectious')
  plot(unlist(config_disease$start_symptomatic),xlab='days',ylab='probability',main='start_symptomatic')
  plot(unlist(config_disease$time_symptomatic),xlab='days',ylab='probability',main='time_symptomatic')
  
  prob_start_inf  <- as.numeric(unlist(config_disease$start_infectiousness))
  prob_dur_inf    <- as.numeric(unlist(config_disease$time_infectious))
  prob_start_symp <- as.numeric(unlist(config_disease$start_symptomatic))
  prob_dur_symp   <- as.numeric(unlist(config_disease$time_symptomatic))
  
  diff(prob_start_inf)
  n_sim <- 100
  start_inf  <- sample(1:length(prob_start_inf),n_sim,prob=c(0,diff(prob_start_inf)),replace = T)
  dur_inf    <- sample(1:length(prob_dur_inf),n_sim,prob=c(0,diff(prob_dur_inf)),replace = T)
  start_symp <- sample(1:length(prob_start_symp),n_sim,prob=c(0,diff(prob_start_symp)),replace = T)
  dur_symp   <- sample(1:length(prob_dur_symp),n_sim,prob=c(0,diff(prob_dur_symp)),replace = T)
  
  all_inf <- matrix(0,n_sim,30)
  all_symp <- matrix(0,n_sim,30)
  for(i in 1:n_sim){
    all_inf[i,(1:dur_inf[i])+start_inf[i]] <- 1
    all_symp[i,(1:dur_symp[i])+start_symp[i]] <- 1
    
  }
  
  par(mfrow=c(1,1))
  plot(colMeans(all_inf),ylab='fraction',xlab='day',type='l',lwd=3,col=2)
  lines(colMeans(all_symp),lwd=3,col=4)
  legend('topleft',c('infectious','symptomatic'),col=c(2,4),lwd=4,cex=0.8)
  
  R0_poly_orig <- .rstride$f_poly_r0(poly_input,
                                     as.numeric(config_disease$transmission$b0),
                                     as.numeric(config_disease$transmission$b1),
                                     as.numeric( config_disease$transmission$b2))
  
  plot(poly_input,R0_poly_orig,type='l',lwd=2,xlab='transmission probability',ylab='secundary cases')
  lines(poly_input,R0_poly_fit,col=3,lwd=2)
  legend('topleft',c('original fit','new fit'),col=c(1,3),lwd=1)
  
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
  num_realisations            <- unique(table(project_summary[,names(par_exp_design)]))
  
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
                               num_realisations        = num_realisations
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

