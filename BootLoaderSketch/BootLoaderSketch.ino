#include "Wire.h"
#include "Modulo.h"

#define FUNCTION_GLOBAL_RESET 0
#define FUNCTION_EXIT_BOOTLOADER 100
#define FUNCTION_GET_BOOTLOADER_VERSION 101
#define FUNCTION_GET_NEXT_DEVICE_ID 102
#define FUNCTION_SET_DEVICE_ID 103
#define FUNCTION_GET_MCU_SIGNATURE 104
#define FUNCTION_READ_PAGE 105
#define FUNCTION_ERASE_PAGE 106
#define FUNCTION_WRITE_PAGE 107
#define FUNCTION_READ_EEPROM 108
#define FUNCTION_WRITE_EEPROM 109

void setup() {
    Wire.begin();
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

uint8_t broadcastAddress = 9;

void exitBootloader(uint16_t deviceID) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write((uint8_t)FUNCTION_EXIT_BOOTLOADER);
    Wire.write((uint8_t)0); //len
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();
}

int16_t getBootloaderVersion(uint16_t deviceID) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_GET_BOOTLOADER_VERSION);
    Wire.write(2);  //len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();

    Wire.requestFrom(broadcastAddress, 2);
    if (Wire.available()) {
        return Wire.read() | (Wire.read() << 8);
    }
    return -1;
}

uint16_t getNextDeviceID(uint16_t deviceID) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_GET_NEXT_DEVICE_ID);
    Wire.write(2);  //len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();

    Wire.requestFrom(broadcastAddress, 2);
    if (Wire.available()) {
        return Wire.read() | (Wire.read() << 8);
    }
    return 0xFFFF;
}

void setDeviceID(uint16_t oldDeviceID, int16_t newDeviceID) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_SET_DEVICE_ID);
    Wire.write(4);  //len
    Wire.write((uint8_t)oldDeviceID);
    Wire.write((uint8_t)(oldDeviceID >> 8));
    Wire.write((uint8_t)newDeviceID);
    Wire.write((uint8_t)(newDeviceID >> 8));
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();
}

uint32_t getMCUSignature(uint16_t deviceID) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_GET_MCU_SIGNATURE);
    Wire.write(2); // len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();

    Wire.requestFrom(broadcastAddress, 3);
    if (Wire.available()) {
        return Wire.read() | (Wire.read() << 8) | (Wire.read() << 16);
    }
    return 0;
}

bool readPage(uint16_t deviceID, uint32_t address, uint8_t data[16], uint8_t len) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_READ_PAGE);
    Wire.write(7); // len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write((uint8_t)address);
    Wire.write((uint8_t)(address >> 8));
    Wire.write((uint8_t)(address >> 16));
    Wire.write((uint8_t)(address >> 24));
    Wire.write((uint8_t)len);
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();

    Wire.requestFrom(broadcastAddress, 16);
    if (Wire.available()) {
        for (int i=0; i < 16; i++) {
            data[i] = Wire.read();
        }
    }
}

bool readEEPROM(uint16_t deviceID, uint16_t address, uint8_t data[16], uint8_t len) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_READ_EEPROM);
    Wire.write(7); // len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write((uint8_t)address);
    Wire.write((uint8_t)(address >> 8));
    Wire.write((uint8_t)(address >> 16));
    Wire.write((uint8_t)(address >> 24));
    Wire.write((uint8_t)len);
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();

    Wire.requestFrom(broadcastAddress, 16);
    if (Wire.available()) {
        for (int i=0; i < 16; i++) {
            data[i] = Wire.read();
        }
    }
}

bool erasePage(uint16_t deviceID, uint16_t address) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_ERASE_PAGE);
    Wire.write(4); // len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write((uint8_t)address);
    Wire.write((uint8_t)(address >> 8));
    Wire.write((uint8_t)(address >> 16));
    Wire.write((uint8_t)(address >> 24));
    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();
}

bool writePage(uint16_t deviceID, uint32_t address, uint8_t *data, uint8_t len) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_WRITE_PAGE);
    Wire.write(len+6); // len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write((uint8_t)address);
    Wire.write((uint8_t)(address >> 8));
    Wire.write((uint8_t)(address >> 16));
    Wire.write((uint8_t)(address >> 24));

    for (int i=0; i < len; i++) {
        Wire.write(data[i]);
    }

    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();
}

bool writeEEPROM(uint16_t deviceID, uint16_t address, uint8_t data[16]) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_WRITE_EEPROM);
    Wire.write(22); // len
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.write((uint8_t)address);
    Wire.write((uint8_t)(address >> 8));
    Wire.write((uint8_t)(address >> 16));
    Wire.write((uint8_t)(address >> 24));

    for (int i=0; i < 16; i++) {
        Wire.write(data[i]);
    }

    Wire.write(0xFF); // Fake CRC
    Wire.endTransmission();
}


