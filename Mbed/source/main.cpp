#include <main.hpp>

const float g_baseTick = 0.0001; // seconds

UnbufferedSerial g_rpi(USBTX, USBRX, 19200);
UnbufferedSerial g_bt(PA_2, PA_3, 9600);

periodics::CBlinker g_blinker(0.5 / g_baseTick, LED1);

periodics::CImu g_imu(0.1 / g_baseTick, g_bt, I2C_SDA, I2C_SCL);

drivers::CSerialMonitor::CSerialSubscriberMap g_serialMonitorSubscribers = {
    {"7",mbed::callback(&g_imu,&periodics::CImu::ImuPublisherCommand)}
};

drivers::CSerialMonitor g_serialMonitor(g_rpi, g_serialMonitorSubscribers);

utils::CTask* g_taskList[] = {
    &g_blinker,
    &g_imu,
    &g_serialMonitor
}; 

utils::CTaskManager g_taskManager(g_taskList, sizeof(g_taskList)/sizeof(utils::CTask*), g_baseTick);

/**
 * @brief Setup function for initializing some objects and transmitting a startup message through the serial. 
 * 
 * @return uint32_t Error level codes error's type.
 */
uint32_t setup()
{
    g_rpi.write("\r\n\r\n", 4);
    g_rpi.write("#################\r\n", 19);
    g_rpi.write("#               #\r\n", 19);
    g_rpi.write("#   I'm alive   #\r\n", 19);
    g_rpi.write("#               #\r\n", 19);
    g_rpi.write("#################\r\n", 19);
    g_rpi.write("\r\n", 2);
    return 0;    
}

/**
 * @brief Loop function has aim to apply repeatedly task
 * 
 * @return uint32_t Error level codes error's type.
 */
uint32_t loop()
{
    g_taskManager.mainCallback();
    return 0;
}

/**
 * @brief Main function applies the setup function and the loop function periodically. 
 * It runs automatically after the board started.
 * 
 * @return int Error level codes error's type.  
 */
int main() 
{
    uint32_t  l_errorLevel = setup();  
    while(!l_errorLevel) 
    {
        l_errorLevel = loop();
    }
    return l_errorLevel;
}


// #include "mbed.h"

// char buffer[256];

// // Define UART pins and settings
// UnbufferedSerial uart(PA_9, PA_10, 115200); // TX, RX, Baud rate 115200
// UnbufferedSerial g_rpi(USBTX, USBRX, 19200);
// // Function to send a command to the DWM1001 over UART
// void send_command(const char *command) {
//     uart.write(command, strlen(command)); // Send the command
// }

// // Function to read response from the DWM1001 over UART
// void read_response(char *response, size_t max_length) {
//     size_t response_length = 0;
//     while (response_length < max_length - 1) {
//         if (uart.readable()) {
//             char c;
//             uart.read(&c, 1);
//             response[response_length++] = c;
//         }
//         else 
//             break;
//     }
//     response[response_length] = '\0'; // Null-terminate the response
// }

// // Function to wake up the DWM1001
// void wake_up_dwm1001() {
//     const char wakeup_command[] = "\n\r";
//     send_command(wakeup_command); // Send the wakeup command
// }

// int main() {
//     // Allow some time for the system to initialize
//     ThisThread::sleep_for(1000ms);

//     // Wake up the DWM1001
//     wake_up_dwm1001();
    
//     // Allow some time for the DWM1001 to wake up
//     ThisThread::sleep_for(500ms);

//     // Buffer to store the response
//     char response[256];

//     // Example command to send to the DWM1001
//     // const char command[] = "lep\r"; // Replace with the actual command sequence
//     // send_command(command);

//     // Allow some time for the DWM1001 to process the command
//     ThisThread::sleep_for(500ms);

//     // Main loop to continuously read and print data from the DWM1001
//     while (true) {
//         // Read the response from the DWM1001
//         read_response(response, sizeof(response));
//         // Print the received response
//         if (strlen(response) > 0) {
//             snprintf(buffer, sizeof(buffer), "@Raspuns: %s",response);
//             g_rpi.write(buffer,strlen(buffer));
//         }
//         snprintf(buffer, sizeof(buffer), "Aici %d\n",strlen(response));
//         g_rpi.write(buffer,strlen(buffer));
//         // Small delay to prevent overwhelming the output
//         ThisThread::sleep_for(100ms);
//     }
// }

