import serial
import pandas as pd
import numpy as np
import time


def read_and_populate(port, baud_rate):
    data = []
    ser = serial.Serial(port, baud_rate)
    for i in range(20):
        port_data = ser.readline()
        data.append(port_data)
        print(port_data)
        time.sleep(2)

    return data


data = np.array(read_and_populate('/dev/ttyUSB0', 9600))
df = pd.DataFrame()
df['Good'] = data.tolist()
df.to_csv('dataset/good-bad.csv', mode='w', index=False)
print(df.head())
