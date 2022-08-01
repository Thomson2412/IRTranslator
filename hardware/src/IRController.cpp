//
// Created by Thomas Fink on 31/07/2022.
//

#include "IRController.h"

const uint16_t recvPin = D5;
const uint16_t irLedPin = D2;

IRrecv irrecv(recvPin);
IRsend irsend(irLedPin);

decode_results results;
uint64_t sendData;

static bool needsProcessing = false;
static bool canSend = false;
static u_long previousSendTime = 0;

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
        // Serial.println(resultToSourceCode(&results));
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
    sendIRCode(HKOn, false, 1);
}

void IRController::sendIRPowerOff(){
    sendIRCode(HKOff, false, 1);
}

void IRController::sendIRVolumeUp(int amount){
    sendIRCode(HKVolUp, true, amount );
}

void IRController::sendIRVolumeDown(int amount){
    sendIRCode(HKVolDown, true, amount);
}

void IRController::sendIRCode(uint64_t code, bool doubleDuty, int amount){
    if (canSend){
        u_long currentTime = millis();
        u_long timeDifference = currentTime - previousSendTime;
//        Serial.println(timeDifference);
        if (doubleDuty && timeDifference > 4000){
//            Serial.println("Double duty");
            irsend.sendNEC(code);
            delay(40);
//            amount = amount + 1;
        }

        previousSendTime = currentTime;


        for(int i = 0; i < amount; i++) {
            irsend.sendNEC(code);
            delay(40);
        }
//        Serial.print("Send IR Code: ");
//        Serial.println(code);

//        irsend.sendNEC(code, kNECBits, amount);
    }
}

bool IRController::needsToProcess() {
    return needsProcessing;
}

