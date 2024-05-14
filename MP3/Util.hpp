#pragma once

#include <fstream>
#include <iostream>


//Seeks to the byte at the end of the next continuous run of 11 set bits.
//(ie. after seeking the cursor will be on the byte of which its 3 most 
// significant bits are part of the frame sync)
bool SeekFrameSync(std::istream& in)
{
  char cur;
  while(in.get(cur))
  {
    //read bytes until EOF or a byte with all bits set is encountered
    if( (cur & 0b11111111) != 0b11111111)
      continue;

    //peek next byte, ensure its not past EOF, and check that its 3 most
    //significant bits are set to complete the continuous run of 11
    char next = in.peek();

    if(in.eof())
      break;

    if( (next & 0b11100000) != 0b11100000 )
    {
      //if the next byte does not have its 3 most significant bits set it is not
      //the end of the framesync and it also cannot be the start of a framesync
      //so just skip over it here without the check
      in.get();
      continue;
    }

    return true;
  }

  return false;
}
