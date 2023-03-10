#include <iostream>
#include <thread>
#include <mutex>
#include "Socket.hpp"

using namespace std;
mutex consoleWriteMutex;

void listenToConnection(Connection* connection)
{
	if (connection->getHandle() > -1)
	{
		HttpMessage request = connection->receiveData();
		HttpMessage msg(200,{{"content-type","application/json"}},"{\"message\":\"You sent a " + request.getHttpMethodAsString() + " request!\"}");
		connection->sendData(msg);
		delete connection;

		lock_guard<mutex> guard(consoleWriteMutex);
		cout << request.printAsRequest() << endl 
		<< "-------------------------" << endl
		<< msg.printAsResponse()<<endl
		<< "-------------------------" << endl;
	}
	else
	{
		lock_guard<mutex> guard(consoleWriteMutex);
		cout << "Thread Closed" << endl;
	}
}

void listenForKillCommand(Socket* listeningSocket)
{
	const HttpMessage KILL_MESSAGE(HttpMessage::DELETE,"/non_public_uri",{{"host", "the_scp_foundation"},{"operation","kill"}}, "死神");
	Socket killSocket(6000);
	killSocket.listenPort();
	HttpMessage response(HttpMessage::CONNECT);
	bool cont = true;

	while (cont)
	{
		Connection* killConnection = killSocket.openConnection();
		response = killConnection->receiveData();
		string body;

		if (response == KILL_MESSAGE)
		{
			cont = false;
			body = "\"俺は死んでいます。\"";
			listeningSocket->closePort();
		}
		else
		{
			body = "\"何ですか。\"";
		}

		HttpMessage msg(200,{{"content-type", "application/json"}},"{\"message\":" + body + "}");
		killConnection->sendData(msg);
		delete killConnection;
		
		lock_guard<mutex> guard(consoleWriteMutex);
		cout << "kill request received: "<< (cont ? "[continuing]" : "[terminated]");
		cout << endl;
	}
}

int main(int argc, char const* argv[])
{
	Socket listeningSocket(8080);
	listeningSocket.listenPort();

	thread killThread(listenForKillCommand, &listeningSocket);
		
	while (listeningSocket.getHandle() > -1)
	{
		Connection* connection = listeningSocket.openConnection();
		thread listenerThread(listenToConnection, connection);
		listenerThread.detach();
	}

	killThread.join();
	return 0;
}