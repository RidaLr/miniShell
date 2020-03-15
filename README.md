# Mishell #

  __  __   _____        ____   _    _   _____   _       _      
 |  \/  | |_   _|     / ____| | |  | | |  ___| | |     | |     
 | \  / |   | |  __  | (___   | |__| | | |__   | |     | |     
 | |\/| |   | | |__|  \___ \  |  __  | |  __|  | |     | |     
 | |  | |  _| |_       ___) | | |  | | | |___  | |___  | |___ 
 |_|  |_| |_____|     |____/  |_|  |_| |_____| |_____| |_____|


===================== MISHELL ==============================
ADVANCED OPERATING SYSTEMS -PROJECT | ISTY 2020 IATIC4         
                 R I D A       L A K S I R                     
============================================================
	
	
This project aims to create a mini shell in c,
I implement a textual interface for the shell, to help the user enter the commands, if the user type CTRL+D the shell will quit (EOF),
i implement also two primitives ( cd and exit), using getcwd and chdir.
The user could execute any command ( ls, cat , rm ....) , this option is implemented using fork, execvp and waitpid.


#Compile the program #

to compile the program type this command : make


#Execute the program#

After compiling all the files you can type : ./main

#Requirements#

gcc compiler 
make
linux OS

#Installations #

gcc  : sudo apt-get install gcc
make : sudo apt-get install make

#Bonus #

The shell print the username retrieved from the system ( like : rida@ity), plus the current directory ( rida@isty:/home/MP1/ % ),
and this line is colored with a modern colors to give to the user a great user experience,

Also this shell offer a help to user ( to test : type just <cd>)
