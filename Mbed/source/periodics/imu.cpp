#include <periodics/imu.hpp>
#include "imu.hpp"
#include <cmath> // Include cmath for round function

namespace periodics {

    // Static member initialization
    I2C* periodics::CImu::i2c_instance = nullptr;

    // Class constructor
    CImu::CImu(
            uint32_t f_period, 
            UnbufferedSerial& f_serial,
            PinName SDA,
            PinName SCL) 
        : utils::CTask(f_period)
        , m_isActive(true)
        , m_serial(f_serial)
        , m_velocityX(0.0)
        , m_velocityY(0.0)
        , m_velocityZ(0.0)
        , prev_accelX(0.0)
        , prev_accelY(0.0)
        , prev_accelZ(0.0) 
        , m_velocityStationaryCounter(0)
    {
        s32 comres = BNO055_ERROR;
        u8 power_mode = BNO055_INIT_VALUE;

        // Initialize I2C instance
        i2c_instance = new I2C(SDA, SCL);
        i2c_instance->frequency(400000);

        // Configure I2C interface
        I2C_routine();

        // Initialize BNO055 sensor
        comres = bno055_init(&bno055);
        power_mode = BNO055_POWER_MODE_NORMAL;
        comres += bno055_set_power_mode(power_mode);
        comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
    }

    // Class destructor
    CImu::~CImu()
    {
        s32 comres = BNO055_ERROR;
        u8 power_mode = BNO055_INIT_VALUE;
        power_mode = BNO055_POWER_MODE_SUSPEND;
        comres += bno055_set_power_mode(power_mode);

        if (i2c_instance != nullptr)
        {
            delete i2c_instance;
            i2c_instance = nullptr;
        }
    }

    // Serial callback method to activate or deactivate the publisher
    void CImu::ImuPublisherCommand(char const * a, char * b) {
        int l_isActivate = 0;
        uint32_t l_res = sscanf(a, "%d", &l_isActivate);
        if (l_res == 1) {
            m_isActive = (l_isActivate >= 1);
            sprintf(b, "ack");
        } else {
            sprintf(b, "syntax error");
        }
    }

    // Read sensor data function
    s32 CImu::bno055_data_readout_template(void)
    {
        s32 comres = BNO055_ERROR;

        s16 accel_datax = BNO055_INIT_VALUE;
        s16 accel_datay = BNO055_INIT_VALUE;
        s16 accel_dataz = BNO055_INIT_VALUE;
        struct bno055_accel_t accel_xyz;
        comres += bno055_read_accel_x(&accel_datax);
        comres += bno055_read_accel_y(&accel_datay);
        comres += bno055_read_accel_z(&accel_dataz);
        comres += bno055_read_accel_xyz(&accel_xyz);

        s16 mag_datax = BNO055_INIT_VALUE;
        s16 mag_datay = BNO055_INIT_VALUE;
        s16 mag_dataz = BNO055_INIT_VALUE;
        struct bno055_mag_t mag_xyz;
        comres += bno055_read_mag_x(&mag_datax);
        comres += bno055_read_mag_y(&mag_datay);
        comres += bno055_read_mag_z(&mag_dataz);
        comres += bno055_read_mag_xyz(&mag_xyz);

        s16 gyro_datax = BNO055_INIT_VALUE;
        s16 gyro_datay = BNO055_INIT_VALUE;
        s16 gyro_dataz = BNO055_INIT_VALUE;
        struct bno055_gyro_t gyro_xyz;
        comres += bno055_read_gyro_x(&gyro_datax);
        comres += bno055_read_gyro_y(&gyro_datay);
        comres += bno055_read_gyro_z(&gyro_dataz);
        comres += bno055_read_gyro_xyz(&gyro_xyz);

        s16 euler_data_h = BNO055_INIT_VALUE;
        s16 euler_data_r = BNO055_INIT_VALUE;
        s16 euler_data_p = BNO055_INIT_VALUE;
        struct bno055_euler_t euler_hrp;
        comres += bno055_read_euler_h(&euler_data_h);
        comres += bno055_read_euler_r(&euler_data_r);
        comres += bno055_read_euler_p(&euler_data_p);
        comres += bno055_read_euler_hrp(&euler_hrp);

        s16 quaternion_data_w = BNO055_INIT_VALUE;
        s16 quaternion_data_x = BNO055_INIT_VALUE;
        s16 quaternion_data_y = BNO055_INIT_VALUE;
        s16 quaternion_data_z = BNO055_INIT_VALUE;
        struct bno055_quaternion_t quaternion_wxyz;
        comres += bno055_read_quaternion_w(&quaternion_data_w);
        comres += bno055_read_quaternion_x(&quaternion_data_x);
        comres += bno055_read_quaternion_y(&quaternion_data_y);
        comres += bno055_read_quaternion_z(&quaternion_data_z);
        comres += bno055_read_quaternion_wxyz(&quaternion_wxyz);

        s16 linear_accel_data_x = BNO055_INIT_VALUE;
        s16 linear_accel_data_y = BNO055_INIT_VALUE;
        s16 linear_accel_data_z = BNO055_INIT_VALUE;
        struct bno055_linear_accel_t linear_acce_xyz;
        comres += bno055_read_linear_accel_x(&linear_accel_data_x);
        comres += bno055_read_linear_accel_y(&linear_accel_data_y);
        comres += bno055_read_linear_accel_z(&linear_accel_data_z);
        comres += bno055_read_linear_accel_xyz(&linear_acce_xyz);

        s16 gravity_data_x = BNO055_INIT_VALUE;
        s16 gravity_data_y = BNO055_INIT_VALUE;
        s16 gravity_data_z = BNO055_INIT_VALUE;
        struct bno055_gravity_t gravity_xyz;
        comres += bno055_read_gravity_x(&gravity_data_x);
        comres += bno055_read_gravity_y(&gravity_data_y);
        comres += bno055_read_gravity_z(&gravity_data_z);
        comres += bno055_read_gravity_xyz(&gravity_xyz);

        return comres;
    }

