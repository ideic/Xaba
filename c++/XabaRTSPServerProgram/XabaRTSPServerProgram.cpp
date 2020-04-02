// XabaRTSPServerProgram.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "InputParser.h"
#include <Logger\LoggerFactory.h>
#include <Network\NetworkServer.h>
int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
		std::cout << "Param" << i << ":" << argv[i] << std::endl;

	InputParser parser(argc, argv);

	// Validate the parameters
	if (argc != 13) {
		printf("usage: --hostip [HOST IP] --portFrom [PORT_FROM] --portTo [PORT_TO] --logfile [FULLPATH WITH EXTENSION] --threads [number of threads] --destFolder [DEST_FOLDER]\n");
		return 1;
	}


	NetworkServer recorderServer;
	//LoggerFactory::InitFileLogger(parser.getCmdOption("--logfile")); //"d:\\Idei\\POC\\RecorderGitHub\\output\\Recorder.Log");

	int from = std::stoi(parser.getCmdOption("--portFrom"));
	int to = std::stoi(parser.getCmdOption("--portTo"));
	uint8_t threads = std::stoi(parser.getCmdOption("--threads"));
	std::string destFolder = parser.getCmdOption("--destFolder");
	std::string host = parser.getCmdOption("--hostip");

	//setup converter
	//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

	//std::wstring destFolderW = converter.from_bytes(destFolder);

	std::vector<int> fromTo;
	for (auto i = from; i <= to; i++)
	{
		fromTo.emplace_back(i);
	}

	try
	{
		recorderServer.StartServer(host, fromTo, threads);
		std::cin.ignore();
		recorderServer.StopServer();
	}
	catch (const std::exception & e)
	{
		LOGGER->LogError(e, "RecordingServer failed ");
		exit(1);
	}
	catch (...)
	{
		LOGGER->LogError(std::exception(), "Fatal Error ");
	}

	LOGGER->LogInfo("RecordingServer stopped ");
	return 0;
}
