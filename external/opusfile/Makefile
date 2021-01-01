# Makefile of the library "external/opus"
# David Weenink, 29 December 2020

include ../../makefile.defs

CPPFLAGS = -I ../../melder -I ../vorbis

OPUSFILE_OBJECTS = opusfile.o opusfile_info.o opusfile_internal.o opusfile_stream.o

SILK_OBJECTS = silk/CNG.o silk/code_signs.o silk/init_decoder.o silk/decode_core.o silk/decode_frame.o \
silk/decode_parameters.o silk/decode_indices.o silk/decode_pulses.o silk/decoder_set_fs.o silk/dec_API.o \
silk/enc_API.o silk/encode_indices.o silk/encode_pulses.o silk/gain_quant.o silk/interpolate.o \
silk/LP_variable_cutoff.o silk/NLSF_decode.o silk/NSQ.o silk/NSQ_del_dec.o silk/PLC.o \
silk/shell_coder.o silk/tables_gain.o silk/tables_LTP.o silk/tables_NLSF_CB_NB_MB.o silk/tables_NLSF_CB_WB.o \
silk/tables_other.o silk/tables_pitch_lag.o silk/tables_pulses_per_block.o \
silk/VAD.o silk/control_audio_bandwidth.o silk/quant_LTP_gains.o silk/VQ_WMat_EC.o \
silk/HP_variable_cutoff.o silk/NLSF_encode.o silk/NLSF_VQ.o silk/NLSF_unpack.o silk/NLSF_del_dec_quant.o \
silk/process_NLSFs.o silk/stereo_LR_to_MS.o silk/stereo_MS_to_LR.o silk/check_control_input.o \
silk/control_SNR.o silk/init_encoder.o silk/control_codec.o silk/A2NLSF.o silk/ana_filt_bank_1.o \
silk/biquad_alt.o silk/bwexpander_32.o silk/bwexpander.o silk/debug.o silk/decode_pitch.o \
silk/inner_prod_aligned.o silk/lin2log.o silk/log2lin.o silk/LPC_analysis_filter.o silk/LPC_inv_pred_gain.o \
silk/table_LSF_cos.o silk/NLSF2A.o silk/NLSF_stabilize.o silk/NLSF_VQ_weights_laroia.o silk/pitch_est_tables.o \
silk/resampler.o silk/resampler_down2_3.o silk/resampler_down2.o silk/resampler_private_AR2.o \
silk/resampler_private_down_FIR.o silk/resampler_private_IIR_FIR.o silk/resampler_private_up2_HQ.o \
silk/resampler_rom.o silk/sigm_Q15.o silk/sort.o silk/sum_sqr_shift.o silk/stereo_decode_pred.o \
silk/stereo_encode_pred.o silk/stereo_find_predictor.o silk/stereo_quant_pred.o silk/LPC_fit.o

OBJECTS = $(OPUSFILE_OBJECTS) \
	$(SILK_OBJECTS)

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
