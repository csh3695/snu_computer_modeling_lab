import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from glob import glob

print("[+] Analyze.py")

log = pd.read_csv(f'./log/log.txt', delimiter = ' ')

def to_file(fname, x, y, xrange = None):
    assert(len(x) == len(y))
    with open(fname, 'w') as f:
        if xrange is not None:
            for xdat, ydat in zip(x[xrange], y[xrange]):
                f.write(f"{xdat} {ydat}\n")
        else:
            for xdat, ydat in zip(x, y):
                f.write(f"{xdat} {ydat}\n")
    

lmd = log['lambda']

ref_res = {}
ref_res['L0'] = 3*lmd / (1-3*lmd)
ref_res['L1'] = lmd / (1-lmd)
ref_res['L2'] = 2*lmd / (1-2*lmd)
ref_res['L3'] = 4*lmd / (1-4*lmd)

ref_res['W0'] = 0.75 / (1-3*lmd)
ref_res['W1'] = 1 / (1-lmd)
ref_res['W2'] = 2 / (1-2*lmd)
ref_res['W3'] = 4 / (1-4*lmd)

ref_res['U0'] = 3*lmd
ref_res['U1'] = lmd
ref_res['U2'] = 2*lmd
ref_res['U3'] = 4*lmd

ref_res['L'] = ref_res['L0'] + ref_res['L1'] + ref_res['L2'] + ref_res['L3'] 
ref_res['W'] = ref_res['L'] / lmd

to_file('./log/plot_log/sim_res/L.txt', lmd, log.L)
to_file('./log/plot_log/theo_res/L.txt', lmd, ref_res['L'], (1-4*lmd) > 0)

to_file('./log/plot_log/sim_res/W.txt', lmd, log.W)
to_file('./log/plot_log/theo_res/W.txt', lmd, ref_res['W'], (1-4*lmd) > 0)

to_file('./log/plot_log/sim_res/L0.txt', lmd, log.L0)
to_file('./log/plot_log/theo_res/L0.txt', lmd, ref_res['L0'], (1-3*lmd) > 0)

to_file('./log/plot_log/sim_res/W0.txt', lmd, log.W0)
to_file('./log/plot_log/theo_res/W0.txt', lmd, ref_res['W0'], (1-3*lmd) > 0)

to_file('./log/plot_log/sim_res/L1.txt', lmd, log.L1)
to_file('./log/plot_log/theo_res/L1.txt', lmd, ref_res['L1'], (1-lmd) > 0)

to_file('./log/plot_log/sim_res/W1.txt', lmd, log.W1)
to_file('./log/plot_log/theo_res/W1.txt', lmd, ref_res['W1'], (1-lmd) > 0)

to_file('./log/plot_log/sim_res/L2.txt', lmd, log.L2)
to_file('./log/plot_log/theo_res/L2.txt', lmd, ref_res['L2'], (1-2*lmd) > 0)

to_file('./log/plot_log/sim_res/W2.txt', lmd, log.W2)
to_file('./log/plot_log/theo_res/W2.txt', lmd, ref_res['W2'], (1-2*lmd) > 0)

to_file('./log/plot_log/sim_res/L3.txt', lmd, log.L3)
to_file('./log/plot_log/theo_res/L3.txt', lmd, ref_res['L3'], (1-4*lmd) > 0)

to_file('./log/plot_log/sim_res/W3.txt', lmd, log.W3)
to_file('./log/plot_log/theo_res/W3.txt', lmd, ref_res['W3'], (1-4*lmd) > 0)

to_file('./log/plot_log/sim_res/U0.txt', lmd, log.U0)
to_file('./log/plot_log/theo_res/U0.txt', lmd, ref_res['U0'], ref_res['U0'] <= 1)
to_file('./log/plot_log/sim_res/U1.txt', lmd, log.U1)
to_file('./log/plot_log/theo_res/U1.txt', lmd, ref_res['U1'], ref_res['U1'] <= 1)
to_file('./log/plot_log/sim_res/U2.txt', lmd, log.U2)
to_file('./log/plot_log/theo_res/U2.txt', lmd, ref_res['U2'], ref_res['U2'] <= 1)
to_file('./log/plot_log/sim_res/U3.txt', lmd, log.U3)
to_file('./log/plot_log/theo_res/U3.txt', lmd, ref_res['U3'], ref_res['U3'] <= 1)



print("[+] Measurements Saved")