    // Writes data to the device over the I2C bus
    s8 CImu::BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
    {
        s32 BNO055_iERROR = BNO055_INIT_VALUE;
        u8 array[I2C_BUFFER_LEN];
        u8 stringpos = BNO055_INIT_VALUE;

        array[BNO055_INIT_VALUE] = reg_addr;
        for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
        {
            array[stringpos + BNO055_I2C_BUS_WRITE_ARRAY_INDEX] = *(reg_data + stringpos);
        }

        if (i2c_instance->write(dev_addr, (const char*)array, cnt + 1) == 0)
        {
            BNO055_iERROR = BNO055_SUCCESS; // Return success (0)
        }
        else
        {
            BNO055_iERROR = BNO055_ERROR; // Return error (-1)
        }

        return (s8)BNO055_iERROR;
    }

    // Reads data from the device over the I2C bus
    s8 CImu::BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
    {
        s32 BNO055_iERROR = BNO055_INIT_VALUE;
        u8 array[I2C_BUFFER_LEN] = { BNO055_INIT_VALUE };
        u8 stringpos = BNO055_INIT_VALUE;

        array[BNO055_INIT_VALUE] = reg_addr;

        for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
        {
            *(reg_data + stringpos) = array[stringpos];
        }

        if (i2c_instance->write(dev_addr, (const char*)&reg_addr, 1) != 0)
        {
            BNO055_iERROR = BNO055_ERROR; // Return error (-1)
            return (s8)BNO055_iERROR;
        }

        if (i2c_instance->read(dev_addr, (char*)reg_data, cnt) == 0)
        {
            BNO055_iERROR = BNO055_SUCCESS; // Return success (0)
        }
        else
        {
            BNO055_iERROR = BNO055_ERROR; // Return error (-1)
        }
        return (s8)BNO055_iERROR;
    }
    
    // Initialize I2C routine for BNO055 sensor
    void CImu::I2C_routine(void)
    {
        bno055.bus_write = BNO055_I2C_bus_write;
        bno055.bus_read = BNO055_I2C_bus_read;
        bno055.delay_msec = BNO055_delay_msek;
        bno055.dev_addr = BNO055_I2C_ADDR2 << 1;
    }

