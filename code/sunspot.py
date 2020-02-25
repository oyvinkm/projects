import csv
import numpy as np
import matplotlib.pyplot as plt


def readdataset(filename):
    """readdataset - reads the sunspot data set from the file with filename.
       Returns tuple of (X, t)."""
    X_str = []
    t_str = []
    with open(filename, newline='') as datafile:
        data_reader = csv.reader(datafile, delimiter=' ')
        for row in data_reader:
            X_str.append(row[0:5])
            t_str.append(row[5])

    X = np.array(X_str, dtype=np.float)
    t = np.array(t_str, dtype=np.float)
    return X, t

def factorial_scalar(n):
    """This auxillary function allows for computing factorials of non-integers"""
    return np.prod(np.arange(1, np.math.floor(n)+1, dtype=np.uint64))

def factorial(n_vec):
    """This auxillary function allows for computing factorials of vectors non-integers"""
    f = np.vectorize(factorial_scalar)
    return f(n_vec)

def log_factorial_scalar(n):
    """This auxillary function allows for computing the logarithm of factorials of non-integers
       using Stirling's approximation."""
    n_floor = np.math.floor(n)
    if n_floor == 0:
        return 0.0
    else:
        return n_floor * np.log(n_floor) - n_floor  + 0.5 * np.log(2 * np.pi * n_floor)

def log_factorial(n_vec):
    """This auxillary function allows for computing logarithm of factorials of vectors of non-integers"""
    f = np.vectorize(log_factorial_scalar)
    return f(n_vec)


# Read the training set
X_train, t_train = readdataset('data/sunspotsTrainStatML.dt')

N_train, D = X_train.shape
print("Training set has X dimension D = " + str(D) + " and N = " + str(N_train) + ' samples.')


# Read the test set
X_test, t_test = readdataset('data/sunspotsTestStatML.dt')

N_test, D_test = X_test.shape
print("Test set has X dimension D = " + str(D_test) + " and N = " + str(N_test) + ' samples.')


# Visualize the data set
plt.figure()
plt.plot(X_train[:,4], t_train, 'o')
plt.title("Train")
plt.xlabel('X[4]')
plt.ylabel('t')

plt.figure()
plt.plot(X_test[:,4], t_test, 'o')
plt.title("Test")
plt.xlabel('X[4]')
plt.ylabel('t')

plt.figure()
plt.hist(t_train)
plt.title('Train')
plt.xlabel('t values')
plt.ylabel('hist(t)')

plt.figure()
plt.hist(t_test)
plt.title('Test')
plt.xlabel('t values')
plt.ylabel('hist(t)')

count = 0

def genProp(w, sig, X):
    """This functions generates a proposal using multivariate
        normal distribution based on the last sample and the variance"""
    prop = np.random.multivariate_normal(w, sig)
    return prop

def postRatio(w, t, x, mu, sig):
    """This function calulates the ratio of the posterior for a given sample"""
    prob = 0
    for i in range(len(x)):
        f = np.dot(w.T,x[i])
        prob += t[i] * np.log(f) - f\
        - log_factorial_scalar(t[i])   
    prob -= 1/(2*sig[0,0])*np.dot((w-mu).T,(w-mu) \
            - np.log(np.sqrt((2*np.pi*sig[0,0])**(D+1))))
    return prob    

def acceptance(new_w, w, t, x, mu, sig, i):
    """This functions evaluate if the new proposal should be accepted by 
        comparing the proposal with the last sample"""
    posteriorNew = postRatio(new_w, t, x, mu, sig)
    posteriorPrev = postRatio(w, t, x ,mu, sig)
    ratio = posteriorNew/posteriorPrev
    u = np.random.uniform(0,1)
    if (0 <= np.log(ratio)):
        return True
    elif (np.log(u) > np.log(ratio)):
        return False
    else:
        return True
    
def metropolisHastings(w,t, X, mu, sig, iterations):
    """This function creates a proposal, checks if it is accepted and then
        return a list of accepted samples, and a list of rejected samples"""
    w_prev = w
    acceptedSamples = []
    rejectedSamples = []
    firstTime = True
    for i in range(iterations):
        proposal = genProp(w_prev,sig, X)
        if(np.all(np.dot(proposal, X.T) > 0)): 
            if (firstTime == True):   
                print("Burn in: ", i)
                firstTime = False
            if (acceptance(proposal, w_prev, t, X, mu, sig, i)):
                w_prev = proposal
                acceptedSamples.append(w_prev)
            else:
                rejectedSamples.append(w_prev)
    acceptedSamples = np.array(acceptedSamples)
    rejectedSamples = np.array(rejectedSamples)
    return acceptedSamples, rejectedSamples  
       
    
