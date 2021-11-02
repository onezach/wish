# wish
Wish is currently a pretty basic shell emulator with a few unique features.
## General Implementation
Wish comes with two modes of operation: Interactive and Batch. Interactive mode is very similar to a normal shell. There is a constant "wish>" prompt that one can use to call any command within the /bin or /usr/bin directories. Alternatively, one could use the 'path' command to redirect to a path directory of their choice. There is also an included "loop" command and associated loop variable "$loop". For example:

    wish> loop 2 echo hello world $loop
    hello world 1
    hello world 2
    wish> 
 
Users also have the capability to view their wish history with the hist command. 

    wish> clear
    wish> hist
    loop 2 echo hello world $loop
    clear
    wish>

Users can also clear their history using the chist command. 

Finally, there is batch mode which is comparable to running a bash script. One simply needs to call wish, passing the batch file as an argument. 

    ./wish batch.txt

All wish needs is to be compiled and executed, and it can do most of a normal shell would otherwise be able to do. Future changes will enhance wish with better streamlined code and more customizability for the user. Revamping to this README will also happen soon. 
