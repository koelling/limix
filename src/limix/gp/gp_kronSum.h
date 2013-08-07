/*
 * gp_kronSum.h
 *
 *  Created on: Jul 29, 2013
 *      Author: casale
 */

#ifndef GP_KronSum_H_
#define GP_KronSum_H_

#include "gp_base.h"

//debug
#include "limix/utils/matrix_helper.h"

namespace limix {

//forward definition:
class CGPkronSum;
class CGPkronSumCache;

class CGPkronSumCache : public CParamObject
{
	friend class CGPkronSum;
protected:
	MatrixXd SsigmaCache;
	MatrixXd ScstarCache;
	MatrixXd UcstarCache;
	MatrixXd LambdacCache;
	MatrixXd SomegaCache;
	MatrixXd SrstarCache;
	MatrixXd UrstarCache;
	MatrixXd LambdarCache;
	MatrixXd YrotPartCache;
	MatrixXd YrotCache;
	MatrixXd YtildeCache;
	MatrixXd Rrot;
	MatrixXd OmegaRot;

	CGPkronSum* gp;
	bool SVDcstarCacheNull,SVDrstarCacheNull;
	bool LambdarCacheNull,LambdacCacheNull;
	bool YrotPartCacheNull,YrotCacheNull,YtildeCacheNull;
	bool RrotCacheNull,OmegaRotCacheNull;
	//sync states
	Pbool syncLik,syncCovarc1,syncCovarc2,syncCovarr1,syncCovarr2,syncData;
	//validate & clear cache
	void validateCache();
	void updateSVDcstar();
	void updateSVDrstar();
public:
	PCovarianceFunctionCache covarc1;
	PCovarianceFunctionCache covarc2;
	PCovarianceFunctionCache covarr1;
	PCovarianceFunctionCache covarr2;

	CGPkronSumCache(CGPkronSum* gp);
	virtual ~CGPkronSumCache()	{};

	MatrixXd& rgetSsigma();
	MatrixXd& rgetScstar();
	MatrixXd& rgetUcstar();
	MatrixXd& rgetLambdac();
	MatrixXd& rgetSomega();
	MatrixXd& rgetSrstar();
	MatrixXd& rgetUrstar();
	MatrixXd& rgetLambdar();
	MatrixXd& rgetYrotPart();
	MatrixXd& rgetYrot();
	MatrixXd& rgetYtilde();
	MatrixXd& rgetRrot();
	MatrixXd& rgetOmegaRot();

	void argetSsigma(MatrixXd* out)
	{
		(*out) = rgetSsigma();
	}
	void argetScstar(MatrixXd* out)
	{
		(*out) = rgetScstar();
	}
	void argetUcstar(MatrixXd* out)
	{
		(*out) = rgetUcstar();
	}
	void argetLambdac(MatrixXd* out)
	{
		(*out) = rgetLambdac();
	}
	void argetSomega(MatrixXd* out)
	{
		(*out) = rgetSomega();
	}
	void argetSrstar(MatrixXd* out)
	{
		(*out) = rgetSrstar();
	}
	void argetUrstar(MatrixXd* out)
	{
		(*out) = rgetUrstar();
	}
	void argetLambdar(MatrixXd* out)
	{
		(*out) = rgetLambdar();
	}
	void argetYrotPart(MatrixXd* out)
	{
		(*out) = rgetYrotPart();
	}
	void argetYrot(MatrixXd* out)
	{
		(*out) = rgetYrot();
	}
	void argetYtilde(MatrixXd* out)
	{
		(*out) = rgetYtilde();
	}
	void argetRrot(MatrixXd* out)
	{
		(*out) = rgetRrot();
	}
	void argetOmegaRot(MatrixXd* out)
	{
		(*out) = rgetOmegaRot();
	}

};
typedef sptr<CGPkronSumCache> PGPkronSumCache;



#if (defined(SWIG) && !defined(SWIG_FILE_WITH_INIT))
%ignore CGPkronSum::predictMean;
%ignore CGPkronSum::predictVar;

%rename(predictMean) CGPkronSum::apredictMean;
%rename(predictVar) CGPkronSum::apredictVar;
#endif

class CGPkronSum: public CGPbase {
	friend class CGPkronSumCache;
	virtual void updateParams() throw (CGPMixException);

protected:
	//row and column covariance functions:
	PCovarianceFunction covarc1;
	PCovarianceFunction covarc2;
	PCovarianceFunction covarr1;
	PCovarianceFunction covarr2;
	//cache:
	PGPkronSumCache cache;

