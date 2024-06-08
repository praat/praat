

#define ooSTRUCT SoundToFormantAnalysisWorkspace
oo_DEFINE_CLASS (SoundToFormantAnalysisWorkspace, SampledAnalysisWorkspace)

	oo_OBJECT (SoundToLPCAnalysisWorkspace, soundToLPC)
	oo_OBJECT (LPCToFormantAnalysisWorkspace, lpcToFormant)

oo_END_CLASS (SoundToFormantAnalysisWorkspace)
#undef ooSTRUCT


/* End of file SoundToFormantAnalysisWorkspace_def.h */
 
