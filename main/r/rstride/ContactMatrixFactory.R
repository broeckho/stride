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
#  Copyright 2018, Willem L, Kuylen E & Broeckhove J
#############################################################################
# 
# R script to pre-process the social contact rates into Stride xml format
#
#############################################################################

# load (xml) help functions
source('./bin/rstride/misc.R')

if(0==1) # for debugging
{
 #  source('./Misc.R')
 # .rstride$set_wd()

    ref_data_tag <- 'ref_fl2010_15min'
    generate_social_contact_data_file(ref_data_tag)
  # if(grepl('15touch',exp_summary$age_contact_matrix_file)){
  #   ref_data_tag <- 'ref_fl2010_15touch'
  # }
}

#############################################################################
# GENERATE XML FILE WITH SOCIAL CONTACT PATTERNS                           ##
#############################################################################

generate_social_contact_data_file <- function(ref_data_tag){
  
  ref_data_tag
  data_dir <- './data'
  output_prefix <- paste0('contact_matrix_',ref_data_tag)
  output_prefix <- sub('ref_','',output_prefix)
  
  
 cnt_matrices_opt <- paste0(ref_data_tag,
                            c('_regular_weekday_household_gam_mij_rec.csv',
                           '_regular_weekday_school_student_age24_gam_mij_median.csv',
                           '_regular_weekday_workplace_employed_age_gam_mij_median.csv',
                           '_regular_weekday_community_gam_mij_rec.csv',
                           '_weekend_community_gam_mij_rec.csv'))
 
 cnt_context_opt <- c('household',
                   'school',
                   'work',
                   'secondary_community',
                   'primary_community')
  

  # setup XML doc (to add prefix)
  xml_doc = newXMLDoc()
  
  cnt_matrix_xml  <- newXMLNode("matrices", doc = xml_doc)
  cnt_matrix_meta <- newXMLNode("raw_data_files", parent = cnt_matrix_xml)
  i_context <- 1
  for(i_context in 1:length(cnt_matrices_opt))
  {
    print(cnt_matrices_opt[i_context])
    
    # add input file name
    cnt_meta_data           <- newXMLNode(cnt_context_opt[i_context], parent=cnt_matrix_meta)
    xmlValue(cnt_meta_data) <- cnt_matrices_opt[i_context]
    
    # add contact data
    survey_mij <- read.table(file=file.path(data_dir,cnt_matrices_opt[i_context]),sep=';',dec=',',header=T)
    
    cnt_context <- newXMLNode(cnt_context_opt[i_context], parent=cnt_matrix_xml)
    for(i in 1:nrow(survey_mij)){
      
      participant <- newXMLNode("participant",parent=cnt_context)
      
      part_age  <- newXMLNode("age",parent=participant)
      xmlValue(part_age) <- paste(i)
      
      contacts  <- newXMLNode("contacts",parent=participant)
      
      for(j in 1:ncol(survey_mij)){
        contact  <- newXMLNode("contact",parent=contacts)
        age <- newXMLNode("age", parent=contact);
        xmlValue(age) <- j
        rate <- newXMLNode("rate", parent=contact);
        xmlValue(rate) <- paste(survey_mij[i,j])
      }
    }
  }
  
  # create filename
  filename <- paste0(output_prefix,'.xml')
  
  # xml prefix
  xml_prefix <- paste0(' This file is part of the Stride software [', format(Sys.time()), ']')
  
  # save as XML,
  # note: if we use an XMLdoc to include prefix, the line break dissapears...
  # fix: http://r.789695.n4.nabble.com/saveXML-prefix-argument-td4678407.html
  cat( saveXML( xml_doc, indent = TRUE, prefix = newXMLCommentNode(xml_prefix)),  file = filename) 
  
}

