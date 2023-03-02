#include <iostream>
#include "Socket.hpp"

using namespace std;

int main(int argc, char const* argv[])
{
	Socket listeningSocket(8080);
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