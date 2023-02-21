#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "HttpMessage.hpp"
#define PORT 8080

using namespace std;

int main(int argc, char const* argv[])
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) 
	{
		if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,	sizeof(opt)) >= 0) 
		{
			if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) >= 0) 
			{
				if (listen(server_fd, 3) >= 0)
				{
					if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) >= 0)
					{
						HttpMessage request(new_socket, &read);
						cout << "-------------------------" << endl;
						cout << request.printAsRequest() << endl;
						cout << "-------------------------" << endl;

						HttpMessage msg(200,{{"content-type","application/json"}},"{\"message\":\"You sent a " + request.getHttpMethodAsString() + " request!\"}");
						std::string response = msg.printAsResponse();
						send(new_socket, response.c_str(), response.size(), 0);
						close(new_socket);
					}
				}
			}
		}
		shutdown(server_fd, SHUT_RDWR);
	}
	
	return 0;
}
