import re
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from itertools import product

f = open('./log.txt', 'r')


line = None
ddic = {}

config_id_regex = re.compile(r'Preparing for running configuration ')
while line != "":
    line = f.readline()
    fidx = config_id_regex.findall(line)
    if len(fidx) > 0:
        config = line.replace(fidx[0], '').split(',')[0]
        while True:
            line = f.readline()
            if line.startswith('[INFO]'):
                break
        ddic[config] = {}
        ddic[config]['nJob'] = float(f.readline().split(':')[-1])
        ddic[config]['avgR'] = float(f.readline().split(':')[-1])
        ddic[config]['maxR'] = float(f.readline().split(':')[-1])
        ddic[config]['stdR'] = float(f.readline().split(':')[-1])


dat = pd.DataFrame.from_dict(ddic, orient='index')
dat['config'] = dat.index
range_gen = lambda l, h, n: np.arange(l, h + (h-l)/(n-1), (h-l)/(n-1)).astype(np.float32)

big_ia = range_gen(40, 1000, 50)
small_ia = range_gen(0.4, 10, 50)

def get_by(size, mode):
    if mode == 'background':
        mode = 0
    elif mode == 'polling':
        mode = 1
    elif mode == 'deferrable':
        mode = 2
    elif mode == 'sporadic':
        mode = 3
    return dat[dat['config'].apply(lambda x: x.startswith(size) and x.find(f'mode{mode}') >= 0)]


size = ['big', 'small']
mode = ['background', 'polling', 'deferrable', 'sporadic']

for s, m in product(size, mode):
    with open(f'./log/{s}_{m}.txt', 'w') as f:
        if s == 'big':
            ia = big_ia
        elif s == 'small':
            ia = small_ia
        else:
            raise ValueError
        res = list(get_by(s, m)['avgR'])[:50]
        for x, y in zip(ia, res):
            f.write(f"{x} {y}\n")


"""
for i in range(4):
    plt.plot(big_ia, list(get_by('big', i)['maxR'])[:50])

plt.legend(['background', 'polling', 'deferrable', 'sporadic'])

plt.show()
"""