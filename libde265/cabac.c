/*
 * H.265 video codec.
 * Copyright (c) 2013 StrukturAG, Dirk Farin, <farin@struktur.de>
 *
 * This file is part of libde265.
 *
 * libde265 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libde265 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libde265.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cabac.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

/*
 * CABAC decoder implementation
 */

/*
 * Table 9-40
 * This lookup table converts pStateIdx and qRangeIdx to LPS range
 */
static const uint8_t LPS_table[64][4] =
  {
    { 128, 176, 208, 240},
    { 128, 167, 197, 227},
    { 128, 158, 187, 216},
    { 123, 150, 178, 205},
    { 116, 142, 169, 195},
    { 111, 135, 160, 185},
    { 105, 128, 152, 175},
    { 100, 122, 144, 166},
    {  95, 116, 137, 158},
    {  90, 110, 130, 150},
    {  85, 104, 123, 142},
    {  81,  99, 117, 135},
    {  77,  94, 111, 128},
    {  73,  89, 105, 122},
    {  69,  85, 100, 116},
    {  66,  80,  95, 110},
    {  62,  76,  90, 104},
    {  59,  72,  86,  99},
    {  56,  69,  81,  94},
    {  53,  65,  77,  89},
    {  51,  62,  73,  85},
    {  48,  59,  69,  80},
    {  46,  56,  66,  76},
    {  43,  53,  63,  72},
    {  41,  50,  59,  69},
    {  39,  48,  56,  65},
    {  37,  45,  54,  62},
    {  35,  43,  51,  59},
    {  33,  41,  48,  56},
    {  32,  39,  46,  53},
    {  30,  37,  43,  50},
    {  29,  35,  41,  48},
    {  27,  33,  39,  45},
    {  26,  31,  37,  43},
    {  24,  30,  35,  41},
    {  23,  28,  33,  39},
    {  22,  27,  32,  37},
    {  21,  26,  30,  35},
    {  20,  24,  29,  33},
    {  19,  23,  27,  31},
    {  18,  22,  26,  30},
    {  17,  21,  25,  28},
    {  16,  20,  23,  27},
    {  15,  19,  22,  25},
    {  14,  18,  21,  24},
    {  14,  17,  20,  23},
    {  13,  16,  19,  22},
    {  12,  15,  18,  21},
    {  12,  14,  17,  20},
    {  11,  14,  16,  19},
    {  11,  13,  15,  18},
    {  10,  12,  15,  17},
    {  10,  12,  14,  16},
    {   9,  11,  13,  15},
    {   9,  11,  12,  14},
    {   8,  10,  12,  14},
    {   8,   9,  11,  13},
    {   7,   9,  11,  12},
    {   7,   9,  10,  12},
    {   7,   8,  10,  11},
    {   6,   8,   9,  11},
    {   6,   7,   9,  10},
    {   6,   7,   8,   9},
    {   2,   2,   2,   2}
  };

static const uint8_t renorm_table[32] =
  {
    6,  5,  4,  4,
    3,  3,  3,  3,
    2,  2,  2,  2,
    2,  2,  2,  2,
    1,  1,  1,  1,
    1,  1,  1,  1,
    1,  1,  1,  1,
    1,  1,  1,  1
  };

/*
 * Table 9-41
 * Lookup table used by CABAC decoder to update pStateIdx after decoding 
 * a MPS bin
 */
static const uint8_t next_state_MPS[64] =
  {
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
    33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
    49,50,51,52,53,54,55,56,57,58,59,60,61,62,62,63
  };

/*
 * Table 9-41
 * Lookup table used by CABAC decoder to update pStateIdx after decoding 
 * a LPS bin
 */
static const uint8_t next_state_LPS[64] =
  {
    0,0,1,2,2,4,4,5,6,7,8,9,9,11,11,12,
    13,13,15,15,16,16,18,18,19,19,21,21,22,22,23,24,
    24,25,26,26,27,27,28,29,29,30,30,30,31,32,32,33,
    33,33,34,34,35,35,35,36,36,36,37,37,37,38,38,63
  };





#ifdef DE265_LOG_TRACE
int logcnt=1;
#endif

void init_CABAC_decoder(CABAC_decoder* decoder, uint8_t* bitstream, int length)
{
  decoder->bitstream_start = bitstream;
  decoder->bitstream_curr  = bitstream;
  decoder->bitstream_end   = bitstream+length;
}

