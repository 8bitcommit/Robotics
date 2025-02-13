import pigpio
import time

# Define the Raspberry Pi GPIO pins for the servos
SERVO1_GPIO = 9   # GPIO Pin for Servo 1
SERVO2_GPIO = 11  # GPIO Pin for Servo 2

# Define Pulse Widths (in microseconds) for motion
FORWARD_FAST = 2000
FORWARD_SLOW = 1700
STOP = 1500
REVERSE_SLOW = 1300
REVERSE_FAST = 1000

# Turn duration in seconds (adjust as needed)
TURN_TIME = 0.75

# Initialize pigpio daemon
pi = pigpio.pi()

# Set servos to neutral position
pi.set_servo_pulsewidth(SERVO1_GPIO, STOP)
pi.set_servo_pulsewidth(SERVO2_GPIO, STOP)

def turn_right():
    """ Makes the robot turn right """
    pi.set_servo_pulsewidth(SERVO1_GPIO, FORWARD_FAST)
    pi.set_servo_pulsewidth(SERVO2_GPIO, REVERSE_FAST)
    time.sleep(TURN_TIME)

def turn_left():
    """ Makes the robot turn left """
    pi.set_servo_pulsewidth(SERVO1_GPIO, FORWARD_FAST)
    pi.set_servo_pulsewidth(SERVO2_GPIO, REVERSE_FAST)
    time.sleep(TURN_TIME)

def move_forward():
    """ Moves both servos forward """
    pi.set_servo_pulsewidth(SERVO1_GPIO, FORWARD_FAST)
    pi.set_servo_pulsewidth(SERVO2_GPIO, FORWARD_FAST)

def stop():
    """ Stops both servos """
    pi.set_servo_pulsewidth(SERVO1_GPIO, STOP)
    pi.set_servo_pulsewidth(SERVO2_GPIO, STOP)

# Main Loop
try:
    while True:
        stop()
        time.sleep(1)

        move_forward()
        time.sleep(1)

        stop()
        time.sleep(1)

        turn_right()

except KeyboardInterrupt:
    print("Stopping")
    stop()
    pi.stop()

