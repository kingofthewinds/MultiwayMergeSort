# Multiway Merge Sort : 
In this repository, you will find an implementation of the Multiway Merge Sort algorithms in 
c++ using mapped files. Tests are done on multiple files and the average is then considererd! 
The results will be updated below regularly !

#Mapped file performance : 
A 400mb file is first generated. The, mapping is done multiple times ith different 
buffer sizez. It'w worth noticing that the machine on which the tests are run has a
SSD drive so all the disk seek time etc. that apply to rotating disks don't apply here : 

| block size(mb)  | time to read and write (sec) |
| --------------  | ---------------------------- |
| 0.5             | 4.130                        |
| 1               | 3.976                        |
| 10              |  4.153                       |
| 100             | 4.104                        |
| 400             | 4.270                        |

as we can see, there is a minimum at 1mb ! 
(the page size being at 4 kilobytes)

#Building and Execution
1. To build, use the makefile with the following commend : <br/>                 make mws 
2. Then to execute, you need to supply three parameters in the following order : <br/>                ./mws [fileName] [m] [d]
   * name of the file to merge 
   * the size of the buffer to use (in number of ints)
   * the number of merges at each iteration 
 