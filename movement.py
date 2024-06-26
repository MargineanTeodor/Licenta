import serial
import time

class MessageConverter:
    """Creates the message to be sent over the serial communication"""

    commands = {
        "1": [["speed"], [float], [False]],
        "2": [["steerAngle"], [float], [False]],
        "3": [["steerAngle"], [float], [False]],
        "5": [["activate"], [bool], [False]],
        "6": [["activate"], [bool], [False]],
        "7": [["activate"], [bool], [False]],
        "8": [
            [
                "point1x",
                "point1y",
                "point2x",
                "point2y",
                "point3x",
                "point3y",
                "point4x",
                "point4y",
            ],
            [float, float, float, float, float, float, float, float],
            [False],
        ],
        "9": [["speed", "time", "steer"], [float, float, float], [False]],
    }

    def get_command(self, action, **kwargs):
        self.verify_command(action, kwargs)

        enhPrec = MessageConverter.commands[action][2][0]
        listKwargs = MessageConverter.commands[action][0]

        command = "#" + action + ":"

        for key in listKwargs:
            value = kwargs.get(key)
            valType = type(value)

            if valType == float:
                if enhPrec:
                    command += "{0:.6f};".format(value)
                else:
                    command += "{0:.2f};".format(value)
            elif valType == bool:
                command += "{0:d};".format(value)

        command += ";\r\n"
        return command

    def verify_command(self, action, commandDict):
        assert len(commandDict.keys()) == len(
            MessageConverter.commands[action][0]
        ), "Number of arguments does not match"
        for i, [key, value] in enumerate(commandDict.items()):
            assert key in MessageConverter.commands[action][0], (
                action + " should not contain key: " + key
            )
            assert type(value) == MessageConverter.commands[action][1][i], (
                action + " should be of type "
                + str(MessageConverter.commands[action][1][i])
                + " instead of "
                + str(type(value))
            )

def generate_instructions(path_type):
    if path_type == "square":
        return [
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0),   # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0),   # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0),   # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0)    # Turn 90 degrees
        ]
    elif path_type == "8":
        return [
            # First square
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0),   # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0),   # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0),   # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, 20.0),   # Turn 90 degrees
            
            # Move to the next square without stopping
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            
            # Second square
            ('sts', 20.0, 5.6, -20.0),  # Turn 90 degrees to start second square
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, -20.0),  # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, -20.0),  # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0),   # Move straight
            ('sts', 20.0, 5.6, -20.0),  # Turn 90 degrees
            ('sts', 20.0, 10.0, 0.0)    # Move straight to complete the loop
        ]
    else:
        raise ValueError("Invalid path type")

def execute_instructions(dev_file, path_type):
    # Initialize serial communication
    serial_com = serial.Serial(dev_file, 19200, timeout=0.1)
    serial_com.flushInput()
    serial_com.flushOutput()

    message_converter = MessageConverter()

    instructions = generate_instructions(path_type)

    for instruction in instructions:
        function, speed, time_duration, steer = instruction
        if function == 'sts':
            command = {
                "action": "9",
                "time": float(time_duration),
                "speed": float(speed),
                "steer": float(steer),
            }
            command_msg = message_converter.get_command(**command)
            serial_com.write(command_msg.encode("ascii"))
            time.sleep(time_duration)

    serial_com.close()

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Execute path instructions for the car.")
    parser.add_argument('--path', type=str, choices=['square', '8'], required=True, help="Path type to execute")
    parser.add_argument('--device', type=str, default="/dev/ttyACM0", help="Serial device file")

    args = parser.parse_args()

    execute_instructions(args.device, args.path)
