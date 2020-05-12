// XabaRTSPServerProgram.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "InputParser.h"
#include <Logger\LoggerFactory.h>
#include <Network\NetworkServer.h>
#include <MessageProcessors\MessageProcessor.h>
#include <MessageProcessors\MessageProcessorLogger.h>
#include <MessageProcessors\MessageProcessorRTSPMessage.h>
int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
		std::cout << "Param" << i << ":" << argv[i] << std::endl;

	InputParser parser(argc, argv);

	// Validate the parameters
	if (argc != 7) {
		printf("usage: --hostip [HOST IP] --port [PORT] --threads [number of threads]\n");
		return 1;
	}


	NetworkServer networkServer;
    MessageProcessor messageProcessor;
	MessageProcessorLogger msgProcessorLogger;

	messageProcessor.Subscribe(std::bind(&MessageProcessorLogger::LogNetworkPackage, msgProcessorLogger, std::placeholders::_1));

	MessageProcessorRTSPMessage msgProcessorrtsp;
	messageProcessor.Subscribe(std::bind(&MessageProcessorRTSPMessage::ProcessNetworkPackage, &msgProcessorrtsp, std::placeholders::_1));

	//LoggerFactory::InitFileLogger(parser.getCmdOption("--logfile")); //"d:\\Idei\\POC\\RecorderGitHub\\output\\Recorder.Log");

	int port = std::stoi(parser.getCmdOption("--port"));
	uint8_t threads = std::stoi(parser.getCmdOption("--threads"));
	std::string host = parser.getCmdOption("--hostip");

	//setup converter
	//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

	//std::wstring destFolderW = converter.from_bytes(destFolder);

	std::vector<int> fromTo{ port };

	try {
        messageProcessor.Start(threads * 2);
		networkServer.StartServer(host, fromTo, threads);
		std::string cmd;
		while (cmd != "exit") {
			std::cin >> cmd;
		}
		//std::cin.ignore();
		networkServer.StopServer();
        messageProcessor.Stop();
		
	}
	catch (const std::exception & e){
		LOGGER->LogError(e, "NetworkServer failed ");
		exit(1);
	}
	catch (...)
	{
		LOGGER->LogError(std::exception(), "Fatal Error ");
	}

	LOGGER->LogInfo("NetworkServer stopped ");
	return 0;
}
