#include "global_stats_listener.h"

Define_Module(GlobalStatsListener);

void GlobalStatsListener::initialize() {
    // numTotalGenerated = 0;
    numTotalDropped = 0;
    // numTotalDroppedInAnalyzer = 0;
    numTotalDelivered = 0;
}

void GlobalStatsListener::handleMessage(cMessage *msg) {}

void GlobalStatsListener::finish() {
    // recordScalar("#numTotalGenerated", numTotalGenerated);
    recordScalar("#numTotalDropped", numTotalDropped);
    // recordScalar("#numTotalDroppedInAnalyzer", numTotalDroppedInAnalyzer);
    recordScalar("#numTotalDelivered", numTotalDelivered);
}

void GlobalStatsListener::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "drpd: %lu dlvrd: %lu",
        numTotalDropped, numTotalDelivered);
    getDisplayString().setTagArg("t", 0, buf);
}
