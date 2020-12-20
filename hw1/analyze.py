import pandas as pd
import matplotlib.pyplot as plt
from glob import glob

print("[+] Analyze.py")

logs = []
for i in range(100):
    logs.append(pd.read_csv(f'./log/log{i}.txt', delimiter = ' '))

log = sum(logs) / len(logs)

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

dlogs = []
flist = sorted(glob(f'./log/detail/detailed_0.*_log.txt'))
for f in flist:
    dlogs.append(pd.read_csv(f, delimiter = ' '))

time = []
load = []
qsize = []

for f, d in zip(flist, dlogs):
    time += list(d.time)
    load += [f[22:30]]*len(d)
    qsize += list(d.nq)

log_1 = pd.read_csv('./log/detail/detailed_1_log.txt', delimiter = ' ')
time += list(log_1.time)
load += [1]*len(log_1)
qsize += list(log_1.nq)

log_1_5 = pd.read_csv('./log/detail/detailed_1_5_log.txt', delimiter = ' ')
time += list(log_1_5.time)
load += [1.5]*len(log_1_5)
qsize += list(log_1_5.nq)

to_file('./log/plot_log/time_load_qsize.txt', time, load, qsize)

print("[+] Detailed Logs Saved")

"""
plt.plot(log.traffic_load, log.njob_avg)
plt.plot(log.traffic_load, l_ref)
plt.show()

plt.plot(log.traffic_load, log.nq_avg)
plt.plot(log.traffic_load, lq_ref)
plt.show()

plt.plot(log.traffic_load, log.waiting_by_job)
plt.plot(log.traffic_load, w_ref)
plt.show()
"""