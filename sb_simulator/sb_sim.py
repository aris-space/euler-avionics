from time import sleep
import serial
import argparse
import time


class baro_data_t:
    def __init__(self, pressure, temperature, ts):
        self.pressure = pressure
        self.temperature = temperature
        self.ts = ts

    def __str__(self):
        fields = vars(self)
        return ','.join([str(fields[item]) for item in fields])


class imu_data_t:
    def __init__(self, gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, ts):
        self.gyro_x = gyro_x
        self.gyro_y = gyro_y
        self.gyro_z = gyro_z
        self.acc_x = acc_x
        self.acc_y = acc_y
        self.acc_z = acc_z
        self.ts = ts

    def __str__(self):
        fields = vars(self)
        return ','.join([str(fields[item]) for item in fields])


class sb_data_t:
    def __init__(self, sb_id, baro_data, imu_data):
        # self.sb_id = sb_id
        self.baro_data = baro_data
        self.imu_data = imu_data

    def __str__(self):
        fields = vars(self)
        return ';'.join([str(fields[item]) for item in fields])


def update_values(last_readings, sensor_id, reading_type, ts, value):
    if reading_type not in last_readings:
        last_readings[reading_type] = {'sb_id_cnt': 0}

    if sensor_id not in last_readings[reading_type]:
        last_readings[reading_type][sensor_id] = {'sb_id': last_readings[reading_type]['sb_id_cnt']}
        last_readings[reading_type]['sb_id_cnt'] += 1

    last_readings[reading_type][sensor_id]['ts'] = ts
    last_readings[reading_type][sensor_id]['value'] = value


def split_line(line_str):
    split_line = line_str.split(',')
    return split_line[1][:4], split_line[2], float(split_line[3]), float(split_line[4][1:-2])


imu_data = imu_data_t(0, 0, 0, 0, 0, format(0, '05d'), format(0, '05d'))
baro_data = baro_data_t(format(0, '05d'), format(0, '05d'), format(0, '05d'))

boards = [sb_data_t(1, baro_data, imu_data),
          sb_data_t(2, baro_data, imu_data),
          sb_data_t(3, baro_data, imu_data)]


def send_readings(ser, latest_readings):
    # TODO: this needs to be modified if there are more reading types incoming
    if 'acceleration_x' in latest_readings:
        for board_id, board_values in latest_readings['acceleration_x'].items():
            if board_id != 'sb_id_cnt':
                board_idx = board_values['sb_id']
                boards[board_idx].imu_data.acc_z = format(round(board_values['value'] * 1024 / 9.81), '05d')
                boards[board_idx].imu_data.ts = format(round(board_values['ts'] * ticks_per_second_mb), '05d')
    if 'pressure' in latest_readings:
        for board_id, board_values in latest_readings['pressure'].items():
            if board_id != 'sb_id_cnt':
                board_idx = board_values['sb_id']
                boards[board_idx].baro_data.pressure = format(round(board_values['value']), '05d')
                boards[board_idx].baro_data.ts = format(round(board_values['ts'] * ticks_per_second_mb), '05d')
    if 'temperature' in latest_readings:
        for board_id, board_values in latest_readings['temperature'].items():
            if board_id != 'sb_id_cnt':
                board_idx = board_values['sb_id']
                boards[board_idx].baro_data.temperature = format(round(board_values['value'] * 100), '05d')
                boards[board_idx].baro_data.ts = format(round(board_values['ts'] * ticks_per_second_mb), '05d')
    out_str = f"{'|'.join(list(map(str, boards)))}\n".ljust(250)[:250]
    ser.write(out_str.encode())
    #print(out_str)


def run(ser_port, baud_rate, ticks_per_second_mb, filename):
    with serial.Serial(ser_port, baud_rate, timeout = 4) as ser:
        with open('Sensor_data_100Hz.csv', 'r') as f:
            #ignore header
            f.readline()

            last_readings = {}

            prev_sensor_id, prev_reading_type, prev_ts, prev_value = split_line(f.readline())
            update_values(last_readings, prev_sensor_id, prev_reading_type, prev_ts, prev_value)

            for line in f:
                start = time.time()
                sensor_id, reading_type, ts, value = split_line(line)
                while prev_ts == ts:
                    update_values(last_readings, sensor_id, reading_type, ts, value)
                    prev_sensor_id, prev_reading_type, prev_ts, prev_value = sensor_id, reading_type, ts, value

                    next_line = next(f)
                    sensor_id, reading_type, ts, value = split_line(next_line)
                else:
                    #print("SENDING:")
                    #print(last_readings)
                    send_readings(ser, last_readings)
                    #print('\n')
                    update_values(last_readings, sensor_id, reading_type, ts, value)

                prev_sensor_id, prev_reading_type, prev_ts, prev_value = sensor_id, reading_type, ts, value

                #maybe change this
                
                sleep(0.0088)
                #sleep(0.01)
                print(time.time() - start)

#args
ser_port = 'COM10'
baud_rate = 9600
ticks_per_second_mb = 1000
filename = 'Sensor_data_100Hz'


parser = argparse.ArgumentParser()

parser.add_argument('-s', action='store',
                    dest='ser_port',
                    help='Serial port name')

parser.add_argument('-b', action='store',
                    dest='baud_rate',
                    help='Serial port baud rate')

parser.add_argument('-t', action='store',
                    dest='ticks_per_second_mb',
                    help='Ticks per second on the main board')

# parser.add_argument('-d', action='store',
#                     dest='delay',
#                     help='Delay between readings in seconds')

parser.add_argument('-f', action='store',
                    dest='filename',
                    help='Simulation file location')

if __name__ == '__main__':
    print(f'Serial port: {ser_port}\n'
          f'Baud rate: {baud_rate}\n'
          f'Mb ticks per second: {ticks_per_second_mb}\n'
          f'Input file: {filename}' )
    run(ser_port, baud_rate, ticks_per_second_mb, filename)
