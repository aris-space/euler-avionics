#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

from threading import Thread
import serial
import collections
from tkinter import messagebox
import sys
import glob
import logging
from myutils import dict_commands, data_struct
import csv
from datetime import datetime


begin_seq = b'\xfa\xfa\xfa\xfa'
end_seq = b'\xdc\xdc\xdc\xdc'


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
    """
    Flattens a dict of dicts
    """
    items = []
    for k, v in d.items():
        new_key = parent_key + sep + k if parent_key else k
        if isinstance(v, collections.abc.MutableMapping):
            items.extend(flatten(v, new_key, sep=sep).items())
        else:
            items.append((new_key, v))
    return dict(items)


telemetry_t = data_struct()

data_types_order = flatten(telemetry_t)
fmt = ''.join(data_types_order.values())
measurements = data_types_order.keys()
# print(list(measurements))
# print(fmt)


def get_measurement_names():
    return measurements


class SerialConnection:
    """
    This class handles the serial communication with the Xbee module.
    """
    def __init__(self, root, serial_port='COM6', serial_baud=115200):
        """
        Constructor.

        Parameters
        ----------
        root : MainWindow
            main window object.
        serial_port : str
            defines which port
        serial_baud : int
            baud rate
        """
        self.root = root
        self.logger = logging.getLogger()
        self.port = serial_port
        self.baud = serial_baud
        self.rawData = bytearray(1) #79
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
        self.data = None
        self.serialConnection = None
        self.reset_flag = False

        now = datetime.now()
        dt_string = now.strftime("%d-%m-%Y_%H_%M")
        self.f_name = dt_string + '_raw.csv'
        self.outfile = open(self.f_name, 'a')
        self.writer = csv.writer(self.outfile)

    def start_connection(self):
        """
        Establishes serial connection.
        """
        self.logger.info('Trying to connect to: ' + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
        try:
            self.serialConnection = serial.Serial(self.port,
                                                  self.baud,
                                                  parity=serial.PARITY_NONE,
                                                  stopbits=serial.STOPBITS_ONE,
                                                  rtscts=True,
                                                  xonxoff=False,
                                                  timeout=None)

            self.logger.info('Connected to ' + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')

            return True
        except (OSError, serial.SerialException):
            self.serialConnection = None
            self.logger.info("Failed to connect with " + str(self.port) + ' at ' + str(self.baud) + ' BAUD.')
            return False

    def read_serial_start(self):
        """
        Starts reading data from serial connection.
        """
        if self.thread is None:
            self.thread = Thread(target=self.back_ground_thread)
            self.thread.start()
            self.logger.info('Ready to receive data')

    def get_serial_data(self):
        if self.data is not None:
            return self.data
        return 0

    def send(self, command):
        """
        Writes data to serial connection.
        """
        try:
            self.serialConnection.write(dict_commands.get(command))
            self.logger.info(command+' command was sent.')
        except (OSError, serial.SerialException):
            self.logger.info('was not able to send command: '+command)

    def save_raw_data(self):
        try:
            self.writer.writerow(self.rawData)
        except PermissionError as e:
            self.logger.error(e)

    def back_ground_thread(self):  # retrieve data
        self.serialConnection.reset_input_buffer()
        while self.isRun:
            try:
                self.serialConnection.read_until(terminator=begin_seq)
                self.rawData = self.serialConnection.read_until(terminator=end_seq)
                self.isReceiving = True
            except (OSError, serial.SerialException):
                print('Lost serial connection to'+str(self.port))
                messagebox.showerror('Error', 'Lost serial connection to '+str(self.port))
                break

            # print(self.rawData.hex())
            # print('length', len(self.rawData))

            self.save_raw_data()

    def get_current_raw_data(self):
        return self.rawData

    def close(self):
        self.isRun = False
        self.outfile.close()
        if self.thread is not None:
            self.thread.join()
        if self.serialConnection is not None:
            self.serialConnection.close()
        self.logger.info('Serial connection closed')
