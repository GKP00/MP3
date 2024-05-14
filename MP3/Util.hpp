#pragma once

#include "./MP3.hpp"
#include "../Util/Util.hpp"

#include <fstream>
#include <iostream>
#include <tuple>
#include <optional>

//Seeks to the byte at the end of the next continuous run of 11 set bits.
//(ie. after seeking the cursor will be on the byte of which its 3 most 
// significant bits are part of the frame sync)
bool SeekFrameSync(std::istream& in)
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

    return true;
  }

  return false;
}


FrameHeader ReadFrameHeader(std::istream& in, bool skipFrameSync=true)
{
  FrameHeader header;

  unsigned char cur;

  if(!skipFrameSync)
    cur = in.get();

  cur = in.get();

  header.AudioVersion = 
    ExtractBits<FrameHeader::AudioVersionID>(cur, FrameHeader::AUDIO_VERSION_MASK);

  header.Layer = 
    ExtractBits<FrameHeader::LayerID>(cur, FrameHeader::LAYER_DESCRIPTION_MASK);

  header.Protection = 
    ExtractBits<bool>(cur, FrameHeader::PROTECTION_BIT_MASK);

  cur = in.get();

  header.BitrateIndex = 
    ExtractBits<std::bitset<4>>(cur, FrameHeader::BITRATE_INDEX_MASK);

  header.SampleRateIndex = 
    ExtractBits<std::bitset<2>>(cur, FrameHeader::SAMPLERATE_INDEX_MASK);

  header.Padding = 
    ExtractBits<bool>(cur, FrameHeader::PADDING_BIT_MASK);

  header.Private = 
    ExtractBits<bool>(cur, FrameHeader::PRIVATE_BIT_MASK);

  cur = in.get();

  header.ChannelMode = 
    ExtractBits<FrameHeader::ChannelModeID>(cur, FrameHeader::CHANNEL_MODE_MASK);

  header.ExtentionMode = 
    ExtractBits<std::bitset<2>>(cur, FrameHeader::MODE_EXTENTION_MASK);

  header.Copyright = 
    ExtractBits<bool>(cur, FrameHeader::COPYRIGHT_BIT_MASK);
  
  header.Original = 
    ExtractBits<bool>(cur, FrameHeader::ORIGINAL_BIT_MASK);

  header.Emphasis = 
    ExtractBits<FrameHeader::EmphasisID>(cur, FrameHeader::EMPHASIS_MASK);

  return header;
}

enum class FrameInvalidationReason
{
  INVALID_BITRATE_FOR_VERSION,
  INVALID_SAMPLERATE_FOR_VERSION,
  INVALID_MPEG_VERSION,
  INVALID_LAYER,
};

std::tuple<bool, std::optional<FrameInvalidationReason>> 
ValidateFrameHeader(const FrameHeader& header)
{
  if(header.AudioVersion == FrameHeader::AudioVersionID::INVALID)
    return {false, FrameInvalidationReason::INVALID_MPEG_VERSION};

  if(header.Layer == FrameHeader::LayerID::INVALID)
    return {false, FrameInvalidationReason::INVALID_LAYER};
  
  //TODO: bitrate index & samplerate index validation

  return {true, {}};
}
