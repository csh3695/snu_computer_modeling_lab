import numpy as np
from itertools import product

general = """[General]
network = RM_Scheduler
sim-time-limit = 100000s
#cmdenv-express-mode=true
record-eventlog = false

RM_Scheduler.pGen[0].period = 4s
RM_Scheduler.pGen[0].execTime = 1s

RM_Scheduler.pGen[1].period = 10s
RM_Scheduler.pGen[1].execTime = 2s

RM_Scheduler.pGen[0].priority = 0
RM_Scheduler.pGen[1].priority = 1
RM_Scheduler.aGen.priority = 2

"""


config = """[Config {}]
RM_Scheduler.scheduler.mode = {} # 0 for background, 1 for polling, 2 for deferrable, 3 for sporadic
RM_Scheduler.scheduler.background = {}
RM_Scheduler.scheduler.budget = {}s
RM_Scheduler.aGen.sendIaTime = exponential({}s)
RM_Scheduler.aGen.execTime = exponential({}s)

"""



range_gen = lambda l, h, n: np.arange(l, h + (h-l)/(n-1), (h-l)/(n-1)).astype(np.float32)

big_ia = range_gen(40, 1000, 50)
small_ia = range_gen(0.4, 10, 50)

big_ia_prod = product([(0,1),(1,1),(2,0.833333),(3,1)],['true'],[10],enumerate(big_ia))
small_ia_prod = product([(0,1),(1,1),(2,0.833333),(3,1)],['true'],[0.1],enumerate(small_ia))


config_list = []

with open('./omnetpp.ini', 'w') as f:
    f.write(general)
    index = 0
    for _, linv in enumerate(big_ia_prod):
        (mode, budget), backgroud, execTime, (idx, sendIaTime) = linv
        print(sendIaTime)
        config_name = f"big_mode{mode}_ia{idx}_exec{'10'}"
        config_list.append(config_name)
        f.write(config.format(config_name, mode, backgroud, budget, sendIaTime, execTime))
        index += 1

    for _, linv in enumerate(small_ia_prod):
        (mode, budget), backgroud, execTime, (idx, sendIaTime) = linv
        print(sendIaTime)
        config_name = f"small_mode{mode}_ia{idx}_exec{'1e-1'}"
        config_list.append(config_name)
        f.write(config.format(config_name, mode, backgroud, budget, sendIaTime, execTime))
        index += 1


with open('./run.sh', 'w') as f:
    f.write("rm log.txt\n")
    for i in config_list:
        f.write("./hw5 -u Cmdenv -c {} >> ./log.txt\necho {} done\n".format(i, i))