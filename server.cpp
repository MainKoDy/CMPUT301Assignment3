#include <iostream>
#include <string>
#include<stdio.h>

using namespace std;

int main(int argc, char *argv[]) {


    string port;
    port = argv[1];
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
        return 0;
    }
}