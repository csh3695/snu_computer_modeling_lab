simple FFGenerator{
    parameters:
        volatile double sendIaTime @unit(s);
    gates:
        output out;
}

simple FFFifo{
    gates:
        input in;
        output out;
}

simple FFSink{
    gates:
        input in;
}

simple FFS0Hub{
    parameters:
        volatile double serviceTime0 @unit(s);
    gates:
        input in[];
        output out;
}

simple FFSxHub{
    parameters:
        double p0;
        double p1;
        double p2;
        double p3;

        volatile double serviceTime1 @unit(s);
        volatile double serviceTime2 @unit(s);
        volatile double serviceTime3 @unit(s);
    gates:
        input in;
        output out[];
}

network Four_Queue{
    submodules:
        gen: FFGenerator;
        fifo[4]: FFFifo;
        sink: FFSink;
        H0: FFS0Hub;
        Hx: FFSxHub;

    connections:
        gen.out --> H0.in++;
        H0.out --> fifo[0].in;
        fifo[0].out --> Hx.in;
        Hx.out++ --> sink.in;
        Hx.out++ --> fifo[1].in;
        Hx.out++ --> fifo[2].in;
        Hx.out++ --> fifo[3].in;
        fifo[1].out --> H0.in++;
        fifo[2].out --> H0.in++;
        fifo[3].out --> H0.in++;
}