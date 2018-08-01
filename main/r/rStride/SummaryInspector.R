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

explore_input_output_behavior <- function(project_dir)
{
  # load project output summary
  project_summary <- .rstride$load_project_summary(project_dir)
  
  # retrieve all variable model parameters
  input_opt     <- .rstride$get_variable_model_param(project_summary)
  
  # calculate a scale factor to add a second y-axis [cases vs. incidence]
  axis_factor <- median(project_summary$num_cases / project_summary$AR)
  axis_scale  <- round(diff(range(project_summary$num_cases)/axis_factor)/5,digits=4)
  
  # OPEN PDF STREAM
  pdf(file.path(project_dir,'parameter_exploration.pdf'))
  
  # loop over the changing input parameters => plot cases and incidence
  #par(mfrow=c(2,2))
  par(mar = c(5, 4, 4, 4) + 0.3)  # Leave space for 3rd axis
  for(i in 1:length(input_opt)){
    boxplot(num_cases ~ project_summary[,names(input_opt)[i]],
            data = project_summary,
            xlab = names(input_opt)[i],
            ylab = '')
    axis(4, at = seq(0,1,axis_scale)*axis_factor , labels =seq(0,1,axis_scale) )
    mtext("incidence", side=4, line=2,cex=0.9)
    mtext("number of cases", side=2, line=2,cex=0.9)
  }
  
  dev.off()
  
  # terminal message
  .rstride$cli_print('INPUT-OUTPUT EXPLORATION COMPLETE')
  
}


## HELP FUNCTION
.rstride$get_variable_model_param <- function(project_summary){
  
  col_output <- c('run_time', 'total_time', 'num_cases', 'AR' )
  col_extra  <- c('rng_seed','output_prefix','transmission_rate') 
  col_input  <- !(names(project_summary) %in% c(col_output,col_extra))
  
  input_opt     <- lapply(project_summary[col_input],unique)
  input_opt     <- input_opt[lapply(input_opt,length)>1]

  return(input_opt)

}