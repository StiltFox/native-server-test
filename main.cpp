#include <iostream>
#include "Socket.hpp"
#define PORT 8080

using namespace std;

int main(int argc, char const* argv[])
{
	Socket listeningSocket(PORT);
	listeningSocket.listenPort();
	Connection inboundConnection = listeningSocket.openConnection();
	HttpMessage request = inboundConnection.blockingReceiveData();

	cout << "-------------------------" << endl 
		<< request.printAsRequest() << endl 
		<< "-------------------------" << endl;
	HttpMessage msg(200,{{"content-type","application/json"}},"{\"message\":\"You sent a " + request.getHttpMethodAsString() + " request!\"}");
	inboundConnection.sendData(msg);
	cout << msg.printAsResponse()<<endl;

	return 0;
}