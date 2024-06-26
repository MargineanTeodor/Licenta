import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def generate_ideal_movement(speed, wheelbase, steering_angle, side_length):
    dt = 0.1  # seconds
    speed_m_s = speed / 100.0
    wheelbase_m = wheelbase / 1000.0
    steering_angle_rad = np.deg2rad(steering_angle)
    turning_radius = wheelbase_m / np.tan(steering_angle_rad)
    time_per_side = side_length / speed_m_s
    time_to_turn = (np.pi / 2) * turning_radius / speed_m_s

    x, y = 0.0, 0.0
    theta = 0.0

    x_positions = [x]
    y_positions = [y]

    for side in range(4):
        for t in np.arange(0, time_per_side, dt):
            x += speed_m_s * np.cos(theta) * dt
            y += speed_m_s * np.sin(theta) * dt
            x_positions.append(x)
            y_positions.append(y)
        start_theta = theta
        for t in np.arange(0, time_to_turn, dt):
            x += speed_m_s * np.cos(theta) * dt
            y += speed_m_s * np.sin(theta) * dt
            theta = start_theta + (speed_m_s / turning_radius) * t
            x_positions.append(x)
            y_positions.append(y)
        theta = start_theta + np.pi / 2

    return x_positions, y_positions

def generate_8_movement(speed, wheelbase, steering_angle, side_length):
    dt = 0.1  # seconds
    speed_m_s = speed / 100.0
    wheelbase_m = wheelbase / 1000.0
    steering_angle_rad = np.deg2rad(steering_angle)
    turning_radius = wheelbase_m / np.tan(steering_angle_rad)
    time_per_side = side_length / speed_m_s
    time_to_turn = (np.pi / 2) * turning_radius / speed_m_s

    x, y = 0.0, 0.0
    theta = 0.0

    x_positions = [x]
    y_positions = [y]

    for loop in range(2):  # Two squares to form an 8
        for side in range(4):
            for t in np.arange(0, time_per_side, dt):
                x += speed_m_s * np.cos(theta) * dt
                y += speed_m_s * np.sin(theta) * dt
                x_positions.append(x)
                y_positions.append(y)
            start_theta = theta
            for t in np.arange(0, time_to_turn, dt):
                x += speed_m_s * np.cos(theta) * dt
                y += speed_m_s * np.sin(theta) * dt
                theta = start_theta + (speed_m_s / turning_radius) * t
                x_positions.append(x)
                y_positions.append(y)
            theta = start_theta + np.pi / 2
        if loop == 0:
            # Shift to the next square position to the right
            x += side_length + 2 * turning_radius  # Account for the turning maneuver

    return x_positions, y_positions

def read_sensor_data(file_path):
    df = pd.read_excel(file_path)
    quaternions = df[['Quaternion W', 'Quaternion X', 'Quaternion Y', 'Quaternion Z']].to_numpy()
    accel_data = df[['Accelerometer X', 'Accelerometer Y', 'Accelerometer Z']].to_numpy()
    return quaternions, accel_data

def quaternion_to_rotation_matrix(w, x, y, z):
    norm = np.sqrt(w**2 + x**2 + y**2 + z**2)
    w, x, y, z = w/norm, x/norm, y/norm, z/norm
    return np.array([
        [1 - 2*y**2 - 2*z**2, 2*x*y - 2*z*w, 2*x*z + 2*y*w],
        [2*x*y + 2*z*w, 1 - 2*x**2 - 2*z**2, 2*y*z - 2*x*w],
        [2*x*z - 2*y*w, 2*y*z + 2*x*w, 1 - 2*x**2 - 2*y**2]
    ])

def predict_trajectory(quaternions, accel_data, dt):
    trajectory = []
    position = np.array([0.0, 0.0])
    velocity = np.array([0.0, 0.0])
    orientation = np.eye(3)  # Initial orientation matrix (identity matrix)
    
    for i, quaternion in enumerate(quaternions):
        w, x, y, z = quaternion
        R = quaternion_to_rotation_matrix(w, x, y, z)
        orientation = R @ orientation  # Update orientation
        
        # Transform the acceleration to the global frame
        accel_global = orientation @ accel_data[i]
        
        # Only consider x and y components
        accel_global = accel_global[1:3]
        
        # Update velocity and position
        velocity += accel_global * dt
        print(velocity)
        position += velocity * dt
        print(position)
        trajectory.append(position.copy())
    
    return np.array(trajectory)

