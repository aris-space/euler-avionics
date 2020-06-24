from threading import Thread
import serial
import time
import collections
import struct
from tkinter import messagebox
import sys
import glob
import logging


def available_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def flatten(d, parent_key='', sep='_'):
    items = []
    for k, v in d.items():
        new_key = parent_key + sep + k if parent_key else k
        if isinstance(v, collections.abc.MutableMapping):
            items.extend(flatten(v, new_key, sep=sep).items())
        else:
            items.append((new_key, v))
    return dict(items)


imu_data_t = {'gyro_x': 'h',
              'gyro_y': 'h',
              'gyro_z': 'h',
              'acc_x': 'h',
              'acc_y': 'h',
              'acc_z': 'h',
              'ts': 'l'}

baro_data_t = {'pressure': 'l',
               'temperature': 'l',
               'ts': 'l'}

sb_data_t = {'baro': baro_data_t,
             'imu': imu_data_t,
             'checksum': 'B'}


flight_phase_detection_t = {
    'flight_phase': 'B',
    'mach_regime': 'B',
    'mach_number': 'f',
    'num_samples_positive': 'b'
}

telemetry_t = {'sb1': sb_data_t,
               'sb2': sb_data_t,
               'sb3': sb_data_t,
               'height': 'l',
               'velocity': 'l',
               'ts': 'l',
               'flight_phase': 'B',
               'mach_regime': 'B'}

data_types_order = flatten(telemetry_t)
fmt = ''.join(data_types_order.values())
measurements = data_types_order.keys()
# print(measurements)


def get_measurement_names():
    return measurements


class SerialConnection:
    def __init__(self, root, serial_port='COM6', serial_baud=115200, data_num_bytes=108):
        self.root = root
        self.logger = logging.getLogger()
        self.port = serial_port
        self.baud = serial_baud
        self.dataNumBytes = data_num_bytes
        self.rawData = bytearray(data_num_bytes)
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
        self.data = None
        self.serialConnection = None

    def start_connection(self):
        print('Trying to connect to: ' + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
        self.logger.info('Trying to connect to: ' + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
        try:
            self.serialConnection = serial.Serial(self.port, self.baud, timeout=None)
            print('Connected to ' + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
            self.logger.info('Connected to ' + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
            # self.read_serial_start()
            return True
        except (OSError, serial.SerialException):
            print("Failed to connect with " + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
            self.logger.info("Failed to connect with " + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
            return False

    def read_serial_start(self):
        if self.thread is None:
            self.thread = Thread(target=self.back_ground_thread)
            self.thread.start()
            self.logger.info('Ready to receive data')
            # Block till we start receiving values
            # while not self.isReceiving:
            #    time.sleep(1.0)

    def get_serial_data(self):
        if self.data is not None:
            return self.data
        return 0

    def back_ground_thread(self):  # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while self.isRun:
            try:
                numbytes = self.serialConnection.inWaiting()
            except (OSError, serial.SerialException):
                print('Lost serial connection to' + str(self.port))
                messagebox.showerror('Error', 'Lost serial connection to ' + str(self.port))
                break
            if numbytes:
                try:
                    self.serialConnection.readinto(self.rawData)
                    self.serialConnection.reset_input_buffer()
                    self.isReceiving = True
                except (OSError, serial.SerialException):
                    print('Lost serial connection to'+str(self.port))
                    messagebox.showerror('Error', 'Lost serial connection to '+str(self.port))
                    break
                print(self.rawData.hex())
                # print('length', len(self.rawData))
                self.data = struct.unpack(fmt+'b'+'b', self.rawData)
                data_dict = dict(zip(measurements, self.data))
                print(data_dict)
                # print(self.serialConnection.inWaiting())
                self.root.update_values(self.data)

            time.sleep(0.009)

    def close(self):
        self.isRun = False
        if self.thread is not None:
            self.thread.join()
        self.serialConnection.close()
        self.logger.info('Serial connection closed')

