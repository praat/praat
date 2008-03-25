static inline
void set_initial_result (gsl_integration_workspace * workspace, 
                         double result, double error);

static inline
void set_initial_result (gsl_integration_workspace * workspace, 
                         double result, double error)
{
  workspace->size = 1;
  workspace->rlist[0] = result;
  workspace->elist[0] = error;
}
