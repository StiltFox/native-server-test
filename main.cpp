#include <iostream>
#include "Socket.hpp"

using namespace std;
const HttpMessage KILL_MESSAGE(HttpMessage::DELETE,"/non_public_uri",{{"host", "the_scp_foundation"},{"operation","kill"}}, "死神");

int main(int argc, char const* argv[])
{
	bool shutdown = false;
	Socket listeningSocket(8080);
	listeningSocket.listenPort();
		
	while (!shutdown)
	{
		Connection inboundConnection = listeningSocket.openConnection();
		HttpMessage request = inboundConnection.blockingReceiveData();

		cout << "-------------------------" << endl 
			<< request.printAsRequest() << endl 
			<< "-------------------------" << endl;

		HttpMessage msg(200,{{"content-type","application/json"}},"{\"message\":\"You sent a " + request.getHttpMethodAsString() + " request!\"}");
		inboundConnection.sendData(msg);
		cout << msg.printAsResponse()<<endl
			<< "-------------------------" << endl;

		shutdown = request == KILL_MESSAGE;
	}

	return 0;
}