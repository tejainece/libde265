#ifndef DE265_DBG_CABAC_H
#define DE265_DBG_CABAC_H

struct decoder_context;

typedef enum Dbg_cabac_se_idx {
  DBG_CSECI_END_OF_SLICE_SEGMENT_FLAG,
  DBG_CSECI_END_OF_SUB_STREAM_ONE_BIT,
  DBG_CSECI_SAO_MERGE_LEFT_FLAG,
  DBG_CSECI_SAO_MERGE_UP_FLAG,
  DBG_CSECI_SAO_TYPE_IDX_LUMA,
  DBG_CSECI_SAO_TYPE_IDX_CHROMA,
  DBG_CSECI_SAO_OFFSET_ABS,
  DBG_CSECI_SAO_OFFSET_SIGN,
  DBG_CSECI_SAO_BAND_POSITION,
  DBG_CSECI_SAO_EO_CLASS_LUMA,
  DBG_CSECI_SAO_EO_CLASS_CHROMA,
  DBG_CSECI_SPLIT_CU_FLAG,
  DBG_CSECI_CU_TRANSQUANT_BYPASS_FLAG,
  DBG_CSECI_CU_SKIP_FLAG,
  DBG_CSECI_PRED_MODE_FLAG,
  DBG_CSECI_PART_MODE,
  DBG_CSECI_PCM_FLAG,
  DBG_CSECI_PREV_INTRA_LUMA_PRED_FLAG,
  DBG_CSECI_MPM_IDX,
  DBG_CSECI_REM_INTRA_LUMA_PRED_MODE,
  DBG_CSECI_INTRA_CHROMA_PRED_MODE,
  DBG_CSECI_RQT_ROOT_CBF,
  DBG_CSECI_MERGE_FLAG,
  DBG_CSECI_MERGE_IDX,
  DBG_CSECI_INTER_PRED_IDC,
  DBG_CSECI_REF_IDX_L0,
  DBG_CSECI_REF_IDX_L1,
  DBG_CSECI_MVP_L0_FLAG,
  DBG_CSECI_MVP_L1_FLAG,
  DBG_CSECI_SPLIT_TRANSFORM_FLAG,
  DBG_CSECI_CBF_CB,
  DBG_CSECI_CBF_CR,
  DBG_CSECI_CBF_LUMA,
  DBG_CSECI_ABS_MVD_GREATER0_FLAG,
  DBG_CSECI_ABS_MVD_GREATER1_FLAG,
  DBG_CSECI_ABS_MVD_MINUS2,
  DBG_CSECI_MVD_SIGN_FLAG,
  DBG_CSECI_CU_QP_DELTA_ABS,
  DBG_CSECI_CU_QP_DELTA_SIGN_FLAG,
  DBG_CSECI_TRANSFORM_SKIP_FLAG,
  DBG_CSECI_LAST_SIG_COEFF_X_PREFIX,
  DBG_CSECI_LAST_SIG_COEFF_Y_PREFIX,
  DBG_CSECI_LAST_SIG_COEFF_X_SUFFIX,
  DBG_CSECI_LAST_SIG_COEFF_Y_SUFFIX,
  DBG_CSECI_CODED_SUB_BLOCK_FLAG,
  DBG_CSECI_SIG_COEFF_FLAG,
  DBG_CSECI_COEFF_ABS_LEVEL_GREATER1_FLAG,
  DBG_CSECI_COEFF_ABS_LEVEL_GREATER2_FLAG,
  DBG_CSECI_COEFF_ABS_LEVEL_REMAINING,
  DBG_CSECI_COEFF_SIGN_FLAG,
  DBG_CSECI_LENGTH
} Dbg_cabac_se_idx;

extern char Dbg_cabac_se_name[DBG_CSECI_LENGTH][30];

enum Dbg_cabac_dectyp_idx {
  DBG_CDT_DECODE_DECISION,
  DBG_CDT_DECODE_BYPASS,
  DBG_CDT_DECODE_TERMINATE,
  DBG_CDT_LENGTH
} ;

typedef struct Debug_cabac {
  //Keeps count of occurance of each syntax element
  uint32_t se_cnt[DBG_CSECI_LENGTH];
  //Keeps count of occurance of each bin in syntax element
  uint32_t se_bin_cnt[DBG_CSECI_LENGTH];
  //Keeps count of occurance od differenct type of decoding
  uint32_t dectyp_cnt[DBG_CDT_LENGTH];
  //Keeps count of renormalization during SE
  uint32_t se_renorm_bits_cnt[DBG_CSECI_LENGTH];
  //Keeps count of renormalization during SE
  uint32_t se_wo_renorm_mps_cnt[DBG_CSECI_LENGTH];
  //Keeps count of renormalization during SE
  uint32_t se_w_renorm_mps_cnt[DBG_CSECI_LENGTH];
  //Keeps count of renormalization during SE
  uint32_t se_lps_cnt[DBG_CSECI_LENGTH];
} Debug_cabac;

void printCabacDebugInfo(struct decoder_context*);
void incCabacDbgSeCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx);
void  incCabacDbgSeBinCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx);
void addNCabacDbgSeBinCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx, int n);
void incCabacDbgDectypCnt(struct decoder_context* ctx, enum Dbg_cabac_dectyp_idx);
void  incCabacDbgSeRenormBitsCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx);
void addNCabacDbgSeRenormBitsCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx, int n);

#endif
