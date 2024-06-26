#include <main.hpp>

const float g_baseTick = 0.0001; // seconds

UnbufferedSerial g_rpi(USBTX, USBRX, 19200);
UnbufferedSerial g_bt(PA_2, PA_3, 9600);

periodics::CBlinker g_blinker(0.5 / g_baseTick, LED1);

periodics::CImu g_imu(0.1, g_bt, I2C_SDA, I2C_SCL);

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
