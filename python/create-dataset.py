import pandas as pd
import time
import os.path
from serial import Serial

file_exists = os.path.isfile('dataset/good-bad.csv')
if not file_exists:
    file = open('dataset/good-bad.csv', mode='w')
    df = pd.DataFrame(columns=['feature', 'target'])
    df.to_csv(file, index=False)
    file.close()
    print('File created successfully!')

target = input('Enter symbol to record: ')
data = []
ser = Serial('/dev/ttyUSB0', 9600)
for _ in range(500):
    try:
        feature = ser.readline().decode('utf-8').strip()
        print(feature)
        data.append({'feature': feature,
                     'target': target})
        time.sleep(0.5)
    except KeyboardInterrupt:
        break

with open('dataset/good-bad.csv', mode='a') as file:
    df = pd.DataFrame(data)
    df.to_csv(file, index=False)
    print('Rows appended successfully!')
