#!/usr/bin/python3

import matplotlib.pyplot as plt
import numpy as np

x = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024] 
y = [11.2, 6.4,  4.5, 3.3, 3.6, 3.3, 3.3, 3.5, 3.4, 3.3, 3.4]

S = [y[0] / y[i] for i, _ in enumerate(y)]
E = [S[i]/x[i] for i, _ in enumerate(S)]

plt.figure(figsize=[10, 4])

plt.grid()
plt.xlabel('OMP_NUM_THREADS')
plt.ylabel('time, sec')
plt.xscale("log")
plt.ylim((0, 12))

plt.scatter(x, y)

for i, _ in enumerate(x):
    plt.annotate(str(x[i]), (x[i], y[i]))

plt.savefig('graph_T.png')

#---

plt.figure(figsize=[10, 4])

plt.grid()
plt.xlabel('OMP_NUM_THREADS')
plt.ylabel('Ускорение')
plt.xscale("log")

plt.scatter(x, S)

for i, _ in enumerate(x):
    plt.annotate(str(x[i]), (x[i], S[i]))

plt.savefig('graph_S.png')

#---

plt.figure(figsize=[10, 4])

plt.grid()
plt.xlabel('OMP_NUM_THREADS')
plt.ylabel('Эффективность')
plt.xscale("log")

plt.scatter(x, E)

for i, _ in enumerate(x):
    plt.annotate(str(x[i]), (x[i], E[i]))

plt.savefig('graph_E.png')