	//dimensions
	muint_t N;
	muint_t P;

	//running times
	mfloat_t rtSVDcols;
	mfloat_t rtSVDrows;
	mfloat_t rtLambdac;
	mfloat_t rtLambdar;
	mfloat_t rtYrotPart;
	mfloat_t rtYrot;
	mfloat_t rtYtilde;
	mfloat_t rtRrot;
	mfloat_t rtOmegaRot;
	mfloat_t rtCC1part1a;
	mfloat_t rtCC1part1b;
	mfloat_t rtCC1part1c;
	mfloat_t rtCC1part1d;
	mfloat_t rtCC1part1e;
	mfloat_t rtCC1part1f;
	mfloat_t rtCC1part2;
	mfloat_t rtCC2part1;
	mfloat_t rtCC2part2;
	mfloat_t rtCR1part1a;
	mfloat_t rtCR1part1b;
	mfloat_t rtCR1part2;
	mfloat_t rtCR2part1a;
	mfloat_t rtCR2part1b;
	mfloat_t rtCR2part2;
	mfloat_t rtLMLgradCovar;
	mfloat_t rtLMLgradDataTerm;
	mfloat_t is_it;
	mfloat_t rtGrad;
	mfloat_t rtLML1a;
	mfloat_t rtLML1b;
	mfloat_t rtLML1c;
	mfloat_t rtLML1d;
	mfloat_t rtLML1e;
	mfloat_t rtLML2;
	mfloat_t rtLML3;
	mfloat_t rtLML4;


public:
	CGPkronSum(const MatrixXd& Y,PCovarianceFunction covarr1, PCovarianceFunction covarc1,PCovarianceFunction covarr2, PCovarianceFunction covarc2, PLikelihood lik, PDataTerm dataTerm);
	virtual ~CGPkronSum();

	//getter for parameter bounds and hyperparam Mask
	virtual CGPHyperParams getParamBounds(bool upper) const;
	virtual CGPHyperParams getParamMask() const;

	mfloat_t LML() throw (CGPMixException);

	// Gradient
	CGPHyperParams LMLgrad() throw (CGPMixException);
	virtual void aLMLgrad_covarc1(VectorXd* out) throw (CGPMixException);
	virtual void aLMLgrad_covarc2(VectorXd* out) throw (CGPMixException);
	virtual void aLMLgrad_covarr1(VectorXd* out) throw (CGPMixException);
	virtual void aLMLgrad_covarr2(VectorXd* out) throw (CGPMixException);
	virtual void aLMLgrad_dataTerm(MatrixXd* out) throw (CGPMixException);


	/* DEBUGGING */

	//cache stuff
	void agetSc(MatrixXd *out) {(*out)=cache->covarc1->rgetSK();}
	void agetUc(MatrixXd *out) {(*out)=cache->covarc1->rgetUK();}
	void agetSr(MatrixXd *out) {(*out)=cache->covarr1->rgetSK();}
	void agetUr(MatrixXd *out) {(*out)=cache->covarr1->rgetUK();}
	void agetSsigma(MatrixXd *out) {(*out)=cache->covarc2->rgetSK();}
	void agetUsigma(MatrixXd *out) {(*out)=cache->covarc2->rgetUK();}
	void agetSomega(MatrixXd *out) {(*out)=cache->covarr2->rgetSK();}
	void agetUomega(MatrixXd *out) {(*out)=cache->covarr2->rgetUK();}
	void agetScstar(MatrixXd *out) {(*out)=cache->rgetScstar();}
	void agetUcstar(MatrixXd *out) {(*out)=cache->rgetUcstar();}
	void agetSrstar(MatrixXd *out) {(*out)=cache->rgetSrstar();}
	void agetUrstar(MatrixXd *out) {(*out)=cache->rgetUrstar();}
	void agetLambdac(MatrixXd *out) {(*out)=cache->rgetLambdac();}
	void agetLambdar(MatrixXd *out) {(*out)=cache->rgetLambdar();}
	void agetYrotPart(MatrixXd *out) {(*out)=cache->rgetYrotPart();}
	void agetYrot(MatrixXd *out) {(*out)=cache->rgetYrot();}
	void agetCstar(MatrixXd *out)
	{
		MatrixXd USisqrt;
		aUS2alpha(USisqrt,cache->covarc2->rgetUK(),cache->covarc2->rgetSK(),-0.5);
		(*out) = USisqrt.transpose()*cache->covarc1->rgetK()*USisqrt;
	}

