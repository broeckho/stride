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
  
  # retrieve all variable model parameters
  input_opt_design   <- .rstride$get_variable_model_param(project_summary)
  col_intervention   <- colnames(input_opt_design)
  
  # age-specific data => use "transmission data"
  data_transmission <- load(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_data_transmission.RData')))
  data_transmission <- get(data_transmission)
  
  # age-specific data => use "transmission data"
  data_intervention <- load(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_data_vaccination.RData')))
  data_intervention <- get(data_intervention)
  
  # get cost-benefit parameters
  cea_param <- get_cea_param()
  
  # burden of disease: age specific
  data_transmission$age_cat <- cut(data_transmission$part_age,cea_param$age_cat,right=F)
  cases_total_age <- table(data_transmission$exp_id,data_transmission$age_cat)
  cases_hosp_age  <- cases_total_age * cea_param$hosp_rate
  cases_deaths    <- cases_total_age * cea_param$cfr_per_case

  # costs & effects
  cost_direct     <- cases_total_age * cea_param$direct_cost_case
  qaly_disease    <- cases_total_age * cea_param$QALY_lost
  qaly_deaths     <- cases_total_age    * cea_param$life_expectancy
  
  # program costs
  num_vaccines    <- hist(data_intervention$exp_id,c(0:max(project_summary$exp_id)),plot=F)$counts
  cost_program    <- num_vaccines * (cea_param$price_dose+cea_param$price_admin_dose)
  
  # total cost
  cost_total <- rowSums(cost_direct) + cost_program

  # total effect
  qaly_total <- rowSums(qaly_disease + qaly_deaths)
  
  # compare
  project_summary$scenario <- as.numeric(as.factor(project_summary[,col_intervention]))
  qaly_gain     <- qaly_total[project_summary$scenario == 1] - qaly_total[project_summary$scenario == 2]
  incr_cost     <- cost_total[project_summary$scenario == 2] - cost_total[project_summary$scenario == 1]
  
  icer <-  incr_cost /  qaly_gain
  
  pdf(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_ce_plane.pdf')))
  plot(qaly_gain,incr_cost)
  abline(h=0,lty=2)
  abline(v=0,lty=2)
  dev.off()
  mean(incr_cost/qaly_gain)

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
  
  # # convert case fatality rate
  # data_cea$cfr_per_case <- data_cea$cfr_per100000/1e5
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

