import numpy as np
import pandas as pd
import re

import matplotlib.pyplot as plt

file = "./log/cmdlog.txt"
f = open(file)
 
# teststr = "At time +11.0787s packet sink received 536 bytes from 10.1.0.1 port 49153 total Rx 2144 bytes"
rtime = re.compile(r"[\d]*[.]*[\d]*s")
rbyte = re.compile(r"[\d]+ bytes")
rfrom = re.compile(r"[\d]*[.][\d]*[.][\d]*[.][\d]*")

out_dict = {}

while True:
    line = f.readline()
    if line == "":
        break
    
    if line.startswith('At time'):
        time = float(rtime.findall(line)[0].replace('s', ''))
        byte = int(rbyte.findall(line)[0].replace(' bytes',''))
        src = rfrom.findall(line)[0]
        if src not in out_dict.keys():
            out_dict[src] = []
        out_dict[src].append((time, byte))


sinkaddr = {0:'10.1.0.1', 1:'10.1.3.1', 2:'10.1.5.2'}
out_list = {0: [], 1: [], 2: []}
out_integral_list = {0: [], 1: [], 2: []}
timedelta = np.arange(10.25, 20.5, 0.5)
integral_timedelta = np.arange(10.0, 20.6, 0.1)

for sink_id in range(3):
    df = pd.DataFrame(out_dict[sinkaddr[sink_id]], columns=['time', 'bytes'])
    for i in range(len(timedelta)):
        lt, ut = timedelta[i] - 0.25, timedelta[i] + 0.25
        total_bytes = df[(lt <= df.time) & (df.time < ut)].bytes.sum()
        out_list[sink_id].append((timedelta[i], total_bytes))
    
    for i in range(len(integral_timedelta)):
        lt, ut = integral_timedelta[i] - 0.5, integral_timedelta[i] + 0.5
        total_bytes = df[(lt <= df.time) & (df.time < ut)].bytes.sum()
        out_integral_list[sink_id].append((integral_timedelta[i], total_bytes))


for sink_id in range(3):
    with open(f'./log/log{sink_id}.txt', 'w') as f:
        for t, b in out_list[sink_id]:
            f.write(f"{t} {b}\n")

with open(f'./log/log_integral_toRight.txt', 'w') as f:
    for i in range(len(integral_timedelta)):
        t, b = out_integral_list[0][i]
        b += out_integral_list[1][i][1]
        f.write(f"{t} {b}\n")

with open(f'./log/log_integral_maxToRight.txt', 'w') as f:
    for i in range(len(integral_timedelta)):
        t, _ = out_integral_list[0][i]
        f.write(f"{t} {262144}\n")


with open(f'./log/log_integral_toLeft.txt', 'w') as f:
    for i in range(len(integral_timedelta)):
        t, b = out_integral_list[2][i]
        f.write(f"{t} {b}\n")

with open(f'./log/log_integral_maxToLeft.txt', 'w') as f:
    for i in range(len(integral_timedelta)):
        t, _ = out_integral_list[2][i]
        f.write(f"{t} {131072}\n")

"""
for i in range(3):
    x, y = list(zip(*out_list[i]))
    plt.plot(x, y)
"""