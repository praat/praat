# meson.build for library "dwsys".
# David Weenink 30 May 2025

sources = '''
	ChebyshevSeries.cpp Collection_extensions.cpp Command.cpp
	DoublyLinkedList.cpp Eigen.cpp ExtendedReal.cpp
	FileInMemory.cpp
	FunctionSeries.cpp
	Graphics_extensions.cpp Index.cpp
	LegendreSeries.cpp
	MAT_numerics.cpp
	NMF.cpp NUM2.cpp NUMhuber.cpp NUMmachar.cpp 
	NUMcomplex.cpp NUMFourier.cpp NUMsorting.cpp
	NUMmathlib.cpp NUMstring.cpp NUMselect.cpp
	Permutation.cpp PermutationInversionCounter.cpp Permutation_and_Index.cpp
	Polynomial.cpp
	Roots.cpp
	SlopeSelector.cpp Spline.cpp SVD.cpp WorkvectorPool.cpp'''.split()

libdwsys = static_library (
	'dwsys',
	sources : sources,
	dependencies: gtk_dep,
	include_directories : [sys_inc, clapack_inc, melder_inc, dwtools_inc, fon_inc, gsl_inc, kar_inc, stat_inc, num_inc]
)

libdwsys_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libdwsys
)


