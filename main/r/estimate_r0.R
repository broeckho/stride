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
#  Copyright 2018, Willem L
#############################################################################
#
# ESTIMATE R0 FROM STRIDE SIMULATIONS
#
#############################################################################
#rm(list=ls())

if(0==1) # for debugging
{
  #setwd('..')
  project_dir <- c('./sim_output/20180503_165502/') 
  project_dir <- c('./sim_output/20180501_231948/', './sim_output/20180501_231255','./sim_output/20180501_220954') # all
  estimate_r0(project_dir)
}

estimate_r0 <- function(project_dir)
{
  
  all_data <- read.table(file = file.path(project_dir[1],'summary.csv'),header=T,stringsAsFactors = F,sep=',')
  
  if(length(project_dir)>1){
    for(i in 2:length(project_dir)){
      tmp_data <- read.table(file = file.path(project_dir[i],'summary.csv'),header=T,stringsAsFactors = F,sep=',')
      all_data <- rbind(all_data,tmp_data)
    }
  }
    
  all_data$infected_transmission_mean <- all_data$infected_transmission / all_data$infected_seed
  
  # open file stream
  pdf(file.path(project_dir[1],'estimate_R0.pdf'))
  
  # FIT
  temp <- data.frame(x=all_data$transmission_param_probability, y=all_data$infected_transmission_mean)
  mod <- summary(lm(y ~ 0 + x + I(x^2), data= temp))
  mod
  
  b0 <- 0
  b1 <- mod$coefficients[1,1]
  b2 <- mod$coefficients[2,1]
  
  R2_poly2 <- round(mod$r.squared,digits=4)
  
  poly_input   <- sort(temp$x)
  R0_poly_fit  <- f_poly_r0(poly_input,b0,b1,b2)
  
  table(poly_input)
  table(R0_poly_fit)
  
  #R0 limit
  R0_limit <- -b1^2/(4*b2) 

  boxplot(all_data$infected_transmission_mean ~ round(all_data$transmission_param_probability,digits=3),
          xlab='transmission probability',ylab='secundary cases',
          at=sort(round(unique(all_data$transmission_param_probability),digits=3)),
          xlim=range(all_data$transmission_param_probability),
          boxwex=0.005)
  
  lines(poly_input,R0_poly_fit,type='l',col=3,lwd=4)
  leg_text_bis <- paste0(c(paste0('b',0:2,': '),'R^2: ','R0 lim: '),round(c(b0,b1,b2,mod$r.squared,R0_limit),digits=2))
  legend('topleft',legend=leg_text_bis,cex=0.8,title='b0+b1*x+b2*x^2',fill=3)
  
  
  boxplot(all_data$infected_transmission_mean ~ all_data$r0,
          xlab='R0',ylab='secundary cases',
          at=sort(unique(all_data$r0)),
          boxwex=0.5)
  abline(0,1)
  
  flag <- grepl('part_rate',names(all_data))
  par(mar=c(5.1,10.1,4.1, 2.1))
  barplot(colMeans(all_data[,flag]),las=1,horiz = T,xlim=0:1)
  
  # close file stream
  dev.off()
    
} # end function: estimate_r0

#############################################################
## HELPER FUNCTIONS
r2prob <- function(r0,b0,b1)
{
  # from Simulator::Simulator function
  transmission_rate <- (r0-b0)/b1
  return(transmission_rate)
}

prob2r <- function(transmission_rate,b0,b1)
{
  # from Simulator::Simulator function
  r0 <- b0 + transmission_rate*b1 
  return(r0)
}

f_poly <- function(x,b0,b1,b2) {b0 + b1*x + b2*(x^2)}
f_poly_r0 <- function(x,b0,b1,b2) {b0 + b1*x + b2*(x^2)}
f_poly_transm <- function(r0,b0,b1,b2) {
  
  c = b0 - r0
  b = b1
  a = b2
  
  d <- b^2 - (4 * a * c)
  
  x1 <- (-b + sqrt(d)) / (2*a)
  x2 <- (-b - sqrt(d)) / (2*a)
  
  return(x1)
}
#############################################################





