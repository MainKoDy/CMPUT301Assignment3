#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <netinet/in.h> 
#include <sys/time.h>
#include <chrono>
#include <thread>
#include <vector>
#include <bits/stdc++.h>
#include "tands.h"

// Citation: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

using namespace std;

int main(int argc, char *argv[]) {

    // Initialize variables
    typedef std::chrono::high_resolution_clock Clock;
    string port;
    port = argv[1];
    timeval timeout;
    vector<string> listofTrans;
    vector<int> transNums;
    int indTrans = 0, totalNumTrans = 0;
    bool hasConnected = false;
    double sumTime = 0;
    char totalTimeBuf[1024];
    char totalTransBuf[1024];
    map<string, int> mp;

    // Check that given port is a non-negative integer.
    for(char c : port) {
        if (!isdigit(c) || &c == "-") {
            printf("Given port number is not a proper int, or is negative. Exiting ...\n");
            return 0;
        }
    }

    // CHeck that the port is in the range 5,000 and 64,000.
    if (((stoi(port)-64000) * (stoi(port)-5000)) > 0) {
        printf("Port not in range 5000 to 64000. Exiting ...\n");
        return 0;
    }
    else {

        // Open a server, initialize variables.
        int opt = true;  
        int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30 , activity, i, sd;  
        int max_sd;  
        struct sockaddr_in address;  
            
        char buffer[1025];  // Initialize data buffer to be of size 1025.
            
        // set of socket descriptors 
        fd_set readfds;  
        
        // initialise all client_socket[] to 0 so not checked 
        for (i = 0; i < max_clients; i++) {  
            client_socket[i] = 0;  
        }  
            
        // create a master socket 
        if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {  
            perror("socket failed");  
            exit(EXIT_FAILURE);  
        }  
        
        // set master socket to allow multiple connections.
        if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
            sizeof(opt)) < 0 ) {  
            perror("setsockopt");  
            exit(EXIT_FAILURE);  
        }  
        
        // type of socket created 
        address.sin_family = AF_INET;  
        address.sin_addr.s_addr = INADDR_ANY;  
        address.sin_port = htons(stoi(port)); // Set socket port to the port given. 
            
        // bind the socket to localhost port
        if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {  
            perror("bind failed");  
            exit(EXIT_FAILURE);  
        }  

        printf("Using port %i \n", stoi(port));  
            
        // try to specify maximum of 50 pending connections for the master socket.
        if (listen(master_socket, 50) < 0) {  
            perror("listen");  
            exit(EXIT_FAILURE);  
        }  
            
        // accept the incoming connection.
        addrlen = sizeof(address); 

        // Initialize and start the clock timers
        auto start = Clock::now();
        auto end = Clock::now();
    
        while(true) {  
            // clear the socket set 
            FD_ZERO(&readfds);  

            // Set the server timeout to be 30 seconds.
            timeout.tv_sec = 30;
            timeout.tv_usec = 0;
        
            // add master socket to set 
            FD_SET(master_socket, &readfds);  
            max_sd = master_socket;  
                
            // add child sockets to set 
            for ( i = 0 ; i < max_clients ; i++) {  

                
                // socket descriptor 
                sd = client_socket[i];  
                    
                // if valid socket descriptor, then add to the read list.
                if(sd > 0)  
                    FD_SET( sd , &readfds);  
                    
                // highest file descriptor number, need it for the select function 
                if(sd > max_sd)  
                    max_sd = sd;  
            }  
            // wait for an activity on one of the sockets , timeout is 30 seconds.
            if (activity = select( max_sd + 1 , &readfds , NULL , NULL , &timeout) <= 0) {

                // If timeout occurs, print the summary of the transactions.
                printf("\n");
                printf("SUMMARY \n");


                if (hasConnected == true) {
                    sprintf(totalTimeBuf, "%.2f", sumTime); // Get total time transactions took, to 2 decimal places.
                    double transPerSec = totalNumTrans / sumTime;
                    sprintf(totalTransBuf, "%.1f", transPerSec); // Get transaction / sec, to 3 decimal places.

                    
                    for(auto it = mp.begin(); it != mp.end(); ++it) { // Print out all keys and values pair from map
                        cout << it->second << " transaction(s) from " << it->first << endl;
                    }

                    mp.clear(); // Free the map, so it can start storing any new transactions.

                    cout << totalTransBuf << " transactions/sec (" << totalNumTrans << "/" << totalTimeBuf << ")" << endl;
                }
                else {

                    // If there have been no transactions given, print out message.
                    printf("No transactions received. \n");
                }

                return 0;
            }; 
                
            // If something happened on the master socket, then its an incoming connection.
            if (FD_ISSET(master_socket, &readfds)) {  
                if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {  
                    perror("accept");  
                    exit(EXIT_FAILURE);  
                }  
                    
                //add new socket to array of sockets 
                for (i = 0; i < max_clients; i++) {  
                
                    //if position is empty:
                    if( client_socket[i] == 0 ) {  
                        client_socket[i] = new_socket;           
                        break;  
                    }  
                }  
            }  
                
            //else its some IO operation on some other socket
            for (i = 0; i < max_clients; i++) {  
                sd = client_socket[i];  
                    
                if (FD_ISSET( sd , &readfds)) {  

                    // Read the incoming data:
                    read( sd , buffer, 1024);

                    // Initialize buffers to read the time sinch epoch, and the host that sent the message.
                    char timeBuffer[1024];
                    char hostBuffer[1024];

                    // Get the current time since epoch.
                    auto p1 = std::chrono::system_clock::now();
                    double thisTime = (chrono::duration_cast<chrono::nanoseconds>(p1.time_since_epoch()).count())/1000000; // return current execution time
                    double timeElapsed = thisTime / 1000;
                    sprintf(timeBuffer, "%.2f", timeElapsed);
                    gethostname(hostBuffer, 1024);  // Get the host name.

                    string recString = buffer;

                    // Transaction argument given
                    string recTrans = recString.substr(0, recString.find(' '));
                    recString = recString.substr(recString.find(' ')+1, recString.size());
                    const char* trans = recTrans.c_str();

                    // If the recieved transaction is "E", close the current socket.
                    if (recTrans == "E") {
                        close( sd );    
                        client_socket[i] = 0; 
                    }
                    else {
                        // Get the host name given.
                        string recHost = recString.substr(0, recString.find(' '));             
                        recString = recString.substr(recString.find(' ')+1, recString.size());
                        const char* host = recHost.c_str();

                        // Get the PID of the transaction given.
                        string recPid = recString.substr(0, recString.find(' '));
                        const char* pid = recPid.c_str();

                        // Initiailize key value to be stored in the map.
                        string hostnPid = recHost + "." + recPid;

                        totalNumTrans++;

                        printf("%s: # %i (T %i) from %s.%s \n", timeBuffer, totalNumTrans, atoi(trans), host, pid);

                        // If the host and PID value is not in the map, initialize a new key value pair to be stored in the map.
                        if (mp.count(hostnPid) == 0) {

                            hasConnected = true;
                            mp.insert({hostnPid, 1});
                        }
                        
                        // If it is in the map, then find the key in the map, and update value.
                        else {
                            auto it = mp.find(hostnPid);
                            it->second++; 
                        }

                        start = Clock::now(); // Start execution timer.

                        Trans(atoi(trans)); // Do the transaction.

                        end = Clock::now(); // End execution timer.

                        // Add the transaction time to the total time elapsed.
                        double ttimeNow = (chrono::duration_cast<chrono::nanoseconds>(end - start).count())/1000000;
                        double ttimeElapsed = ttimeNow / 1000;
                        sumTime += ttimeElapsed;
                        

                        p1 = std::chrono::system_clock::now();

                        thisTime = (chrono::duration_cast<chrono::nanoseconds>(p1.time_since_epoch()).count())/1000000; // return current execution time
                        timeElapsed = thisTime / 1000;

                        sprintf(timeBuffer, "%.2f", timeElapsed);

                        printf("%s: # %i (Done) from %s.%s \n", timeBuffer, totalNumTrans, host, pid);

                        string result = timeBuffer;
                        result += ": Recv (D " + to_string(totalNumTrans) + ")";

                        const char* sendBuf = result.c_str();

                        //send results back to the client.
                        send(sd, sendBuf, strlen(sendBuf), 0);                        
                    }
                }  
            }  
        } 
        return 0;
    }
}