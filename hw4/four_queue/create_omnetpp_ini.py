import numpy as np

general = """[General]
network = Four_Queue
sim-time-limit = 100000s
cmdenv-express-mode=true
record-eventlog = false

Four_Queue.Hx.p0 = 0.25
Four_Queue.Hx.p1 = 0.25
Four_Queue.Hx.p2 = 0.25
Four_Queue.Hx.p3 = 0.25

Four_Queue.H0.serviceTime0 = exponential(0.75s)
Four_Queue.Hx.serviceTime1 = exponential(1s)
Four_Queue.Hx.serviceTime2 = exponential(2s)
Four_Queue.Hx.serviceTime3 = exponential(4s)

"""


config = """[Config l{}]
Four_Queue.gen.sendIaTime = exponential({}s)

"""


rng = 1/np.arange(0.05, 1.0 + (1.0-0.05)/99, (1.0-0.05)/99)

with open('./omnetpp.ini', 'w') as f:
    f.write(general)
    for i, linv in enumerate(rng):
        f.write(config.format(i, linv))


with open('./omnetpp.ini', 'w') as f:
    f.write(general)
    for i, linv in enumerate(rng):
        f.write(config.format(i, linv))

with open('./run.sh', 'w') as f:
    for i in range(len(rng)):
        f.write("./four_queue -u Cmdenv -c l{} >> ./log.txt\necho {} done\n".format(i, i))