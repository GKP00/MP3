//Counts frame syncs

#include "./Util.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " <file.mp3>";
    return -1;
  }

  std::ifstream infile{argv[1], std::ios::binary};

  size_t frameCount = 0u;

  while(SeekFrameSync(infile))
  {
    ++frameCount;

    auto resetFlags = std::cout.flags();
    std::cout 
      << std::hex
      << std::uppercase
      << std::setw(6)
      << std::setfill('0');

    std::cout << "Frame sync found at: 0x" << infile.tellg() << '\n';
    std::cout.flags(resetFlags);
  }

  std::cout << "Frames syncs: " << frameCount << '\n';

  return 0;
}