void init_CABAC_decoder_2(CABAC_decoder* decoder)
{
  int length = decoder->bitstream_end - decoder->bitstream_curr;

  decoder->range = 510;
  decoder->bits_needed = 8;

  decoder->value = 0;

  if (length>0) { decoder->value  = (*decoder->bitstream_curr++) << 8;  decoder->bits_needed-=8; }
  if (length>1) { decoder->value |= (*decoder->bitstream_curr++);       decoder->bits_needed-=8; }

  logtrace(LogCABAC,"[%3d] init_CABAC_decode_2 r:%x v:%x\n", logcnt, decoder->range, decoder->value);
}


//#include <sys/types.h>
//#include <signal.h>

/**
 * Decodes DecodeDecision as mentioned in Figure 9-6
 * This decodes the non-equiprobable syntax element bins of the bit stream
 */
/*int  decode_CABAC_bit_parallel_2(struct decoder_context* ctx, CABAC_decoder* decoder, context_model* model, enum Dbg_cabac_se_idx se_idx)
{
  int decoded_bit;

  //stage0
  int LPS_s0 = LPS_table[model->state][ ( decoder->range >> 6 ) - 4 ];	//get the lps range from table 9-40
  //stage0_mps
  int range_s0_pmps = decoder->range - LPS_s0;	//calculate current range for MPS
  //stage0_lps
  int range_s0_plps = 0; //TODO: fix this

  //stage1_mps <= stage0_mps
  int LPS_s1_pmps = LPS_table[model->state][ ( range_s0_pmps >> 6 ) - 4 ];
  int LPS_s1_plps = LPS_table[model->state][ ( range_s0_plps >> 6 ) - 4 ];

  decoder->range = range_s0_pmps;

  uint32_t scaled_range = decoder->range << 7;
  if (decoder->value < scaled_range) { // MPS path
      decoded_bit = model->MPSbit;
      model->state = next_state_MPS[model->state];
      if (scaled_range < ( 256 << 7 ) )	{ // renormalize
          // scaled range, highest bit (15) not set
          decoder->range = scaled_range >> 6; // shift range by one bit
          decoder->value <<= 1;               // shift value by one bit
          decoder->bits_needed++;
          if (decoder->bits_needed == 0) {
              decoder->bits_needed = -8;
              if (decoder->bitstream_curr != decoder->bitstream_end) {
            	  decoder->value |= *decoder->bitstream_curr++; }
          }
        }
    } else { // LPS path
      int num_bits = renorm_table[ LPS >> 3 ];
      decoder->value = (decoder->value - scaled_range);
      decoder->value <<= num_bits;
      decoder->range   = LPS << num_bits;
      decoded_bit      = 1 - model->MPSbit;
      if (model->state==0) { model->MPSbit = 1-model->MPSbit; }
      model->state = next_state_LPS[model->state];
      decoder->bits_needed += num_bits;
      if (decoder->bits_needed >= 0) {
          if (decoder->bitstream_curr != decoder->bitstream_end)
            { decoder->value |= (*decoder->bitstream_curr++) << decoder->bits_needed; }
          decoder->bits_needed -= 8;
      }
    }
  return decoded_bit;
}*/

/**
 * Decodes DecodeDecision as mentioned in Figure 9-6
 * This decodes the non-equiprobable syntax element bins of the bit stream
 */
