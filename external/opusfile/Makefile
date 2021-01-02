# Makefile of the library "external/opus"
# David Weenink, 29 December 2020

include ../../makefile.defs

CPPFLAGS = -I ../../melder -I ../vorbis -I opus -I opus/celt -I opus/silk -I opus/silk/float

CFLAGS += -DHAVE_CONFIG_H 

OPUSFILE_OBJECTS = opusfile.o opusfile_info.o opusfile_internal.o opusfile_stream.o

OPUS_OBJECTS = opus/analysis.c opus/mlp.c opus/opus.c opus/opus_decoder.c opus/opus_multistream.c opus/repacketizer_demo.c \
opus/mapping_matrix.c opus/mlp_data.c opus/opus_compare.c opus/opus_demo.c opus/opus_multistream_decoder.c \
opus/opus_projection_decoder.c opus/repacketizer.c


CELT = opus/celt/

CELT_OBJECTS = $(CELT)bands.o $(CELT)celt.o $(CELT)celt_encoder.o $(CELT)celt_decoder.o \
$(CELT)cwrs.o $(CELT)entcode.o $(CELT)entdec.o $(CELT)entenc.o $(CELT)kiss_fft.o \
$(CELT)laplace.o $(CELT)mathops.o $(CELT)mdct.o $(CELT)modes.o $(CELT)pitch.o \
$(CELT)celt_lpc.o $(CELT)quant_bands.o $(CELT)rate.o $(CELT)vq.o

SILK = opus/silk/

SILK_OBJECTS = $(SILK)CNG.o $(SILK)code_signs.o $(SILK)init_decoder.o $(SILK)decode_core.o $(SILK)decode_frame.o \
$(SILK)decode_parameters.o $(SILK)decode_indices.o $(SILK)decode_pulses.o $(SILK)decoder_set_fs.o $(SILK)dec_API.o \
$(SILK)enc_API.o $(SILK)encode_indices.o $(SILK)encode_pulses.o $(SILK)gain_quant.o $(SILK)interpolate.o \
$(SILK)LP_variable_cutoff.o $(SILK)NLSF_decode.o $(SILK)NSQ.o $(SILK)NSQ_del_dec.o $(SILK)PLC.o \
$(SILK)shell_coder.o $(SILK)tables_gain.o $(SILK)tables_LTP.o $(SILK)tables_NLSF_CB_NB_MB.o $(SILK)tables_NLSF_CB_WB.o \
$(SILK)tables_other.o $(SILK)tables_pitch_lag.o $(SILK)tables_pulses_per_block.o \
$(SILK)VAD.o $(SILK)control_audio_bandwidth.o $(SILK)quant_LTP_gains.o $(SILK)VQ_WMat_EC.o \
$(SILK)HP_variable_cutoff.o $(SILK)NLSF_encode.o $(SILK)NLSF_VQ.o $(SILK)NLSF_unpack.o $(SILK)NLSF_del_dec_quant.o \
$(SILK)process_NLSFs.o $(SILK)stereo_LR_to_MS.o $(SILK)stereo_MS_to_LR.o $(SILK)check_control_input.o \
$(SILK)control_SNR.o $(SILK)init_encoder.o $(SILK)control_codec.o $(SILK)A2NLSF.o $(SILK)ana_filt_bank_1.o \
$(SILK)biquad_alt.o $(SILK)bwexpander_32.o $(SILK)bwexpander.o $(SILK)debug.o $(SILK)decode_pitch.o \
$(SILK)inner_prod_aligned.o $(SILK)lin2log.o $(SILK)log2lin.o $(SILK)LPC_analysis_filter.o $(SILK)LPC_inv_pred_gain.o \
$(SILK)table_LSF_cos.o $(SILK)NLSF2A.o $(SILK)NLSF_stabilize.o $(SILK)NLSF_VQ_weights_laroia.o $(SILK)pitch_est_tables.o \
$(SILK)resampler.o $(SILK)resampler_down2_3.o $(SILK)resampler_down2.o $(SILK)resampler_private_AR2.o \
$(SILK)resampler_private_down_FIR.o $(SILK)resampler_private_IIR_FIR.o $(SILK)resampler_private_up2_HQ.o \
$(SILK)resampler_rom.o $(SILK)sigm_Q15.o $(SILK)sort.o $(SILK)sum_sqr_shift.o $(SILK)stereo_decode_pred.o \
$(SILK)stereo_encode_pred.o $(SILK)stereo_find_predictor.o $(SILK)stereo_quant_pred.o $(SILK)LPC_fit.o

SILKFLOAT = opus/silk/float/

SILKFLOAT_OBJECTS = $(SILKFLOAT)apply_sine_window_FLP.c $(SILKFLOAT)autocorrelation_FLP.c \
$(SILKFLOAT)burg_modified_FLP.c $(SILKFLOAT)bwexpander_FLP.c $(SILKFLOAT)corrMatrix_FLP.c \
$(SILKFLOAT)encode_frame_FLP.c $(SILKFLOAT)energy_FLP.c $(SILKFLOAT)find_LPC_FLP.c $(SILKFLOAT)find_LTP_FLP.c \
$(SILKFLOAT)find_pitch_lags_FLP.c $(SILKFLOAT)find_pred_coefs_FLP.c $(SILKFLOAT)inner_product_FLP.c $(SILKFLOAT)k2a_FLP.c \
$(SILKFLOAT)LPC_analysis_filter_FLP.c $(SILKFLOAT)LPC_inv_pred_gain_FLP.c $(SILKFLOAT)LTP_analysis_filter_FLP.c \
$(SILKFLOAT)LTP_scale_ctrl_FLP.c $(SILKFLOAT)noise_shape_analysis_FLP.c $(SILKFLOAT)pitch_analysis_core_FLP.c \
$(SILKFLOAT)process_gains_FLP.c $(SILKFLOAT)regularize_correlations_FLP.c $(SILKFLOAT)residual_energy_FLP.c \
$(SILKFLOAT)scale_copy_vector_FLP.c $(SILKFLOAT)scale_vector_FLP.c $(SILKFLOAT)schur_FLP.c $(SILKFLOAT)sort_FLP.c \
$(SILKFLOAT)warped_autocorrelation_FLP.c $(SILKFLOAT)wrappers_FLP.c


OBJECTS = $(OPUSFILE_OBJECTS) \
	$(OPUS_OBJECTS) \
	$(CELT_OBJECTS) \
	$(SILK_OBJECTS) \
	$(SILKFLOAT_OBJECTS)

.PHONY: all clean

all: libopusfile.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libopusfile.a

libopusfile.a: $(OBJECTS)
	touch libopusfile.a
	rm libopusfile.a
	$(AR) cq libopusfile.a $(OBJECTS)
	$(RANLIB) libopusfile.a

$(OBJECTS): *.h ../../melder/*.h
