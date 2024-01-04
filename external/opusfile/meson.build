# meson.build of the external library "opusfile"
# David Weenink, 4 January 2024


sources = '''
	opusfile.cpp opusfile_info.cpp opusfile_internal.cpp
	opusfile_stream.cpp'''.split()

# to avoid recursion into the subdirectories opus, opus/celt, opus/silk
# and opus/silk/float where we could use the "files('f1.c', 'f2.c' ...'fn.c')"
# in which we have to single quote each individual file.
opus_raw_sources ='''
	analysis.c mlp.c opus.c opus_decoder.c 
	opus_multistream.c mapping_matrix.c mlp_data.c
	opus_multistream_decoder.c opus_projection_decoder.c
	repacketizer.c'''.split()

opus_sources = []
foreach file : opus_raw_sources
	opus_sources += 'opus' / file
endforeach

opus_celt_raw_sources = '''
	bands.c celt.c celt_encoder.c celt_decoder.c
	cwrs.c entcode.c entdec.c entenc.c kiss_fft.c
	laplace.c mathops.c mdct.c modes.c pitch.c
	celt_lpc.c quant_bands.c rate.c vq.c'''.split()

opus_celt_sources = []
foreach file : opus_celt_raw_sources
	opus_celt_sources += 'opus/celt/' / file
endforeach


opus_silk_raw_sources = '''
	CNG.c code_signs.c init_decoder.c decode_core.c
	decode_frame.c decode_parameters.c decode_indices.c
	decode_pulses.c decoder_set_fs.c dec_API.c
	enc_API.c encode_indices.c encode_pulses.c
	gain_quant.c interpolate.c LP_variable_cutoff.c
	NLSF_decode.c NSQ.c NSQ_del_dec.c
	PLC.c shell_coder.c tables_gain.c 
	tables_LTP.c tables_NLSF_CB_NB_MB.c tables_NLSF_CB_WB.c
	tables_other.c tables_pitch_lag.c tables_pulses_per_block.c
	VAD.c control_audio_bandwidth.c quant_LTP_gains.c
	VQ_WMat_EC.c HP_variable_cutoff.c NLSF_encode.c 
	NLSF_VQ.c NLSF_unpack.c NLSF_del_dec_quant.c
	process_NLSFs.c stereo_LR_to_MS.c stereo_MS_to_LR.c
	check_control_input.c control_SNR.c init_encoder.c
	control_codec.c A2NLSF.c ana_filt_bank_1.c
	biquad_alt.c bwexpander_32.c bwexpander.c
	debug.c decode_pitch.c inner_prod_aligned.c 
	lin2log.c log2lin.c LPC_analysis_filter.c 
	LPC_inv_pred_gain.c table_LSF_cos.c NLSF2A.c 
	NLSF_stabilize.c NLSF_VQ_weights_laroia.c pitch_est_tables.c
	resampler.c resampler_down2_3.c resampler_down2.c
	resampler_private_AR2.c resampler_private_down_FIR.c resampler_private_IIR_FIR.c resampler_private_up2_HQ.c
	resampler_rom.c sigm_Q15.c sort.c sum_sqr_shift.c
	stereo_decode_pred.c stereo_encode_pred.c
	stereo_find_predictor.c stereo_quant_pred.c LPC_fit.c'''.split()

opus_silk_sources = []
foreach file : opus_silk_raw_sources
	opus_silk_sources += 'opus/silk/' / file
endforeach

opus_silk_float_raw_sources = '''
	apply_sine_window_FLP.c autocorrelation_FLP.c
	burg_modified_FLP.c bwexpander_FLP.c corrMatrix_FLP.c
	encode_frame_FLP.c energy_FLP.c find_LPC_FLP.c
	find_LTP_FLP.c find_pitch_lags_FLP.c find_pred_coefs_FLP.c
	inner_product_FLP.c k2a_FLP.c LPC_analysis_filter_FLP.c
	LPC_inv_pred_gain_FLP.c LTP_analysis_filter_FLP.c
	LTP_scale_ctrl_FLP.c noise_shape_analysis_FLP.c
	pitch_analysis_core_FLP.c process_gains_FLP.c
	regularize_correlations_FLP.c residual_energy_FLP.c
	scale_copy_vector_FLP.c scale_vector_FLP.c schur_FLP.c
	sort_FLP.c warped_autocorrelation_FLP.c wrappers_FLP.c'''.split()

opus_silk_float_sources = []
foreach file : opus_silk_float_raw_sources
	opus_silk_float_sources += 'opus/silk/float/' / file
endforeach

opus_inc = include_directories ('opus', 'opus/celt', 'opus/silk', 'opus/silk/float')

libopusfile = static_library (
	'opusfile',
	sources : sources + opus_sources + opus_celt_sources + opus_silk_sources + opus_silk_float_sources,
	include_directories : [melder_inc, vorbis_inc, opus_inc]
)

libopusfile_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libopusfile
)

