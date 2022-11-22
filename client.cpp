#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    string port, ip;
    port = argv[1];
    ip = argv[2];
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

    // Check if ip address is the machine that opened the server
}