	//dimensions
	muint_t getN() 	{return N;}
	muint_t getP() 	{return P;}


	//running times
	mfloat_t getRtSVDcols() 	{return rtSVDcols;}
	mfloat_t getRtSVDrows() 	{return rtSVDrows;}
	mfloat_t getRtLambdac() 	{return rtLambdac;}
	mfloat_t getRtLambdar() 	{return rtLambdar;}
	mfloat_t getRtYrotPart() 	{return rtYrotPart;}
	mfloat_t getRtYrot() 		{return rtYrot;}
	mfloat_t getRtYtilde() 		{return rtYtilde;}
	mfloat_t getRtRrot() 		{return rtRrot;}
	mfloat_t getRtOmegaRot() 	{return rtOmegaRot;}
	mfloat_t getRtCC1part1a() 	{return rtCC1part1a;}
	mfloat_t getRtCC1part1b() 	{return rtCC1part1b;}
	mfloat_t getRtCC1part1c() 	{return rtCC1part1c;}
	mfloat_t getRtCC1part1d() 	{return rtCC1part1d;}
	mfloat_t getRtCC1part1e() 	{return rtCC1part1e;}
	mfloat_t getRtCC1part1f() 	{return rtCC1part1f;}
	mfloat_t getRtCC1part2() 	{return rtCC1part2;}
	mfloat_t getRtCC2part1() 	{return rtCC2part1;}
	mfloat_t getRtCC2part2() 	{return rtCC2part2;}
	mfloat_t getRtCR1part1a() 	{return rtCR1part1a;}
	mfloat_t getRtCR1part1b() 	{return rtCR1part1b;}
	mfloat_t getRtCR1part2() 	{return rtCR1part2;}
	mfloat_t getRtCR2part1a() 	{return rtCR2part1a;}
	mfloat_t getRtCR2part1b() 	{return rtCR2part1b;}
	mfloat_t getRtCR2part2() 	{return rtCR2part2;}
	mfloat_t getRtLMLgradCovar() 	{return rtLMLgradCovar;}
	mfloat_t getRtLMLgradDataTerm() 	{return rtLMLgradDataTerm;}
	mfloat_t getIs_it() 	{return is_it;}
	mfloat_t getRtGrad() 	{return rtGrad;}
	mfloat_t getRtLML1a() 	{return rtLML1a;}
	mfloat_t getRtLML1b() 	{return rtLML1b;}
	mfloat_t getRtLML1c() 	{return rtLML1c;}
	mfloat_t getRtLML1d() 	{return rtLML1d;}
	mfloat_t getRtLML1e() 	{return rtLML1e;}
	mfloat_t getRtLML2() 	{return rtLML2;}
	mfloat_t getRtLML3() 	{return rtLML3;}
	mfloat_t getRtLML4() 	{return rtLML4;}






	/*
	void setX1(const CovarInput& X) throw (CGPMixException);
	void setX2(const CovarInput& X) throw (CGPMixException);
	void setY(const MatrixXd& Y);
	void setCovar1(PCovarianceFunction covar);
	void setCovar2(PCovarianceFunction covar);

	PCovarianceFunction getCovar1() {return this->covar1;};
	PCovarianceFunction getCovar2() {return this->covar2;};

	PGPkronSumCache agetCache() {return this->cache;}

	virtual mfloat_t LML(const CGPHyperParams& params) throw (CGPMixException)
	{
		return CGPbase::LML(params);
	}
	//same for concatenated list of parameters
	virtual mfloat_t LML(const VectorXd& params) throw (CGPMixException)
	{
		return CGPbase::LML(params);
	}

	*/

};
typedef sptr<CGPkronSum> PGPkronSum;


} /* namespace limix */
#endif /* GP_KronSum_H_ */
