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
#  Copyright 2019, Willem L, Kuylen E & Broeckhove J
#############################################################################

#############################################################################
# ESTIMATE COST-EFFECTIVENESS  ##
#############################################################################

calculate_cost_effectiveness <- function(project_dir){
  
  # load project output summary
  project_summary    <- .rstride$load_project_summary(project_dir)
  
  # get cost-benefit parameters
  cea_param <- get_cea_param()
  
  # retrieve all variable model parameters
  input_opt_design   <- .rstride$get_variable_model_param(project_summary)
  col_intervention   <- colnames(input_opt_design)
  
  # age-specific data => use "transmission data"
  data_transmission <- load(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_data_transmission.RData')))
  data_transmission <- get(data_transmission)
  
  # select secundary cases
  data_transmission <- data_transmission[!is.na(data_transmission$infector_id),]
  
  # burden of disease: age specific
  data_transmission$age_cat   <- cut(data_transmission$part_age,cea_param$age_cat,right=F)
  tbl_cases_total_age         <- as.data.frame(table(data_transmission$exp_id,data_transmission$age_cat))
  cases_total_age             <- data.frame(matrix(tbl_cases_total_age$Freq,nrow=max(data_transmission$exp_id)))
  names(cases_total_age)      <- paste0('cases_age_',levels(data_transmission$age_cat))
  cases_total_age$exp_id      <- 1:nrow(cases_total_age)
  
  # casefinding => use "vaccination data"
  datafile_intervention <- file.path(project_dir,paste0(project_summary$run_tag[1],'_data_vaccination.RData'))
  if(file.exists(datafile_intervention)){
   data_intervention <- load(datafile_intervention)
   data_intervention <- get(data_intervention)
  } else{
    data_intervention <- data.frame(exp_id=0)
  }
  
  tmp_tbl <- data.frame(table(data_intervention$exp_id))
  names(tmp_tbl) <- c('exp_id','num_vaccines')

  # CREATE SUMMARY STATISTICS PER RUN
  outbreak_summary <- data.frame(exp_id       = project_summary$exp_id,
                                 population_size     = project_summary$population_size,
                                 seeding_rate = project_summary$seeding_rate,
                                 intervention = as.numeric(as.factor(project_summary[,col_intervention])))
  
  outbreak_summary <- merge(outbreak_summary,tmp_tbl,all=T)
  outbreak_summary$num_vaccines[is.na(outbreak_summary$num_vaccines)] <- 0
  
  outbreak_summary$num_inf_seeds <- outbreak_summary$seeding_rate * outbreak_summary$population_size
  
  outbreak_summary <- merge(outbreak_summary,cases_total_age)

  std_err <- function(x) sd(x)/sqrt(length(x))
  
  outbreak_std_err <- aggregate(. ~ intervention, data=outbreak_summary,std_err)
  outbreak_mean    <- aggregate(. ~ intervention, data=outbreak_summary,mean)
  
  num_samples <- 10000
  hist(rnorm(num_samples,outbreak_mean$`cases_age_[0,5)`[1],outbreak_std_err$`cases_age_[0,5)`[1]),seq(-1,20,0.1),freq = F,xlim=c(0,10),ylim=0:1)
  hist(rnorm(num_samples,outbreak_mean$`cases_age_[0,5)`[2],outbreak_std_err$`cases_age_[0,5)`[2]),seq(-10,20,0.1),freq = F,add=T,col=2)
  
  hist(outbreak_summary$`cases_age_[0,5)`[outbreak_summary$intervention==1],seq(0,15,1),freq = F,xlim=c(0,15),ylim=0:1)
  hist(outbreak_summary$`cases_age_[0,5)`[outbreak_summary$intervention==2],seq(0,15,1),freq = F,add=T,col=2)
  
  # burden of disease: age specific
  data_transmission$age_cat <- cut(data_transmission$part_age,cea_param$age_cat,right=F)
  cases_total_age <- table(data_transmission$exp_id,data_transmission$age_cat)
  
  cases_hosp_age  <- cases_total_age * cea_param$hosp_rate
  cases_deaths    <- cases_total_age * cea_param$cfr_per_case

  # costs & effects
  cost_direct     <- cases_total_age * cea_param$direct_cost_case
  qaly_disease    <- cases_total_age * cea_param$QALY_lost
  qaly_deaths     <- cases_deaths * cea_param$life_expectancy
  
  # program costs
  num_vaccines    <- hist(data_intervention$exp_id,c(0:max(project_summary$exp_id)),plot=F)$counts
  cost_program    <- num_vaccines * (cea_param$price_dose+cea_param$price_admin_dose)
  
  # total cost
  cost_total <- rowSums(cost_direct) + cost_program

  # total effect
  qaly_total <- rowSums(qaly_disease + qaly_deaths)
  
  
  # compare
  project_summary$scenario <- as.numeric(as.factor(project_summary[,col_intervention]))
  cea_out <- data.frame(cost_total,
                   qaly_total,
                   scenario = project_summary$scenario,
                   rng_seed = project_summary$rng_seed)
  
  cea_out <- merge(cea_out[cea_out$scenario == 1,],cea_out[cea_out$scenario == 2,],
                    by='rng_seed',suffixes = c("_scen1","_scen2"))

  head(cea_out)
  qaly_gain     <- cea_out$qaly_total_scen1 - cea_out$qaly_total_scen2
  incr_cost     <- cea_out$cost_total_scen2 - cea_out$cost_total_scen1
  
  icer <-  incr_cost /  qaly_gain
  
  pdf(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_ce_plane.pdf')))
  plot(qaly_gain,incr_cost)
  abline(h=0,lty=2)
  abline(v=0,lty=2)
  dev.off()

  
}



get_cea_param <- function()
{
  
  # source: Carabin et al. (2003) The cost of measles in industrialised countries
  data_cea <- list( reporting_prob     = 0.75,      # Carabin et al 2003 (Upper Income Country)
                    severe_case_prob   = 0.8,       # Carabin et al 2003 (Upper Income Country)
                    hospital_stay_days = 4,         # Carabin et al 2003 (Upper Income Country)
                    QALY_lost          = 6.8/365,   # Thorrington et al 2014 (UK,
                          
                    age_cat            = c(0  , 5,   10, 15,20, 100), # Beutels & Gay 2003 
                    cfr_per100000      = c(18 , 9.6, 32, 87.2, 92.6), # Beutels & Gay 2003 
                    hosp_rate          = c(5.8, 1.7, 1.7, 1.6, 5.3)/100, # Beutels & Gay 2003 
                    direct_cost_case   = c(123, 112, 110, 111,148), # Beutels & Gay 2003 
                    life_expectancy    = c(73,   68, 63, 58, 27),   # Beutels & Gay 2003 
                    price_dose         = 6.3,                       # Beutels & Gay 2003 
                    price_admin_dose   = 7.3)                       # Beutels & Gay 2003 
  
  # convert case fatality rate
  data_cea$cfr_per_case <- data_cea$cfr_per100000/1e5
  # 
  # # unit cost: 2001USD
  # data_cost2001 <- data.frame(cost_hospital_day = 253,      # Carabin et al 2003 (BE)
  #                             cost_gp_visit     = 16,       # Carabin et al 2003 (BE)
  #                             cost_mmr_delivery = 4.3,      # Carabin et al 2003 (BE)
  #                             cost_mmr_dose     = 5)        # Carabin et al 2003 (BE)
  # 
  # cpi_2001USD <- 81.20256846   #2001USDA
  # cpi_2017USD <- 112.4115573   #2017USD
  # conversion_2001USDA_2017EURO <- 0.80          #2017EURO == 2017USD
  # data_cost <- data_cost2001 / cpi_2001USD * cpi_2017USD * conversion_2001USDA_2017EURO
     
  return(data_cea)
}

get_averted_burden <- function(project_dir){
  
  # load project output summary
  project_summary    <- .rstride$load_project_summary(project_dir)
  
  # # get cost-benefit parameters
  # cea_param <- get_cea_param()
  # 
  # retrieve all variable model parameters
  input_opt_design   <- .rstride$get_variable_model_param(project_summary)
  col_intervention   <- colnames(input_opt_design)
  project_summary[,col_intervention]
  
  project_summary$intervention_factor <- as.factor(project_summary[,col_intervention])
  project_summary$intervention_id     <- as.numeric(project_summary$intervention_factor)
  intervention_legend <- paste0(col_intervention,'_',levels(project_summary$intervention_factor))
  
  # age-specific data => use "transmission data"
  data_transmission <- load(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_data_transmission.RData')))
  data_transmission <- get(data_transmission)
  
  # select secundary cases
  #data_transmission <- data_transmission[!is.na(data_transmission$infector_id),]
  
  # casefinding => use "vaccination data"
  datafile_intervention <- file.path(project_dir,paste0(project_summary$run_tag[1],'_data_vaccination.RData'))
  if(file.exists(datafile_intervention)){
    data_intervention <- load(datafile_intervention)
    data_intervention <- get(data_intervention)
  } else{
    data_intervention <- data.frame(exp_id=0)
  }
  
  tmp_tbl <- data.frame(table(data_intervention$exp_id))
  names(tmp_tbl) <- c('exp_id','num_vaccines')
  
  # count number of cases per index case
  outbreak_total_cases <- data.frame(table(data_transmission$id_index_case))
  names(outbreak_total_cases) <- c('id_index_case','total_cases')
  
  # get outbreak detals: experiment id and intervention
  outbreak_exp_id <- data.frame(unique(cbind(data_transmission$id_index_case,data_transmission$exp_id)))
  names(outbreak_exp_id) <- c('id_index_case','exp_id')
  outbreak_exp_id <- merge(outbreak_exp_id,project_summary[,c('exp_id','intervention_id')])
  
  # get vaccines per outbreak
  data_intervention
  # outbreak_total_vaccines <- data.frame(table(data_intervention$id_index_case))
  # names(outbreak_total_vaccines) <- c('id_index_case','total_vaccines')
  outbreak_total_vaccines <- data.frame(table(data_intervention$exp_id))
  names(outbreak_total_vaccines) <- c('exp_id','total_vaccines')
  
  # aggregate outbreak data
  outbreak_summary <- merge(outbreak_total_cases,outbreak_exp_id)
  outbreak_summary <- merge(outbreak_summary,outbreak_total_vaccines, all.x=T)
  outbreak_summary$total_vaccines[is.na(outbreak_summary$total_vaccines)] <- 0
  
  head(outbreak_summary)
  
  boxplot(total_cases~intervention_id,data=outbreak_summary)

  outbreak_summary_exp        <- aggregate(. ~ exp_id + intervention_id, data=outbreak_summary,sum) 
  outbreak_summary_index_case <- aggregate(. ~ id_index_case + intervention_id, data=outbreak_summary,sum) 
  
  boxplot(total_cases/52~intervention_id,data=outbreak_summary_exp)
  boxplot(total_cases~intervention_id,data=outbreak_summary_index_case)
  
  boxplot(total_vaccines/52~intervention_id,data=outbreak_summary_exp)
}

