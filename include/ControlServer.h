//
// Created by thomas on 13-12-20.
//

#ifndef ESPTEST_CONTROLSERVER_H
#define ESPTEST_CONTROLSERVER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

class ControlServer {
public:
    static void startServer();
    static void stopServer();
    static void processRequest();
    static bool needsToProcess();

private:
    static void handleRoot();
    static void handlePowerOn();
    static void handlePowerOff();
    static void handleVolumeUp();
    static void handleVolumeDown();

    static void handleNotFound();
};


#endif //ESPTEST_CONTROLSERVER_H