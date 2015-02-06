# Copyright(c) 2014, The LIMIX developers (Christoph Lippert, Paolo Francesco Casale, Oliver Stegle)
#
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.

"""
PANAMA module in limix
"""
import limix.modules.qtl as qtl
import limix.stats.fdr as fdr
from limix.stats.pca import *
import limix
import scipy as sp 
import scipy.linalg as la
import pdb
import scipy.linalg as linalg
import time

class varDecompSet:
    """variance decomposition set class"""
    def __init__(self,Y=None,Ks=None,standardize=True,noise_off=False):
        """
        Args:
            X: alternatively SNP data
            Y: alternativel expression data 
            Ks: list of covariance matrices 
            standardize: if True, phenotypes are standardized
        """
        assert Y is not None, 'Specify Y'
        assert Ks is not None, 'Specify Ks'
        if type(Ks)!=list: Ks = [Ks]
        self.Y  = Y
        self.Ks = Ks
        if standardize:
            self.Y -= self.Y.mean(axis=0)
            self.Y /= self.Y.std(axis=0)
        self.N = self.Y.shape[0]
        self.P = self.Y.shape[1]
        self.noise_off = noise_off
        self.n_terms = len(self.Ks)+1
        if noise_off: self.n_terms-=1

    def train(self,jitter=1e-4):
        """train vds module"""
        covar  = limix.CSumCF()
        covar_params = []
        for K in self.Ks:
            covar.addCovariance(limix.CFixedCF(K+jitter*sp.eye(self.N)))
            covar_params.append(1.0/sp.sqrt(self.n_terms))
        if not self.noise_off:
            covar.addCovariance(limix.CFixedCF(sp.eye(self.N)))
            covar_params.append(1.0/sp.sqrt(self.n_terms))

        hyperparams = limix.CGPHyperParams()
        hyperparams['covar'] = sp.array(covar_params)
        constrainU = limix.CGPHyperParams()
        constrainL = limix.CGPHyperParams()
        constrainU['covar'] = +5*sp.ones_like(covar_params);
        constrainL['covar'] = -5*sp.ones_like(covar_params);

        self.gp=limix.CGPbase(covar,limix.CLikNormalNULL())
        self.gp.setY(self.Y)
        lml0 = self.gp.LML(hyperparams)
        dlml0 = self.gp.LMLgrad(hyperparams)        
        gpopt = limix.CGPopt(self.gp)
        gpopt.setOptBoundLower(constrainL);
        gpopt.setOptBoundUpper(constrainU);

        t1 = time.time()
        conv = gpopt.opt()
        t2 = time.time()

        RV = {'Converged': True, 'time': t2-t1}

    def getVarianceComps(self):
        """
        Returns:
            vector of variance components of the PANAMA, Kpop and noise contributions
        """
        RV = self.gp.getParams()['covar']**2
        return RV 

