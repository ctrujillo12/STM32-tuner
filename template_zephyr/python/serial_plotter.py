import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# --- SETTINGS ---
SERIAL_PORT = "COM4"     # change to your Nucleo's COM port
BAUD_RATE = 115200       # must match Zephyr console UART
MAX_SAMPLES = 500        # number of points to show on plot
# ----------------

ser = serial.Serial(SERIAL_PORT, BAUD_RATE)

# circular buffer for incoming samples
data = deque([0] * MAX_SAMPLES, maxlen=MAX_SAMPLES)

fig, ax = plt.subplots()
line, = ax.plot(range(MAX_SAMPLES), data)
ax.set_ylim(-35000, 35000)   # for 16-bit audio range
ax.set_title("I2S Mic Live Data")
ax.set_xlabel("Sample Index")
ax.set_ylabel("Amplitude")

def update(frame):
    while ser.in_waiting:
        raw = ser.readline()
        print("RAW:", raw)  # <-- debug: shows exactly what’s coming from the board
        try:
            val = int(raw.decode("utf-8").strip())
            data.append(val)
        except ValueError:
            pass  # skip malformed lines
    line.set_ydata(data)
    return line,


ani = animation.FuncAnimation(fig, update, interval=10, blit=True)
plt.show()
