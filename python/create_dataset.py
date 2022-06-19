import pandas as pd
import time
import os.path
from serial import Serial

"""
file_exists = os.path.isfile('dataset/good-bad.csv')
if not file_exists:
    file = open('dataset/good-bad.csv', mode='w')
    df = pd.DataFrame(columns=['ACCEL_XOUT_H', 'ACCEL_YOUT_H', 'ACCEL_ZOUT_H',
                               'GYRO_XOUT_H', 'GYRO_YOUT_H', 'GYRO_ZOUT_H',
                               'FLEX1', 'FLEX2', 'FLEX3', 'FLEX4', 'FLEX5', 'CHECKSUM', 'target'])
    df.to_csv(file, index=False)
    file.close()
    print('File created successfully!')
"""

target = input('Enter symbol to record: ')
data = []
ser = Serial('/dev/ttyUSB0', 9600)
for _ in range(500):
    try:
        feature = ser.readline().decode('utf-8').split(',')
        print(feature)
        data.append({'ACCEL_XOUT_H': feature[0],
                     'ACCEL_YOUT_H': feature[1],
                     'ACCEL_ZOUT_H': feature[2],
                     'GYRO_XOUT_H': feature[3],
                     'GYRO_YOUT_H': feature[4],
                     'GYRO_ZOUT_H': feature[5],
                     'FLEX1': feature[6],
                     'FLEX2': feature[7],
                     'FLEX3': feature[8],
                     'FLEX4': feature[9],
                     'FLEX5': feature[10],
                     'CHECKSUM': feature[11].strip(),
                     'target': target})
        time.sleep(0.4)
    except KeyboardInterrupt:
        break

with open('dataset/good-bad.csv', mode='a') as file:
    df = pd.DataFrame(data)
    df.to_csv(file, index=False)
    print('Rows appended successfully!')
