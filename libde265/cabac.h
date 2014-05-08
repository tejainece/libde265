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

#ifndef DE265_CABAC_H
#define DE265_CABAC_H

#include <stdint.h>

struct decoder_context;
struct thread_context;
#include "dbg_cabac.h"

typedef struct {
  uint8_t* bitstream_start;
  uint8_t* bitstream_curr;
  uint8_t* bitstream_end;

  uint32_t range;		//current range
  uint32_t value;		 //this is the value(from the bit stream) to be decoded
  int16_t  bits_needed;  //?
} CABAC_decoder;


typedef struct {
  uint8_t MPSbit : 1;
  uint8_t state  : 7;
} context_model;


void init_CABAC_decoder(CABAC_decoder* decoder, uint8_t* bitstream, int length);
void init_CABAC_decoder_2(CABAC_decoder* decoder);
int  decode_CABAC_bit_2_parallel(struct decoder_context* ctx, struct thread_context* tctx, int ctxId[4]);
int  decode_CABAC_bit(struct decoder_context* ctx, CABAC_decoder* decoder, context_model* model, enum Dbg_cabac_se_idx se_idx);
int  decode_CABAC_TU(struct decoder_context *ctx, CABAC_decoder* decoder, int cMax, context_model* model, Dbg_cabac_se_idx se_idx);
int  decode_CABAC_term_bit(struct decoder_context *ctx, CABAC_decoder* decoder, Dbg_cabac_se_idx se_idx);

int  decode_CABAC_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, Dbg_cabac_se_idx se_idx);
int  decode_CABAC_TU_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int cMax, Dbg_cabac_se_idx se_idx);
int  decode_CABAC_FL_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int nBits, Dbg_cabac_se_idx se_idx);
int  decode_CABAC_TR_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int cRiceParam, int cTRMax, Dbg_cabac_se_idx se_idx);
int  decode_CABAC_EGk_bypass(struct decoder_context *ctx, CABAC_decoder* decoder, int k, Dbg_cabac_se_idx se_idx);

#endif
