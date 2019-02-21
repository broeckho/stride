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
#
# MODEL SUMMARY EXPLORATION
# - input-output behavior
# - transmission events and context
#
#############################################################################

inspect_summary <- function(project_dir)
{
  
  # load project output summary
  project_summary    <- .rstride$load_project_summary(project_dir)
  
  # retrieve all variable model parameters
  input_opt_design   <- .rstride$get_variable_model_param(project_summary)
  
  # stop if there are no different input parameter values
  if(nrow(input_opt_design)<=1){
    
    # terminal message
    .rstride$cli_print('NO VARYING INPUT PARAMETERS IN THE SUMMARY DATA')
    
    # stop
    return(.rstride$no_return_value())
  }
  # calculate a scale factor and ticks for the second y-axis [cases vs. incidence]
  range_num_cases   <- range(project_summary$num_cases)
  ticks_cases       <- seq(range_num_cases[1],range_num_cases[2],diff(range_num_cases)/5) 
  
  r0_axis_factor     <- median(project_summary$num_cases / project_summary$AR)
  r0_axis_scale      <- diff(range_num_cases)/r0_axis_factor
  
  num_digits        <- ceiling(abs(log10(r0_axis_scale)))
  ticks_r0          <- round(ticks_cases/r0_axis_factor,digits=num_digits)
  
  # OPEN PDF STREAM
  .rstride$create_pdf(project_dir,'summary_inspection',10,7)
  
  # loop over the changing input parameters => plot cases and incidence
  #par(mfrow=c(2,2))
  par(mar = c(10, 4, 4, 4) + 0.3)  # Leave space for 3rd axis
  for(i in 1:ncol(input_opt_design)){
    boxplot(num_cases ~ project_summary[,colnames(input_opt_design)[i]],
            data = project_summary,
            xlab = colnames(input_opt_design)[i],
            ylab = '',cex.axis=0.8,las=2)
    axis(4, at = ticks_cases , labels = ticks_r0 )
    mtext("incidence", side=4, line=2,cex=0.9)
    mtext("number of cases", side=2, line=2,cex=0.9)
  }
  
  par(mar = c(10, 4, 1, 4) + 0.3)  # Leave space for 3rd axis
  boxplot(as.formula(paste("num_cases",paste(colnames(input_opt_design),collapse='+'),sep="~")),
          data = project_summary,
          ylab = '',
          las=2,
          cex.axis=0.8)
  axis(4, at = ticks_cases , labels = ticks_r0 )
  mtext("incidence", side=4, line=2,cex=0.9)
  mtext("total number of cases", side=2, line=2,cex=0.9)
  
  # exclude 'start date' from the overview, if present in the experimental design
  if(('start_date' %in% colnames(input_opt_design))){
    par(mar = c(10, 4, 1, 4) + 0.3)  # Leave space for 3rd axis
    param_selection <- colnames(input_opt_design)[!grepl('start_date',colnames(input_opt_design))]
    boxplot(as.formula(paste("num_cases",paste(param_selection,collapse='+'),sep="~")),
            data = project_summary,
            ylab = '',
            las=2,
            cex.axis=0.8)
    axis(4, at = ticks_cases , labels = ticks_r0 )
    mtext("incidence", side=4, line=2,cex=0.9)
    mtext("total number of cases", side=2, line=2,cex=0.9)
  }
  
  
  dev.off()
  
  # terminal message
  .rstride$cli_print('INSPECTION OF SUMMARY DATA COMPLETE')
  
}

## HELP FUNCTION
.rstride$get_variable_model_param <- function(project_summary){
  
  input_opt    <- .rstride$get_unique_param_list(project_summary)
  input_opt    <- input_opt[lapply(input_opt,length)>1]
  
  # get parameter combinations
  input_opt_design <- unique(project_summary[,names(input_opt)])
  
  # with only one parameter, convert vector into matrix
  if(length(input_opt)==1){
    input_opt_design <- as.matrix(data.frame(input_opt))
  }
  
  # with only identical parameters, use the r0
  if(length(input_opt)==0){
    input_opt_design <- as.matrix(data.frame(r0=unique(project_summary$r0)))
  }
  
  return(input_opt_design)
  
}

## HELP FUNCTION
.rstride$get_unique_param_list <- function(project_summary){
  
  col_output <- c('run_time', 'total_time', 'num_cases', 'AR' )
  col_extra  <- c('rng_seed','output_prefix','transmission_rate','exp_id') 
  col_input  <- !(names(project_summary) %in% c(col_output,col_extra))
  
  # get unique values per parameter
  input_opt    <- lapply(project_summary[,col_input],unique)
  
  # return
  return(input_opt)
}