    // Introduces a delay for the specified duration in milliseconds
    void CImu::BNO055_delay_msek(u32 msek)
    {
        ThisThread::sleep_for(chrono::milliseconds(msek));
    }

void CImu::_run()
{
    if (!m_isActive) return;
    const float alpha = 0.1;  
    char buffer[256];
    s32 comres = BNO055_SUCCESS;

    float converted_euler_h_deg = BNO055_INIT_VALUE;
    float converted_euler_p_deg = BNO055_INIT_VALUE;
    float converted_euler_r_deg = BNO055_INIT_VALUE;

    comres += bno055_convert_float_euler_h_deg(&converted_euler_h_deg);
    comres += bno055_convert_float_euler_p_deg(&converted_euler_p_deg);
    comres += bno055_convert_float_euler_r_deg(&converted_euler_r_deg);

    struct bno055_linear_accel_t linear_accel = {0};
    comres += bno055_read_linear_accel_xyz(&linear_accel);
    float filtered_accelX = alpha * (linear_accel.x * 0.001 * 9.81) + (1 - alpha) * prev_accelX;
    float filtered_accelY = alpha * (linear_accel.y * 0.001 * 9.81) + (1 - alpha) * prev_accelY;
    float filtered_accelZ = alpha * (linear_accel.z * 0.001 * 9.81) + (1 - alpha) * prev_accelZ;

    // Update previous values
    prev_accelX = filtered_accelX;
    prev_accelY = filtered_accelY;
    prev_accelZ = filtered_accelZ;

    struct bno055_mag_float_t mag_xyz = {0};

    comres += bno055_convert_float_mag_xyz_uT(&mag_xyz);
    float converted_mag_x_uT = mag_xyz.x;
    float converted_mag_y_uT = mag_xyz.y;
    float converted_mag_z_uT = mag_xyz.z;

    // Scaling factor for quaternions
    const float QUATERNION_SCALE = 1.0f / 16384.0f; // 2^14 = 16384

    struct bno055_quaternion_t quaternion_raw;
    comres += bno055_read_quaternion_wxyz(&quaternion_raw);

    float quaternion_data_w_raw_converted = quaternion_raw.w * QUATERNION_SCALE;
    float quaternion_data_x_raw_converted = quaternion_raw.x * QUATERNION_SCALE;
    float quaternion_data_y_raw_converted = quaternion_raw.y * QUATERNION_SCALE;
    float quaternion_data_z_raw_converted = quaternion_raw.z * QUATERNION_SCALE;

    // Read quaternions using improved function
    struct bno055_quaternion_t quaternion_improved;
    comres += bno055_read_quaternion_wxyz_improved(&quaternion_improved);

    float quaternion_data_w_improved_converted = quaternion_improved.w * QUATERNION_SCALE;
    float quaternion_data_x_improved_converted = quaternion_improved.x * QUATERNION_SCALE;
    float quaternion_data_y_improved_converted = quaternion_improved.y * QUATERNION_SCALE;
    float quaternion_data_z_improved_converted = quaternion_improved.z * QUATERNION_SCALE;

    if (comres != BNO055_SUCCESS)
    {
        return;
    }

    bool debugging = false;
    if (debugging)
    {
        snprintf(buffer, sizeof(buffer), "@Gyroscope:%.3f;%.3f;%.3f;;\r\n",
                 converted_euler_r_deg, converted_euler_p_deg, converted_euler_h_deg);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@Accelerometru:%.3f;%.3f;%.3f;;\r\n",
                 filtered_accelX, filtered_accelY, filtered_accelZ);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@Magnetometru:%.3f;%.3f;%.3f;;\r\n",
                 converted_mag_x_uT, converted_mag_y_uT, converted_mag_z_uT);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@QuaternioniVechi:%.3f;%.3f;%.3f;%.3f;;\r\n",
                 quaternion_data_w_raw_converted, quaternion_data_x_raw_converted, quaternion_data_y_raw_converted, quaternion_data_z_raw_converted);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@QuaternioniImproved:%.3f;%.3f;%.3f;%.3f;;\r\n",
                 quaternion_data_w_improved_converted, quaternion_data_x_improved_converted, quaternion_data_y_improved_converted, quaternion_data_z_improved_converted);
        m_serial.write(buffer, strlen(buffer));
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "@1:%.3f;%.3f;%.3f;;\r\n",
                 converted_euler_r_deg, converted_euler_p_deg, converted_euler_h_deg);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@2:%.3f;%.3f;%.3f;;\r\n",
                 filtered_accelX, filtered_accelY, filtered_accelZ);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@3:%.3f;%.3f;%.3f;;\r\n",
                 converted_mag_x_uT, converted_mag_y_uT, converted_mag_z_uT);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@4:%.3f;%.3f;%.3f;%.3f;;\r\n",
                 quaternion_data_w_raw_converted, quaternion_data_x_raw_converted, quaternion_data_y_raw_converted, quaternion_data_z_raw_converted);
        m_serial.write(buffer, strlen(buffer));
        snprintf(buffer, sizeof(buffer), "@5:%.3f;%.3f;%.3f;%.3f;;\r\n",
                 quaternion_data_w_improved_converted, quaternion_data_x_improved_converted, quaternion_data_y_improved_converted, quaternion_data_z_improved_converted);
        m_serial.write(buffer, strlen(buffer));
    }

    ThisThread::sleep_for(chrono::milliseconds(100));
}

};
