#include <windows.h>
#include "usb2anyLMX\USB2ANY_API_Defs.h"
#include "usb2anyLMX\USB2ANY_API.h"
#include <vector>
#include <iostream>
#include <string>
#include <sstream>  
using namespace std;
#define PIPE_NAME "\\\\.\\pipe\\usb2any_pipe"


U2A_HANDLE usb2anyHandle24 = NULL;
U2A_HANDLE usb2anyHandle58 = NULL;

vector<pair<int, int>> registers = {
    {0x008D, 0x00}, {0x008C, 0x27}, {0x008B, 0x10}, {0x008A, 0x00}, {0x0089, 0x00},
    {0x0088, 0x00}, {0x0087, 0x00}, {0x0086, 0x00}, {0x0085, 0x27}, {0x0084, 0x10},
    {0x0083, 0x00}, {0x0082, 0x00}, {0x0081, 0x00}, {0x0080, 0x00}, {0x007F, 0x00},
    {0x007E, 0x27}, {0x007D, 0x10}, {0x007C, 0x00}, {0x007B, 0x00}, {0x007A, 0x00},
    {0x0079, 0x00}, {0x0078, 0x00}, {0x0077, 0x27}, {0x0076, 0x10}, {0x0075, 0x00},
    {0x0074, 0x00}, {0x0073, 0x00}, {0x0072, 0x00}, {0x0071, 0x00}, {0x0070, 0x27},
    {0x006F, 0x10}, {0x006E, 0x00}, {0x006D, 0x00}, {0x006C, 0x00}, {0x006B, 0x00},
    {0x006A, 0x00}, {0x0069, 0x27}, {0x0068, 0x10}, {0x0067, 0x00}, {0x0066, 0x00},
    {0x0065, 0x00}, {0x0064, 0x00}, {0x0063, 0x00}, {0x0062, 0x27}, {0x0061, 0x10},
    {0x0060, 0x3F}, {0x005F, 0xFF}, {0x005E, 0xFA}, {0x005D, 0x6E}, {0x005C, 0x20},
    {0x005B, 0x27}, {0x005A, 0x10}, {0x0059, 0x00}, {0x0058, 0x00}, {0x0057, 0x05},
    {0x0056, 0x92}, {0x0055, 0x00}, {0x0054, 0x00}, {0x0053, 0x00}, {0x0052, 0x01},
    {0x0051, 0x5C}, {0x0050, 0x28}, {0x004F, 0xF5}, {0x004E, 0xFF}, {0x004D, 0x47},
    {0x004C, 0xAE}, {0x004B, 0x15}, {0x004A, 0x00}, {0x0049, 0x00}, {0x0048, 0x00},
    {0x0047, 0x00}, {0x0046, 0x0A}, {0x0045, 0x00}, {0x0044, 0xFF}, {0x0043, 0x47},
    {0x0042, 0xAE}, {0x0041, 0x15}, {0x0040, 0x00}, {0x003F, 0x01}, {0x003E, 0x5C},
    {0x003D, 0x28}, {0x003C, 0xF5}, {0x003B, 0x00}, {0x003A, 0x01}, {0x0039, 0x00},
    {0x002D, 0x00}, {0x002C, 0x00}, {0x002B, 0x00}, {0x002A, 0x00}, {0x0029, 0x00},
    {0x0028, 0x00}, {0x0027, 0x52}, {0x0026, 0x18}, {0x0025, 0x10}, {0x0024, 0x08},
    {0x0023, 0x41}, {0x0022, 0x04}, {0x0021, 0x20}, {0x0020, 0x00}, {0x001F, 0x32},
    {0x001E, 0x0A}, {0x001D, 0x00}, {0x001C, 0x1F}, {0x001B, 0x08}, {0x001A, 0x00},
    {0x0019, 0x01}, {0x0018, 0x00}, {0x0017, 0x03}, {0x0016, 0xE8}, {0x0015, 0x00},
    {0x0014, 0x00}, {0x0013, 0x00}, {0x0012, 0x2C}, {0x0011, 0x00}, {0x0010, 0x18},
    {0x000F, 0x00}, {0x000E, 0x00}, {0x000D, 0x00}, {0x000C, 0x00}, {0x000B, 0x00},
    {0x000A, 0x00}, {0x0009, 0x00}, {0x0008, 0x00}, {0x0007, 0x00}, {0x0006, 0x00},
    {0x0005, 0x00}, {0x0004, 0x00}, {0x0003, 0x00}, {0x0002, 0x01}, {0x0001, 0x00},
    {0x0000, 0x18}
};
vector<pair<int, int>> registers58 = {
    {0x008D, 0x00}, {0x008C, 0x27}, {0x008B, 0x10}, {0x008A, 0x00}, {0x0089, 0x00},
    {0x0088, 0x00}, {0x0087, 0x00}, {0x0086, 0x00}, {0x0085, 0x27}, {0x0084, 0x10},
    {0x0083, 0x00}, {0x0082, 0x00}, {0x0081, 0x00}, {0x0080, 0x00}, {0x007F, 0x00},
    {0x007E, 0x27}, {0x007D, 0x10}, {0x007C, 0x00}, {0x007B, 0x00}, {0x007A, 0x00},
    {0x0079, 0x00}, {0x0078, 0x00}, {0x0077, 0x27}, {0x0076, 0x10}, {0x0075, 0x00},
    {0x0074, 0x00}, {0x0073, 0x00}, {0x0072, 0x00}, {0x0071, 0x00}, {0x0070, 0x27},
    {0x006F, 0x10}, {0x006E, 0x00}, {0x006D, 0x00}, {0x006C, 0x00}, {0x006B, 0x00},
    {0x006A, 0x00}, {0x0069, 0x27}, {0x0068, 0x10}, {0x0067, 0x00}, {0x0066, 0x00},
    {0x0065, 0x00}, {0x0064, 0x00}, {0x0063, 0x00}, {0x0062, 0x27}, {0x0061, 0x10},
    {0x0060, 0x3F}, {0x005F, 0xFF}, {0x005E, 0xF7}, {0x005D, 0xCF}, {0x005C, 0x20},
    {0x005B, 0x27}, {0x005A, 0x10}, {0x0059, 0x00}, {0x0058, 0x00}, {0x0057, 0x08},
    {0x0056, 0x31}, {0x0055, 0x00}, {0x0054, 0x00}, {0x0053, 0x00}, {0x0052, 0x00},
    {0x0051, 0x00}, {0x0050, 0x00}, {0x004F, 0x00}, {0x004E, 0x02}, {0x004D, 0x00},
    {0x004C, 0x00}, {0x004B, 0x00}, {0x004A, 0x00}, {0x0049, 0x00}, {0x0048, 0x00},
    {0x0047, 0x00}, {0x0046, 0x00}, {0x0045, 0x00}, {0x0044, 0x25}, {0x0043, 0x00},
    {0x0042, 0x00}, {0x0041, 0x00}, {0x0040, 0x00}, {0x003F, 0x29}, {0x003E, 0x00},
    {0x003D, 0x00}, {0x003C, 0x00}, {0x003B, 0x00}, {0x003A, 0x01}, {0x0039, 0x00},
    {0x002D, 0x00}, {0x002C, 0x00}, {0x002B, 0x00}, {0x002A, 0x00}, {0x0029, 0x00},
    {0x0028, 0x00}, {0x0027, 0x52}, {0x0026, 0x18}, {0x0025, 0x10}, {0x0024, 0x08},
    {0x0023, 0x41}, {0x0022, 0x04}, {0x0021, 0x20}, {0x0020, 0x00}, {0x001F, 0x32},
    {0x001E, 0x0A}, {0x001D, 0x00}, {0x001C, 0x1F}, {0x001B, 0x08}, {0x001A, 0x00},
    {0x0019, 0x01}, {0x0018, 0x00}, {0x0017, 0x00}, {0x0016, 0x00}, {0x0015, 0x40},
    {0x0014, 0x00}, {0x0013, 0x00}, {0x0012, 0x2C}, {0x0011, 0x00}, {0x0010, 0x39},
    {0x000F, 0x00}, {0x000E, 0x00}, {0x000D, 0x00}, {0x000C, 0x00}, {0x000B, 0x00},
    {0x000A, 0x00}, {0x0009, 0x00}, {0x0008, 0x00}, {0x0007, 0x00}, {0x0006, 0x00},
    {0x0005, 0x00}, {0x0004, 0x00}, {0x0003, 0x00}, {0x0002, 0x00}, {0x0001, 0x00},
    {0x0000, 0x18}
};

