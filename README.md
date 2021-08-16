# AgreementTesting
  The program correspond to the algorithm for testing agreement of phylogentic trees (internal-labeled and leaf-labeled).  
  The algorithm firstly appeared in ISBRA2020:  
  [1] Fern ́andez-Baca, D., Liu, L.: Testing the agreement of trees with internal labels. In: Cai, Z., Mandoiu, I., Narasimhan, G., Skums, P., Guo, X. (eds.)    Bioinformatics Research and Applications, pp. 127–139. Springer, Cham (2020). 
  An experimental study using this program is curretnly in review.

# Usage
  You need to firstly build the program if this step is necessary.  
  
  To run the program:  

  using ./ImprovedAgreementTesting -i <inputfile> -o <outputfile> -p <true/false>   
  when you have an input file containing a set of phylogenetic source trees with full path <inputfile>.  
  If the source trees agree, then it will be outputed on both terminal and output file you specified.  
  -p flag is used for turning on/off multi-level HDT. Value of false is recommended for efficiency.  

  if you are interested in using built-in agreement profile generator, you may use:  
  ./ImprovedAgreementTesting -s [number of labels] [number of input trees] [maximum degree of a node]. 
  example: ./ImprovedAgreementTesting -s 127 7 3  

# Contact
  The program worked fine in our various experiments. But it may not work correctly if it is used in unexpected situation.  
  If you have any concern about it, please contact authors of the paper list above for bug fixing.  
