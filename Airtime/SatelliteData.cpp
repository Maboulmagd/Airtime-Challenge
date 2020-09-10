#include "SatelliteData.h"

SatelliteData::SatelliteData(const std::vector<byte>& _data, const int& _totalBytesReceived) : data(_data), totalBytesReceived(_totalBytesReceived)
{
}

SatelliteData::~SatelliteData()
{
}

bool SatelliteData::CheckTheCheckSum(const SatelliteData::Packet& _packet) const
{
    int32_t SEQ = _packet.SEQ;

    int32_t CHK = _packet.CHECKSUM;

    int32_t LEN = _packet.LEN;

    std::vector<byte> PCMByteData = _packet.PCMByteData;

    for (int b = 0; b < LEN; b += 4)
    {
        // Concatenate 4 bytes of PCM data...
        int32_t chunkOfData = 0;

        // Get all 4 bytes...
        if ((b + 3) < LEN) { chunkOfData = ((PCMByteData[b] << 24) | (PCMByteData[b + 1] << 16) | (PCMByteData[b + 2] << 8) | (PCMByteData[b + 3])); }

        // Get first 3 bytes...
        else if ((b + 2) < LEN) { chunkOfData = ((PCMByteData[b] << 24) | (PCMByteData[b + 1] << 16) | (PCMByteData[b + 2] << 8) | 0xAB); }

        // Get first 2 bytes...
        else if ((b + 1) < LEN) { chunkOfData = ((PCMByteData[b] << 24) | (PCMByteData[b + 1] << 16) | (0xAB << 8) | 0xAB); }

        // Get first 1 byte...AKA the last remaining byte in the PCMByteData...
        else { chunkOfData = ((PCMByteData[b] << 24) | (0xAB << 16) | (0xAB << 8) | 0xAB); }

        // XOR SEQ with chunkOfData...
        SEQ ^= chunkOfData;
    }

    return SEQ == CHK;
}

void SatelliteData::WriteByteDataToFile(const std::vector<SatelliteData::Packet>& vectorOfPackets, const char* pFilePath) const
{
    // Check that the filePath exists...if it does not, simply print a message and return...
    struct stat info;
    int ret = stat(pFilePath, &info);

    if (ret != 0)
    {
        printf("Filepath doesn't exist.\n");
        assert(ret == 1);
        return;
    }

    // opens the file in the constructor...
    std::ofstream myFile(pFilePath);

    // Write byte by byte to the file...
    for (const SatelliteData::Packet& packet : vectorOfPackets)
    {
        int32_t LEN = packet.LEN;

        std::vector<byte> PCMByteData = packet.PCMByteData;

        for (const byte& b : PCMByteData)
        {
            myFile << b;
        }
    }

    myFile.close();
}

// We want to rearrange the packets we received in ASC order by SEQ number...then we can custom sort the vector of packets...and then verify the checksums to distinguish between valid and invalid packets...
// NOTE: There seems to be a plethora of packets with the same SEQ number...hence probably why we need to check/verify the checksum number...
std::vector<SatelliteData::Packet> SatelliteData::ReorderPackets() const
{
    std::vector<SatelliteData::Packet> res;
    res.reserve(500);
    // As an optimization...could reserve some arbitrary size...after some analysis...it seems as though we never receive more than 5000 packets...only about 390-430 unique SEQ numbers though as well...

    int32_t LEN = 0;

    for (int b = 0; b < this->totalBytesReceived; b += 12 + LEN)
    {
        // Concatenate the first four bytes to represent the SEQ number...
        int32_t SEQ = ((this->data[b] << 24) | (this->data[b + 1] << 16) | (this->data[b + 2] << 8) | (this->data[b + 3]));

        // Concatenate the second four bytes to represent the CHECKSUM...
        int32_t CHECKSUM = ((this->data[b + 4] << 24) | (this->data[b + 5] << 16) | (this->data[b + 6] << 8) | (this->data[b + 7]));

        // Concatenate the third four bytes to represent the LEN...
        LEN = ((this->data[b + 8] << 24) | (this->data[b + 9] << 16) | (this->data[b + 10] << 8) | (this->data[b + 11]));

        // Now copy over the actual PCMByteData...
        std::vector<byte> PCMByteData(LEN, 0);
        for (int k = 0; k < LEN; ++k) { PCMByteData[k] = this->data[b + 12 + k]; }

        // Now, construct the packet object...
        SatelliteData::Packet packet(SEQ, CHECKSUM, LEN, PCMByteData);

        // NOTE: A great optimization would be to verify the checksum here...prior to adding to the result vector/or writing to the output file...
        // Verify checksum...
        if (this->CheckTheCheckSum(packet))
        {
            // Finally, add it to our vector of packets...
            res.emplace_back(packet);
        }
    }

    std::sort(res.begin(), res.end(), [](const Packet& _packetA, const Packet& _packetB)
        {
            return _packetA.SEQ < _packetB.SEQ;
        });

    return res;
}