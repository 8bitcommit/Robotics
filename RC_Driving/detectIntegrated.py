import cv2
import numpy as np
import serial
import time

arduino = serial.Serial('/dev/ttyACM0', 115200)
time.sleep(2)

cap = cv2.VideoCapture(0)

# Camera resolution is 1280x960
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 960)

# Robot states
STATE_ROAMING = 0
STATE_AIMING = 1
STATE_SHOOTING = 2
STATE_PAUSING = 3
current_state = STATE_ROAMING  # since ROAMING is set to True on startup on Ardunio Code
last_state_change = time.time()


def send(cmd):
    """
    Purpose: Send the command to Arduino.
    Input: cmd <type: str> which reflects the serial command sent to Arduino
    Output: None
    """
    arduino.write((cmd + '\n').encode())
    print(f"Sent: {cmd}")
    time.sleep(0.1)  # Adding delays throughout help adjust for any lag between communication


def detect_red_balloon(frame):
    """
    Purpose: Detect red balloons and return the center coordinate and area if all valid.
    Input: frame <type: int> from camera
    Output: x <int>, y <int>, z <int> coordinates of balloon (x,y) and square area of balloon (z).
    """
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # So the red color range is on both ends of the HSV
    lower_red1 = np.array([0, 100, 50])
    upper_red1 = np.array([10, 255, 255])
    lower_red2 = np.array([170, 100, 50])
    upper_red2 = np.array([180, 255, 255])

    mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
    mask = cv2.bitwise_or(mask1, mask2)

    # This should reduce some noise in detection, I'll test for difference.
    kernel = np.ones((5, 5), np.uint8)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest_contour = max(contours, key=cv2.contourArea)  # i.e., biggest red circle
        area = cv2.contourArea(largest_contour)

        if area < 3000:  # if red circle is small: ignore
            return None, None

        x, y, w, h = cv2.boundingRect(largest_contour)  # else: save centre of bounding rectangle around balloon
        center_x = x + w // 2

        # //VISUALS//
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cv2.circle(frame, (center_x, y + h // 2), 5, (0, 0, 255), -1)
        cv2.putText(frame, f"Area: {area}", (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

        return center_x, area

    return None, None  # No valid balloon detected


def shoot():
    """
    Purpose: send a shoot command to Arduino.
    Input: None
    Output: None
    """
    send("FIRE")
    print("FIRING!")


if __name__ == "__main__":
    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                print("cap.read() failed in main loop.")
                break

            # draws a crosshair
            h, w = frame.shape[:2]
            center_x, center_y = w // 2, h // 2
            cv2.line(frame, (center_x - 20, center_y), (center_x + 20, center_y), (0, 0, 255), 2)
            cv2.line(frame, (center_x, center_y - 20), (center_x, center_y + 20), (0, 0, 255), 2)

            # might not need
            current_time = time.time()

            if current_state == STATE_ROAMING:
                # Look for balloons while roaming
                balloon_x, balloon_area = detect_red_balloon(frame)

                if balloon_x is not None:
                    # Balloon detected! Stop and switch to aiming state
                    send('s')  # 's' means stop
                    current_state = STATE_AIMING
                    last_state_change = current_time
                    print("Balloon detected! Stopping to aim...")
                else:
                    # No balloon, ensure we're in roaming mode
                    if current_time - last_state_change > 5:  # Send roam command periodically
                        send('a')  # btw 'a' means roam, but that's short for automatic_roam() from Arduino
                        last_state_change = current_time

            elif current_state == STATE_AIMING:
                # Fine-tune aim at the balloon SL or SR for slow-left or slow-right
                balloon_x, balloon_area = detect_red_balloon(frame)

                if balloon_x is None:
                    # if balloon goes out of frame then we'll go back to roaming
                    current_state = STATE_ROAMING
                    send('a')
                    last_state_change = current_time
                    print("Lost balloon, resuming roam...")
                else:
                    # Offset from center is the vector to find distance needed to adjust SL or SR
                    offset = balloon_x - center_x

                    # Draw horizontal targeting line
                    cv2.line(frame, (center_x, center_y), (balloon_x, center_y), (255, 0, 0), 2)
                    cv2.putText(frame, f"Offset: {offset}", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

                    # If we've been aiming for at least 1 second, start turning if needed
                    if current_time - last_state_change > 1:
                        # if "close enough" to centre, then shoot
                        if abs(offset) < 30:  # works best in testing, might need to adjust once mounted fully
                            current_state = STATE_SHOOTING
                            last_state_change = current_time
                            print("Target locked! Preparing to shoot...")  # crosshair is lined up, now shoot
                        else:
                            # turn duration
                            turn_factor = min(abs(offset) / 50, 3)  # scaled
                            turn_time = int(turn_factor * 100)  # ^

                            if offset < 0:
                                send(f"SL{turn_time}")  # Slow left
                                print(f"Turning left for {turn_time}ms")
                            else:
                                send(f"SR{turn_time}")  # Slow right
                                print(f"Turning right for {turn_time}ms")

                            last_state_change = current_time  # timer resets after each adjustment

            elif current_state == STATE_SHOOTING:
                # We're in position to shoot
                if current_time - last_state_change < 2:  # Wait 2 seconds to stabilize from wiggle or jerk
                    cv2.putText(frame, "STABILIZING...", (center_x - 100, center_y - 50),
                                cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                else:  # BANG
                    shoot()
                    current_state = STATE_PAUSING
                    last_state_change = current_time
                    print("Shot fired! Pausing...")

            elif current_state == STATE_PAUSING:
                # Delay after shot is fired
                cv2.putText(frame, f"PAUSING: {3 - int(current_time - last_state_change)}s",
                            (center_x - 100, center_y - 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 2)

                if current_time - last_state_change > 3:  # Pause for 3 seconds
                    current_state = STATE_ROAMING
                    send('a')  # Resume roaming
                    last_state_change = current_time
                    print("Resuming roam...")

            states = ["ROAMING", "AIMING", "SHOOTING", "PAUSING"]
            cv2.putText(frame, f"STATE: {states[current_state]}", (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 0), 2)

            cv2.imshow("Balloon Detection", frame)

            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break
            elif key == ord('s'):
                send('s')
            elif key == ord('a'):
                send('a')

    finally:  # stop turret and end
        send('s')
        cap.release()
        cv2.destroyAllWindows()
        arduino.close()
        print("Program terminated safely")