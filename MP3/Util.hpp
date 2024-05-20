#pragma once

#include "./MP3.hpp"

#include <fstream>
#include <iostream>
#include <tuple>
#include <optional>

//Seeks to the byte at the end of the next continuous run of 11 set bits.
//(ie. after seeking the cursor will be on the byte of which its 3 most 
// significant bits are part of the frame sync)
bool SeekFrameSync(std::istream& in, bool seekPastSyncByte = false)
{
  char cur;
  while(in.get(cur))
  {
    //read bytes until EOF or a byte with all bits set is encountered
    if( (cur & 0b11111111) != 0b11111111 )
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

    //if we weren't supposed to seek past the sync byte then put it back
    if(!seekPastSyncByte)
      in.putback(0b11111111);

    return true;
  }

  return false;
}


FrameHeader ReadFrameHeader(std::istream& in, bool skipFirstSyncByte = false)
{
  FrameHeader header;
  in.read(reinterpret_cast<char*>(&header) + (skipFirstSyncByte ? 1 : 0), 
          FrameHeader::SERIALIZED_SIZE     - (skipFirstSyncByte ? 1 : 0));

  return header;
}

enum class FrameInvalidationReason
{
  INVALID_BITRATE_FOR_VERSION,
  INVALID_SAMPLERATE_FOR_VERSION,
  INVALID_MPEG_VERSION,
  INVALID_LAYER,
};

std::optional<FrameInvalidationReason>
ValidateFrameHeader(const FrameHeader& header)
{
  if(header.AudioVersion == FrameHeader::AudioVersionID::INVALID)
    return FrameInvalidationReason::INVALID_MPEG_VERSION;

  if(header.Layer == FrameHeader::LayerID::INVALID)
    return FrameInvalidationReason::INVALID_LAYER;

  if(header.GetBitrate() == FrameHeader::SpecialBitrate::INVALID)
    return FrameInvalidationReason::INVALID_BITRATE_FOR_VERSION;
  
  //TODO: samplerate index validation

  return {};
}
