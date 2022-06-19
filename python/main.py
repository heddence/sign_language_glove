from ml_classification import scaler, LSVC
from serial import Serial
import time
import pandas as pd

symbols = {0: 'A',
           1: 'B',
           2: 'H',
           3: 'I',
           4: 'OK',
           5: 'Плохо',
           6: 'Хорошо',
           7: ''}


def main():
    ser = Serial('/dev/ttyUSB0', 9600)
    time.sleep(1)
    while True:
        try:
            glove_data = ser.readline().decode('utf-8').split(',')
            data_array = [{'ACCEL_XOUT_H': glove_data[0],
                           'ACCEL_YOUT_H': glove_data[1],
                           'ACCEL_ZOUT_H': glove_data[2],
                           'GYRO_XOUT_H': glove_data[3],
                           'GYRO_YOUT_H': glove_data[4],
                           'GYRO_ZOUT_H': glove_data[5],
                           'FLEX1': glove_data[6],
                           'FLEX2': glove_data[7],
                           'FLEX3': glove_data[8],
                           'FLEX4': glove_data[9],
                           'CHECKSUM': glove_data[11].strip()}]
            df_data = pd.DataFrame(data_array)
            scaled_array = scaler.transform(df_data)
            print(f'Symbol: {symbols[LSVC.predict(scaled_array)[0]]}')
            time.sleep(0.4)
        except KeyboardInterrupt:
            print('Program is finished!')
            break


if __name__ == '__main__':
    main()
