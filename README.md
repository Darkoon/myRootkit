# myRootkit
Simple linux kernel mode rootkit made as a project for OS Software lecture on Gdańsk University of Technology.

Functionalities:
- hiding module in lsmod listing
- hiding module objects
- using NetFilter interface to filter out connection from fixed IP address (now harcoded "192.168.1.20")
- using userhelper API to fire User mode thread: ping 212.77.100.18 (wp.pl, favourite site of polish developers :))
  
The main goal of rootkit: hide yourself, wait for signal from hacker machine, then participate in DDOS attack. 

It's very naive version of such rootkit/backdoor, just for sake of learning some Linux OS internals and how module managment work.

TODO: 
- hiding ping log in kernel messages
- make OS stabe after rootkit activation
