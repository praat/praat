# Makefile of the directory "external/opusfile/opus/silk"
# David Weenink, 1 Januari 2021

include ../../../../makefile.defs

CPPFLAGS = -I ../../melder -I ../../../vorbis -I .. -I ../celt

OBJECTS = CNG.o code_signs.o init_decoder.o decode_core.o decode_frame.o \
decode_parameters.o decode_indices.o decode_pulses.o decoder_set_fs.o dec_API.o \
enc_API.o encode_indices.o encode_pulses.o gain_quant.o interpolate.o \
LP_variable_cutoff.o NLSF_decode.o NSQ.o NSQ_del_dec.o PLC.o \
shell_coder.o tables_gain.o tables_LTP.o tables_NLSF_CB_NB_MB.o tables_NLSF_CB_WB.o \
tables_other.o tables_pitch_lag.o tables_pulses_per_block.o \
VAD.o control_audio_bandwidth.o quant_LTP_gains.o VQ_WMat_EC.o \
HP_variable_cutoff.o NLSF_encode.o NLSF_VQ.o NLSF_unpack.o NLSF_del_dec_quant.o \
process_NLSFs.o stereo_LR_to_MS.o stereo_MS_to_LR.o check_control_input.o \
control_SNR.o init_encoder.o control_codec.o A2NLSF.o ana_filt_bank_1.o \
biquad_alt.o bwexpander_32.o bwexpander.o debug.o decode_pitch.o \
inner_prod_aligned.o lin2log.o log2lin.o LPC_analysis_filter.o LPC_inv_pred_gain.o \
table_LSF_cos.o NLSF2A.o NLSF_stabilize.o NLSF_VQ_weights_laroia.o pitch_est_tables.o \
resampler.o resampler_down2_3.o resampler_down2.o resampler_private_AR2.o \
resampler_private_down_FIR.o resampler_private_IIR_FIR.o resampler_private_up2_HQ.o \
resampler_rom.o sigm_Q15.o sort.o sum_sqr_shift.o stereo_decode_pred.o \
stereo_encode_pred.o stereo_find_predictor.o stereo_quant_pred.o LPC_fit.o

.PHONY: all clean

all: libsilk.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libsilk.a

libsilk.a: $(OBJECTS)
	touch libsilk.a
	rm libsilk.a
	$(AR) cq libsilk.a $(OBJECTS)
	$(RANLIB) libsilk.a

$(OBJECTS): *.h ../../../../melder/*.h
