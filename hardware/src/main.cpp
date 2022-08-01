#include <Arduino.h>
#include "SoftAPServer.h"
#include "ControlServer.h"
#include "IRController.h"
#include <EEPROM.h>

const uint32_t baudRate = 115200;

void WiFiConnected(){
    ControlServer::startServer();
    IRController::initSend();
//    IRController::startIRTranslator();
}

void setup(){
    Serial.begin(baudRate);
    while (!Serial)
        delay(50);

    EEPROM.begin(512);

    SoftAPServer::checkConnection(WiFiConnected, true, false);
}

void loop(){
    if(SoftAPServer::needsToProcess()){
        SoftAPServer::processRequest();
    }

    if(ControlServer::needsToProcess()){
        ControlServer::processRequest();
    }

    if(IRController::needsToProcess()){
        IRController::processIR();
    }

//    yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.
}