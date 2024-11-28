#!/usr/bin/python3

import matplotlib.pyplot as plt
import numpy as np

x = [1, 2, 3, 4, 5, 6, 8]
y = [1.82, 1.23, 1.04, 0.92, 0.97, 1.04, 1.03]

S = [y[0] / y[i] for i, _ in enumerate(y)]
E = [S[i] / x[i] for i, _ in enumerate(S)]

plt.figure(figsize=[10, 4])

plt.grid()
plt.xlabel('Proc number')
plt.ylabel('time, sec')
plt.ylim((0, 2))

plt.scatter(x, y)

# for i, _ in enumerate(x):
#     plt.annotate(str(x[i]), (x[i], y[i]))

plt.savefig('graph_T.png')

# ---

plt.figure(figsize=[10, 4])

plt.grid()
plt.xlabel('Proc number')
plt.ylabel('Ускорение')
# plt.xscale("log")

plt.scatter(x, S)

# for i, _ in enumerate(x):
#     plt.annotate(str(x[i]), (x[i], S[i]))

plt.savefig('graph_S.png')

# ---

plt.figure(figsize=[10, 4])

plt.grid()
plt.xlabel('Proc number')
plt.ylabel('Эффективность')
# plt.xscale("log")

plt.scatter(x, E)

# for i, _ in enumerate(x):
#     plt.annotate(str(x[i]), (x[i], E[i]))

plt.savefig('graph_E.png')
