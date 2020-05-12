#include "pch.h"
#include <filesystem>
#include <process.h>
#include <NetworkTestClient.h>
#include <thread>
using namespace std::string_literals;
using namespace std::chrono_literals;

class AcceptanceTest : public ::testing::Test {
private:
    intptr_t _processHandle;
protected:
    void SetUp() {
        WSADATA wsaData;
        auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult));
        }
    }

    void StartRTSPServer() {
        wchar_t path[MAX_PATH] = { 0 };

        GetModuleFileName(nullptr, path, MAX_PATH);

        auto exePath = std::filesystem::path(path);
        exePath = exePath.replace_filename("XabaRTSPServerProgram.exe");

       auto retvalue = _spawnl(
           _P_NOWAIT,
           exePath.string().c_str(),
          "XabaRTSPServerProgram.exe",
           "--port 554 --threads 1 --hostip 127.0.0.10",
           NULL);
       if (retvalue < 0) {
           auto err = errno;
           throw std::runtime_error("Process start failed with errorCode:" + std::to_string(errno));
       }
       std::this_thread::sleep_for(2s);
       _processHandle = retvalue;
    }

    void StopRTSPServer() {
        const UINT exitCode = 0;
        const auto result = TerminateProcess(reinterpret_cast<HANDLE>(_processHandle), exitCode);
        CloseHandle(reinterpret_cast<HANDLE>(_processHandle));
        _processHandle = 0;

    }
};

TEST_F(AcceptanceTest, OptionTest) {
    StartRTSPServer();
    NetworkTestClient client;

    client.Init("127.0.0.10"s, "554"s);

    client.Send("OPTIONS rtsp ://10.0.0.6:554 RTSP/1.0\r\nCSeq: 2\r\nUser-Agent: LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)\r\n\r\n");

    std::string givenMessage;

    givenMessage = client.Receive();

    EXPECT_TRUE(givenMessage == "RTSP/1.0 200 OK\r\nCSeq:0\r\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n"s);

    client.Close();

    StopRTSPServer();
}