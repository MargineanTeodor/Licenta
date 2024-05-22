

/* Header file for the motion controller functionality */
#include <main.hpp>
/// Base sample time for the task manager. The measurement unit of base sample time is second.
const float g_baseTick = 0.0001; // seconds

// Serial interface with the another device(like single board computer). It's an built-in class of mbed based on the UART communication, the inputs have to be transmitter and receiver pins. 
UnbufferedSerial g_rpi(USBTX, USBRX, 19200);
UnbufferedSerial g_bt(PA_2, PA_3, 9600);
// It's a task for blinking periodically the built-in led on the Nucleo board, signaling the code is uploaded on the nucleo.
periodics::CBlinker g_blinker(0.5 / g_baseTick, LED1);

// It's a task for sending periodically the IMU values
periodics::CImu g_imu(0.1 / g_baseTick, g_bt, I2C_SDA, I2C_SCL);

// Map for redirecting messages with the key and the callback functions. If the message key equals to one of the enumerated keys, than it will be applied the paired callback function.
drivers::CSerialMonitor::CSerialSubscriberMap g_serialMonitorSubscribers = {
    {"7",mbed::callback(&g_imu,&periodics::CImu::ImuPublisherCommand)}
};

// Create the serial monitor object, which decodes, redirects the messages and transmits the responses.
drivers::CSerialMonitor g_serialMonitor(g_rpi, g_serialMonitorSubscribers);

// List of the task, each task will be applied their own periodicity, defined by the initializing the objects.
utils::CTask* g_taskList[] = {
    &g_blinker,
    &g_imu,
    &g_serialMonitor
}; 

// Create the task manager, which applies periodically the tasks, miming a parallelism. It needs the list of task and the time base in seconds. 
utils::CTaskManager g_taskManager(g_taskList, sizeof(g_taskList)/sizeof(utils::CTask*), g_baseTick);

/**
 * @brief Setup function for initializing some objects and transmitting a startup message through the serial. 
 * 
 * @return uint32_t Error level codes error's type.
 */
uint32_t setup()
{
    // g_rpi.format(
    //     /* bits */ 8,
    //     /* parity */ SerialBase::None,
    //     /* stop bit */ 1
    // );
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
     // Create BufferedSerial object
    uint32_t  l_errorLevel = setup();  // Ensure setup() is properly defined and implemented
    while(!l_errorLevel) 
    {
        l_errorLevel = loop();
    }
    return l_errorLevel;
}