def plot_trajectories(ideal_x, ideal_y, predicted_trajectory, plot_type):
    plt.figure(figsize=(10, 10))
    if plot_type == 'square':
        plt.plot(ideal_x, ideal_y, label='Ideal Movement', color='blue')
    elif plot_type == '8':
        plt.plot(ideal_x, ideal_y, label='8-shaped Path', color='blue')
    if len(predicted_trajectory) > 0:
        plt.plot(predicted_trajectory[:, 0], predicted_trajectory[:, 1], label='Predicted Movement', color='red')
    plt.xlabel('X Position (m)')
    plt.ylabel('Y Position (m)')
    plt.title(f'Ideal vs Predicted Movement of the Car ({plot_type})')
    plt.legend()
    plt.grid(True)
    plt.axis('equal')
    plt.xlim(-3, 10)
    plt.ylim(-3, 10)
    plt.xticks(np.arange(-3, 5, 0.5))
    plt.yticks(np.arange(-3, 3, 0.5))
    plt.show()

def main(plot_type='square'):
    speed = 20  # cm/s
    wheelbase = 260  # mm
    steering_angle = 20  # degrees
    side_length = 2  # meters

    if plot_type == 'square':
        ideal_x, ideal_y = generate_ideal_movement(speed, wheelbase, steering_angle, side_length)
    elif plot_type == '8':
        ideal_x, ideal_y = generate_8_movement(speed, wheelbase, steering_angle, side_length)

    excel_file = './square.xlsx'
    quaternions, accel_data = read_sensor_data(excel_file)
    dt = 0.1  # Each row represents 1 second
    predicted_trajectory = predict_trajectory(quaternions, accel_data, dt)

    plot_trajectories(ideal_x, ideal_y, predicted_trajectory, plot_type)

if __name__ == "__main__":
    plot_type = '8'  # Change to 'square' to plot the square path
    main(plot_type="8")
# import numpy as np
# import pandas as pd
# import matplotlib.pyplot as plt

# def read_sensor_data(file_path):
#     df = pd.read_excel(file_path)
#     accel_data = df[['Accelerometer X', 'Accelerometer Y', 'Accelerometer Z']].to_numpy()
#     euler_angles = df[['Euler Angle X', 'Euler Angle Y', 'Euler Angle Z']].to_numpy()
#     return accel_data, euler_angles

# def integrate(data, dt):
#     return np.cumsum(data) * dt

# def calculate_positions(accel_data, euler_angles, dt):
#     positions_x = [0]  # starting at x=0
#     positions_y = [0]  # starting at y=0

#     # Initialize velocity
#     velocity_x = 0
#     velocity_y = 0

#     for i in range(len(accel_data)):
#         # Rotate accelerations from body to earth frame if necessary
#         # For simplicity, assume no rotation needed or apply a rotation matrix here
#         # Using the Euler angles: rotation matrix could be added here to adjust accelerations
#         acc_x, acc_y = accel_data[i, 0], accel_data[i, 1]

#         # Integrate acceleration to get velocity
#         velocity_x += acc_x * dt
#         velocity_y += acc_y * dt

#         # Integrate velocity to get position
#         new_pos_x = positions_x[-1] + velocity_x * dt
#         new_pos_y = positions_y[-1] + velocity_y * dt

#         positions_x.append(new_pos_x)
#         positions_y.append(new_pos_y)

#     return positions_x, positions_y

# def main():
#     file_path = 'square.xlsx'  # Adjust path to your Excel file
#     dt = 0.1  # time interval in seconds between data points

#     accel_data, euler_angles = read_sensor_data(file_path)
#     positions_x, positions_y = calculate_positions(accel_data, euler_angles, dt)

#     # Plotting the results
#     plt.figure(figsize=(10, 5))
#     plt.plot(positions_x, positions_y, marker='o', linestyle='-', markersize=4, label='Trajectory')
#     plt.title('Movement Trajectory from Sensor Data')
#     plt.xlabel('Position X (meters)')
#     plt.ylabel('Position Y (meters)')
#     plt.grid(True)
#     plt.legend()
#     plt.show()

# if __name__ == "__main__":
#     main()

