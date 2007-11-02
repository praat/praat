#include "Matrix.h"
#include "Eigen.h"

int Matrix_eigen (I, Matrix *eigenvectors, Matrix *eigenvalues) 
{
	iam (Matrix);
	Eigen eigen = new (Eigen);
	long i, j;

	*eigenvectors = NULL, *eigenvalues = NULL;

	if (eigen == NULL) return 0;	
	if (my nx != my ny) return Melder_error1 (L"(Matrix_eigen:) Matrix not square.");

	if (! Eigen_initFromSymmetricMatrix_f (eigen, my z, my nx)) goto end;
	
	*eigenvectors = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		my ymin, my ymax, my ny, my dy, my y1);
	if (*eigenvectors == NULL) goto end;
	*eigenvalues = Matrix_create (1, 1, 1, 1, 1, my ymin, my ymax, my ny, my dy, my y1);
	if (*eigenvalues == NULL) goto end;
	
	for (i = 1; i <= my nx; i ++)
	{
		(*eigenvalues) -> z[i][1] = eigen -> eigenvalues[i];
		for (j = 1; j <= my nx; j ++)
			(*eigenvectors) -> z[i][j] = eigen -> eigenvectors[i][j];
	}
end:
	forget (eigen);
	if (Melder_hasError ()) {
		_Thing_forget ((Thing *) eigenvectors); *eigenvectors = NULL;
		_Thing_forget ((Thing *) eigenvalues); *eigenvalues = NULL;
		return 0;
	}
	return 1;
}
