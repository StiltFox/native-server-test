#ifndef StiltFox_UniversalLibrary_Socket
#define StiltFox_UniversalLibrary_Socket
#include <netinet/in.h>
#include "HttpMessage.hpp"
class Connection
{
    int handle;

    public:
    Connection(int handle);
    HttpMessage blockingReceiveData();
    void sendData(HttpMessage data);
    ~Connection();
};

class Socket
{
    int socketHandle;
    int queue;
    sockaddr_in address;
    
    public:
    Socket(int portNumber, int queueSize = 3);
    bool listenPort();
    Connection openConnection();
    void closePort();
    ~Socket();
};
#endif