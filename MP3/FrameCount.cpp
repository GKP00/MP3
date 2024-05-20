//Counts frame syncs

#include "./MP3.hpp"
#include "./Util.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " <file.mp3>\n";
    return -1;
  }

  auto resetFlags = std::cout.flags();
  std::cout 
    << std::hex
    << std::uppercase
    << std::setfill('0');

  std::ifstream infile{argv[1], std::ios::binary};

  size_t frameCount = 0u;

  while(SeekFrameSync(infile, true))
  {
    FrameHeader header = ReadFrameHeader(infile, true);
    auto invalidReason = ValidateFrameHeader(header);

    if(!invalidReason)
    {
      ++frameCount;
      std::cout << "\e[32m" << "Valid frame sync found at: 0x" << infile.tellg()
        << " (version: " << header.GetVersionStr() << ')'
        << " (layer: " << header.GetLayerStr() << ')'
        << " (bitrate: " << header.GetBitrateStr() << ')'
        << " (frequency: " << header.GetSampleRateStr() << ")\n";
    }
    else
    {
      std::cout << "\e[31m" << "Invalid frame sync found at: 0x" << infile.tellg()
        << " (version: " << header.GetVersionStr() << ')'
        << " (layer: " << header.GetLayerStr() << ')'
        << " (bitrate: " << header.GetBitrateStr() << ')'
        << " (frequency: " << header.GetSampleRateStr() << ")\n";
    }
  }

  std::cout.flags(resetFlags);
  std::cout << "\e[0m" << "Valid frames syncs: " << frameCount << '\n';
  return 0;
}