bool writeSPI(int regIndex, int value)
{
    if (!usb2anyHandle24) return false;

    uint8_t spiData[3];
    spiData[0] = (regIndex >> 8) & 0xFF;  // MSB of register index (always 0)
    spiData[1] = regIndex & 0xFF;         // LSB of register index
    spiData[2] = value & 0xFF;            // Data byte

    int status = u2aSPI_WriteAndRead(usb2anyHandle24, sizeof(spiData), spiData);
    return (status == sizeof(spiData));
}
int setupSPI()
{
    int divider = 24000 / 400;
    int  _DividerHigh = (divider >> 8) & 0xFF;
    int   _DividerLow = divider & 0xFF;

     int status = u2aSPI_Control(
        usb2anyHandle24,
        SPI_Capture_On_Trailing_Edge,   // CPHA = 0 (Leading Edge)
        SPI_Inactive_State_High,        // CPOL = 1 
        SPI_MSB_First,                 // MSB First
        SPI__8_Bit,                     // 8-bit SPI transfers
        SPI_With_Every_Packet,         // CS active for the whole packet
        SPI_CS_Active_High,                // CS active LOW
         _DividerHigh,
         _DividerLow                     // SPI Clock Divider (Adjustable)
    );
    return status;
}
bool updateVector(int regIndex, int value)
{
    if (!usb2anyHandle24) return false;
    for (auto& reg : registers) {
        if (reg.first == regIndex)
        {
            reg.second = value;
            return true;
        }
    }
    return false;
}

