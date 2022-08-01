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
                    sendIRVolumeUp();
                    break;
                case SonyVolDown:
                    sendIRVolumeDown();
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
    sendIRCode(HKOn);
}

void IRController::sendIRPowerOff(){
    sendIRCode(HKOff);
}

void IRController::sendIRVolumeUp(){
    sendIRCode(HKVolUp);
}

void IRController::sendIRVolumeDown(){
    sendIRCode(HKVolDown);
}

void IRController::sendIRCode(uint64_t code){
    if (canSend){
        u_long currentTime = millis();
        u_long timeDifference = currentTime - previousSendTime;
//        Serial.println(timeDifference);
        if (timeDifference > 4000){
//            Serial.println("Double duty");
            irsend.sendNEC(code);
        }

        previousSendTime = currentTime;

        irsend.sendNEC(code);
//        Serial.print("Send IR Code: ");
//        Serial.println(code);
    }
}

bool IRController::needsToProcess() {
    return needsProcessing;
}

