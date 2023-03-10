/*
* These are preprocessor commands. preprocessor commands start with the # symbol and are run before the code is compiled.
* You can think of these as special instructions for how to write the executable.
*
* The command #include tells the compiler to insert the code in the include file at this point in the code.
* The <> brackets indicate that the referred to file is an external dependency.
* The "" marks indicate that the referred to file is a internal project file.
*/
#include <iostream>
#include <thread>
#include <mutex>
#include "Socket.hpp"

/*
* C++ allows for both objects and normal functions to exist in the same code base. This can cause problems with name collision if you're not careful.
* To help with this issue C++ uses scoping to help prevent name collisions. Scopes are defined using the following notation: Namespace::function();
* This can get extremely wordy so to fix this we have the Using command.
* using namespace std; tells the compiler to assume that i'm using the std namespace.
* This means I can write cout instead of std::cout.
*/
using namespace std;
mutex consoleWriteMutex; /*A mutex is used to synchronize resources that are shared between threads.
* While one thread has a lock on a mutex, another thread cannot acquire the lock, until the thread that owns it, unlocks it.
* This protects us from race conditions where reading and writing could be corrupted by the memory address being modified in unpredictable ways.
* This mutex is used to insure that writing to the console logs happens as intended.
*/

/*
* This function is used to listen to a connection and respond with an HTTP response. This function is intended to be thread safe.
* The connection it takes in represents a client that is connected to our API.
*/
void listenToConnection(Connection* connection)
{
	if (connection->getHandle() > -1) //Make sure that the connection is not closed, or experiencing an error
	{
		HttpMessage request = connection->receiveData(); //Get the request from the client
		HttpMessage msg(200,{{"content-type","application/json"}},"{\"message\":\"You sent a " + request.getHttpMethodAsString() + " request!\"}"); //Create a 200 ok response with a message telling the user what kind of request they made
		connection->sendData(msg); //Send the response back to the client
		delete connection; //This will close the connection and free the heap memory allocated by the caller.

		lock_guard<mutex> guard(consoleWriteMutex); //Make sure it's safe to write to console. Maintain ownership of mutex till this object leaves scope.
		cout << request.printAsRequest() << endl  //print the request to console
		<< "-------------------------" << endl //print a seperator bar to console
		<< msg.printAsResponse()<<endl // print the response to console
		<< "-------------------------" << endl; // and another seperator bar.
	}
	else
	{
		//the connection was closed for some reason
		lock_guard<mutex> guard(consoleWriteMutex); //lock the console mutex 
		cout << "Thread Closed" << endl; //express our frustration to the world.
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