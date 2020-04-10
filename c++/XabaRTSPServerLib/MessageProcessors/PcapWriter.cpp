#include "pch.h"
#include <WinSock2.h>
#include <Windows.h>
#include "PcapWriter.h"
#include <Logger\LoggerFactory.h>
 struct pcap_file_header {
    uint32_t magic;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone; /* gmt to local correction */
    uint32_t sigfigs; /* accuracy of timestamps */
    uint32_t snaplen; /* max length saved portion of each pkt */
    uint32_t linktype; /* data link type (LINKTYPE_*) */
};

struct pcap_pkthdr {
    struct timeval ts; /* time stamp */
    uint32_t caplen; /* length of portion present */
    uint32_t len; /* length this packet (off wire) */
};
void PcapWriter::CreateFile(const std::string &fileName){
   pcapFile.open(fileName, std::ofstream::binary | std::ofstream::out);

    if (pcapFile.fail()) {
        LOGGER->LogWarning("Cannot Create file:" + fileName);
        return;
    }

    pcap_file_header fileheader;

    fileheader.version_major = 2;
    fileheader.version_minor = 4;
    fileheader.magic = 0xa1b23c4d; //1234  determines the BigEndian, Little Indian rule
    fileheader.thiszone = 0;
    fileheader.linktype = 1;
    fileheader.sigfigs = 0;
    fileheader.snaplen = 65535;

    pcapFile.write(reinterpret_cast<char*>(&fileheader), sizeof(fileheader));
    pcapFile.close();
}

void PcapWriter::WritePackage(const NetworkPackage& pckg){
    //pcap header
    pcap_pkthdr packetHeader;
    packetHeader.caplen = pckg.buffer.size();
    packetHeader.len = packetHeader.caplen;

    pcapFile.write(reinterpret_cast<char*>(&packetHeader), sizeof(packetHeader));
    //pcap content

    pcapFile.write(reinterpret_cast<const char*>(pckg.buffer.data()), pckg.buffer.size());
}
