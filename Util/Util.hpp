#pragma once


template <typename RetT, typename ValT, typename MaskT>
constexpr RetT ExtractBits(ValT val, MaskT mask)
{
  int shift = 0;

  while( ((mask >> shift) & 1) == 0)
    ++shift;

  return static_cast<RetT>( (val & mask) >> shift );
}

