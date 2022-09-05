#include "ControlServer.h"
#include "IRController.h"
#include <cctype>

ESP8266WebServer controlWebserver(80);

static bool needsProcessing = false;


void ControlServer::startServer(){
    Serial.println("Starting ControlServer");

    controlWebserver.on("/", HTTP_GET, handleRoot);

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
<form action="/poweron" method="POST">
    <input type="submit" value=" Power on">
</form>
<form action="/poweroff" method="POST">
    <input type="submit" value="Power off">
</form>
<form action="/volumeup" method="POST">
    <input type="submit" value="Volume up">
</form>
<form action="/volumedown" method="POST">
    <input type="submit" value="Volume down">
</form>
)");
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
    String amountArg = controlWebserver.arg("amount");
    int amountConv = amountArg.toInt();
    if (amountArg != "" && amountConv != 0){
        amount = amountConv;
    }
    IRController::sendIRVolumeUp(amount);
}

void ControlServer::handleVolumeDown() {
//    Serial.println("handleVolumeDown");
    controlWebserver.send(200);
    int amount = 1;
    String amountArg = controlWebserver.arg("amount");
    int amountConv = amountArg.toInt();
    if (amountArg != "" && amountConv != 0){
        amount = amountConv;
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