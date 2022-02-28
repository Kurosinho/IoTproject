# IoTproject
Simple IoT project meant to be simulated on a EBSimUnoEthCurses Arduino UNO simulator

This project represents a simple scenario with 4 nodes and a server. 2 nodes are sensors, and 2 are indicators. Whenever conditions coded in server.c are met, a message 
is sent to one of the nodes in order to change the status of LED or turn on/off the A/C. 

Server uses sockets to communicate with nodes, and all logic is within the server.c file. Nodes are only transmitting or receiving, and do not perform any operations themselves.
