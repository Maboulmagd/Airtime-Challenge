#ifndef SATELLITEDATA_H
#define SATELLITEDATA_H

#include "WinSocket.h"
#include "sys/stat.h"
#include <vector>
#include <algorithm>
#include <fstream>

class SatelliteData
{
public:
    struct Packet
    {
        int32_t SEQ;
        int32_t CHECKSUM;
        int32_t LEN;
        std::vector<byte> PCMByteData;

        Packet(int32_t _SEQ, int32_t _CHECKSUM, int32_t _LEN, std::vector<byte>& _PCMByteData) : SEQ(_SEQ), CHECKSUM(_CHECKSUM), LEN(_LEN), PCMByteData(_PCMByteData)
        {
        }

        Packet(const Packet& _packet)
        {
            this->SEQ = _packet.SEQ;
            this->CHECKSUM = _packet.CHECKSUM;
            this->LEN = _packet.LEN;
            this->PCMByteData = _packet.PCMByteData;
        }

        Packet& operator=(const Packet& _packet)
        {
            this->SEQ = _packet.SEQ;
            this->CHECKSUM = _packet.CHECKSUM;
            this->LEN = _packet.LEN;
            this->PCMByteData = _packet.PCMByteData;

            return *this;
        }

        ~Packet()
        {
        }
    };

	const std::vector<byte> data;
	const int totalBytesReceived;

    SatelliteData(const std::vector<byte>& _data, const int& _totalBytesReceived);

    SatelliteData(const SatelliteData& _SatelliteData) = delete;

    SatelliteData& operator=(const SatelliteData& _SatelliteData) = delete;

    ~SatelliteData();

    std::vector<SatelliteData::Packet> ReorderPackets() const;

    bool CheckTheCheckSum(const SatelliteData::Packet& _packet) const;

    void WriteByteDataToFile(const std::vector<SatelliteData::Packet>& vectorOfPackets, const char* pFilePath) const;
};

#endif