import pandas as pd
import numpy as np

def generate_ideal_movement(speed, wheelbase, steering_angle, side_length):
    dt = 0.1  # seconds
    speed_m_s = speed / 100.0  # Convert speed from cm/s to m/s
    wheelbase_m = wheelbase / 1000.0  # Convert wheelbase from mm to m
    steering_angle_rad = np.deg2rad(steering_angle)
    turning_radius = wheelbase_m / np.tan(steering_angle_rad)
    time_per_side = side_length / speed_m_s

    x, y = 0.0, 0.0
    theta = 0.0
    positions = []

    for side in range(4):
        start_theta = theta
        for t in np.arange(0, time_per_side, dt):
            x += speed_m_s * np.cos(theta) * dt
            y += speed_m_s * np.sin(theta) * dt
        theta = start_theta + np.pi / 2  # Rotate 90 degrees after each side
        positions.append((x, y))
    return positions

def generate_8_movement(speed, wheelbase, steering_angle, side_length):
    dt = 0.1  # seconds
    speed_m_s = speed / 100.0  # Convert speed from cm/s to m/s
    wheelbase_m = wheelbase / 1000.0  # Convert wheelbase from mm to m
    steering_angle_rad = np.deg2rad(steering_angle)
    turning_radius = wheelbase_m / np.tan(steering_angle_rad)
    time_per_side = side_length / speed_m_s

    x, y = 0.0, 0.0
    theta = 0.0
    positions = []

    for loop in range(2):  # Two loops to form an 8
        for side in range(4):
            start_theta = theta
            for t in np.arange(0, time_per_side, dt):
                x += speed_m_s * np.cos(theta) * dt
                y += speed_m_s * np.sin(theta) * dt
            theta = start_theta + np.pi / 2  # Rotate 90 degrees after each side
            positions.append((x, y))
        if loop == 0:
            x += side_length  # Move the starting point of the next square to the right

    return positions

def integrate_acceleration(acc_data, quaternions, dt=0.1):
    pos = np.array([0.0, 0.0, 0.0])
    vel = np.array([0.0, 0.0, 0.0])
    positions = []
    
    for i in range(len(acc_data)):
        q = quaternions[i]
        acc_world = acc_data[i]
        vel += acc_world * dt
        pos += vel * dt
        positions.append(pos.copy())
    return positions

def read_sensor_data(file_path):
    df = pd.read_excel(file_path)
    return {
        'quaternions': df[['Quaternion W', 'Quaternion X', 'Quaternion Y', 'Quaternion Z']].to_numpy(),
        'modified_quaternions': df[['Quaternion W2', 'Quaternion X2', 'Quaternion Y2', 'Quaternion Z2']].to_numpy(),
        'accel_data': df[['Accelerometer X', 'Accelerometer Y', 'Accelerometer Z']].to_numpy()
    }

def save_to_excel(data, filename):
    df = pd.DataFrame(data)
    df.to_excel(filename, index=False)
    print(f"Data saved to {filename}")

def main(file_path, output_file, movement_type):
    sensor_data = read_sensor_data(file_path)
    original_quaternions = sensor_data['quaternions']
    modified_quaternions = sensor_data['modified_quaternions']
    accel_data = sensor_data['accel_data']
    
    # Calculate ideal positions based on movement type
    if movement_type == 'square':
        ideal_positions = generate_ideal_movement(speed=20, wheelbase=260, steering_angle=20, side_length=2)
    elif movement_type == '8':
        ideal_positions = generate_8_movement(speed=20, wheelbase=260, steering_angle=20, side_length=2)
    else:
        raise ValueError("Unsupported movement type. Choose 'square' or '8'.")

    # Predict positions for both sets of quaternions
    original_positions = integrate_acceleration(accel_data, original_quaternions)
    modified_positions = integrate_acceleration(accel_data, modified_quaternions)

    # Prepare data for Excel
    data_to_save = {
        'Ideal X': [pos[0] for pos in ideal_positions],
        'Ideal Y': [pos[1] for pos in ideal_positions],
        'Quaternion X': [pos[0] for pos in original_positions],  
        'Quaternion Y': [pos[1] for pos in original_positions],  
        'Quaternion + Kalman X': [pos[0] for pos in modified_positions],  
        'Quaternion + Kalman Y': [pos[1] for pos in modified_positions]  
    }
    save_to_excel(data_to_save, output_file)

if __name__ == "__main__":
    file_path = 'sensor_data.xlsx'
    output_file = 'movement_data.xlsx'
    movement_type = '8'  # or 'square'
    main(file_path, output_file, movement_type)
