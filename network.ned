simple Sink
{
    parameters:
        @display("i=device/pc");
    gates:
        input in;
}

simple Sdn
{
    parameters:
        @display("i=device/smallrouter");
    gates:
        input in;
        output out;
}

simple Generator
{
    parameters:
        @display("i=device/pc2");
        int packetLength @unit(byte) = default(64B);
        int sendRate @unit(gigabit/sec) = default(1Gbps);
    gates:
        output out;
}

simple Analyzer
{
    parameters:
        @display("i=device/server");
    gates:
        input in;
        output out;
}

//Because inheritance in NED is a little bit odd.
simple Acx_psl
{
    parameters:
        bool singleQueue;
        @display("i=device/mainframe");
    gates:
        input psl;
        inout pwr;
        inout pw;
}

simple Acx_pw
{
    parameters:
        bool singleQueue;
        @display("i=device/mainframe");
    gates:
        output pw;
        inout psl;
        inout pcss;
        inout il;
}

simple Acx_il
{
    parameters:
        bool singleQueue;
        @display("i=device/mainframe");
    gates:
        output il;
        inout pw;
        inout pg;
}

simple Acx_pg
{
    parameters:
        bool singleQueue;
        @display("i=device/mainframe");
    gates:
        inout il;
        inout pcss;
}

simple Acx_pcss
{
    parameters:
        bool singleQueue;
        @display("i=device/mainframe");
    gates:
        output pcss;
        inout pg;
        inout pw;
        inout pwr;
}

simple Acx_pwr
{
    parameters:
        bool singleQueue;
        @display("i=device/mainframe");
    gates:
        output pwr;
        inout pcss;
        inout psl;
}

network net
{
    @display("bgb=880,559");
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate = 1Gbps;
            delay = 0ms;
        }
    submodules:
        generator: Generator {
            @display("p=650,450");
        }
        sdn_psl: Sdn {
            @display("p=550,450");
        }
        sdn_pw: Sdn {
            @display("p=550,200");
        }
        sdn_il: Sdn {
            @display("p=550,50");
        }
        sdn_pcss: Sdn {
            @display("p=150,200");
        }
        sdn_pwr: Sdn {
            @display("p=150,350");
        }
        sink_pw: Sink {
            @display("p=650,200");
        }
        sink_il: Sink {
            @display("p=650,50");
        }
        sink_pcss: Sink {
            @display("p=50,200");
        }
        sink_pwr: Sink {
            @display("p=50,350");
        }
        analyzer: Analyzer {
            @display("p=450,450");
        }
        acx_psl: Acx_psl {
            @display("p=450,350");
        }
        acx_pw: Acx_pw {
            @display("p=450,200");
        }
        acx_il: Acx_il {
            @display("p=450,50");
        }
        acx_pg: Acx_pg {
            @display("p=250,50");
        }
        acx_pcss: Acx_pcss {
            @display("p=250,200");
        }
        acx_pwr: Acx_pwr {
            @display("p=250,350");
        }
    connections:
        generator.out --> Channel --> sdn_psl.in;
        //generator.out --> Channel --> acx_psl.psl;
        sdn_psl.out --> Channel --> analyzer.in;
        analyzer.out --> Channel --> acx_psl.psl;
        //analyzer.out --> Channel --> sdn_psl.in;
        acx_pw.pw --> Channel --> sdn_pw.in;
        sdn_pw.out --> Channel --> sink_pw.in;
        acx_il.il --> Channel --> sdn_il.in;
        sdn_il.out --> Channel --> sink_il.in;
        acx_pcss.pcss --> Channel --> sdn_pcss.in;
        sdn_pcss.out --> Channel --> sink_pcss.in;
        acx_pwr.pwr --> Channel --> sdn_pwr.in;
        sdn_pwr.out --> Channel --> sink_pwr.in;
        acx_psl.pw <--> Channel <--> acx_pw.psl;
        acx_psl.pwr <--> Channel <--> acx_pwr.psl;
        acx_pw.pcss <--> Channel <--> acx_pcss.pw;
        acx_pw.il <--> Channel <--> acx_il.pw;
        acx_il.pg <--> Channel <--> acx_pg.il;
        acx_pg.pcss <--> Channel <--> acx_pcss.pg;
        acx_pcss.pwr <--> Channel <--> acx_pwr.pcss;
}
