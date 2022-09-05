//
// Created by Thomas Fink on 31/07/2022.
//

#include "IRController.h"

const uint16_t recvPin = D5;
const uint16_t irLedPin = D2;

const int sendDelay = 40;
const int doubleDutyTimeout = 4000;

IRrecv irrecv(recvPin);
IRsend irsend(irLedPin);

decode_results results;
uint64_t sendData;

static bool needsProcessing = false;
static bool canSend = false;
static u_long previousDoubleDutyTime = 0;

void IRController::initSend(){
    irsend.begin();
    canSend = true;
}

void IRController::startIRTranslator(){
    Serial.println("Starting IRTranslator");
    irrecv.enableIRIn();
    irsend.begin();
    needsProcessing = true;
}

void IRController::stopIRTranslator() {
    needsProcessing = false;
    irrecv.disableIRIn();
}


void IRController::processIR() {
    if (irrecv.decode(&results)) {

        decode_type_t protocol = results.decode_type;
        Serial.println(resultToSourceCode(&results));
        // Only act on Sony protocol
        if (protocol == decode_type_t::SONY) {
            uint64_t value = results.value;

            switch (value) {
                case SonyVolUp:
                    sendIRVolumeUp(1);
                    break;
                case SonyVolDown:
                    sendIRVolumeDown(1);
                    break;
                case SonyRed:
                    sendIRPowerOff();
                    break;
                case SonyGreen:
                    sendIRPowerOn();
                    break;
                default:
                    break;
            }
        }

        irrecv.resume();
    }
}

void IRController::sendIRPowerOn(){
    sendIRCode(HKOn, false, 1, sendDelay);
}

void IRController::sendIRPowerOff(){
    sendIRCode(HKOff, false, 1, sendDelay);
}

void IRController::sendIRVolumeUp(int amount){
    sendIRCode(HKVolUp, true, amount, sendDelay);
}

void IRController::sendIRVolumeDown(int amount){
    sendIRCode(HKVolDown, true, amount, sendDelay);
}

void IRController::sendIRVolumeMute(){
    sendIRCode(HKVolMute, false, 1, sendDelay);
}

void IRController::sendIRCode(uint64_t code, bool doubleDuty, int amount, int delay_ms){
    if (canSend){
        u_long currentTime = millis();
        u_long timeDifference = currentTime - previousDoubleDutyTime;
//        Serial.println(timeDifference);
        if (doubleDuty){
            previousDoubleDutyTime = currentTime;
            if(timeDifference > doubleDutyTimeout){
                //            Serial.println("Double duty");
                irsend.sendNEC(code);
                delay(delay_ms);
//            amount = amount + 1;
            }
        }

        for(int i = 0; i < amount; i++) {
            irsend.sendNEC(code);
            delay(delay_ms);
//            Serial.print("Send IR Code: ");
//            Serial.println(code);
        }

//        irsend.sendNEC(code, kNECBits, amount);
    }
}

bool IRController::needsToProcess() {
    return needsProcessing;
}

