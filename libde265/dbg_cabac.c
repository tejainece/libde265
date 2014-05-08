#include <stdio.h>
#include <decctx.h>

char Dbg_cabac_se_name[DBG_CSECI_LENGTH][30] = {
  "end_of_slice_segment_flag", "end_of_sub_stream_one_bit",
  "sao_merge_left_flag",
  "sao_merge_up_flag",
  "sao_type_idx_luma",
  "sao_type_idx_chroma",
  "sao_offset_abs",
  "sao_offset_sign",
  "sao_band_position",
  "sao_eo_class_luma",
  "sao_eo_class_chroma",
  "split_cu_flag", "cu_transquant_bypass_flag", "cu_skip_flag",
  "pred_mode_flag", "part_mode ", "pcm_flag",
  "prev_intra_luma_pred_flag", "mpm_idx",
  "rem_intra_luma_pred_mode", "intra_chroma_pred_mode", "rqt_root_cbf",
  "merge_flag", "merge_idx",
  "inter_pred_idc", 
  "ref_idx_l0", "ref_idx_l1",
  "mvp_l0_flag", "mvp_l1_flag",
  "split_transform_flag",
  "cbf_cb", "cbf_cr", "cbf_luma",
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
    printf("\n===============CABAC se debug info==============\n");
    for(index = 0; index < DBG_CSECI_LENGTH; index++) {
    	printf("%*s:\t%d\t%d\t%d\t%d\t%d\n", 30, Dbg_cabac_se_name[index],
    			cabac_d.se_cnt[index],
    			cabac_d.se_bin_cnt[index],
    			cabac_d.se_mps_wo_renorm_cnt[index],
    			cabac_d.se_mps_w_renorm_cnt[index],
    			cabac_d.se_lps_cnt[index]);
    }
    printf("\n================================================\n");
}

LIBDE265_API void incCabacDbgSeCnt(decoder_context* ctx, enum Dbg_cabac_se_idx se_idx) {
    ctx->dbg_cabac.se_cnt[se_idx]++;
}

LIBDE265_API void incCabacDbgSeBinCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx) {
	ctx->dbg_cabac.se_bin_cnt[se_idx]++;
}

LIBDE265_API void addNCabacDbgSeBinCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx, int n) {
	ctx->dbg_cabac.se_bin_cnt[se_idx] = ctx->dbg_cabac.se_bin_cnt[se_idx] + n;
}

LIBDE265_API void incCabacDbgDectypCnt(struct decoder_context* ctx, enum Dbg_cabac_dectyp_idx dt_idx) {
    ctx->dbg_cabac.dectyp_cnt[dt_idx]++;
}

LIBDE265_API void incCabacDbgSeMPSWoRenormCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx) {
	ctx->dbg_cabac.se_mps_wo_renorm_cnt[se_idx]++;
}

LIBDE265_API void incCabacDbgSeMPSWRenormCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx) {
	ctx->dbg_cabac.se_mps_w_renorm_cnt[se_idx]++;
}

LIBDE265_API void incCabacDbgSeLPSCnt(struct decoder_context* ctx, enum Dbg_cabac_se_idx se_idx) {
	ctx->dbg_cabac.se_lps_cnt[se_idx]++;
}
