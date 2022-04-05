#include <Arduino.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <IRcodes.h>

const uint16_t recvPin = D5;
const uint16_t irLedPin = D2;

const uint32_t baudRate = 9600;
const uint16_t captureBufferSize = 1024;
const uint8_t timeout = 50;

IRsend irsend(irLedPin);
IRrecv irrecv(recvPin, captureBufferSize, timeout, false);

decode_results results;
uint64_t sendData;
bool success = true;


void setup(){
    irrecv.enableIRIn();
    irsend.begin();

    Serial.begin(baudRate);
    while (!Serial)
        delay(50);
    Serial.println();
    Serial.println("IRTranslator started");
}

void loop(){
    if (irrecv.decode(&results)) {
        success = true;
        decode_type_t protocol = results.decode_type;
        // Only act on Sony protocol
        if (protocol == decode_type_t::SONY) {
            uint64_t value = results.value;

            switch (value) {
                case SonyVolUp:
                    sendData = HKVolUp;
                    break;
                case SonyVolDown:
                    sendData = HKVolDown;
                    break;
                case SonyRed:
                    sendData = HKOff;
                    break;
                case SonyGreen:
                    sendData = HKOn;
                    break;
                default:
                    success = false;
            }
            if(success) {
                irsend.sendNEC(sendData);
            }
        }

        irrecv.resume();
    }
    yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.
}