#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <chrono>
#include <thread>
#include <vector>
#include <bits/stdc++.h>
#include "tands.h"

using namespace std;

int main(int argc, char *argv[]) {


    // Citation: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

    typedef std::chrono::high_resolution_clock Clock;
    string port;
    port = argv[1];
    timeval timeout;

    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    vector<string> listofTrans;
    vector<int> transNums;
    int numTrans = 0, indTrans = 0, totalNumTrans = 0;
    bool beginTrans = false;
    char totalTimeBuf[1024];
    char totalTransBuf[1024];

    map<string, int> mp;

    for(char c : port) {
        if (!isdigit(c) || &c == "-") {
            printf("Given port number is not a proper int. Exiting ...\n");
            return 0;
        }
    }
    if (((stoi(port)-64000) * (stoi(port)-5000)) > 0) {
        printf("Port not in range 5000 to 64000. Exiting ...\n");
        return 0;
    }
    else {
        // Open a server.

        int opt = true;  
        int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30 , activity, i, valread, sd;  
        int max_sd;  
        struct sockaddr_in address;  
            
        char buffer[1025];  //data buffer of 1K 
            
        //set of socket descriptors 
        fd_set readfds;  
        
        //initialise all client_socket[] to 0 so not checked 
        for (i = 0; i < max_clients; i++) {  
            client_socket[i] = 0;  
        }  
            
        //create a master socket 
        if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {  
            perror("socket failed");  
            exit(EXIT_FAILURE);  
        }  
        
        //set master socket to allow multiple connections , 
        //this is just a good habit, it will work without this 
        if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
            sizeof(opt)) < 0 ) {  
            perror("setsockopt");  
            exit(EXIT_FAILURE);  
        }  
        
        //type of socket created 
        address.sin_family = AF_INET;  
        address.sin_addr.s_addr = INADDR_ANY;  
        address.sin_port = htons(stoi(port));  
            
        //bind the socket to localhost port
        if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {  
            perror("bind failed");  
            exit(EXIT_FAILURE);  
        }  
        printf("Using port %i \n", stoi(port));  
            
        //try to specify maximum of 50 pending connections for the master socket 
        if (listen(master_socket, 50) < 0) {  
            perror("listen");  
            exit(EXIT_FAILURE);  
        }  
            
        //accept the incoming connection 
        addrlen = sizeof(address);  
        // puts("Waiting for connections ..."); 

        while(true) {  
            //clear the socket set 
            FD_ZERO(&readfds);  
        
            //add master socket to set 
            FD_SET(master_socket, &readfds);  
            max_sd = master_socket;  
                
            //add child sockets to set 
            for ( i = 0 ; i < max_clients ; i++) {  
                //socket descriptor 
                sd = client_socket[i];  
                    
                //if valid socket descriptor then add to read list 
                if(sd > 0)  
                    FD_SET( sd , &readfds);  
                    
                //highest file descriptor number, need it for the select function 
                if(sd > max_sd)  
                    max_sd = sd;  
            }  
            auto start = Clock::now(); // Start execution timer
            //wait for an activity on one of the sockets , timeout is NULL , 
            //so wait indefinitely 
            if (activity = select( max_sd + 1 , &readfds , NULL , NULL , &timeout) <= 0) {
                printf("SUMMARY \n");
                if (beginTrans == true) {
                    auto end = Clock::now();
                    double thisTime = (chrono::duration_cast<chrono::nanoseconds>(end - start).count())/1000000; // return current execution time
                    double timeElapsed = thisTime / 1000;
                    sprintf(totalTimeBuf, "%.2f", timeElapsed); 
                    double transPerSec = totalNumTrans / timeElapsed;
                    sprintf(totalTransBuf, "%.1f", transPerSec); 

                    
                    for(auto it = mp.begin(); it != mp.end(); ++it) {
                        cout << it->second << " transaction(s) from " << it->first << endl;
                    }

                    cout << totalTransBuf << " transactions/sec (" << totalNumTrans << "/" << totalTimeBuf << ")" << endl;
                }
                else {
                    printf("No transactions processed. \n");
                }

                return 0;
            };  


            beginTrans = true;
            start = Clock::now(); // Start execution timer
        
            if ((activity < 0) && (errno!=EINTR)) {  
                printf("select error");  
            }  
                
            //If something happened on the master socket, 
            //then its an incoming connection 
            if (FD_ISSET(master_socket, &readfds)) {  
                if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {  
                    perror("accept");  
                    exit(EXIT_FAILURE);  
                }  
                
                //inform user of socket number - used in send and receive commands 
                printf("New connection , socket fd is %d , ip is: %s , port: %i\n", new_socket, inet_ntoa(address.sin_addr), ntohs (address.sin_port));  
                    
                //add new socket to array of sockets 
                for (i = 0; i < max_clients; i++) {  
                    //if position is empty 
                    if( client_socket[i] == 0 ) {  
                        client_socket[i] = new_socket;  
                        printf("Adding to list of sockets as %d\n" , i);  
                            
                        break;  
                    }  
                }  
            }  
                
            //else its some IO operation on some other socket
            for (i = 0; i < max_clients; i++) {  
                sd = client_socket[i];  
                    
                if (FD_ISSET( sd , &readfds)) {  
                    //Check if it was for closing , and also read the 
                    //incoming message 
                    if ((valread = read( sd , buffer, 1024)) == 0) {  
                        //Somebody disconnected , get his details and print 
                        getpeername(sd , (struct sockaddr*)&address , \
                            (socklen_t*)&addrlen);  
                        printf("Host disconnected , ip %s , port %d \n" , 
                            inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                            
                        //Close the socket and mark as 0 in list for reuse 
                        close( sd );  
                        client_socket[i] = 0;  
                    } 

                        
                    //Echo back the message that came in 
                    else { 
                        char timeBuffer[1024];
                        char hostBuffer[1024];


                        

                        auto p1 = std::chrono::system_clock::now();

                        double thisTime = (chrono::duration_cast<chrono::nanoseconds>(p1.time_since_epoch()).count())/1000000; // return current execution time
                        double timeElapsed = thisTime / 1000;
                        sprintf(timeBuffer, "%.2f", timeElapsed);



                        gethostname(hostBuffer, 1024);

                        string recString = buffer;

                        // Transaction argument given
                        string recTrans = recString.substr(0, recString.find(' '));
                        recString = recString.substr(recString.find(' ')+1, recString.size());
                        const char* trans = recTrans.c_str();
                        // printf("%s Fuckshit \n", trans);

                        // Host given
                        string recHost = recString.substr(0, recString.find(' '));

                        
                        recString = recString.substr(recString.find(' ')+1, recString.size());
                        
                        const char* host = recHost.c_str();
                        // printf("%s Fuckshit \n", host);


                        // PID given
                        string recPid = recString.substr(0, recString.find(' '));
                        const char* pid = recPid.c_str();
                        // printf("%s Fuckshit \n", pid);


                        string hostnPid = recHost + "." + recPid;

                        // printf("%s\n", timeBuffer); 
                        printf("%s: # %i (T %i) from %s.%s \n", timeBuffer, numTrans, atoi(trans), host, pid);
                        
                        bool inList = false;
                        if (mp.count(hostnPid) == 0) {
                            // listofTrans.push_back(recHost);
                            // transNums.push_back(numTrans);
                            numTrans = 1;

                            // Create new map key, insert value.
                            mp.insert({hostnPid, numTrans});
                        }
                        else {
                            numTrans++;
                            // Find key in map, and update value.
                            auto it = mp.find(hostnPid);
                            it->second = numTrans; 
                            cout << hostnPid << " Fucklord jhonse" << endl;
                        }

                        totalNumTrans++;
                        
                        Trans(atoi(trans));
                        

                        p1 = std::chrono::system_clock::now();

                        thisTime = (chrono::duration_cast<chrono::nanoseconds>(p1.time_since_epoch()).count())/1000000; // return current execution time
                        timeElapsed = thisTime / 1000;

                        sprintf(timeBuffer, "%.2f", timeElapsed);

                        printf("%s: # %i (Done) from %s.%s \n", timeBuffer, numTrans, host, pid);

                        string result = timeBuffer;
                        result += ": Recv (D " + to_string(numTrans) + ") \n";

                        const char* sendBuf = result.c_str();

                        //send new connection greeting message 
                        if( send(new_socket, sendBuf, strlen(sendBuf), 0) != strlen(sendBuf) ) {  
                            perror("send");  
                        }  

                        // buffer[valread] = '\0';

                        // send(sd , buffer , strlen(buffer) , 0 );  
                    }  
                }  
            }  
        } 
        return 0;
    }
}