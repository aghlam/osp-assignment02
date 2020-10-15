# OSP-assignment02
### Author  
Alan Lam s3436174
  
## Description  
Operating System Principles assignment 02 to implement a memory allocator which uses a 'First fit', 'Best fit' or 'Worst fit' strategy.  
All the code is contained in the main.cpp file and can execute one or all of the schedulers at once.  
The program takes an input file and will output data to a file. The output file will work best with a csv type file but can also be any other format.  

## Instructions  
- Make sure both Makefile and main.cpp are in the same folder  
- Run Makefile with 'make'  
- Execute program with './main \<strategy> \<input> \<output>'  
- Inputs for \<strategy> can be '-first', '-best' or '-worst'  
- \<input> is the name of the input file including extension if any  
- \<output> is the name of the output file including extension if any  

To run a First Fit strategy example:  
- \# ./main -first first-names.txt output.csv
