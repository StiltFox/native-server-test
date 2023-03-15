//Here we start by doing something special. MacOs needs this header file to run
//however linux does not.
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

/*
* This method right here is small because HttpMessage does most of the heavy
* lifting.
* This method is how we receive requests from the client.
*/
HttpMessage Connection::receiveData()
{
    /*
    * We pass in the handle to our socket and the read function from sys/socket.h
    * with this information the HttpMessage will construct itself and return to the
    * caller.
    */
    return HttpMessage(handle, &read);
}

//Here is where we can respond to the client.
void Connection::sendData(HttpMessage data)
{
    std::string toSend = data.printAsResponse();
    send(handle, toSend.c_str(), toSend.size(), 0);
}

/*
* This is a deconstructor. Like a constructor it has no return type. In C++ we
* are responsible for managing our own memory. This means that there may be actions
* an object wants to take before being removed from the heap/stack. The deconstructor
* gets called under the following conditions:
* 1) the object was on the stack and goes out of scope.
* 2) delete is called on the object and the object is on the heap.
*
* A note about the heap and the stack:
* Stack memory and Heap memory both represent addresses in RAM memory that our program
* owns. However there is a critical difference between the two. Stack memory is objects
* and data that the program knows about. ie: a static list, a constant, a function
* variable. Stack memory is dealt with by the compiler and there is nothing  we need to
* do about it as programmers.
*
* Heap memory on the other hand represents unplanned or dynamic memory usage. Every time
* we call the new key word it allocates memory to the heap. In this instance you can think
* of RAM memory as a restaurant, and us allocating memory like reserving a seat. If you 
* reserve a seat, new guests wont be able to use it. The operating system will rope off
* that memory until you tell it you're done with it. You can let the operating system
* know that you're done using this memory by calling the delete operator on the pointer.
* when we delete memory we're calling the restaurant to tell them we're done with our
* reservation. Other customers can now use this table. If we fail to call and cancel,
* eventually the restaurant will fill up with empty seats, and no new customers can
* be served. This is called a memory leak. Always be sure to delete your allocated
* memory.
*/
Connection::~Connection()
{
    //here we close our TCP connection so the operating system can free up that
    //socket for someone else.
    close(handle);
    handle = -1; //it is good practice to null or negative handles when done with them.
}

inline bool setOptions(int handle)
{
    //Mac does not require these options and will error. So we just skip setting this
    //if we're on Mac.
    #ifdef MAC
        return true;
    #else
        int opt = 1;
        return setsockopt(handle, SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt)) >= 0;
    #endif
}

//This constructor is super simple and just sets a bunch of data.
Socket::Socket(int portNumber, int queueSize)
{
    socketHandle = -1;
    queue = queueSize;
    address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(portNumber);
}

//This function sets the socket into listening mode, and tells the operating
//system to reserve a port for us.
bool Socket::listenPort()
{
    bool output = false; //I like to define my output variable first. This
                         //helps me keep track of a function's entrance and
                         //exit.

    //Tell the operating system we want a tcp socket.
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

    return output; //if for any reason binding to the socket fails we return false
}

/*
* This function creates a connection between us and a potential client. WARNING:
* calling this function will block the thread until a connection is received or
*  an error occurs.
*
* A note on threads and blocking:
* When a thread is blocked it is put in a waiting state by the operating system
* until a trigger occurs. This means that no more code in that thread will run.
*/
Connection* Socket::openConnection()
{
    int addrlen = sizeof(address);
    return new Connection(accept(socketHandle,(struct sockaddr*)&address,(socklen_t*)&addrlen));
}

/*
* This send method sends a HttpMessage to the server. 
* This is different from the other send data as this method acts like a client,
* where as the send data on the connection object is more like a server replying.
*/
void Socket::sendData(HttpMessage data)
{
    int addrlen = sizeof(address);
    if (connect(socketHandle, (struct sockaddr*)&address, (socklen_t)addrlen) >= 0)
    {
        std::string request = data.printAsRequest();
        send(socketHandle, request.c_str(), request.length(),0);
    }
}

//This will close the port we're listening to and all connections we've made on it.
void Socket::closePort()
{
    if (socketHandle > -1) //make sure we're not already closed.
    {
        shutdown(socketHandle, SHUT_RDWR); //tell the OS that we're done with the socket.
        socketHandle = -1; //set this object to closed.
    }
}

int Socket::getHandle()
{
    return socketHandle;
}

//be sure to close the port if this object is ever freed.
Socket::~Socket()
{
    closePort();
}

/*
* This concludes the guided tour of the code base. There's still the stringmanip module
* but by now you should understand enough about this project and C++ to read it on your
* own.
*
* We hope you've enjoyed reading through our codebase.
* A big thank you from Stilt Fox™
*/