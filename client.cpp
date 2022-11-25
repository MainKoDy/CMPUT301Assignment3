#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <chrono>
#include "tands.h"



using namespace std;

int main(int argc, char *argv[])
{
    char hostBuffer[1024], pidBuffer[1024];
    string port, ip;
    port = argv[1];
    ip = argv[2];
    const char* thisIp = ip.c_str();
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

    printf("Using port %i \n", stoi(port));
    printf("Using server address %s \n", thisIp);

    // Check if ip address is the machine that opened the server

    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(stoi(port));
 
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, thisIp, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    gethostname(hostBuffer, 1024);

    int thisPid = getpid();
    printf("Host %s.%i \n", hostBuffer, thisPid);

    int numTrans = 0;
    
    string line;

    while (getline(cin, line)) {
        if (line.size() == 0) { // If reached end of input, break.
            break;
        }
        if (line[0] == 'T') { // If given T<n>, execute a transaction with parameter n.

            string transTime;

            transTime = line.substr(1, line.size());
            const auto p1 = std::chrono::system_clock::now();

            double thisTime = (chrono::duration_cast<chrono::nanoseconds>(p1.time_since_epoch()).count())/1000000; // return current execution time
            double timeElapsed = thisTime / 1000;

            sprintf(buffer, "%.2f", timeElapsed);
            
            printf("%s: Send (T %i) \n", buffer, stoi(transTime));

            numTrans++;

            // Send the shit to server
        
            string hostBufStr = hostBuffer;
            const char *hostBuf = hostBufStr.c_str();
            sprintf( buffer, "%i %s %i ", stoi(transTime), hostBuf, thisPid);
            // printf("%s \n",buffer);

            send(sock, buffer, strlen(buffer), 0);
            // printf("Transaction sent\n");
            valread = read(sock, buffer, 1024);
            printf("%s \n", buffer);

            
        } else if (line[0] == 'S' ) { // If given S<n>, execute a transaction with parameter n.
            string sleepTime;
            sleepTime = line.substr(1, line.size());

            printf("Sleep %i units \n", stoi(sleepTime));

            Sleep(stoi(sleepTime));
        }

    }


    printf("Sent %i transactions \n", numTrans);

    close(client_fd);
}