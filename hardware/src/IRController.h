//
// Created by Thomas Fink on 31/07/2022.
//

#ifndef IRTRANSLATOR_IRCONTROLLER_H
#define IRTRANSLATOR_IRCONTROLLER_H

#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRcodes.h>

class IRController {
public:
    static void initSend();
    static void startIRTranslator();
    static void stopIRTranslator();
    static void processIR();
    static bool needsToProcess();
    static void sendIRPowerOn();
    static void sendIRPowerOff();
    static void sendIRVolumeUp(int amount);
    static void sendIRVolumeDown(int amount);
    static void sendIRCode(uint64_t code, bool doubleDuty, int amount);
};


#endif //IRTRANSLATOR_IRCONTROLLER_H
