#include "ChatClient.h"

void sendMessage(SOCKET s, bool* isRunning);
void recevMessage(SOCKET s, bool* isRunning);

int main()
{
    //for windows
#ifdef _WIN32
    WORD versionRequested;
    WSADATA wsaData;
    bool stop = false;

    versionRequested = MAKEWORD(2, 2);
    if (WSAStartup(versionRequested, &wsaData) < 0)
    {
        std::cout << "Error WSAStartup";
        return EXIT_FAILURE;
    }
#endif

    //server informations
    std::string serverIp = "127.0.0.1";
    int port = 54000;

    std::string serverIpString;
    std::string portString;

    //configuration
    std::cout << "Press \"ENTER\" for address and port to connect to default configuration " << std::endl;
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

    //set up socket
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        std::cout << "Error socket" << std::endl;
        return EXIT_FAILURE;
    }

    //server address structure
    sockaddr_in addrv4;
    addrv4.sin_family = AF_INET;
    addrv4.sin_port = htons(port);

    //set up addr
    if (inet_pton(AF_INET, serverIp.c_str(), &(addrv4.sin_addr)) < 0)
    {
        std::cout << "Error inet_pton";
        return EXIT_FAILURE;
    }

    //connection to serveur
    int serverConnection = connect(s, (sockaddr*)&addrv4, sizeof(addrv4));
    if (serverConnection < 0)
    {
        std::cout << "Connection to server impossible... Press \"Enter\" to end";
        std::cin.ignore();
        closesocket(s);
        return EXIT_FAILURE;
    }

    bool isRunning = true;

    //launch tread to read and write
    std::thread recevThread(recevMessage, s, &isRunning);
    std::thread sendThread(sendMessage, s, &isRunning);     
    

    // synchronize threads
    recevThread.join();
    sendThread.join();

    shutdown(s, SD_BOTH);
    closesocket(s);

    return EXIT_SUCCESS;

#ifdef _WIN32
    WSACleanup();
#endif
}

void sendMessage(SOCKET s, bool* isRunning)
{
    char buffer[2048];
    while (*isRunning) {
        std::string data;

        getline(std::cin, data);
        memset(&buffer, 0, sizeof(buffer));//clear the buffer
        strcpy(buffer, data.c_str());

        send(s, (char*)&buffer, strlen(buffer), 0);
    }
}

void recevMessage(SOCKET s, bool* isRunning)
{
    char buffer[2048];
    while (*isRunning) {
        memset(&buffer, 0, sizeof(buffer));//clear the buffer
        int bytesRecv = recv(s, (char*)&buffer, sizeof(buffer), 0);
        if (bytesRecv <= 0) {
            *isRunning = false;
            std::cout << "Connection with server lost, press \"ENTER\" to end..." << std::endl;
        }
        else {
            std::cout << buffer << std::endl;
        }
    }
}