#include <stdio.h>
#include <decctx.h>

char Dbg_cabac_se_name[DBG_CSECI_LENGTH][30] = {
  "split_cu_flag", "cu_skip_flag",

  "abs_mvd_greater0_flag", "abs_mvd_greater1_flag", 
  "abs_mvd_minus2", "mvd_sign_flag",
  "cu_qp_delta_abs", "cu_qp_delta_sign_flag",
  "transform_skip_flag",
  "last_sig_coeff_x_prefix", "last_sig_coeff_y_prefix",
  "last_sig_coeff_x_suffix", "last_sig_coeff_y_suffix",
  "coded_sub_block_flag", "sig_coeff_flag",
  "coeff_abs_level_greater1_flag", "coeff_abs_level_greater2_flag",
  "coeff_abs_level_remaining", "coeff_sign_flag"
};

LIBDE265_API void printCabacDebugInfo(decoder_context* ctx) {
    uint32_t index = 0;
    Debug_cabac cabac_d = ctx->dbg_cabac;
    printf("\n===============CABAC debug info==============\n");
    for(index = 0; index < DBG_CSECI_LENGTH; index++) {
    	printf("%*s: %d\n", 30, Dbg_cabac_se_name[index], cabac_d.se_cnt[index]);
    }
    printf("\n=============================================\n");
}

LIBDE265_API void incCabacDbgSeCnt(decoder_context* ctx, enum Dbg_cabac_se_idx se_idx) {
    ctx->dbg_cabac.se_cnt[se_idx]++;
}

LIBDE265_API void incCabacDbgDectypCnt(struct decoder_context* ctx, enum Dbg_cabac_dectyp_idx dt_idx) {
    ctx->dbg_cabac.dectyp_cnt[dt_idx]++;
}
