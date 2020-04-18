#!/usr/bin/env python3

import numpy as np

from scipy.stats import multivariate_normal
from matplotlib.colors import Normalize

import matplotlib.pyplot as plt


x, y = np.mgrid[-5:6:.1, -5:6:.1]

pos = np.empty(x.shape + (2,))

pos[:, :, 0] = x
pos[:, :, 1] = y

rv = multivariate_normal([0.0, 0.0], [[1.0, 0.0], [0.0, 1.0]])

#z = 10 * rv.pdf(pos)
z = np.sin(y * x)


vx = []
vy = []
vz = []


for i in range(pos.shape[0] - 1):
    for j in range(pos.shape[1]):
        if i % 2 == 0:
            vx.append(x[i, j])
            vy.append(y[i, j])
            vz.append(z[i, j])
            vx.append(x[i+1, j])
            vy.append(y[i+1, j])
            vz.append(z[i+1, j])
        else:
            vx.append(x[i+1, (pos.shape[1]-1) - j])
            vy.append(y[i+1, (pos.shape[1]-1) - j])
            vz.append(z[i+1, (pos.shape[1]-1) - j])
            if j == pos.shape[1] - 1:
                continue
            vx.append(x[i, (pos.shape[1]-1) - (j+1)])
            vy.append(y[i, (pos.shape[1]-1) - (j+1)])
            vz.append(z[i, (pos.shape[1]-1) - (j+1)])

d = np.vstack([vx, vy, vz]).T


cmap = plt.get_cmap('viridis')

normalized_z = Normalize()(vz)
cols = cmap(normalized_z)

np.savetxt('col.csv', cols, delimiter=',')
np.savetxt('strip.csv', d, delimiter=',')