bool writeSPI58(int regIndex, int value)
{
    if (!usb2anyHandle58) return false;

    uint8_t spiData[3];
    spiData[0] = (regIndex >> 8) & 0xFF;  // MSB of register index (always 0)
    spiData[1] = regIndex & 0xFF;         // LSB of register index
    spiData[2] = value & 0xFF;            // Data byte

    int status = u2aSPI_WriteAndRead(usb2anyHandle58, sizeof(spiData), spiData);
    return (status == sizeof(spiData));
}
int setupSPI58()
{
    int divider = 24000 / 400;
    int  _DividerHigh = (divider >> 8) & 0xFF;
    int   _DividerLow = divider & 0xFF;

    int status = u2aSPI_Control(
        usb2anyHandle58,
        SPI_Capture_On_Trailing_Edge,   // CPHA = 0 (Leading Edge)
        SPI_Inactive_State_High,        // CPOL = 1 
        SPI_MSB_First,                 // MSB First
        SPI__8_Bit,                     // 8-bit SPI transfers
        SPI_With_Every_Packet,         // CS active for the whole packet
        SPI_CS_Active_High,                // CS active LOW
        _DividerHigh,
        _DividerLow                     // SPI Clock Divider (Adjustable)
    );
    return status;
}
bool updateVector58(int regIndex, int value)
{
    if (!usb2anyHandle58) return false;
    for (auto& reg : registers58) {
        if (reg.first == regIndex)
        {
            reg.second = value;
            return true;
        }
    }
    return false;
}