int  decode_CABAC_bit(struct decoder_context* ctx, CABAC_decoder* decoder, context_model* model, enum Dbg_cabac_se_idx se_idx)
{
  //if (logcnt >= 1100000) { enablelog(); }

  // if (logcnt==400068770) { raise(SIGINT); }

  logtrace(LogCABAC,"[%3d] decodeBin r:%x v:%x state:%d\n",logcnt,decoder->range, decoder->value, model->state);

  //cabac analyze and debug
  incCabacDbgSeBinCnt(ctx, se_idx);

  //assert(decoder->range>=0x100);

  int decoded_bit;
  int LPS = LPS_table[model->state][ ( decoder->range >> 6 ) - 4 ];	//get the lps range from table 9-40
  decoder->range -= LPS;	//calculate current range for MPS

  uint32_t scaled_range = decoder->range << 7;

  logtrace(LogCABAC,"[%3d] sr:%x v:%x\n",logcnt,scaled_range, decoder->value);

  if (decoder->value < scaled_range)
    {
      logtrace(LogCABAC,"[%3d] MPS\n",logcnt);

      // MPS path                                                                                    

      decoded_bit = model->MPSbit;
      model->state = next_state_MPS[model->state];

      if (scaled_range < ( 256 << 7 ) )	// renormalize
        {
    	  incCabacDbgSeRenormBitsCnt(ctx, se_idx);
    	  incCabacDbgSeWRenormMPSCnt(ctx, se_idx);
//Enable to monitor renorm range
#if 1
    	  if(decoder->range < 128) {
    		  printf("========================Renorm error will occur========================================\n");
    	  }
    	  //printf("MPS path range before renorm: %d\n", decoder->range);
#endif
          // scaled range, highest bit (15) not set

          decoder->range = scaled_range >> 6; // shift range by one bit
          decoder->value <<= 1;               // shift value by one bit
          decoder->bits_needed++;

          if (decoder->bits_needed == 0)
            {
              decoder->bits_needed = -8;
              if (decoder->bitstream_curr != decoder->bitstream_end)
                { decoder->value |= *decoder->bitstream_curr++; }
            }
//Enable to monitor renorm range
#if 1
          //printf("MPS path range after renorm: %d\n", decoder->range);
          if(decoder->range < 256) {
        	  printf("========================Renorm error========================================\n\n\n");
          }
#endif
        } else {
        	incCabacDbgSeWORenormMPSCnt(ctx, se_idx);
        }
    }
  else
    {
      logtrace(LogCABAC,"[%3d] LPS\n",logcnt);

      // LPS path                                                                                    

      int num_bits = renorm_table[ LPS >> 3 ];
      decoder->value = (decoder->value - scaled_range);

      decoder->value <<= num_bits;
      decoder->range   = LPS << num_bits;  /* this is always >= 0x100 except for state 63,
                                              but state 63 is never used */
      decoded_bit      = 1 - model->MPSbit;

      if (model->state==0) { model->MPSbit = 1-model->MPSbit; }
      model->state = next_state_LPS[model->state];

      decoder->bits_needed += num_bits;

      if (decoder->bits_needed >= 0)
        {
          logtrace(LogCABAC,"bits_needed: %d\n", decoder->bits_needed);
          if (decoder->bitstream_curr != decoder->bitstream_end)
            { decoder->value |= (*decoder->bitstream_curr++) << decoder->bits_needed; }

          decoder->bits_needed -= 8;
        }
      addNCabacDbgSeRenormBitsCnt(ctx, se_idx, num_bits);
      incCabacDbgSeLPSCnt(ctx, se_idx);
    }

  logtrace(LogCABAC,"[%3d] -> bit %d  r:%x v:%x\n", logcnt, decoded_bit, decoder->range, decoder->value);
#ifdef DE265_LOG_TRACE
  logcnt++;
#endif

  //assert(decoder->range>=0x100);

  return decoded_bit;
}

/**
 * Decodes DecodeTerminate as mentioned in Figure 9-9
 */
int  decode_CABAC_term_bit(struct decoder_context *ctx, CABAC_decoder* decoder, Dbg_cabac_se_idx se_idx)
{
  decoder->range -= 2;
  uint32_t scaledRange = decoder->range << 7;

  //cabac analyze and debug
  incCabacDbgSeBinCnt(ctx, se_idx);

  if (decoder->value >= scaledRange)
    {
      return 1;
    }
  else
    {
      // there is a while loop in the standard, but it will always be executed only once

      if (scaledRange < (256<<7))
        {
          decoder->range = scaledRange >> 6;
          decoder->value *= 2;

          decoder->bits_needed++;
          if (decoder->bits_needed==0)
            {
              decoder->bits_needed = -8;

              if (decoder->bitstream_curr != decoder->bitstream_end) {
                decoder->value += (*decoder->bitstream_curr++);
              }
            }
        }

      return 0;
    }
}


/**
 * Decodes DecodeBypass bit as mentioned in Figure 9-8
 */
int  decode_CABAC_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, Dbg_cabac_se_idx se_idx)
{
  logtrace(LogCABAC,"[%3d] bypass r:%x v:%x\n",logcnt,decoder->range, decoder->value);
  //cabac analyze and debug
  incCabacDbgSeBinCnt(ctx, se_idx);

  //assert(decoder->range>=0x100);

//Enable this to analyze working of bypass decoding
#if 0
  uint32_t offset_b = decoder->value >> 7;
  uint32_t range = decoder->range;
  uint32_t offset_cur = 0;

  decoder->value <<= 1;
  decoder->bits_needed++;
#endif

  if (decoder->bits_needed >= 0)
    {
      //assert(decoder->bits_needed==0);

      decoder->bits_needed = -8;
      decoder->value |= *decoder->bitstream_curr++;
    }

//Enable this to analyze the working of Bypass decoding
#if 0
  offset_cur = decoder->value >> 7;
#endif

  int bit;
  uint32_t scaled_range = decoder->range << 7;
  if (decoder->value >= scaled_range)
    {
      decoder->value -= scaled_range;
      bit=1;
    }
  else
    {
      bit=0;
    }

  logtrace(LogCABAC,"[%3d] -> bit %d  r:%x v:%x\n", logcnt, bit, decoder->range, decoder->value);
#ifdef DE265_LOG_TRACE
  logcnt++;
#endif

//Enable this to analyze the working of bypass decoding
#if 0
  printf("%d\t%d\t%d\t%d\t%d\t%d\n", range, offset_cur % 2, offset_b, offset_cur, decoder->value >> 7, bit);
#endif

  //assert(decoder->range>=0x100);

  return bit;
}


