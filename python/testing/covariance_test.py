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

import sys
sys.path.append(r'./..')
sys.path.append(r'./../../../pygp')
#sys.path.append(r'./../../build/debug.win32/interfaces/python')


import limix
print limix.__file__
import pygp.covar.linear as lin
import pygp.covar.se as se
import pygp.covar.gradcheck as GC
import pygp.covar.combinators as comb
import scipy as SP
import pdb

n_dimensions=3
X = SP.randn(3,n_dimensions)

params = SP.zeros([0])

if 0:
    c1 = limix.CCovLinearISO()
    c2 = lin.LinearCFISO(n_dimensions=n_dimensions)

    c1.setX(X)

    K1 = c1.K()
    K2 = c2.K(params,X,X)

    dK1 = c1.Kgrad_param(0)
    dK2 = c2.Kgrad_theta(params,X,0)

    dKx1= c1.Kgrad_X(0)
    dKx2= c2.Kgrad_x(params,X,X,0)

    dKx1diag = c1.Kdiag_grad_X(0)
    dKx2diag = c2.Kgrad_xdiag(params,X,0)


if 0:
    c1 = limix.CCovSqexpARD()
    c2 = se.SqexpCFARD(n_dimensions=n_dimensions)

    params = SP.random.randn(n_dimensions+1)
    #params[:] = 0
    c1.setX(X)
    c1.setParams(params)

    K1 = c1.K()
    K2 = c2.K(params,X,X)

    print SP.absolute(K1-K2).max()

    dK1 = c1.Kgrad_param(0)
    dK2 = c2.Kgrad_theta(params,X,0)
    dKx1= c1.Kgrad_X(0)
    dKx2= c2.Kgrad_x(params,X,X,0)
    print SP.absolute(dK1-dK2).max()


if 1:
    params = SP.random.randn(n_dimensions+2)

    #pdb.set_trace()
    c1 = limix.CCovLinearISO(n_dimensions)
    c2 = limix.CCovSqexpARD(n_dimensions)

    c  = limix.CSumCF()
    c.addCovariance(c1)
    c.addCovariance(c2)
    X2 = SP.concatenate((X,X),axis=1)
    c.setX(X2)
    c.setParams(params)
    
    c1_ = lin.LinearCFISO(n_dimensions=n_dimensions)
    c2_ = se.SqexpCFARD(n_dimensions=n_dimensions)
    c_ = comb.SumCF([c1_,c2_])

    
    K_ = c_.K(params,X2,X2)
    K  = c.K()
    
    

        


