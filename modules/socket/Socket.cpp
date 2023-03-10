#ifdef MAC
    #include <sys/types.h>
#endif
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Socket.hpp"

Connection::Connection(int handle)
{
    this->handle = handle;
}

int Connection::getHandle()
{
    return handle;
}

HttpMessage Connection::receiveData()
{
    return HttpMessage(handle, &read);
}

void Connection::sendData(HttpMessage data)
{
    std::string toSend = data.printAsResponse();
    send(handle, toSend.c_str(), toSend.size(), 0);
}

Connection::~Connection()
{
    close(handle);
    handle = -1;
}

inline bool setOptions(int handle)
{
    #ifdef MAC
        return true;
    #else
        int opt = 1;
        return setsockopt(handle, SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt)) >= 0;
    #endif
}

Socket::Socket(int portNumber, int queueSize)
{
    socketHandle = -1;
    queue = queueSize;
    address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(portNumber);
}

bool Socket::listenPort()
{
    bool output = false;

    if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) >= 0)
    {
        if (setOptions(socketHandle))
        {
            if (bind(socketHandle,(struct sockaddr*)&address,sizeof(address)) >= 0)
            {
                if(listen(socketHandle, queue) >= 0) output = true;
            }
        }
    }

    return output;
}

Connection* Socket::openConnection()
{
    int addrlen = sizeof(address);
    return new Connection(accept(socketHandle,(struct sockaddr*)&address,(socklen_t*)&addrlen));
}

void Socket::sendData(HttpMessage data)
{
    int addrlen = sizeof(address);
    if (connect(socketHandle, (struct sockaddr*)&address, (socklen_t)addrlen) >= 0)
    {
        std::string request = data.printAsRequest();
        send(socketHandle, request.c_str(), request.length(),0);
    }
}

void Socket::closePort()
{
    if (socketHandle > -1)
    {
        shutdown(socketHandle, SHUT_RDWR);
        socketHandle = -1;
    }
}

int Socket::getHandle()
{
    return socketHandle;
}

Socket::~Socket()
{
    closePort();
}