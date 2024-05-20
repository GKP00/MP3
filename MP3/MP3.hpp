#pragma once

#include <string>

//MPEG audio frame header
//variables are declared in their serialized order
struct FrameHeader
{
  static const unsigned int SERIALIZED_SIZE = 4;

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

  char FrameSyncByte;
  bool FrameSyncBits: 3;

  //indicates MPEG standard version
  enum class AudioVersionID 
  {
    MPEG_2_5 = 0b00,
    INVALID  = 0b01,
    MPEG_2   = 0b10,
    MPEG_1   = 0b11,
  } AudioVersion : 2;

  //indicates which audio layer of the MPEG standard
  enum class LayerID 
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
  bool BitrateIndex : 4;
  bool SampleRateIndex : 2;

  //indicates whether the audio data is padded with 1 extra byte (slot)
  bool Padding : 1;

  //this is only informative
  bool Private : 1;

  //indicates channel mode
  enum class ChannelModeID 
  {
    STEREO = 0b00,
    JOINT  = 0b01, //joint stereo
    DUAL   = 0b10, //dual channel (2 mono channels)
    SINGLE = 0b11, //single channel (mono)
  } ChannelMode : 2;

  //Only used in joint channel mode. Meaning differ depending on audio layer
  //Use GetExtentionMode()
  bool ExtentionMode : 2;

  //indicates whether the audio is copyrighted
  bool Copyright : 1;

  //indicates whether the frame is located on the original media or a copy 
  bool Original : 1;

  //indicates to the decoder that the file must be de-emphasized, ie the 
  //decoder must 're-equalize' the sound after a Dolby-like noise supression. 
  //It is rarely used.
  enum class EmphasisID
  {
    NONE     = 0b00,
    MS_50_15 = 0b01,
    INVALID  = 0b10,
    CCIT_J17 = 0b10,
  } Emphasis : 2;

  signed short GetBitrate() const
  {
    //version, layer, bits 
    static signed short rateTable[2][3][16] = 
    {
      //V1
      { 
        //L1
        {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, -1}, 
        //L2
        {0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, -1},
        //L3
        {0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, -1},
      },

      //V2
      {
        //L1
        {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, -1},
        //L2
        {0,  8, 16, 24, 32, 40, 48,  56,  64,  80,  96, 112, 128, 144, 160, -1},
        //L3
        {0,  8, 16, 24, 32, 40, 48,  56,  64,  80,  96, 112, 128, 144, 160, -1},
      },
    };

    size_t vIndex = AudioVersion == AudioVersionID::MPEG_1 ? 0 : 1;
    size_t lIndex = Layer == LayerID::LAYER_1 ? 0 :
                    Layer == LayerID::LAYER_2 ? 1 : 2;

    return rateTable[vIndex][lIndex][BitrateIndex];
  }

  std::string GetVersionStr() const
  {
    return AudioVersion == AudioVersionID::MPEG_1   ? "1" :
           AudioVersion == AudioVersionID::MPEG_2   ? "2" :
           AudioVersion == AudioVersionID::MPEG_2_5 ? "2.5" : "INVALID";
  }

  std::string GetLayerStr() const
  {
    return Layer == LayerID::LAYER_1 ? "1" :
           Layer == LayerID::LAYER_2 ? "2" :
           Layer == LayerID::LAYER_3 ? "3" : "INVALID";
  }

};