int  decode_CABAC_TU_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int cMax, Dbg_cabac_se_idx se_idx)
{
  for (int i=0;i<cMax;i++)
    {
      int bit = decode_CABAC_bypass(ctx, decoder, se_idx);
      if (bit==0)
        return i;
    }

  return cMax;
}

/**
 *
 */
//not used anywhere
int  decode_CABAC_TU(struct decoder_context *ctx, CABAC_decoder* decoder, int cMax, context_model* model, Dbg_cabac_se_idx se_idx)
{
  for (int i=0;i<cMax;i++)
    {
      int bit = decode_CABAC_bit(ctx, decoder,model, se_idx);
      if (bit==0)
        return i;
    }

  return cMax;
}


int  decode_CABAC_FL_bypass_parallel(struct decoder_context *ctx, CABAC_decoder* decoder, int nBits, Dbg_cabac_se_idx se_idx)
{
  logtrace(LogCABAC,"[%3d] bypass group r:%x v:%x\n",logcnt,decoder->range, decoder->value);
  //cabac analyze and debug
  addNCabacDbgSeBinCnt(ctx, se_idx, nBits);

  decoder->value <<= nBits;
  decoder->bits_needed+=nBits;

  if (decoder->bits_needed >= 0)
    {
      int input = *decoder->bitstream_curr++;
      input <<= decoder->bits_needed;

      decoder->bits_needed -= 8;
      decoder->value |= input;
    }

  uint32_t scaled_range = decoder->range << 7;
  int value = decoder->value / scaled_range;
  if (unlikely(value>=(1<<nBits))) { value=(1<<nBits)-1; } // may happen with broken bitstreams
  decoder->value -= value * scaled_range;

  logtrace(LogCABAC,"[%3d] -> value %d  r:%x v:%x\n", logcnt+nBits-1,
           value, decoder->range, decoder->value);
#ifdef DE265_LOG_TRACE
  logcnt+=nBits;
#endif

  //assert(decoder->range>=0x100);

  return value;
}


int  decode_CABAC_FL_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int nBits, Dbg_cabac_se_idx se_idx)
{
  int value=0;


  if (likely(nBits<=8)) {
    if (nBits==0) {
      return 0;
    }
    // we could use decode_CABAC_bypass() for a single bit, but this seems to be slower
#if 0
    else if (nBits==1) {
      value = decode_CABAC_bypass(decoder);
    }
#endif
    else {
      value = decode_CABAC_FL_bypass_parallel(ctx, decoder,nBits, se_idx);
    }
  }
  else {
    value = decode_CABAC_FL_bypass_parallel(ctx, decoder,8, se_idx);
    nBits-=8;

    while (nBits--) {
      value <<= 1;
      value |= decode_CABAC_bypass(ctx, decoder, se_idx);
    }
  }

  logtrace(LogCABAC,"      -> FL: %d\n", value);

  return value;
}

int  decode_CABAC_TR_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int cRiceParam, int cTRMax, Dbg_cabac_se_idx se_idx)
{
  int prefix = decode_CABAC_TU_bypass(ctx, decoder, cTRMax>>cRiceParam, se_idx);
  if (prefix==4) { // TODO check: constant 4 only works for coefficient decoding
    return cTRMax;
  }

  int suffix = decode_CABAC_FL_bypass(ctx, decoder, cRiceParam, se_idx);

  return (prefix << cRiceParam) | suffix;
}

int  decode_CABAC_EGk_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int k, Dbg_cabac_se_idx se_idx)
{
  int base=0;
  int n=k;

  for (;;)
    {
      int bit = decode_CABAC_bypass(ctx, decoder, se_idx);
      if (bit==0)
        break;
      else {
        base += 1<<n;
        n++;
      }
    }

  int suffix = decode_CABAC_FL_bypass(ctx, decoder, n, se_idx);
  return base + suffix;
}
