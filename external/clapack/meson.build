# meson.build of the external library "clapack"
# David Weenink, 3 January 2024

sources = '''
	blas.cpp
	lapack.cpp lapack_dg.cpp lapack_dlaq.cpp
	lapack_dlar.cpp lapack_ds.cpp lapack_dt.cpp'''.split()

libclapack = static_library (
	'clapack',
	sources : sources,
	include_directories : melder_inc
)

libclapack_dep = declare_dependency (
	include_directories : include_directories('.'), link_with: libclapack
)
