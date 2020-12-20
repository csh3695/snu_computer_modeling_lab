import re
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

f = open('./log2.txt', 'r')


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

def get_by(mode):
    if mode == 'background':
        mode = 0
    elif mode == 'polling':
        mode = 1
    elif mode == 'deferrable':
        mode = 2
    elif mode == 'sporadic':
        mode = 3
    return dat[dat['config'].apply(lambda x: x.find(f'mode{mode}') >= 0)]

exec_range = range_gen(0.1, 10, 100)


mode = ['background', 'polling', 'deferrable', 'sporadic']

for m in mode:
    with open(f'./log2/{m}.txt', 'w') as f:
        res = list(get_by(m)['avgR'])
        print(len(res))
        print(len(exec_range))
        for x, y in zip(exec_range, res):
            f.write(f"{x} {y}\n")


"""
for i in range(4):
    plt.plot(big_ia, list(get_by('big', i)['maxR'])[:50])

plt.legend(['background', 'polling', 'deferrable', 'sporadic'])

plt.show()
"""