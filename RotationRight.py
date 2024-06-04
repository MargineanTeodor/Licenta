import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Function to create a rotation matrix for a given angle in degrees
def rotation_matrix(angle_degrees):
    angle_radians = np.radians(angle_degrees)
    cos_a = np.cos(angle_radians)
    sin_a = np.sin(angle_radians)
    return np.array([
        [cos_a, -sin_a],
        [sin_a, cos_a]
    ])

# Function to convert quaternion to rotation matrix
def quaternion_to_rotation_matrix(w, x, y, z):
    r00 = 1 - 2 * (y**2 + z**2)
    r01 = 2 * (x*y - z*w)
    r02 = 2 * (x*z + y*w)
    r10 = 2 * (x*y + z*w)
    r11 = 1 - 2 * (x**2 + z**2)
    r12 = 2 * (y*z - x*w)
    r20 = 2 * (x*z - y*w)
    r21 = 2 * (y*z + x*w)
    r22 = 1 - 2 * (x**2 + y**2)
    return np.array([[r00, r01, r02],
                     [r10, r11, r12],
                     [r20, r21, r22]])

# Function to convert Euler angles to rotation matrix
def euler_to_rotation_matrix(roll, pitch, yaw):
    roll, pitch, yaw = np.radians([roll, pitch, yaw])
    R_x = np.array([
        [1, 0, 0],
        [0, np.cos(roll), -np.sin(roll)],
        [0, np.sin(roll), np.cos(roll)]
    ])
    R_y = np.array([
        [np.cos(pitch), 0, np.sin(pitch)],
        [0, 1, 0],
        [-np.sin(pitch), 0, np.cos(pitch)]
    ])
    R_z = np.array([
        [np.cos(yaw), -np.sin(yaw), 0],
        [np.sin(yaw), np.cos(yaw), 0],
        [0, 0, 1]
    ])
    R = np.dot(R_z, np.dot(R_y, R_x))
    return R

# Read data from the Excel file
file_path = './6 sec right turn.xlsx'
data = pd.read_excel(file_path)

# Extract variables from the data
quaternion_w1 = (data['Quaternion W'] / 10).tolist()
quaternion_x1 = (data['Quaternion X'] / 10).tolist()
quaternion_y1 = (data['Quaternion Y'] / 10).tolist()
quaternion_z1 = (data['Quaternion Z'] / 10).tolist()
quaternion_w2 = (data['Quaternion W2'] / 10).tolist()
quaternion_x2 = (data['Quaternion X2'] / 10).tolist()
quaternion_y2 = (data['Quaternion Y2'] / 10).tolist()
quaternion_z2 = (data['Quaternion Z2'] / 10).tolist()
euler_roll = data['Euler Angle X'].tolist()
euler_pitch = data['Euler Angle Y'].tolist()
euler_yaw = data['Euler Angle Z'].tolist()
accel_x = data['Accelerometer X'].tolist()
accel_y = data['Accelerometer Y'].tolist()
accel_z = data['Accelerometer Z'].tolist()

# Constants
time_step = 1  # Assuming each row represents one second
constant_speed = 12.5  # Speed in cm/s
rotation_angle = -23  # Rotation angle in degrees
rotation_duration = 6  # Duration to apply rotation in seconds

# Initialize position and direction
position_x = [0]
position_y = [0]
position_x_combined = [0]
position_y_combined = [0]

# Initial movement direction (e.g., along the x-axis)
movement_direction = np.array([1, 0])
movement_direction_combined = np.array([1, 0, 0])

# Predict position using the rotation angle for the first 6 seconds
for i in range(len(data)):
    if i < rotation_duration:
        # Apply rotation to the current movement direction
        rotation_matrix_step = rotation_matrix(rotation_angle)
        movement_direction = rotation_matrix_step.dot(movement_direction)
    
    # Normalize the direction to maintain constant speed
    movement_direction_normalized = movement_direction / np.linalg.norm(movement_direction)
    
    # Update position using the constant speed and normalized direction
    new_position_x = position_x[-1] + constant_speed * movement_direction_normalized[0] * time_step
    new_position_y = position_y[-1] + constant_speed * movement_direction_normalized[1] * time_step
    
    position_x.append(new_position_x)
    position_y.append(new_position_y)
    
    # Print the positions
    print(f"Time {i}: X = {new_position_x}, Y = {new_position_y}")

# Combined prediction using quaternion, Euler angles, and accelerometer data
initial_quaternion_w1 = quaternion_w1[0]
initial_quaternion_x1 = quaternion_x1[0]
initial_quaternion_y1 = quaternion_y1[0]
initial_quaternion_z1 = quaternion_z1[0]
initial_quaternion_w2 = quaternion_w2[0]
initial_quaternion_x2 = quaternion_x2[0]
initial_quaternion_y2 = quaternion_y2[0]
initial_quaternion_z2 = quaternion_z2[0]
initial_roll = euler_roll[0]
initial_pitch = euler_pitch[0]
initial_yaw = euler_yaw[0]
initial_accel_x = accel_x[0]
initial_accel_y = accel_y[0]
initial_accel_z = accel_z[0]

for i in range(1, len(data)):
    # Convert quaternion to rotation matrix
    rotation_matrix1 = quaternion_to_rotation_matrix(
        quaternion_w1[i],
        quaternion_x1[i],
        quaternion_y1[i],
        quaternion_z1[i]
    )
    rotation_matrix2 = quaternion_to_rotation_matrix(
        quaternion_w2[i],
        quaternion_x2[i],
        quaternion_y2[i],
        quaternion_z2[i]
    )
    rotation_matrix = (rotation_matrix1 + rotation_matrix2) / 2
    
    # Convert Euler angles to rotation matrix
    euler_rotation_matrix = euler_to_rotation_matrix(
        euler_roll[i],
        euler_pitch[i],
        euler_yaw[i]
    )
    
    # Combine rotation matrices
    combined_rotation_matrix = np.dot(rotation_matrix, euler_rotation_matrix)
    
    # Apply rotation to the movement direction
    movement_direction_combined = combined_rotation_matrix.dot(movement_direction_combined)
    
    # Normalize the direction to maintain constant speed
    movement_direction_combined_normalized = movement_direction_combined / np.linalg.norm(movement_direction_combined)
    
    # Update position using the constant speed and normalized direction
    new_position_x_combined = position_x_combined[-1] + constant_speed * movement_direction_combined_normalized[0] * time_step
    new_position_y_combined = position_y_combined[-1] + constant_speed * movement_direction_combined_normalized[1] * time_step
    
    position_x_combined.append(new_position_x_combined)
    position_y_combined.append(new_position_y_combined)

# Plot the predicted positions
plt.figure(figsize=(10, 5))
plt.plot(position_x, position_y, marker='o', linestyle='-', color='b', label='Predicted Movement with 6s Rotation')
plt.plot(position_x_combined, position_y_combined, marker='x', linestyle='-', color='g', label='Combined Predicted Movement')
plt.title('Predicted Movement of the Car')
plt.xlabel('X position (cm)')
plt.ylabel('Y position (cm)')
plt.grid(True)
plt.legend()
plt.show()
