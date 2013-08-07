/*
 * gp_kronSum.cpp
 *
 *  Created on: Jul 29, 2013
 *      Author: casale
 */

#include "gp_kronSum.h"
#include "limix/utils/matrix_helper.h"
#include <ctime>

namespace limix {

mfloat_t te1(clock_t beg){
	clock_t end = clock();
	mfloat_t TE = mfloat_t(end - beg) / CLOCKS_PER_SEC;
	return TE;
}

/* CGPkronSumCache */

CGPkronSumCache::CGPkronSumCache(CGPkronSum* gp)
{
	this->syncCovarc1 = Pbool(new bool);
	this->syncCovarc2 = Pbool(new bool);
	this->syncCovarr1 = Pbool(new bool);
	this->syncCovarr2 = Pbool(new bool);
	this->syncLik = Pbool(new bool);
	this->syncData = Pbool(new bool);
	//add to list of sync parents
	this->addSyncParent(syncCovarc1);
	this->addSyncParent(syncCovarc2);
	this->addSyncParent(syncCovarr1);
	this->addSyncParent(syncCovarr2);
	this->addSyncParent(syncLik);
	this->addSyncParent(syncData);

	this->gp = gp;
	this->covarc1 = PCovarianceFunctionCache(new CCovarianceFunctionCache(this->gp->covarc1));
	this->covarc2 = PCovarianceFunctionCache(new CCovarianceFunctionCache(this->gp->covarc2));
	this->covarr1 = PCovarianceFunctionCache(new CCovarianceFunctionCache(this->gp->covarr1));
	this->covarr2 = PCovarianceFunctionCache(new CCovarianceFunctionCache(this->gp->covarr2));
	//add sync liestener
	covarc1->addSyncChild(this->syncCovarc1);
	covarc2->addSyncChild(this->syncCovarc2);
	covarr1->addSyncChild(this->syncCovarr1);
	covarr2->addSyncChild(this->syncCovarr2);
	this->gp->lik->addSyncChild(this->syncLik);
	this->gp->dataTerm->addSyncChild(this->syncData);
	//set all cache Variables to Null
	SVDcstarCacheNull=true;
	LambdacCacheNull=true;
	SVDrstarCacheNull=true;
	LambdarCacheNull=true;
	YrotPartCacheNull=true;
	YrotCacheNull=true;
	YtildeCacheNull=true;
	RrotCacheNull=true;
	OmegaRotCacheNull=true;
}



void CGPkronSumCache::validateCache()
{
	muint_t maskRR = this->gp->covarr1->getParamMask().sum()+this->gp->covarr2->getParamMask().sum();
	if((!*syncCovarc1) || (!*syncCovarc2))
	{
		SVDcstarCacheNull=true;
		LambdacCacheNull=true;
		YrotCacheNull=true;
		YtildeCacheNull=true;
	}
	if(((!*syncCovarr1) || (!*syncCovarr2)) && maskRR>0)
	{
		SVDrstarCacheNull=true;
		LambdarCacheNull=true;
		YrotPartCacheNull=true;
		YrotCacheNull=true;
		YtildeCacheNull=true;
		RrotCacheNull=true;
		OmegaRotCacheNull=true;
	}
	if((!*syncData)) {
		YrotPartCacheNull=true;
		YrotCacheNull=true;
		YtildeCacheNull=true;
	}
	//set all sync
	setSync();
}

void CGPkronSumCache::updateSVDcstar()
{
	MatrixXd USisqrt;
	SsigmaCache=this->covarc2->rgetSK();
	aUS2alpha(USisqrt,this->covarc2->rgetUK(),SsigmaCache,-0.5);
	MatrixXd Cstar = USisqrt.transpose()*this->covarc1->rgetK()*USisqrt;
	// ADD SOME DIAGONAL STUFF?
	Eigen::SelfAdjointEigenSolver<MatrixXd> eigensolver(Cstar);
	UcstarCache = eigensolver.eigenvectors();
	ScstarCache = eigensolver.eigenvalues();
}

MatrixXd& CGPkronSumCache::rgetSsigma()
{
	validateCache();
	if (SVDcstarCacheNull) {
		updateSVDcstar();
		SVDcstarCacheNull=false;
	}
	return SsigmaCache;
}

MatrixXd& CGPkronSumCache::rgetScstar()
{
	validateCache();
	clock_t beg = clock();
	if (SVDcstarCacheNull) {
		updateSVDcstar();
		SVDcstarCacheNull=false;
	}
    this->gp->rtSVDcols=te1(beg);
	return ScstarCache;
}

MatrixXd& CGPkronSumCache::rgetUcstar()
{
	validateCache();
	if (SVDcstarCacheNull) {
		updateSVDcstar();
		SVDcstarCacheNull=false;
	}
	return UcstarCache;
}

MatrixXd& CGPkronSumCache::rgetLambdac()
{
	validateCache();
	clock_t beg = clock();
	if (LambdacCacheNull) {
		aUS2alpha(LambdacCache,rgetUcstar().transpose(),this->covarc2->rgetSK(),-0.5);
		LambdacCache*=this->covarc2->rgetUK().transpose();
		LambdacCacheNull=false;
	}
    this->gp->rtLambdac=te1(beg);
	return LambdacCache;
}

void CGPkronSumCache::updateSVDrstar()
{
	clock_t beg = clock();
	MatrixXd USisqrt;
	SomegaCache=this->covarr2->rgetSK();
	aUS2alpha(USisqrt,this->covarr2->rgetUK(),SomegaCache,-0.5);
	MatrixXd Rstar = USisqrt.transpose()*this->covarr1->rgetK()*USisqrt;
	// ADD SOME DIAGONAL STUFF?
	Eigen::SelfAdjointEigenSolver<MatrixXd> eigensolver(Rstar);
	UrstarCache = eigensolver.eigenvectors();
	SrstarCache = eigensolver.eigenvalues();
	std::cout<<"SVD rows:   "<<te1(beg)<<std::endl;
}

MatrixXd& CGPkronSumCache::rgetSomega()
{
	validateCache();
	if (SVDrstarCacheNull) {
		updateSVDrstar();
		SVDrstarCacheNull=false;
	}
	return SomegaCache;
}

MatrixXd& CGPkronSumCache::rgetSrstar()
{
	validateCache();
	clock_t beg = clock();
	if (SVDrstarCacheNull) {
		updateSVDrstar();
		SVDrstarCacheNull=false;
	}
    this->gp->rtSVDrows=te1(beg);
	return SrstarCache;
}

MatrixXd& CGPkronSumCache::rgetUrstar()
{
	validateCache();
	if (SVDrstarCacheNull) {
		updateSVDrstar();
		SVDrstarCacheNull=false;
	}
	return UrstarCache;
}

MatrixXd& CGPkronSumCache::rgetLambdar()
{
	validateCache();
	clock_t beg = clock();
	if (LambdarCacheNull) {
		aUS2alpha(LambdarCache,rgetUrstar().transpose(),this->covarr2->rgetSK(),-0.5);
		LambdarCache*=this->covarr2->rgetUK().transpose();
		LambdarCacheNull=false;
	}
    this->gp->rtLambdar=te1(beg);
	return LambdarCache;
}

MatrixXd& CGPkronSumCache::rgetYrotPart()
{
	validateCache();
	clock_t beg = clock();
	if (YrotPartCacheNull) {
		//Rotate columns of Y
		YrotPartCache.resize(gp->getY().rows(),gp->getY().cols());
		MatrixXd& Lambdar = rgetLambdar();
		for (muint_t p=0; p<this->gp->getY().cols(); p++)
			YrotPartCache.block(0,p,gp->getY().rows(),1).noalias()=Lambdar*gp->dataTerm->evaluate().block(0,p,gp->getY().rows(),1);
		YrotPartCacheNull=false;
	}
    this->gp->rtYrotPart=te1(beg);
	return YrotPartCache;
}

MatrixXd& CGPkronSumCache::rgetYrot()
{
	validateCache();
	clock_t beg = clock();
	if (YrotCacheNull) {
		//Rotate rows of Y
		YrotCache.resize(gp->getY().rows(),gp->getY().cols());
		MatrixXd& Lambdac = rgetLambdac();
		for (muint_t n=0; n<this->gp->getY().rows(); n++)
			YrotCache.block(n,0,1,gp->getY().cols()).noalias()=rgetYrotPart().block(n,0,1,gp->getY().cols())*Lambdac.transpose();
		YrotCacheNull=false;
	}
    this->gp->rtYrot=te1(beg);
	return YrotCache;
}

MatrixXd& CGPkronSumCache::rgetYtilde()
{
	validateCache();
	clock_t beg = clock();
	if (YtildeCacheNull) {
	    YtildeCache.resize(gp->getN(),gp->getP());
	    MatrixXd& Yrot = rgetYrot();
	    MatrixXd& Scstar = rgetScstar();
	    MatrixXd& Srstar = rgetSrstar();
	    for (muint_t n=0; n<this->gp->getN(); n++)	{
	        for (muint_t p=0; p<this->gp->getP(); p++)	{
	        	YtildeCache(n,p)=Yrot(n,p)/(Scstar(p,0)*Srstar(n,0)+1);
	    	}
	    }
	    YtildeCacheNull=false;
	}
    this->gp->rtYtilde=te1(beg);
	return YtildeCache;
}

MatrixXd& CGPkronSumCache::rgetRrot()
{
	validateCache();
	clock_t beg = clock();
	if (RrotCacheNull) {
		Rrot=rgetLambdar()*this->covarr1->rgetK()*rgetLambdar().transpose();
		RrotCacheNull=false;
	}
    this->gp->rtRrot=te1(beg);
	return Rrot;
}

MatrixXd& CGPkronSumCache::rgetOmegaRot()
{
	validateCache();
	clock_t beg = clock();
	if (OmegaRotCacheNull) {
		//Rotate rows of Y
		OmegaRot=rgetLambdar()*this->covarr2->rgetK()*rgetLambdar().transpose();
		OmegaRotCacheNull=false;
	}
    this->gp->rtOmegaRot=te1(beg);
	return OmegaRot;
}


/* CGPkronSum */

CGPkronSum::CGPkronSum(const MatrixXd& Y,
						PCovarianceFunction covarr1, PCovarianceFunction covarc1,
						PCovarianceFunction covarr2, PCovarianceFunction covarc2,
    					PLikelihood lik, PDataTerm dataTerm) : CGPbase(covarr1,lik,dataTerm)
{
	this->covarc1=covarc1;
	this->covarc2=covarc2;
	this->covarr1=covarr1;
	this->covarr2=covarr2;
	this->cache = PGPkronSumCache(new CGPkronSumCache(this));
	this->setY(Y);
	this->N=Y.rows();
	this->P=Y.cols();
	rtLML1a=0;
	rtLML1b=0;
	rtLML1c=0;
	rtLML1d=0;
	rtLML1e=0;
	rtLML2=0;
	rtLML3=0;
	rtLML4=0;
	rtGrad=0;
	rtCC1part1a=0;
	rtCC1part1b=0;
	rtCC1part1c=0;
	rtCC1part1d=0;
	rtCC1part1e=0;
	rtCC1part1f=0;
	rtCC1part2=0;
	rtCC2part1=0;
	rtCC2part2=0;
}


CGPkronSum::~CGPkronSum()
{
}

void CGPkronSum::updateParams() throw (CGPMixException)
{

	//is this needed?
	//CGPbase::updateParams();
	if(this->params.exists("covarc1"))
		this->covarc1->setParams(this->params["covarc1"]);

	if(this->params.exists("covarc2"))
		this->covarc2->setParams(this->params["covarc2"]);

	if(this->params.exists("covarr1"))
		this->covarr1->setParams(this->params["covarr1"]);

	if(this->params.exists("covarr2"))
		this->covarr2->setParams(this->params["covarr2"]);

	if(this->params.exists("dataTerm"))
	{
		this->dataTerm->setParams(this->params["dataTerm"]);
	}

}

CGPHyperParams CGPkronSum::getParamBounds(bool upper) const
{
	CGPHyperParams rv;
	//query covariance function bounds
	CovarParams covarc1_lower,covarc1_upper;
	this->covarc1->agetParamBounds(&covarc1_lower,&covarc1_upper);
	if (upper)	rv["covarc1"] = covarc1_upper;
	else		rv["covarc1"] = covarc1_lower;
	CovarParams covarc2_lower,covarc2_upper;
	this->covarc2->agetParamBounds(&covarc2_lower,&covarc2_upper);
	if (upper)	rv["covarc2"] = covarc2_upper;
	else		rv["covarc2"] = covarc2_lower;
	CovarParams covarr1_lower,covarr1_upper;
	this->covarr1->agetParamBounds(&covarr1_lower,&covarr1_upper);
	if (upper)	rv["covarr1"] = covarr1_upper;
	else		rv["covarr1"] = covarr1_lower;
	CovarParams covarr2_lower,covarr2_upper;
	this->covarr2->agetParamBounds(&covarr2_lower,&covarr2_upper);
	if (upper)	rv["covarr2"] = covarr2_upper;
	else		rv["covarr2"] = covarr2_lower;
	return rv;
}


CGPHyperParams CGPkronSum::getParamMask() const {
	CGPHyperParams rv;
	rv["covarc1"] = this->covarc1->getParamMask();
	rv["covarc2"] = this->covarc2->getParamMask();
	rv["covarr1"] = this->covarr1->getParamMask();
	rv["covarr2"] = this->covarr2->getParamMask();

	return rv;
}

mfloat_t CGPkronSum::LML() throw (CGPMixException)
{
	clock_t beg = clock();
    //get stuff from cache
    MatrixXd Ssigma = cache->rgetSsigma();
    rtLML1a+=te1(beg);
    beg = clock();
    MatrixXd Somega = cache->rgetSomega();
    rtLML1b+=te1(beg);
    beg = clock();
    MatrixXd& Scstar = cache->rgetScstar();
    rtLML1c+=te1(beg);
    beg = clock();
    MatrixXd& Srstar = cache->rgetSrstar();
    rtLML1d+=te1(beg);
    beg = clock();
    MatrixXd& Yrot = cache->rgetYrot();
    if (rtLML1e==0) {
    	rtLML1e+=te1(beg);
    }
    else	rtLML1e+=te1(beg);

    beg = clock();
    //1. logdet:
    mfloat_t lml_det = 0;
    mfloat_t temp = 0;
    for (muint_t p=0; p<Yrot.cols(); p++)
    	temp+=std::log(Ssigma(p,0));
    lml_det += Yrot.rows()*temp;
    temp = 0;
    for (muint_t n=0; n<Yrot.rows(); n++)
    	temp+=std::log(Somega(n,0));
    lml_det += Yrot.cols()*temp;
    temp = 0;
    for (muint_t n=0; n<Yrot.rows(); n++)
        for (muint_t p=0; p<Yrot.cols(); p++)
        	temp+=std::log(Scstar(p,0)*Srstar(n,0)+1);
    lml_det += temp;
    lml_det *= 0.5;
    rtLML2+=te1(beg);

    beg = clock();
    //2. quadratic term
    mfloat_t lml_quad = 0;
    for (muint_t n=0; n<Yrot.rows(); n++)	{
        for (muint_t p=0; p<Yrot.cols(); p++)	{
    		lml_quad+=std::pow(Yrot(n,p),2)/(Scstar(p,0)*Srstar(n,0)+1);
    	}
    }
    lml_quad *= 0.5;
    rtLML3+=te1(beg);

    beg = clock();
    //3. constants
    mfloat_t lml_const = 0.5*Yrot.cols()*Yrot.rows() * limix::log((2.0 * PI));
    rtLML4+=te1(beg);

    return lml_quad + lml_det + lml_const;
};


CGPHyperParams CGPkronSum::LMLgrad() throw (CGPMixException)
{
    CGPHyperParams rv;
    //calculate gradients for parameter components in params:
	clock_t beg = clock();
    if(params.exists("covarc1")){
        VectorXd grad_covar;
        aLMLgrad_covarc1(&grad_covar);
        rv.set("covarc1", grad_covar);
    }
    if(params.exists("covarc2")){
        VectorXd grad_covar;
        aLMLgrad_covarc2(&grad_covar);
        rv.set("covarc2", grad_covar);
    }
    if(params.exists("covarr1")){
        VectorXd grad_covar;
        if (covarr1->getParamMask().sum()==0)
        	grad_covar=VectorXd::Zero(covarr1->getNumberParams(),1);
        else
        	aLMLgrad_covarr1(&grad_covar);
        rv.set("covarr1", grad_covar);
    }
    if(params.exists("covarr2")){
        VectorXd grad_covar;
        if (covarr1->getParamMask().sum()==0)
        	grad_covar=VectorXd::Zero(covarr2->getNumberParams(),1);
        else
        aLMLgrad_covarr2(&grad_covar);
        rv.set("covarr2", grad_covar);
    }
    this->rtLMLgradCovar=te1(beg);
	beg = clock();
    if (params.exists("dataTerm"))
    {
    	MatrixXd grad_dataTerm;
    	aLMLgrad_dataTerm(&grad_dataTerm);
    	rv.set("dataTerm",grad_dataTerm);
    }
    this->rtLMLgradDataTerm=te1(beg);
    rtGrad+=rtLMLgradCovar+rtLMLgradDataTerm;
    return rv;
}

void CGPkronSum::aLMLgrad_covarc1(VectorXd *out) throw (CGPMixException)
{
	clock_t beg = clock();
    //get stuff from cache
    MatrixXd& Scstar = cache->rgetScstar();
    this->rtCC1part1a+=te1(beg);
	beg = clock();
    MatrixXd& Srstar = cache->rgetSrstar();
    this->rtCC1part1b+=te1(beg);
	beg = clock();
    MatrixXd& Yrot = cache->rgetYrot();
    this->rtCC1part1c+=te1(beg);
	beg = clock();
	MatrixXd& Lambdac = cache->rgetLambdac();
    this->rtCC1part1d+=te1(beg);
	beg = clock();
	MatrixXd& Ytilde = cache->rgetYtilde();
    this->rtCC1part1e+=te1(beg);
	beg = clock();
	//covar
	MatrixXd& Rrot = cache->rgetRrot();
    this->rtCC1part1f+=te1(beg);

	beg = clock();
    //start loop trough covariance paramenters
    (*out).resize(covarc1->getNumberParams(),1);
    MatrixXd CgradRot(this->getY().rows(),this->getY().rows());
	for (muint_t i=0; i<covarc1->getNumberParams(); i++) {
		CgradRot=Lambdac*covarc1->Kgrad_param(i)*Lambdac.transpose();
		//1. grad logdet
		mfloat_t grad_det = 0;
	    for (muint_t n=0; n<Yrot.rows(); n++)	{
	        for (muint_t p=0; p<Yrot.cols(); p++)	{
	        	grad_det+=CgradRot(p,p)*Rrot(n,n)/(Scstar(p,0)*Srstar(n,0)+1);
	    	}
	    }
	    grad_det*=0.5;
		//2. grad quadratic term
	    // Decomposition in columns and row
	    MatrixXd _Ytilde(Yrot.rows(),Yrot.cols());
	    MatrixXd YtildeR(Yrot.rows(),Yrot.cols());
		for (muint_t p=0; p<Yrot.cols(); p++)
			_Ytilde.block(0,p,Yrot.rows(),1).noalias()=Rrot*Ytilde.block(0,p,Yrot.rows(),1);
		for (muint_t n=0; n<Yrot.rows(); n++)
			YtildeR.block(n,0,1,Yrot.cols()).noalias()=_Ytilde.block(n,0,1,Yrot.cols())*CgradRot.transpose();
		mfloat_t grad_quad = -0.5*(Ytilde.array()*YtildeR.array()).sum();
		(*out)(i,0)=grad_det+grad_quad;
    }
    this->rtCC1part2+=te1(beg);
}

void CGPkronSum::aLMLgrad_covarc2(VectorXd *out) throw (CGPMixException)
{
	clock_t beg = clock();
    //get stuff from cache
    MatrixXd& Scstar = cache->rgetScstar();
    MatrixXd& Srstar = cache->rgetSrstar();
    MatrixXd& Yrot = cache->rgetYrot();
	MatrixXd& Lambdac = cache->rgetLambdac();
	MatrixXd& Ytilde = cache->rgetYtilde();
	//covar
	MatrixXd& OmegaRot = cache->rgetOmegaRot();
    this->rtCC2part1+=te1(beg);

	beg = clock();
    //start loop trough covariance paramenters
    (*out).resize(covarc2->getNumberParams(),1);
    MatrixXd SigmaGradRot(this->getY().rows(),this->getY().rows());
	for (muint_t i=0; i<covarc2->getNumberParams(); i++) {
		SigmaGradRot=Lambdac*covarc2->Kgrad_param(i)*Lambdac.transpose();
		//1. grad logdet
		mfloat_t grad_det = 0;
	    for (muint_t n=0; n<Yrot.rows(); n++)	{
	        for (muint_t p=0; p<Yrot.cols(); p++)	{
	        	grad_det+=SigmaGradRot(p,p)*OmegaRot(n,n)/(Scstar(p,0)*Srstar(n,0)+1);
	    	}
	    }
	    grad_det*=0.5;
		//2. grad quadratic term
	    // Decomposition in columns and row
	    MatrixXd _Ytilde(Yrot.rows(),Yrot.cols());
	    MatrixXd YtildeR(Yrot.rows(),Yrot.cols());
		for (muint_t p=0; p<Yrot.cols(); p++)
			_Ytilde.block(0,p,Yrot.rows(),1).noalias()=OmegaRot*Ytilde.block(0,p,Yrot.rows(),1);
		for (muint_t n=0; n<Yrot.rows(); n++)
			YtildeR.block(n,0,1,Yrot.cols()).noalias()=_Ytilde.block(n,0,1,Yrot.cols())*SigmaGradRot.transpose();
		mfloat_t grad_quad = -0.5*(Ytilde.array()*YtildeR.array()).sum();
		(*out)(i,0)=grad_det+grad_quad;
    }
    this->rtCC2part2+=te1(beg);
}

void CGPkronSum::aLMLgrad_covarr1(VectorXd *out) throw (CGPMixException)
{
	clock_t beg = clock();
    //get stuff from cache
    MatrixXd& Scstar = cache->rgetScstar();
    MatrixXd& Srstar = cache->rgetSrstar();
    MatrixXd& Yrot = cache->rgetYrot();
	MatrixXd& Lambdac = cache->rgetLambdac();
	MatrixXd& Lambdar = cache->rgetLambdar();
	MatrixXd& Ytilde = cache->rgetYtilde();
    this->rtCR1part1a+=te1(beg);
	beg = clock();
	//covar
	MatrixXd Crot = Lambdac*cache->covarc1->rgetK()*Lambdac.transpose();
    this->rtCR1part1b+=te1(beg);

	beg = clock();
    //start loop trough covariance paramenters
    (*out).resize(covarr1->getNumberParams(),1);
    MatrixXd RgradRot(this->getY().rows(),this->getY().rows());
	for (muint_t i=0; i<covarr1->getNumberParams(); i++) {
		beg = clock();
		RgradRot=Lambdar*covarr1->Kgrad_param(i)*Lambdar.transpose();
		this->is_it+=te1(beg);
		//1. grad logdet
		mfloat_t grad_det = 0;
	    for (muint_t n=0; n<Yrot.rows(); n++)	{
	        for (muint_t p=0; p<Yrot.cols(); p++)	{
	        	grad_det+=Crot(p,p)*RgradRot(n,n)/(Scstar(p,0)*Srstar(n,0)+1);
	    	}
	    }
	    grad_det*=0.5;
		//2. grad quadratic term
	    // Decomposition in columns and row
	    MatrixXd _Ytilde(Yrot.rows(),Yrot.cols());
	    MatrixXd YtildeR(Yrot.rows(),Yrot.cols());
		for (muint_t p=0; p<Yrot.cols(); p++)
			_Ytilde.block(0,p,Yrot.rows(),1).noalias()=RgradRot*Ytilde.block(0,p,Yrot.rows(),1);
		for (muint_t n=0; n<Yrot.rows(); n++)
			YtildeR.block(n,0,1,Yrot.cols()).noalias()=_Ytilde.block(n,0,1,Yrot.cols())*Crot.transpose();
		mfloat_t grad_quad = -0.5*(Ytilde.array()*YtildeR.array()).sum();
		(*out)(i,0)=grad_det+grad_quad;
    }
    this->rtCR1part2+=te1(beg);
}

void CGPkronSum::aLMLgrad_covarr2(VectorXd *out) throw (CGPMixException)
{
	clock_t beg = clock();
    //get stuff from cache
    MatrixXd& Scstar = cache->rgetScstar();
    MatrixXd& Srstar = cache->rgetSrstar();
    MatrixXd& Yrot = cache->rgetYrot();
	MatrixXd& Lambdac = cache->rgetLambdac();
	MatrixXd& Lambdar = cache->rgetLambdar();
	MatrixXd& Ytilde = cache->rgetYtilde();
    this->rtCR2part1a+=te1(beg);
	beg = clock();
	//covar
	MatrixXd SigmaRot = Lambdac*cache->covarc2->rgetK()*Lambdac.transpose();
    this->rtCR2part1b+=te1(beg);

	beg = clock();
    //start loop trough covariance paramenters
    (*out).resize(covarr2->getNumberParams(),1);
    MatrixXd OmgaGradRot(this->getY().rows(),this->getY().rows());
	for (muint_t i=0; i<covarr2->getNumberParams(); i++) {
		OmgaGradRot=Lambdar*covarr2->Kgrad_param(i)*Lambdar.transpose();
		//1. grad logdet
		mfloat_t grad_det = 0;
	    for (muint_t n=0; n<Yrot.rows(); n++)	{
	        for (muint_t p=0; p<Yrot.cols(); p++)	{
	        	grad_det+=SigmaRot(p,p)*OmgaGradRot(n,n)/(Scstar(p,0)*Srstar(n,0)+1);
	    	}
	    }
	    grad_det*=0.5;
		//2. grad quadratic term
	    // Decomposition in columns and row
	    MatrixXd _Ytilde(Yrot.rows(),Yrot.cols());
	    MatrixXd YtildeR(Yrot.rows(),Yrot.cols());
		for (muint_t p=0; p<Yrot.cols(); p++)
			_Ytilde.block(0,p,Yrot.rows(),1).noalias()=OmgaGradRot*Ytilde.block(0,p,Yrot.rows(),1);
		for (muint_t n=0; n<Yrot.rows(); n++)
			YtildeR.block(n,0,1,Yrot.cols()).noalias()=_Ytilde.block(n,0,1,Yrot.cols())*SigmaRot.transpose();
		mfloat_t grad_quad = -0.5*(Ytilde.array()*YtildeR.array()).sum();
		(*out)(i,0)=grad_det+grad_quad;
    }
    this->rtCR2part2+=te1(beg);
}

void CGPkronSum::aLMLgrad_dataTerm(MatrixXd* out) throw (CGPMixException)
{
	//Computing Kinv
	MatrixXd& Yrot = cache->rgetYrot();
    MatrixXd& Scstar = cache->rgetScstar();
    MatrixXd& Srstar = cache->rgetSrstar();
	MatrixXd& Lambdar = cache->rgetLambdar();
	MatrixXd& Lambdac = cache->rgetLambdac();
	MatrixXd& Ytilde = cache->rgetYtilde();
	MatrixXd _Ytilde(Yrot.rows(),Yrot.cols());
	MatrixXd KinvY(Yrot.rows(),Yrot.cols());
	for (muint_t p=0; p<Yrot.cols(); p++)
		_Ytilde.block(0,p,Yrot.rows(),1).noalias()=Lambdar.transpose()*Ytilde.block(0,p,Yrot.rows(),1);
	for (muint_t n=0; n<Yrot.rows(); n++)
		KinvY.block(n,0,1,Yrot.cols()).noalias()=_Ytilde.block(n,0,1,Yrot.cols())*Lambdac;
	(*out) = this->dataTerm->gradParams(KinvY);
}



} /* namespace limix */