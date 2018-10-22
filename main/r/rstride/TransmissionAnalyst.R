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
# ESTIMATE TRANSMISSION PROBABILITY PARAMETERS TO CALLIBRATE THE MODEL R0  ##
#############################################################################

analyse_transmission_data_for_r0 <- function(project_dir)
{
  
  # terminal message
  .rstride$cli_print('START TO CALLIBRATE R0')
  
  ##################################
  ## REPRODUCTION NUMBER          ##
  ##################################

  project_summary <- .rstride$load_project_summary(project_dir)
  
  # load the transmission output
  data_transm     <- .rstride$load_aggregated_output(project_dir,'data_transmission',project_summary$exp_id)

  # count secundary infections
  tbl_infections  <- table(data_transm$infector_id)
  sec_transm      <- data.frame(local_id = as.numeric(names(tbl_infections)),
                               sec_cases = as.numeric(tbl_infections))
  
  # add infection time
  infection_time <- data.frame(local_id      = data_transm$local_id,
                               infection_day = data_transm$sim_day,
                               exp_id        = data_transm$exp_id)
  
  sec_transm <- merge(infection_time,sec_transm,all=T)
  sec_transm$sec_cases[is.na(sec_transm$sec_cases)] <- 0
  
  # add R0 and transmission rate from the project_summary
  sec_transm <- merge(sec_transm,project_summary[,c('exp_id','r0','transmission_rate')])
    
  # select index cases
  flag       <- is.na(sec_transm$infection_day)
  sec_transm <- sec_transm[flag,]
  dim(sec_transm)
  
  # plot(sec_transm$r0,sec_transm$sec_cases)
  # boxplot(sec_cases ~ r0, data = sec_transm)
  # boxplot(sec_cases ~ transmission_rate, data = sec_transm)
  # boxplot(r0 ~ transmission_rate, data = project_summary,add =T ,col=2,border=2)
  
  # FIT SECOND ORDER POLYNOMIAL
  temp <- data.frame(x=sec_transm$transmission_rate, y=sec_transm$sec_cases)
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
  sec_transm$R0_poly_fit <- round(.rstride$f_poly_r0(sec_transm$transmission_rate,fit_b0,fit_b1,fit_b2),digits=1)
  
  # fix y-axis limits (default: 0-40)
  y_lim <- range(c(0,36,sec_transm$sec_cases))
  

  # open pdf stream
  run_tag <- unique(project_summary$run_tag)
  pdf(file.path(project_dir,paste0(run_tag,'_fit_r0.pdf')))
  
  boxplot(round(sec_transm$sec_cases,digits=3) ~ round(sec_transm$transmission_rate,digits=2), 
          xlab='transmission probability',ylab='secundary cases',
          at=sort(round(unique(sec_transm$transmission_rate),digits=3)),
          xlim=range(sec_transm$transmission_rate),
          ylim=y_lim,boxwex=0.005)
  
  lines(poly_input,R0_poly_fit,type='l',col=3,lwd=4)
  leg_text_bis <- paste0(c(paste0('b',0:2,': '),'R^2: ','R0 lim: '),round(c(fit_b0,fit_b1,fit_b2,mod$r.squared,R0_limit),digits=2))
  legend('topleft',legend=leg_text_bis,cex=0.8,title='b0+b1*x+b2*x^2',fill=3)
  
  boxplot(sec_transm$sec_cases ~ sec_transm$R0_poly_fit,
          xlab='model R0 (using new transmission parameters)',ylab='secundary cases',
          at=sort(unique(sec_transm$R0_poly_fit)),
          ylim=y_lim,boxwex=0.5)
  abline(0,1,col=2,lwd=2)
  legend('topleft',legend=leg_text_bis,cex=0.8,title='b0+b1*x+b2*x^2',fill=3)
  legend('topright',legend='x=y',cex=0.8,title='reference',col=2,lwd=2)
  
  
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
  
  # compare original and new fit
  R0_poly_orig <- .rstride$f_poly_r0(poly_input,
                                     as.numeric(config_disease$transmission$b0),
                                     as.numeric(config_disease$transmission$b1),
                                     as.numeric( config_disease$transmission$b2))
  
  plot(poly_input,R0_poly_orig,
       type='l',lwd=2,
       xlab='transmission probability',
       ylab='secundary cases',
       ylim=y_lim)
  lines(poly_input,R0_poly_fit,col=3,lwd=2)
  # add grid
  abline(v=seq(0,1,0.1),lty=3,col=9)
  abline(h=seq(0,max(y_lim),2),lty=3,col=9)
  # add legend
  legend('topleft',c('original fit','new fit'),
         col=c(1,3),lwd=1,bg= "white")
  
  
  
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
 
  variable_param <- .rstride$get_variable_model_param(project_summary)
  
  .rstride$get_unique_param_list(project_summary)
  lapply(project_summary,unique)
  ?lapply       
  
  # update transmission param
  config_disease$transmission$b0 <- fit_b0
  config_disease$transmission$b1 <- fit_b1
  config_disease$transmission$b2 <- fit_b2
  
  # add/update meta data
  num_infected_seeds          <- unique(project_summary$seeding_rate * project_summary$population_size)
  par_exp_design              <- .rstride$get_variable_model_param(project_summary) # changing parameters in the exp design
  total_num_index_cases       <- nrow(sec_transm)
  num_rng_seeds               <- length(unique(project_summary$rng_seed))
  dim_exp_design              <- nrow(expand.grid(par_exp_design))
  num_realisations            <- unique(table(project_summary[,colnames(par_exp_design)]))
  
  # if the disease config file has only one value (original state), set value as pathogen
  if(length(config_disease$label) == 1) {
    config_disease$label <- list(pathogen = config_disease$label)
  }
  
  # add all population, contact, disease, model parameters 
  config_disease$label <- unlist(list(pathogen           = config_disease$label$pathogen,
                               num_infected_seeds      = num_infected_seeds,
                               total_num_index_cases   = total_num_index_cases,
                               num_rng_seeds           = num_rng_seeds,
                               dim_exp_design          = dim_exp_design,
                               num_realisations        = num_realisations,
                               fit_r0_limit            = round(R0_limit,digits=2),
                               .rstride$get_unique_param_list(project_summary)
                              ),recursive = F)
  
  # update filename: add run_tag
  disease_config_update_file <- paste0(run_tag,'_',disease_config_file)
  # update filename: remove file extension
  disease_config_update_file <- sub('.xml','',disease_config_update_file)
  
  # save
  new_disease_config_filename <- .rstride$save_config_xml(config_disease,'disease',file.path(project_dir,disease_config_update_file))
  
  # terminal message
  .rstride$cli_print('NEW DISEASE CONFIG FILE', disease_config_update_file)
  
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

