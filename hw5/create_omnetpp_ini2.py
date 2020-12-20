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


exec_range = range_gen(0.1, 10, 100)

config_list = []

with open('./omnetpp2.ini', 'w') as f:
    f.write(general)
    index = 0
    for Te in exec_range:
        for mode in [0,1,2,3]:
            Tia = 100 * Te
            config_name = f"mode{mode}_l{index}"
            config_list.append(config_name)
            f.write(config.format(config_name, mode, 'true', 0.833333 if mode == 2 else 1, Tia, Te))
            index += 1



with open('./run2.sh', 'w') as f:
    f.write("rm log2.txt\n")
    f.write("mv omnetpp.ini omnetpp_tmp.ini\n")
    f.write("mv omnetpp2.ini omnetpp.ini\n")
    for i in config_list:
        f.write("./hw5 -u Cmdenv -c {} >> ./log2.txt\necho {} done\n".format(i, i))

    f.write("mv omnetpp.ini omnetpp2.ini\n")
    f.write("mv omnetpp_tmp.ini omnetpp.ini\n")
