import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

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

# Read data from the Excel file
file_path = './28 sec straight.xlsx'
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

# Constants
time_step = 1  # Assuming each row represents one second
constant_speed = 12.5  # Speed in cm/s

# Initialize position and direction for both quaternions
position_x = [0]
position_y = [0]
position_x_q1 = [0]
position_y_q1 = [0]

# Initial movement direction (e.g., along the x-axis)
initial_direction = np.array([1, 0, 0])

# Predict position using both sets of quaternions
for i in range(len(data)):
    # Convert first set of quaternions to rotation matrix
    rotation_matrix1 = quaternion_to_rotation_matrix(quaternion_w1[i], quaternion_x1[i], quaternion_y1[i], quaternion_z1[i])
    
    # Convert second set of quaternions to rotation matrix
    rotation_matrix2 = quaternion_to_rotation_matrix(quaternion_w2[i], quaternion_x2[i], quaternion_y2[i], quaternion_z2[i])
    
    # Combine both rotation matrices (for simplicity, let's average them)
    rotation_matrix = (rotation_matrix1 + rotation_matrix2) / 2
    
    # Apply rotation to the initial movement direction
    movement_direction = rotation_matrix.dot(initial_direction)
    
    # Normalize the direction to maintain constant speed
    movement_direction_normalized = movement_direction / np.linalg.norm(movement_direction)
    
    # Update position using the constant speed and normalized direction
    new_position_x = position_x[-1] + constant_speed * movement_direction_normalized[0] * 10 * time_step
    new_position_y = position_y[-1] + constant_speed * movement_direction_normalized[1] / 1000 * time_step
    
    position_x.append(new_position_x)
    position_y.append(new_position_y)
    
    # Predict position using only the first set of quaternions
    movement_direction_q1 = rotation_matrix1.dot(initial_direction)
    movement_direction_normalized_q1 = movement_direction_q1 / np.linalg.norm(movement_direction_q1)
    new_position_x_q1 = position_x_q1[-1] + constant_speed * movement_direction_normalized_q1[0] * 10 * time_step
    new_position_y_q1 = position_y_q1[-1] + constant_speed * movement_direction_normalized_q1[1] / 1000 * time_step
    
    position_x_q1.append(new_position_x_q1)
    position_y_q1.append(new_position_y_q1)
    
    # Print the positions
    print(f"Time {i}: X = {new_position_x}, Y = {new_position_y}")
    print(f"Time {i} with Q1: X = {new_position_x_q1}, Y = {new_position_y_q1}")

# Perfect straight-line movement
perfect_position_x = [i * constant_speed for i in range(len(data) + 1)]
perfect_position_y = [0 for _ in range(len(data) + 1)]

# Plot the predicted positions and the perfect straight line
plt.figure(figsize=(10, 5))
plt.plot(position_x, position_y, marker='o', linestyle='-', color='b', label='Predicted Movement with Quaternions 2')
plt.plot(position_x_q1, position_y_q1, marker='x', linestyle='-', color='g', label='Predicted Movement with Quaternion 1')
plt.plot(perfect_position_x, perfect_position_y, linestyle='--', color='r', label='Perfect Straight Line')
plt.title('Predicted Movement of the Car vs Perfect Straight Line')
plt.xlabel('X position (cm)')
plt.ylabel('Y position (cm)')
plt.ylim(-5, 5)  # Set y-axis limits to make the graph more visible
plt.grid(True)
plt.legend()
plt.show()
