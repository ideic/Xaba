#pragma once
#include <Network\NetworkPackage.h>
#include  <fstream>;
class PcapWriter {
private:
    std::ofstream pcapFile;
  
    void CreateFile(const std::string& fileName);
    void WritePackage(const NetworkPackage& pckg);
};
