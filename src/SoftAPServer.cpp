/*
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
 */
//
// Created by thomas on 13-12-20.
//

#include "SoftAPServer.h"

IPAddress local_IP(192, 168, 0, 2);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer softAPWebserver(80);
DNSServer dnsServer;

static const byte DNS_PORT = 53;
static const String SSID = "ESPx";
static bool needsProcessing = false;

typedef void (*Callback)();

static Callback callback = nullptr;

static const int WAIT_TIME = 60;


void SoftAPServer::checkConnection(void (*cb)(), bool waitForConnection, bool forceNewNetwork) {
    callback = cb;
    if (waitForConnection && !forceNewNetwork) {
        WiFi.begin();

        Serial.print("\nSoftAP connecting");
        uint8_t checkCount = 0;
        while (WiFi.status() != WL_CONNECTED && checkCount < WAIT_TIME) {
            Serial.print(".");
            delay(1000);
            checkCount++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("\nCheck EEPROM");

            String ssid_to_connect = "";
            String pass_to_connect = "";

            for (int i = 0; i < 32; ++i) {
                uint8 ssid_char = EEPROM.read(i);
                if (ssid_char != 0) {
                    ssid_to_connect += char(ssid_char);
                } else {
                    break;
                }
            }
            Serial.println();
            Serial.print("SSID: ");
            Serial.println(ssid_to_connect);
            Serial.println("Reading EEPROM pass");
            for (int i = 32; i < 96; ++i) {
                uint8 pass_char = EEPROM.read(i);
                if (pass_char != 0) {
                    pass_to_connect += char(pass_char);
                } else {
                    break;
                }
            }
            Serial.print("PASS: ");
            Serial.println(pass_to_connect);

            WiFi.begin(ssid_to_connect.c_str(), pass_to_connect.c_str());

            Serial.print("\nSoftAP connecting EEPROM");
            checkCount = 0;
            while (WiFi.status() != WL_CONNECTED && checkCount < WAIT_TIME) {
                Serial.print(".");
                delay(1000);
                checkCount++;
            }
        }
    }

    if (WiFi.status() != WL_CONNECTED || forceNewNetwork) {
        if (forceNewNetwork) {
            WiFi.disconnect();
        }
        Serial.println("\nStart soft ap");
        Serial.print("Configuring SoftAP Configuration...");
        Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

        Serial.print("Setting SoftAP...");
        String macAddr = WiFi.macAddress();
        macAddr.replace(":", "");
        Serial.println(WiFi.softAP(SSID + macAddr) ? "Ready" : "Failed!");

        Serial.println();
        Serial.print("connect to the IP Address given below...");
        Serial.println(WiFi.softAPIP());

        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", local_IP);

        softAPWebserver.on("/", HTTP_GET, handleRoot);
        softAPWebserver.on("/connect", HTTP_POST, handleLogin);
        softAPWebserver.onNotFound(handleNotFound);

        softAPWebserver.begin();

        needsProcessing = true;
    } else {
        Serial.println("\nConnected");
        Serial.println("IP: " + WiFi.localIP().toString());
        cb();
    }
}

void SoftAPServer::processRequest() {
    dnsServer.processNextRequest();
    softAPWebserver.handleClient();
}

void SoftAPServer::handleRoot() {
    Serial.println("handleAPRoot");
    softAPWebserver.send(200, "text/html",
                         R"(<form action="/connect" method="POST">
                            <input type="text" name="SSID" placeholder="Ssid">
                            </br>
                            <input type="password" name="password" placeholder="Password">
                            </br>
                            <input type="submit" value="Login"></form>)");
}

void SoftAPServer::handleLogin() {
    Serial.println("handleLogin");
    if (captivePortal()) {
        return;
    }
    if (!softAPWebserver.hasArg("SSID") && !softAPWebserver.hasArg("password")) {
        if (softAPWebserver.arg("SSID") == nullptr || softAPWebserver.arg("password") == nullptr) {
            softAPWebserver.send(400, "text/plain", "400: Invalid Request");
            return;
        }
    }
    String ssid_to_connect = softAPWebserver.arg("SSID");
    String pass_to_connect = softAPWebserver.arg("password");
    softAPWebserver.send(200, "text/html", "<h1>Connecting to " + softAPWebserver.arg("SSID") + "</h1>");

    delay(50);

    WiFi.softAPdisconnect();
    WiFi.disconnect();
    softAPWebserver.close();
    dnsServer.stop();
    needsProcessing = false;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_to_connect, pass_to_connect);

    if (ssid_to_connect.length() > 0 && pass_to_connect.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
            EEPROM.write(i, 0);
        }
        Serial.println(ssid_to_connect);
        Serial.println("");
        Serial.println(pass_to_connect);
        Serial.println("");
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < int(ssid_to_connect.length()); i++) {
            EEPROM.write(i, ssid_to_connect[i]);
            Serial.print("Wrote: ");
            Serial.println(ssid_to_connect[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < int(pass_to_connect.length()); i++) {
            EEPROM.write(32 + i, pass_to_connect[i]);
            Serial.print("Wrote: ");
            Serial.println(pass_to_connect[i]);
        }
        EEPROM.commit();
    }

    checkConnection(callback, true, false);
}

void SoftAPServer::handleNotFound() {
    Serial.println("handleNotFound");
    if (captivePortal()) {
        return;
    }
    softAPWebserver.send(404, "text/plain", "404: Not found");
}

bool SoftAPServer::isIp(const String &str) {
    for (size_t i = 0; i < str.length(); i++) {
        int c = (unsigned char) str.charAt(i);
        if (c != '.' && (c < '0' || c > '9')) {
            return false;
        }
    }
    return true;
}

bool SoftAPServer::captivePortal() {
    if (!isIp(softAPWebserver.hostHeader())) {
        softAPWebserver.sendHeader("Location", String("http://") + softAPWebserver.client().localIP().toString(), true);
        softAPWebserver.send(302, "text/plain",
                             ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
        softAPWebserver.client().stop(); // Stop is needed because we sent no content length
        return true;
    }
    return false;
}

bool SoftAPServer::needsToProcess() {
    return needsProcessing;
}