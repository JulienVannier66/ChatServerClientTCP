#include "ChatServer.h"

void sendToOtherClient(fd_set master, SOCKET fdmax, SOCKET server, SOCKET socket, std::string message);

int main()
{
	//for windows
#ifdef _WIN32
	WORD versionRequested;
	WSADATA wsaData;
	versionRequested = MAKEWORD(2, 2);
	if (WSAStartup(versionRequested, &wsaData) < 0)
	{
		std::cout << "Error WSAStartup";
		return EXIT_FAILURE;
	}
#endif
	//server information
	std::string serverIp = "127.0.0.1";
	int port = 54000;

	std::string serverIpString;
	std::string portString;

	//configuration
	std::cout << "Press \"ENTER\" for address and port to have default configuration " << std::endl;
	std::cout << "IP adress : " << serverIp << std::endl;
	std::cout << "Port : " << port << std::endl << std::endl;

	std::cout << "Enter ip address : " << std::endl;
	getline(std::cin, serverIpString);
	if (serverIpString != "") {
		serverIp = serverIpString;
	}

	std::cout << "Enter port : " << std::endl;
	getline(std::cin, portString);
	if (portString != "") {
		port = std::stoi(portString);
	}
	
	//create a message buffer 
	char buffer[2048];

	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

	//client
	SOCKET client;
	sockaddr clientAddr;
	socklen_t clientAddrSize = sizeof(sockaddr);

	if (server < 0)
	{
		std::cout << "Error socket" << std::endl;
		return EXIT_FAILURE;
	}

	sockaddr_in addrv4;
	addrv4.sin_family = AF_INET;
	addrv4.sin_port = htons(port);

	//set up addr
	if (inet_pton(AF_INET, serverIp.c_str(), &(addrv4.sin_addr)) < 0)
	{
		std::cout << "Error inet_pton" << std::endl;
		closesocket(server);
		std::cin.ignore();
		return EXIT_FAILURE;
	}

	//bind
	if (bind(server, reinterpret_cast<sockaddr*>(&addrv4), sizeof(sockaddr)) < 0)
	{
		std::cout << "Error bind " << std::endl;
		closesocket(server);
		std::cin.ignore();
		return EXIT_FAILURE;
	}

	//listen
	if (listen(server, 255) < 0)
	{
		std::cout << "Error listen" << std::endl;
		closesocket(server);
		std::cin.ignore();
		return EXIT_FAILURE;
	}

	fd_set master;
	fd_set copy;
	FD_ZERO(&master);
	FD_ZERO(&copy);

	FD_SET(server, &master);

	SOCKET fdmax = server;

	std::cout << "Server ready! " << "ip : " << serverIp << " port : " << port << std::endl;

	while (true)
	{
		fd_set copy = master;
		int socketCount = select(fdmax + 1, &copy, nullptr, nullptr, nullptr);

		if (socketCount <= 0) {
			std::cout << "Error select " << std::endl;
			closesocket(server);
			std::cin.ignore();
			return EXIT_FAILURE;
		}

		for (SOCKET socket = 0; socket <= fdmax; socket++) {
			if (FD_ISSET(socket, &copy)) {
				//new client connection
				if (socket == server) {
					client = accept(server, &clientAddr, &clientAddrSize);
					FD_SET(client, &master);

					/* keep track of the maximum */
					if (client > fdmax)
					{
						fdmax = client;
					}
					//send message to new client
					std::string welcomeMessage = "Welcome to the chat";
					send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);

					//log server
					std::string serverMessage = "SOCKET#" + std::to_string(client) + " connected";
					std::cout << serverMessage << std::endl;

					//send message to other client
					serverMessage = "[SERVER] " + serverMessage;
					sendToOtherClient(master, fdmax, server, client, serverMessage);
				}
				else {
					int bytesRecv = recv(socket, (char*)&buffer, sizeof(buffer), 0);
					//client disconnection
					if (bytesRecv <= 0) {
						closesocket(socket);
						FD_CLR(socket, &master);

						//log server
						std::string messageDisconnect = "SOCKET#" + std::to_string(socket) + " disconnected";
						
						//send message to other clients
						std::cout << messageDisconnect << std::endl;
						messageDisconnect = "[SERVER] " + messageDisconnect;
						sendToOtherClient(master, fdmax, server, socket, messageDisconnect);
					}
					else {
						//log server
						std::cout << "SOCKET#" << socket << " send : " << buffer << std::endl;

						//send message to other clients
						std::string message(buffer);
						message = "[SOCKET#" + std::to_string(socket) + "] " + message;
						sendToOtherClient(master, fdmax, server, socket, message);
					}
				}
				//clear the buffer
				memset(&buffer, 0, sizeof(buffer));
			}
		}
	}

	//disconnect all clients
	for (SOCKET closeSocket = 0; closeSocket <= fdmax; closeSocket++) {
		if (FD_ISSET(closeSocket, &master)) {
			closesocket(closeSocket);
			FD_CLR(closeSocket, &master);
		}
	}

	//shutdown
	shutdown(server, SD_BOTH);
	closesocket(server);

#ifdef _WIN32
	WSACleanup();
#endif

	return EXIT_SUCCESS;
}

void sendToOtherClient(fd_set master, SOCKET fdmax, SOCKET server, SOCKET socket, std::string message) {
	for (SOCKET outSocket = 0; outSocket <= fdmax; outSocket++) {
		if (FD_ISSET(outSocket, &master)) {
			if (outSocket != server && outSocket != socket) {
				send(outSocket, message.c_str(), message.size() + 1, 0);
			}
		}
	}
}
