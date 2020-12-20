simple aperiodicGen{
    parameters:
        volatile double sendIaTime @unit(s);
        volatile double execTime @unit(s);
        int priority;
    gates:
        output out;
}

simple periodicTask{
    parameters:
        double period @unit(s);
        double execTime @unit(s);
        int priority;
    gates:
        output out;
}

simple scheduler{
    parameters:
        int mode;
        bool background;
        volatile double budget @unit(s);
    gates:
        input in[];
        output out;
}

simple sink{
    gates:
        input in;
}


network RM_Scheduler{
    submodules:
        aGen: aperiodicGen;
        pGen[2]: periodicTask;
        scheduler: scheduler;
        sink: sink;

    connections:
        aGen.out --> scheduler.in++;
        pGen[0].out --> scheduler.in++;
        pGen[1].out --> scheduler.in++;
        scheduler.out --> sink.in;
}