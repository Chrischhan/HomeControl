
#include "Nextion_functions.h"
#include <Arduino.h>
#include "NexHardware.h"

bool NextionInit() {
    bool ret1 = false;
    bool ret2 = false;
    bool ret3 = false;
    bool ret4 = false;

    dbSerialBegin(115200);
    nexSerial.begin(115200);
    sendCommand("");
    sendCommand("bkcmd=1");
    ret1 = recvRetCommandFinished();
    sendCommand("page 0");
    ret2 = recvRetCommandFinished();
    sendCommand("thup=1");
    ret3 = recvRetCommandFinished();
    sendCommand("thsp=30");
    ret4 = recvRetCommandFinished();
    return ret1 && ret2 && ret3 && ret4;
}
