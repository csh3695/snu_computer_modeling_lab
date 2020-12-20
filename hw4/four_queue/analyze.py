import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import re

f = open('./log.txt', 'r')


dat = {}

line = None
cur = []
idx = 0

config_id_regex = re.compile(r'configuration l\d+')
while line != "":
    line = f.readline()
    fidx = config_id_regex.findall(line)
    if fidx != []:
        idx = int(fidx[0].replace("configuration l", ""))
    if line.startswith('[INFO]'):
        cur.append(line)
    else:
        if cur != []:
            print(idx)
            dat[idx] = cur
            cur = []


for k in dat.keys():
    dat[k] = list(map(lambda x: x.strip().replace('[INFO]\t',''), dat[k]))
    logs = iter(dat[k])
    dics = {}
    while True:
        log = next(logs)
        dic = {}
        if log == "[LOG FOR fifo0]":
            dic['nJobProc'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['W'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['maxTime'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['std'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['L'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['Lq'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['U'] = float(next(logs).split(':')[-1].replace(' ',''))
            dics['s0'] = dic
            dic = {}

        if log == "[LOG FOR fifo1]":
            dic['nJobProc'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['W'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['maxTime'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['std'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['L'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['Lq'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['U'] = float(next(logs).split(':')[-1].replace(' ',''))
            dics['s1'] = dic
            dic = {}

        if log == "[LOG FOR fifo2]":
            dic['nJobProc'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['W'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['maxTime'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['std'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['L'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['Lq'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['U'] = float(next(logs).split(':')[-1].replace(' ',''))
            dics['s2'] = dic
            dic = {}

        if log == "[LOG FOR fifo3]":
            dic['nJobProc'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['W'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['maxTime'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['std'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['L'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['Lq'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['U'] = float(next(logs).split(':')[-1].replace(' ',''))
            dics['s3'] = dic
            dic = {}

        if log == "[OVERALL LOG]":
            dic['nJobProc'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['W'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['maxTime'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['std'] = float(next(logs).split(':')[-1].replace(' ',''))
            dic['L'] = None
            dic['Lq'] = None
            dic['U'] = None
            dics['s'] = dic
            dic = {}
            dat[k] = dics
            del dics
            break


dat = pd.DataFrame.from_dict(dat, orient='index')


rng = np.arange(0.05, 1.0 + (1.0-0.05)/99, (1.0-0.05)/99)

dat.index = rng

f = {}

idx_list = [0, 1, 2, 3, ""]
log_list = ['L', 'W', 'U']

for idx in idx_list:
    f["s" + str(idx)] = { logfile: open(f"./log/plot_log/sim_res/{logfile}{idx}.txt", 'w') for logfile in log_list}


for row in dat.iterrows():
    for col in dat.columns:
        for logfile in log_list:
            if col == "s" and logfile == "L":
                L_aggregate = row[1]['s0']['L'] + row[1]['s1']['L'] + row[1]['s2']['L'] + row[1]['s3']['L']
                print(f[col][logfile].write(f"{row[0]} {L_aggregate}\n"))
            else:
                print(f[col][logfile].write(f"{row[0]} {row[1][col][logfile]}\n"))


for col in dat.columns:
    for logfile in log_list:
        f[col][logfile].close()