#if 0
bool getDeviceType(uint16_t deviceID, char *deviceType, int maxLen) {
    Wire.beginTransmission(broadcastAddress);
    Wire.write(FUNCTION_GET_DEVICE_TYPE);
    Wire.write((uint8_t)deviceID);
    Wire.write((uint8_t)(deviceID >> 8));
    Wire.endTransmission();

    deviceType[0] = 0;

    int len = Wire.requestFrom(broadcastAddress, 32);

    for (int i=0; Wire.available() and i < len and i+1 < maxLen; i++) {
        deviceType[i] = Wire.read();
        deviceType[i+1] = 0;
    }
    while (Wire.available()) {
        Wire.read();
    }

    return true;
}
#endif

void printPage(uint8_t pageData[16]) {
    for (int i=0; i < 16; i++) {
        Serial.print(pageData[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}


uint16_t deviceID = 0xFFFF;


int readInt() {
    char c = Serial.read();
    while (c < '0' and c > '9') {
        c = Serial.read();
    }
    int i = 0;
    while (c >= '0' and c <= '9') {
        i = i*10 + c - '0';
        c = Serial.read();
    }
    return i;
}

void processCommand(char *command) {
    //Serial.println(command);

    if (deviceID == 0xFFFF) {
        deviceID = getNextDeviceID(0);
        Serial.println(command);
        Serial.print("Found modulo ");
        Serial.println(deviceID);
        if (deviceID == 0xFFFF) {
            return;
        }
    }

    if (strcmp(command,"erase") == 0) {
        Serial.print("Erasing modulo ");
        Serial.println(deviceID);
        for (int i=0; i < 6*1024; i += 64) {
            Serial.print("    ");
            Serial.println(i);
            erasePage(deviceID, i);
        }
    } else if (strcmp(command,"flash") == 0) {
        int32_t address = readInt();
        int32_t len = readInt();
        uint8_t data[128] = {0};


        Serial.print("Writing page ");
        Serial.print(address);
        Serial.print(" ");

        for (int i=0; i < len and i < 128; i++) {
            data[i] = readInt();
            Serial.print(data[i]);
            Serial.print(" ");
        }
        Serial.println();

        writePage(deviceID, address, data, len);
    } else if (strcmp(command,"verify") == 0) {
        int32_t address = readInt();
        int32_t len = readInt();
        uint8_t data[128] = {0};


        Serial.print("Verifying page ");
        Serial.print(address);
        Serial.print(" ");
    
        for (int i=0; i < len and i < 128; i++) {
            data[i] = readInt();
            Serial.print(data[i]);
            Serial.print(" ");
        }
        Serial.println();

        uint8_t readData[128] = {0};
        readPage(deviceID, address, readData, len);

        Serial.print("Got page ");
        Serial.print(address);
        Serial.print(" ");
        bool verifyOK = true;
        for (int i=0; i < len and i < 128; i++) {
            Serial.print(readData[i]);
            Serial.print(" ");
            if (data[i] != readData[i]) {
                verifyOK = false;
            }
        }
        Serial.println();

        Serial.print("Verify ");
        Serial.print(address);
        Serial.print(" ");
        Serial.println(verifyOK ? "OK" : "FAILED");

    } else if (strcmp(command,"eeprom") == 0) {
        int16_t address = readInt();
        uint8_t data[16] = {0};
        int32_t len = readInt();

        Serial.print("Writing eeprom ");
        Serial.print(address);
        Serial.print(" ");

        for (int i=0; i < 16; i++) {
            data[i] = readInt();
            Serial.print(data[i]);
            Serial.print(" ");
        }
        Serial.println();

        writeEEPROM(deviceID, address, data);
        
    } else if (strcmp(command,"exit") == 0) {
        Serial.println("Exiting");
        exitBootloader(deviceID);
    } else {
        Serial.print("Invalid command: \"");
        Serial.print(command);
        Serial.println("\"");
    }
        
}

long lastPrint = 0;
void loop() {

    if (millis() > lastPrint+1000) {
        //Serial.print(millis()/1000);
        //Serial.println("-------------------------");
        lastPrint = millis();
    }

    char command[16] = "";
    int commandLen = 0;
    while (Serial.available()) {
        command[commandLen] = Serial.read();

        if (command[commandLen] == '\n' or command[commandLen] == ' ') {
            command[commandLen] = 0;
            if (commandLen > 0) {
                digitalWrite(LED_BUILTIN, true);
                processCommand(command);
                digitalWrite(LED_BUILTIN, false);
                commandLen = 0;
                command[0] = 0;
            }
        } else {
            commandLen++;
        
            if (commandLen == 16) {
                command[commandLen] = 0;
                //Serial.print("Discarding: ");
                //Serial.print(command);
                commandLen = 0;
            }
        }
    }
    
}
