# IoTproject
Simple IoT project meant to be simulated on a EBSimUnoEthCurses Arduino UNO simulator

This project represents a simple scenario with 4 nodes and a server. 2 nodes are sensors, and 2 are indicators. Whenever conditions coded in server.c are met, a message 
is sent to one of the nodes in order to change the status of LED or turn on/off the A/C. 

Server uses sockets to communicate with nodes, and all logic is within the server.c file. Nodes are only transmitting or receiving, and do not perform any operations themselves.

It also uses a custom-built libraries Zsut[name]. They were built by my proffesor at Warsaw Universtity of Technology to accomodate for some bugs and stability issues that may occur while using standard libraries in a simulated environment.