def predictions(X_test, samples):
        target = []
        for i in range(len(X_test)):
            X_row = X_test[i]
            targetValue = 0
            for j in range(len(samples)):
                targetValue += (np.dot(samples[j].T, X_row))
            target.append(targetValue/len(samples))
        return target
    

def dimensionalScale(w_int, X_train, X_test, t_train, D, iterations):
    t = t_train
    X = X_train
    X_t = X_test
    mu = np.ones(D+1)
    sig = 0.25 * np.identity(D+1)
    w_init = np.ones(D+1)*w_int
    print("w_init: ", w_init)
    if (D == 1):
        X = X[:,[0,5]]
        X_t = X_t[:,[0,5]]
    if (D == 2):
        X = X[:, [0, 3, 4]]
        X_t = X_t[:, [0, 3, 4]]
    if (D == 5):
        X = X
        X_t = X_t
    accepted, rejected = \
    metropolisHastings(w_init, t, X, mu, sig, iterations)
    target = predictions(X_t, accepted)
    target = np.array(target)
    return accepted, rejected, target

def RMSE(t, tp):
    return np.sqrt(np.mean((t-tp)**2))


X_train = np.array(X_train).reshape((len(X_train), -1))
ones = np.ones((X_train.shape[0], 1))
X_train = np.concatenate((ones, X_train), axis=1)  

X_test = np.array(X_test).reshape((len(X_test), -1))
ones = np.ones((X_test.shape[0], 1))
X_test = np.concatenate((ones, X_test), axis=1) 

def RMSEComputing(w):
    for i in range(5):
        w = w+1 * i                
        accepted_1, rejected_1, target_1 = \
            dimensionalScale(w, X_train, X_test, t_train, 1, (i+1)*100)
        accepted_2, rejected_2, target_2 = \
            dimensionalScale(w, X_train, X_test, t_train, 2, (i+1)*100)   
        accepted_5, rejected_5, target_5 = \
            dimensionalScale(w, X_train, X_test, t_train, 5, (i+1)*100)
        print(w, '1dim ->', RMSE(target_1, t_test), 'iterations: ', (i+1)*100)
        print(w, '2dim ->', RMSE(target_2,t_test), 'iterations: ', (i+1)*100)
        print(w, '5dim ->', RMSE(target_5, t_test), 'iterations: ', (i+1)*100)
        
accepted_1, rejected_1, target_1 = \
    dimensionalScale(5, X_train, X_test, t_train, 1, 200)

accepted_2, rejected_2, target_2 = \
    dimensionalScale(5, X_train, X_test, t_train, 2, 200)
    
accepted_5, rejected_5, target_5 = \
    dimensionalScale(5, X_train, X_test, t_train, 5, 200)
    
newPred = predictions(X_test[:,[0,5]], accepted_1)


RMSE_1 = RMSE(target_1, t_test)
RMSE_2 = RMSE(target_2, t_test)
RMSE_5 = RMSE(target_5, t_test)



plt.figure()
plt.title('Accepted 1-dimension')
plt.plot(accepted_1)

plt.figure()
plt.title('Target Values 1-Dimension')
plt.hist(target_1)

plt.figure()
plt.title('predictions')    
plt.scatter(t_test, target_1)
print(target_1.shape)
print(t_test.shape)




print("RMSE 1-Dimensional: ", RMSE_1)
print("RMSE 2-Dimensional: ", RMSE_2)
print("RMSE 5-Dimensional: ", RMSE_5)

RMSEComputing(1)






# =============================================================================
# plt.figure()
# plt.title('ALERT')
# plt.text(0.5,0.5,"Exercise 3", size=50, rotation=30.,\
#          ha="center",va="center", \
#          bbox=dict(boxstyle="round",\
#                    ec=(1.,0.5,0.5),\
#          fc=(1.,0.8,0.8),)
#         )
# 
# 
#     
# =============================================================================
        

# Show all figures
plt.show()
