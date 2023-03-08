#include <iostream>
#include <thread>
#include <mutex>
#include "Socket.hpp"

using namespace std;
const HttpMessage KILL_MESSAGE(HttpMessage::DELETE,"/non_public_uri",{{"host", "the_scp_foundation"},{"operation","kill"}}, "死神");
mutex consoleWriteMutex;

void listenToConnection(Connection*& connection)
{
	HttpMessage request = connection->blockingReceiveData();
	HttpMessage msg(200,{{"content-type","application/json"}},"{\"message\":\"You sent a " + request.getHttpMethodAsString() + " request!\"}");
	connection->sendData(msg);
	delete connection;
	connection = nullptr;

	lock_guard<mutex> guard(consoleWriteMutex);
	cout << request.printAsRequest() << endl 
	<< "-------------------------" << endl
	<< msg.printAsResponse()<<endl
	<< "-------------------------" << endl;
}

int main(int argc, char const* argv[])
{
	bool shutdown = false;
	Socket listeningSocket(8080);
	listeningSocket.listenPort();
	Connection* listeners[10] = {nullptr};
		
	while (!shutdown)
	{
		Connection* inboundConnection = listeningSocket.openConnection();
		HttpMessage request = inboundConnection->blockingReceiveData();

		cout << "-------------------------" << endl 
			<< request.printAsRequest() << endl 
			<< "-------------------------" << endl;

		HttpMessage msg(200,{{"content-type","application/json"}},"{\"message\":\"You sent a " + request.getHttpMethodAsString() + " request!\"}");
		inboundConnection->sendData(msg);
		cout << msg.printAsResponse()<<endl
			<< "-------------------------" << endl;

		delete inboundConnection;
		shutdown = request == KILL_MESSAGE;
	}

	return 0;
}