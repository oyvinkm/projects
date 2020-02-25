import numpy

# NOTE: This template makes use of Python classes. If 
# you are not yet familiar with this concept, you can 
# find a short introduction here: 
# http://introtopython.org/classes.html

class BayLinearRegression():
    """
    Linear regression implementation.
    """

    def __init__(self):
        pass
    def variance(self, x):
        if isinstance(x, (float, int)):
            self.var = x
        else:
            self.var = numpy.var(x)
        return self.var
            
    def komat(self, X, E, V):
        if V == '': V = X
        self.variance(V)
        a = 1/(self.var)*numpy.dot(X.T, X)
        self.sig = numpy.linalg.inv(a + numpy.linalg.inv(E))
        return self.sig
    def meanmu (self, X, t, E, my):
        invE = numpy.linalg.inv(E)
        a = 1/self.var*numpy.dot(X.T, t)
        c = a + numpy.dot(invE, my)
        self.mu = numpy.dot(self.sig, c)
        return self.mu
   
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
    
