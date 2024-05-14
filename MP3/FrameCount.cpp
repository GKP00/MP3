//Counts frame syncs

#include "./MP3.hpp"
#include "./Util.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

int main(int argc, char** argv)
{
  FrameHeader headear;

  if(argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " <file.mp3>";
    return -1;
  }

  auto resetFlags = std::cout.flags();
  std::cout 
    << std::hex
    << std::uppercase
    << std::setw(6)
    << std::setfill('0');

  std::ifstream infile{argv[1], std::ios::binary};

  size_t frameCount = 0u;

  while(SeekFrameSync(infile))
  {
    FrameHeader header = ReadFrameHeader(infile);
    auto [valid, invalidReason] = ValidateFrameHeader(header);

    if(valid)
    {
      ++frameCount;
      std::cout << "\e[32m" << "Valid frame sync found at: 0x" << infile.tellg() << '\n';
    }
    else
      std::cout << "\e[31m" << "Invalid frame sync found at: 0x" << infile.tellg() << '\n';
  }

  std::cout.flags(resetFlags);
  std::cout << "\e[0m" << "Valid frames syncs: " << frameCount << '\n';
  return 0;
}
