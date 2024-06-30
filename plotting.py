import pandas as pd
import matplotlib.pyplot as plt
import numpy as np 
def plot_trajectories_from_file(filename):
    df = pd.read_excel(filename)
    
    ideal_x = df['Ideal X']
    ideal_y = df['Ideal Y']
    quaternion_x = df['Quaternion X']
    quaternion_y = df['Quaternion Y']
    kalman_x = df['Quaternion + Kalman X']
    kalman_y = df['Quaternion + Kalman Y']
    
    plt.figure(figsize=(10, 10))
    
    plt.plot(ideal_x, ideal_y, label='Ideal Movement', color='blue')
    plt.plot(quaternion_x, quaternion_y, label='Quaternion', color='red')
    plt.plot(kalman_x, kalman_y, label='Quaternion + Kalman', color='green')
    
    plt.xlabel('X Position (m)')
    plt.ylabel('Y Position (m)')
    plt.title('Ideal vs Predicted Movement of the Car')
    plt.legend()
    plt.grid(True)
    plt.axis('equal')
    plt.xlim(-1, 5)
    plt.ylim(-1, 5)
    plt.xticks(np.arange(-1, 5, 0.5))
    plt.yticks(np.arange(-1, 5, 0.5))
    plt.show()

def main():
    filename = 'movement_data.xlsx'
    plot_trajectories_from_file(filename)

if __name__ == "__main__":
    main()
