﻿#include <iostream>
#include <steam_api.h>
#include <isteamuser.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Link with the WinSock library

using namespace std;

uint32 AddressStrToInt(const char* ipv4_address);
bool SendPacket(char* buffer, int buffer_size, int port);
void Connect(int port);
void Disconnect();

int main()
{
	if (!SteamAPI_IsSteamRunning())
	{
		std::cerr << "Steam client is not running\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Detected Steam Instance\n";

	if (!SteamAPI_Init())
	{
		std::cerr << "Failed to initialize Steam API\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Steam API has been initialized succesfully\n";

	if (!SteamUser()->BLoggedOn())
	{
		std::cerr << "Not connected to Steam servers\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Connected to Steam servers\n";
	
	CSteamID cSteamID = SteamUser()->GetSteamID();

	std::cout << "SteamID: " << cSteamID.ConvertToUint64() << "\n";

	if (!SteamUser()->UserHasLicenseForApp(cSteamID, 10))
	{
		std::cerr << "Couldn't detect a valid license for Counter-Strike 1.6\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Counter-Strike 1.6 license detected\n";

	std::string input;
	std::string port;

	while (true)
	{
		std::cin >> input;

		Connect(std::stoi(input));
	}

	return 0;
}

void Connect(int port)
{
	uint64 steamIDValue27016 = 90182161944798233;
	uint64 steamIDValue27017 = 90182161944807449;
	

	uint32 unIP = AddressStrToInt("51.89.8.25");

	CSteamID cServerSteamID27016(steamIDValue27016);
	CSteamID cServerSteamID27017(steamIDValue27017);

	char pAuthBlob[4096];
	char pAuthBlob2[4096];
	int bytes = SteamUser()->InitiateGameConnection(pAuthBlob, sizeof(pAuthBlob), cServerSteamID27016, unIP, 27016, false);
	int bytes2 = SteamUser()->InitiateGameConnection(pAuthBlob2, sizeof(pAuthBlob2), cServerSteamID27016, unIP, 27017, false);

	if (!bytes)
	{
		std::cerr << "Failed to initialize a connection to the server for 27016\n";
		return;
	}
	else
	{
		std::cout << "Retrieved succesfully a ticket of " << bytes << " bytes\n";
	}

	if (!bytes2)
	{
		std::cerr << "Failed to initialize a connection to the server for 27017\n";
		return;
	}
	else
	{
		std::cout << "Retrieved succesfully a ticket of " << bytes << " bytes\n";
	}

	if (!SendPacket(pAuthBlob, bytes, 27016))
	{
		std::cerr << "Failed to send packet\n";
		return;
	}
	else
	{
		std::cout << "Authentification token sent succesfully to port 27016\n";
	}

	if (!SendPacket(pAuthBlob2, bytes2, 27017))
	{
		std::cerr << "Failed to send packet\n";
		return;
	}
	else
	{
		std::cout << "Authentification token sent succesfully to port 27017\n";
	}
}

void Disconnect()
{
	uint64 steamIDValue = 90182110901640217;
	uint32 unIP = AddressStrToInt("51.89.8.25");

	CSteamID cServerSteamID(steamIDValue);
	SteamUser()->TerminateGameConnection(unIP, 27016);

	std::cout << "Disconnected\n";
}

bool SendPacket(char* buffer, int buffer_size, int port)
{
	// Initialize WinSock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize WinSock" << std::endl;
		return false;
	}

	// Create a UDP socket
	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create UDP socket" << std::endl;
		WSACleanup();
		return false;
	}

	// Set up the server address structure
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port); // Replace with the desired port
	inet_pton(AF_INET, "51.89.8.25", &serverAddr.sin_addr); // Replace with the desired IP address

	// Send a UDP packet to the server
	int result = sendto(udpSocket, buffer, buffer_size, 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

	if (result == SOCKET_ERROR) 
		return false;

	// Close the socket
	closesocket(udpSocket);

	// Clean up WinSock
	WSACleanup();

	return true;
}

uint32 AddressStrToInt(const char* ipv4_address) {
	uint32_t ipAddress = 0;

	if (inet_pton(AF_INET, ipv4_address, &ipAddress) != 1)
	{
		std::cerr << "Failed to convert IPv4 address" << std::endl;
		return 0; 
	}

	return ipAddress;
}