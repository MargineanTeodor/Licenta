import serial
import pandas as pd

def main():
    serial_port = 'COM15'  # Update this to your COM port
    baud_rate = 9600       # Update this if your module uses a different rate
    excel_file = 'sensor_data.xlsx'  # Name of the Excel file to save data

    columns = [
        'Euler Angle X', 'Euler Angle Y', 'Euler Angle Z',
        'Accelerometer X', 'Accelerometer Y', 'Accelerometer Z',
        'Magnetometer X', 'Magnetometer Y', 'Magnetometer Z',
        'Quaternion W', 'Quaternion X', 'Quaternion Y', 'Quaternion Z',
        'Quaternion W2', 'Quaternion X2', 'Quaternion Y2', 'Quaternion Z2'
    ]
    df = pd.DataFrame(columns=columns)
    current_readings = [None] * len(columns)
    error_occurred = False  # Flag to indicate an error in processing

    try:
        ser = serial.Serial(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")
        print("Starting to read data...")

        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()

                if line.startswith("@"):
                    header, data = line.split(':')
                    message_type = int(header[1])

                    if message_type == 1 and error_occurred:
                        # Reset error flag when new valid @1 message is received
                        error_occurred = False
                        current_readings = [None] * len(columns)

                    if not error_occurred:
                        values = data.split(';')[:-2]

                        try:
                            # Attempt to convert each value to float
                            values = [float(v) for v in values]
                        except ValueError:
                            # Set all values to zero in case of an error
                            print(f"Error converting data to float. Setting values to zero for this set. Data received: {data}")
                            error_occurred = True
                            current_readings = [0.0] * len(columns)

                        start_index = {
                            1: 0,   # Euler angles
                            2: 3,   # Accelerometer
                            3: 6,   # Magnetometer
                            4: 9,   # Quaternion 1
                            5: 13   # Quaternion 2
                        }.get(message_type, 0)

                        if message_type in (4, 5):
                            values = [v / 10000 for v in values]

                        if not error_occurred:
                            current_readings[start_index:start_index + len(values)] = values

                            if None not in current_readings:
                                df.loc[len(df)] = current_readings
                                print(f"Data row added to DataFrame: {current_readings}")
                                current_readings = [None] * len(columns)

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {serial_port}: {e}")
    except KeyboardInterrupt:
        print("Stopped by user.")
    finally:
        if ser.is_open:
            ser.close()
            print("Serial port closed.")
        if not df.empty:
            df.to_excel(excel_file, index=False)
            print(f"Final data written to {excel_file}.")

if __name__ == "__main__":
    main()