int main()
{
    char buffer[128];
    DWORD bytesRead;
    DWORD bytesWritten;
    string firstresponse;
    // Create Named Pipe for communication with Qt GUI
    HANDLE hPipe = CreateNamedPipeA(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 1024, 1024, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "Failed to create named pipe." << endl;
        return 1;
    }

    cout << "Waiting for Qt GUI to connect..." << endl;
    BOOL connected = ConnectNamedPipe(hPipe, NULL);
    if (!connected) {
        cerr << "Failed to establish connection with Qt GUI." << endl;
        CloseHandle(hPipe);
        return 1;
    }
    firstresponse = "connected";
    cout << firstresponse << endl;
    WriteFile(hPipe, firstresponse.c_str(), firstresponse.length(), &bytesWritten, NULL);




    // Main command loop

while (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        buffer[bytesRead] = '\0'; // Null termination
        string command(buffer);
        string response;

//WIFI 2.4 GHz
        if (command == "connectLMX2492EVM24") {
            int numDevices = u2aFindControllers();
            if (numDevices <= 0) {
                response = "ERROR: No USB2ANY devices found!";
            }
            else {
                char serialNumber[40];
                if (u2aGetSerialNumber(0, serialNumber) != 0) {
                    response = "ERROR: Failed to retrieve USB2ANY serial number.";
                }
                else {
                    usb2anyHandle24 = u2aOpen(serialNumber);
                    if (usb2anyHandle24 <= 0) {
                        response = "ERROR: Failed to open USB2ANY device.";
                    }
                    else {
                        response = "SUCCESS:LMX2492EVM connected";
                    }
                }
            }
        }
        else if (command == "startTransmission24") {
            if (usb2anyHandle24 != NULL) {
                if (setupSPI() != 0) {
                    response = "SPI configuration failed!";
                }
                else {
                    for (const auto& reg : registers) {
                        if (!writeSPI(reg.first, reg.second)) {
                            response = "Failed to start transmission";
                            break;
                        }
                        else {
                            response = "SUCCESS:LMX2492EVM transmission started";
                        }
                        Sleep(10);  // Small delay for stabilit
                    }
                    if (!writeSPI(2, 1))
                    {
                        response = "Failed to start transmission2";
                        break;
                    }
                    else
                    {
                        response = "SUCCESS:LMX2492EVM transmission started";
                    }
                }
            }
            else {
                response = "Failed usb2anyHandle24 = NULL";
            }
        }
        else if (command.rfind("stopTransmission24", 0) == 0) {
            if (usb2anyHandle24 != NULL) {
                    istringstream iss(command);
                    string cmd;
                    int reg, value;
                    iss >> cmd >> hex >> reg >> value;
                        if (!writeSPI(reg, value))
                        {
                            response = "Failed to stop transmission";
                            break;
                        }
                        else
                        {
                            response = "SUCCESS:LMX2492EVM transmission stoped";
                        }
                    }
            else 
                response = "Failed usb2anyHandle24 = NULL";
        }
        else if (command.rfind("updateConfiguration", 0) == 0) {
            istringstream iss(command);
            string cmd;
            int reg, value;
            iss >> cmd >> hex >> reg >> value;

            if (updateVector(reg, value) == true) {
                response = "SUCCESS:LMX2492EVM register updated";
            }
            else {
                response = "ERROR:LMX2492EVM update register failed";
            }
        }
//WIFI 5.8 GHz
        else if (command == "connectLMX2492EVM58") {
            int numDevices = u2aFindControllers();
            if (numDevices <= 0) {
                response = "ERROR: No USB2ANY devices found!";
            }
            else {
                char serialNumber[40];
                if (u2aGetSerialNumber(0, serialNumber) != 0) {
                    response = "ERROR: Failed to retrieve USB2ANY serial number.";
                }
                else {
                    usb2anyHandle58 = u2aOpen(serialNumber);
                    if (usb2anyHandle58 <= 0) {
                        response = "ERROR: Failed to open USB2ANY device.";
                    }
                    else {
                        response = "SUCCESS:LMX2492EVM58 connected";
                    }
                }
            }
        }
        else if (command == "startTransmission58") {
            if (usb2anyHandle58 != NULL) {
                if (setupSPI58() != 0) {
                    response = "SPI configuration failed!";
                }
                else {
                    for (const auto& reg : registers58) {
                        if (!writeSPI58(reg.first, reg.second)) {
                            response = "Failed to start transmission";
                            break;
                        }
                        else {
                            response = "SUCCESS:LMX2492EVM58 transmission started";
                        }
                        Sleep(10);  // Small delay for stabilit
                    }
                    if (!writeSPI58(2, 1))
                    {
                        response = "Failed to start transmission2";
                        break;
                    }
                    else
                    {
                        response = "SUCCESS:LMX2492EVM58 transmission started";
                    }

                }
            }
            else {
                response = "Failed usb2anyHandle58 = NULL";
            }
        }
        else if (command.rfind("stopTransmission58", 0) == 0) {
            if (usb2anyHandle58 != NULL) {
                istringstream iss(command);
                string cmd;
                int reg, value;
                iss >> cmd >> hex >> reg >> value;
                if (!writeSPI58(reg, value))
                {
                    response = "Failed to stop transmission";
                    break;
                }
                else
                {
                    response = "SUCCESS:LMX2492EVM58 transmission stoped";
                }
            }
            else
                response = "Failed usb2anyHandle58 = NULL";
        }
        else if (command.rfind("updateConfiguration58", 0) == 0) {
            istringstream iss(command);
            string cmd;
            int reg, value;
            iss >> cmd >> hex >> reg >> value;

            if (updateVector(reg, value) == true) {
                response = "SUCCESS:LMX2492EVM58 register updated";
            }
            else {
                response = "ERROR:LMX2492EVM58 update register failed";
            }
        }
        else if (command == "EXIT") {
            response = "Exiting helper process.";
        }
        else {
            response = "ERROR: Unknown command.";
        }

        //  Send response to Qt
        WriteFile(hPipe, response.c_str(), response.length(), &bytesWritten, NULL);
    }
        //  Cleanup before exiting
        if (usb2anyHandle24) {
            u2aClose(usb2anyHandle24);
        }
        if (usb2anyHandle58) {
            u2aClose(usb2anyHandle58);
        }
        CloseHandle(hPipe);

        return 0;
}