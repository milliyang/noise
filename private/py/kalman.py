import numpy as np
import matplotlib.pyplot as plt

class KalmanFilter(object):
    def __init__(self, F = None, B = None, H = None, Q = None, R = None, P = None, x0 = None):

        if(F is None or H is None):
            raise ValueError("Set proper system dynamics.")

        self.n = F.shape[1]
        self.m = H.shape[1]

        self.F = F
        self.H = H
        self.B = 0 if B is None else B
        self.Q = np.eye(self.n) if Q is None else Q
        self.R = np.eye(self.n) if R is None else R
        self.P = np.eye(self.n) if P is None else P
        self.x = np.zeros((self.n, 1)) if x0 is None else x0

    def predict(self, u = 0):
        self.x = np.dot(self.F, self.x) + np.dot(self.B, u)
        self.P = np.dot(np.dot(self.F, self.P), self.F.T) + self.Q
        return self.x

    def update(self, z):
        y = z - np.dot(self.H, self.x)
        S = self.R + np.dot(self.H, np.dot(self.P, self.H.T))
        K = np.dot(np.dot(self.P, self.H.T), np.linalg.inv(S))
        self.x = self.x + np.dot(K, y)
        I = np.eye(self.n)
        self.P = np.dot(np.dot(I - np.dot(K, self.H), self.P), 
        	(I - np.dot(K, self.H)).T) + np.dot(np.dot(K, self.R), K.T)

def example():
	dt = 1.0/60
	F = np.array([	[1, dt, 0],
               		[0, 1, dt],
                    [0, 0,  1]])
	H = np.array([1, 0, 0]).reshape(1, 3)
	Q = np.array([	[0.05, 0.05, 0.0],
					[0.05, 0.05, 0.0],
                    [0.00, 0.00, 0.0]])
	R = np.array([0.5]).reshape(1, 1)

	NUM = 100
	x = np.linspace(-10, 10, NUM)
	measurements = - (x**2 + 2*x - 2)  + np.random.normal(0, 2, NUM)
	#measurements = (x*x*x*x + x**2 + 2*x - 2)  + np.random.normal(0, 2, NUM)

	kf = KalmanFilter(F = F, H = H, Q = Q, R = R)
	predictions = []

	for z in measurements:
		predictions.append(np.dot(H,  kf.predict())[0])
		kf.update(z)

	#Leo
	import pandas as pd
	xxx = np.array(measurements)
	d = pd.Series(xxx)
	ma5 = d.rolling(5).mean()
	ma10 = d.rolling(10).mean()

	plt.plot(range(len(ma5)), ma5, label = 'ma5x')
	# plt.plot(range(len(ma8)), ma8, label = 'ma8x')
	plt.plot(range(len(ma10)), ma10, label = 'ma10x')
	plt.plot(range(len(measurements)), measurements, label = 'Measurements')
	plt.plot(range(len(predictions)), np.array(predictions), label = 'Kalman Filter Prediction')
	plt.legend()
	plt.show()

if __name__ == '__main__':
    example()
    
# https://github.com/zziz/kalman-filter
