#include "server.hpp"
#define RED "\001\e[0;31m\002"
#define GRN "\001\e[0;32m\002"
using namespace std;
int main(int arg, char **argv)
{
    if (arg < 3)
    {
        cout << RED"Bad arguments\n";
        cout << GRN"Use ircserv <port> <password>\n";
        return 0;
    }
    int port = atoi(argv[1]);
    Server *a = new Server("10.21.32.106", port,argv[2],4096);
    a[0].startServer();
    delete a;
}