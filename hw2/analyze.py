import pandas as pd
import matplotlib.pyplot as plt
from glob import glob

print("[+] Analyze.py")

log = pd.read_csv(f'./log/log.txt', delimiter = ' ')
log = pd.read_csv(f'./log/log_total.txt', delimiter = ' ')

def to_file(fname, x, y, z=None):
    assert(len(x) == len(y))
    if z is not None:
        assert(len(y) == len(z))
    with open(fname, 'w') as f:
        if z is not None:
            for xdat, ydat, zdat in zip(x, y, z):
                f.write(f"{xdat} {ydat} {zdat}\n")
        else:
            for xdat, ydat in zip(x, y):
                f.write(f"{xdat} {ydat}\n")
    

l_ref = log.traffic_load / (1-log.traffic_load)
l = log.njob_avg
w_ref = 1 / (log['mu'] - log['lambda'])
w = log.waiting_by_job
lq_ref = log.traffic_load**2 / (1-log.traffic_load)
lq = log.nq_avg

to_file('./log/plot_log/l_ref.txt', log.traffic_load, l_ref)
to_file('./log/plot_log/l.txt', log.traffic_load, l)
to_file('./log/plot_log/w_ref.txt', log.traffic_load, w_ref)
to_file('./log/plot_log/w.txt', log.traffic_load, w)
to_file('./log/plot_log/lq_ref.txt', log.traffic_load, lq_ref)
to_file('./log/plot_log/lq.txt', log.traffic_load, lq)

print("[+] Measurements Saved")