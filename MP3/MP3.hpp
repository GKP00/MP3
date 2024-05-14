#pragma once

#include <bitset>

//MPEG audio frame header
//variables are declared in their serialized order
struct FrameHeader
{
  //bitmasks for frame header fields grouped by byte
  static const unsigned char FRAMESYNC_FIRST_BYTEMASK  = 0b11111111;

  static const unsigned char FRAMESYNC_SECOND_BYTEMASK = 0b1110000;
  static const unsigned char AUDIO_VERSION_MASK        = 0b00011000;
  static const unsigned char LAYER_DESCRIPTION_MASK    = 0b00000110;
  static const unsigned char PROTECTION_BIT_MASK       = 0b00000001;

  static const unsigned char BITRATE_INDEX_MASK        = 0b11110000;
  static const unsigned char SAMPLERATE_INDEX_MASK     = 0b00001100;
  static const unsigned char PADDING_BIT_MASK          = 0b00000010;
  static const unsigned char PRIVATE_BIT_MASK          = 0b00000001;

  static const unsigned char CHANNEL_MODE_MASK         = 0b11000000;
  static const unsigned char MODE_EXTENTION_MASK       = 0b00110000;
  static const unsigned char COPYRIGHT_BIT_MASK        = 0b00001000;
  static const unsigned char ORIGINAL_BIT_MASK         = 0b00000100;
  static const unsigned char EMPHASIS_MASK             = 0b00000011;

  //indicates MPEG standard version
  enum class AudioVersionID : unsigned
  {
    MPEG_2_5 = 0b00,
    INVALID  = 0b01,
    MPEG_2   = 0b10,
    MPEG_1   = 0b11,
  } AudioVersion : 2;

  //indicates which audio layer of the MPEG standard
  enum class LayerID : unsigned
  {
    INVALID = 0b00,
    LAYER_3 = 0b01,
    LAYER_2 = 0b10,
    LAYER_1 = 0b11,
  } Layer : 2;

  //indicates whether theres a 16 bit crc checksum following the header
  bool Protection : 1;

  //sample & bitrate indexes meaning differ depending on MPEG version
  //use GetBitrate() and GetSamplerate() 
  std::bitset<4> BitrateIndex;
  std::bitset<2> SampleRateIndex;

  //indicates whether the audio data is padded with 1 extra byte (slot)
  bool Padding : 1;

  //this is only informative
  bool Private : 1;

  //indicates channel mode
  enum class ChannelModeID : unsigned
  {
    STEREO = 0b00,
    JOINT  = 0b01, //joint stereo
    DUAL   = 0b10, //dual channel (2 mono channels)
    SINGLE = 0b11, //single channel (mono)
  } ChannelMode : 2;

  //Only used in joint channel mode. Meaning differ depending on audio layer
  //Use GetExtentionMode()
  std::bitset<2> ExtentionMode;

  //indicates whether the audio is copyrighted
  bool Copyright : 1;

  //indicates whether the frame is located on the original media or a copy 
  bool Original : 1;

  //indicates to the decoder that the file must be de-emphasized, ie the 
  //decoder must 're-equalize' the sound after a Dolby-like noise supression. 
  //It is rarely used.
  enum class EmphasisID : unsigned
  {
    NONE     = 0b00,
    MS_50_15 = 0b01,
    INVALID  = 0b10,
    CCIT_J17 = 0b10,
  } Emphasis : 2;
};

