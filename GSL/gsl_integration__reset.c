static inline void
reset_nrmax (gsl_integration_workspace * workspace);

static inline void
reset_nrmax (gsl_integration_workspace * workspace)
{
  workspace->nrmax = 0;
  workspace->i = workspace->order[0] ;
}
