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

/*
* This function is used to listen for the kill command. Most programs of this nature run as a Daemon or accept some sort of signal to shutdown,
* however i'm going more the spring boot approach where this program stays running in the background on it's own port and can be proxied too.
* That being said, while this may not be the best solution, for now we are opening a second port to listen for the kill command. Ideally this port
* will not be public to other systems. Configure your firewall carefully! :)
*/
void listenForKillCommand(Socket* listeningSocket)
{
	const HttpMessage KILL_MESSAGE(HttpMessage::DELETE,"/non_public_uri",{{"host", "the_scp_foundation"},{"operation","kill"}}, "死神"); //this message, if received will kill our server!!
	Socket killSocket(6000); //Open up the socket
	killSocket.listenPort(); //Request to start listening
	bool cont = true; //Our loop condition. Continue is a reserved word so I had to use an abbreviation.

	while (cont) //This loop will end once the kill command is received allowing this thread to die and take the others with it.
	{
		Connection* killConnection = killSocket.openConnection(); //wait for a connection. This blocks the thread
		HttpMessage response = killConnection->receiveData(); //receive the data.
		string body; // this will be used for the body of our response

		if (response == KILL_MESSAGE) //Did we get a kill message?
		{
			cont = false; // stop looping
			body = "\"俺は死んでいます。\""; //Cry and dramatically raise our hands to the setting sun as we shut down. Sad music plays here.
			listeningSocket->closePort(); //And this is the part in the horror movie where the villain cuts the phone lines.
		}
		else //but wait! it could be a false alarm.
		{
			body = "\"何ですか。\""; //ridicule your enemy for trying to stop your progress! what was that?! huh?!
		}

		HttpMessage msg(200,{{"content-type", "application/json"}},"{\"message\":" + body + "}"); //Let's calmly construct a message to send back to our assassin.
		killConnection->sendData(msg); //Message sent!
		delete killConnection; //And kill the connection with the client. Wouldn't want them to get back in.
		                       //remember you must delete any heap allocated pointers. This prevents memory leaks.
		lock_guard<mutex> guard(consoleWriteMutex); //secure a lock on the console write command
		cout << "kill request received: "<< (cont ? "[continuing]" : "[terminated]"); //Output a tragic news report about our story.
		cout << endl; //fin.
	}
}

/*
* This is the main entry point to our program. The parameters can be ignored for now as we're not taking anything in from the console.
* The return value tells the operating system how we did. Anything other than zero is interpreted as an error. Looking up what went wrong
* is the caller's responsibility not ours, so we best document our outputs well. Good thing we only output success because everything we do
* is successful.
*/
int main(int argc, char const* argv[])
{
	Socket listeningSocket(8080); //Get a socket on port 8080.
	listeningSocket.listenPort(); //Start listening to port 8080.

	thread killThread(listenForKillCommand, &listeningSocket); //Start a new thread that will run the listenForKillCommand function. Pass it the socket memory address.
		
	while (listeningSocket.getHandle() > -1) //loop until the socket is closed.
	{
		Connection* connection = listeningSocket.openConnection(); //This function will block the thread while looking for a connection. This prevents us from chewing up too many resources.
		thread listenerThread(listenToConnection, connection); //spin up a new thread
		listenerThread.detach(); //This detaches the thread from the object, meaning that the thread continues to run even if the object goes out of scope.
	}

	killThread.join(); //wait for the kill thread to finish processing.
	return 0; //close the program with no errors.
}

/*
* This program is a work in progress and many things can be made to make this better. If you want to have fun playing with this code,
* here are some improvement ideas!
*
* Make the program read from a config file instead of hardcoding port numbers and messages.
* Make a switch statement that does different things based on endpoint and http method type.
* Implement SSL and TLS.
* Implement a configurable limit on the number of listening threads that can be open.
*
* And those are just a few ideas.
*
* If you wish to continue along this tutorial please go to modules/httpmessage/HttpMessage.hpp next.
*/