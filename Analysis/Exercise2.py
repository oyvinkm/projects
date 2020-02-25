
import numpy as np
import matplotlib.pyplot as plt
import linreg as lg
from scipy import stats


men_olympics = np.loadtxt("men-olympics-100.txt", usecols=(0,1), unpack=True)
data = men_olympics.transpose()
X, t = men_olympics[0,:], men_olympics[1,:]
t = t.reshape((len(t)),1)   
X = X-X[0]
X = X/4
X = np.array(X).reshape((len(X), -1))  
ones = np.ones((X.shape[0], 1))
X = np.concatenate((ones, X), axis=1)
plt.plot(X[:,1],t,'ro')
sig = np.array([[100, 0], [0,5]])
mu = np.array([[0],[0]])
def posterior(X, t, E, y, V=''):
    linreg = lg.BayLinearRegression()
    z = linreg.komat(X,E,V)
    y = linreg.meanmu(X,t,E,y)
    print("SIGMA: %" ,z)
    print("MU: %" ,y)
    return z, y
x,y=posterior(X,t,sig,mu)



