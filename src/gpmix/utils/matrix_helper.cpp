/*
 * matrix_helper.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: stegle
 */


#include <gpmix/utils/matrix_helper.h>
#include "matrix_helper.h"
#include <stdlib.h>

namespace gpmix{

bool isnull(const MatrixXd& m)
{
	return (m.cols()==0) & (m.rows()==0);
}

#ifndef PI
#define PI 3.14159265358979323846
#endif

mfloat_t randn(mfloat_t mu, mfloat_t sigma) {
	static bool deviateAvailable=false;	//	flag
	static mfloat_t storedDeviate;			//	deviate from previous calculation
	double dist, angle;

	//	If no deviate has been stored, the standard Box-Muller transformation is
	//	performed, producing two independent normally-distributed random
	//	deviates.  One is stored for the next round, and one is returned.
	if (!deviateAvailable) {

		//	choose a pair of uniformly distributed deviates, one for the
		//	distance and one for the angle, and perform transformations
		dist=sqrt( -2.0 * log(double(rand()) / double(RAND_MAX)) );
		angle= 2.0 * PI * (double(rand()) / double(RAND_MAX));

		//	calculate and store first deviate and set flag
		storedDeviate=dist*cos(angle);
		deviateAvailable=true;

		//	calcaulate return second deviate
		return (mfloat_t)(dist * sin(angle) * sigma + mu);
	}

	//	If a deviate is available from a previous call to this function, it is
	//	returned, and the flag is set to false.
	else {
		deviateAvailable=false;
		return storedDeviate*sigma + mu;
	}
}






/*
MatrixXd array2matrix(const float64* matrix,int32 rows,int32 cols)
{
	//create a matrix from a double array
	MatrixXd m = MatrixXd(rows,cols);
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
		{
			//m(i,j) = matrix[i*cols+j];
			m(i,j) = matrix[j*rows+i];
		}
	return m;
}

void matrix2array(const MatrixXd m,float32_t** matrix, int32_t* rows, int32_t*cols)
{
	int size = m.rows()*m.cols();
	//allocate memory
	(*matrix) = new float32_t[size];
	//set dimensions
	(*rows) = m.rows();
	(*cols) = m.cols();
	for (int i=0;i<m.rows();i++)
		for(int j=0;j<m.cols();j++)
		{
			//(*matrix)[i*m.cols()+j] = m(i,j);
			(*matrix)[j*m.rows()+i] = m(i,j);
		}
}

void matrix2array(const MatrixXd m,float64_t** matrix, int32_t* rows, int32_t*cols)
{
	int size = m.rows()*m.cols();
	//allocate memory
	(*matrix) = new float64_t[size];
	//set dimensions
	(*rows) = m.rows();
	(*cols) = m.cols();
	for (int i=0;i<m.rows();i++)
		for(int j=0;j<m.cols();j++)
		{
			//(*matrix)[i*m.cols()+j] = m(i,j);
			(*matrix)[j*m.rows()+i] = m(i,j);
		}
}
*/


MatrixXd randn(const muint_t n, const muint_t m)
/* create a randn matrix, i.e. matrix of Gaussian distributed random numbers*/
{
	MatrixXd rv(n,m);
	for (muint_t i=0; i<n; i++)
		for (muint_t j=0; j<m; j++) {
			double r = randn(0.0,1.0);
			rv(i,j) = r;
		}
	return rv;
}


MatrixXd Mrandrand(const muint_t n,const muint_t m)
{
	MatrixXd rv(n,m);
	for (muint_t i=0;i<n;i++)
		for(muint_t j=0;j<m;j++)
		{
			rv(i,j) = ((double)rand())/RAND_MAX;
		}
	return rv;
}


}