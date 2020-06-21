from time import sleep
from timeit import default_timer as timer
import serial
import argparse


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


imu_data = imu_data_t(0, 0, 0, 0, 0, 0, 0)
baro_data = baro_data_t(0, 0, 0)

boards = [sb_data_t(1, baro_data, imu_data),
          sb_data_t(2, baro_data, imu_data),
          sb_data_t(3, baro_data, imu_data)]


def save_readings(all_readings, latest_readings, sending_delay):
    #TODO: this needs to be modified if there are more reading types incoming
    if 'acceleration_x' in latest_readings:
        for board_id, board_values in latest_readings['acceleration_x'].items():
            if board_id != 'sb_id_cnt':
                board_idx = board_values['sb_id']
                boards[board_idx].imu_data.acc_x = int(board_values['value']* 1000 / 9.81)
                boards[board_idx].imu_data.ts = int(board_values['ts'] * ticks_per_second_mb)
    if 'pressure' in latest_readings:
        for board_id, board_values in latest_readings['pressure'].items():
            if board_id != 'sb_id_cnt':
                board_idx = board_values['sb_id']
                boards[board_idx].baro_data.pressure = int(board_values['value'])
                boards[board_idx].baro_data.ts = int(board_values['ts'] * ticks_per_second_mb)
    if 'temperature' in latest_readings:
        for board_id, board_values in latest_readings['temperature'].items():
            if board_id != 'sb_id_cnt':
                board_idx = board_values['sb_id']
                boards[board_idx].baro_data.temperature = int(board_values['value'])
                boards[board_idx].baro_data.ts = int(board_values['ts'] * ticks_per_second_mb)
    out_str = f"{'|'.join(list(map(str, boards)))}\n".ljust(90)[:90]
    all_readings.append((out_str.encode(), sending_delay))
    #print(out_str)

def get_all_readings(filename):
    all_readings = []
    with open(filename, 'r') as f:
        #ignore header
        f.readline()
        last_readings = {}
        
        prev_sensor_id, prev_reading_type, prev_ts, prev_value = split_line(f.readline())
        update_values(last_readings, prev_sensor_id, prev_reading_type, prev_ts, prev_value)
        for line in f:
            sensor_id, reading_type, ts, value = split_line(line)
            while prev_ts == ts:
                update_values(last_readings, sensor_id, reading_type, ts, value)
                prev_sensor_id, prev_reading_type, prev_ts, prev_value = sensor_id, reading_type, ts, value
                try:
                    next_line = next(f)
                except StopIteration:
                    print('LAST ROW READ - That one probably not sent :(')
                    break
                sensor_id, reading_type, ts, value = split_line(next_line)
            else:
                #sending delay is the delay between the current and previous timestamp
                #save_readings(all_readings, last_readings, ts-prev_ts)
                save_readings(all_readings, last_readings, 0.01)
                update_values(last_readings, sensor_id, reading_type, ts, value)

            prev_sensor_id, prev_reading_type, prev_ts, prev_value = sensor_id, reading_type, ts, value
    return all_readings
    

def run(ser_port, baud_rate, ticks_per_second_mb, filename):
    all_readings = get_all_readings(filename)
    max_diff = 0
    sum_diff = 0
    #cnt = 0
    with serial.Serial(ser_port, baud_rate, timeout = 4) as ser:
        for reading, sending_delay in all_readings:
            start = timer()
            ser.write(reading)
            end = timer()
            transmission_time = end - start
            if transmission_time < sending_delay:
                sleep((sending_delay - transmission_time))
            end0 = timer()
            delay_diff = (end0 - start) - sending_delay
            max_diff = max(max_diff, delay_diff)
            sum_diff += delay_diff
            #cnt += 1
            #if cnt % 1000 == 0:
            #    print(f'Sent {cnt}. Current Max: {max_diff}, Current Avg. {sum_diff / cnt}')
    print('Simulation ended.')		
    print(f'Max delay: {max_diff}, Avg delay: {sum_diff / len(all_readings)}')

#args
ser_port = 'COM3'
baud_rate = 256000
ticks_per_second_mb = 1000
filename = 'sensor_measurements.csv'


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