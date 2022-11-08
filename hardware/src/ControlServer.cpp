#include "ControlServer.h"
#include "IRController.h"
#include <cctype>

ESP8266WebServer controlWebserver(80);

static bool needsProcessing = false;


void ControlServer::startServer() {
    Serial.println("Starting ControlServer");

    controlWebserver.on("/", HTTP_GET, handleRoot);

//    controlWebserver.on("/action", HTTP_POST, handleAction);

    controlWebserver.on("/poweron", HTTP_POST, handlePowerOn);
    controlWebserver.on("/poweroff", HTTP_POST, handlePowerOff);
    controlWebserver.on("/volumeup", HTTP_POST, handleVolumeUp);
    controlWebserver.on("/volumedown", HTTP_POST, handleVolumeDown);
    controlWebserver.on("/mute", HTTP_POST, handleVolumeMute);

    controlWebserver.onNotFound(handleNotFound);

    needsProcessing = true;

    controlWebserver.begin();
}

void ControlServer::stopServer() {
    needsProcessing = true;
    controlWebserver.stop();
}

void ControlServer::processRequest() {
    controlWebserver.handleClient();
}

void ControlServer::handleRoot() {
    Serial.println("handleControlRoot");
    controlWebserver.send(200, "text/html",
                          R"(
<h1>Hello!</h1>
)");
}

void ControlServer::handleAction() {
    if (controlWebserver.hasArg("action")) {
        String action = controlWebserver.arg("action");

        if (action.equals("poweron")) {
            handlePowerOn();
        } else if (action.equals("poweroff")) {
            handlePowerOff();
        } else if (action.equals("volumeup")) {
            handleVolumeUp();
        } else if (action.equals("volumedown")) {
            handleVolumeDown();
        } else if (action.equals("mute")) {
            handleVolumeMute();
        } else {
            controlWebserver.send(200, "text/plain", "Action value not understood");
        }

        controlWebserver.send(200);

    } else {
        controlWebserver.send(200, "text/plain", "Action param not received");
    }
}

void ControlServer::handlePowerOn() {
//    Serial.println("handlePowerOn");
    controlWebserver.send(200);
    IRController::sendIRPowerOn();
}

void ControlServer::handlePowerOff() {
//    Serial.println("handlePowerOff");
    controlWebserver.send(200);
    IRController::sendIRPowerOff();
}

void ControlServer::handleVolumeUp() {
//    Serial.println("handleVolumeUp");
    controlWebserver.send(200);
    int amount = 1;
    if (controlWebserver.hasArg("amount")) {
        String amountArg = controlWebserver.arg("amount");
        int amountConv = amountArg.toInt();
        if (amountArg != "" && amountConv != 0) {
            amount = amountConv;
        }
    }
    IRController::sendIRVolumeUp(amount);
}

void ControlServer::handleVolumeDown() {
//    Serial.println("handleVolumeDown");
    controlWebserver.send(200);
    int amount = 1;
    if (controlWebserver.hasArg("amount")) {
        String amountArg = controlWebserver.arg("amount");
        int amountConv = amountArg.toInt();
        if (amountArg != "" && amountConv != 0) {
            amount = amountConv;
        }
    }
    IRController::sendIRVolumeDown(amount);
}

void ControlServer::handleVolumeMute() {
//    Serial.println("Mute");
    controlWebserver.send(200);
    IRController::sendIRVolumeMute();
}

void ControlServer::handleNotFound() {
//    Serial.println("handleNotFound");
    controlWebserver.send(404, "text/plain", "404: Not found");
}

bool ControlServer::needsToProcess() {
    return needsProcessing;
}