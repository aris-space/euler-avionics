#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

import sys


dict_commands = {'airbrake': bytearray([217]*4),
                 'sensor': bytearray([73]*4),
                 'frequency': bytearray([13]*4),
                 'buzzer': bytearray([113]*4),
                 'disable': bytearray([251]*4)}


sb_names = ["pressure [Pa]",
            "temp [C]",
            "gyro_x [deg/s]",
            "gyro_y [deg/s]",
            "gyro_z [deg/s]",
            "acc_x [m/s\u00b2]",
            "acc_y [m/s\u00b2]",
            "acc_z [m/s\u00b2]"]

battery_names = ['battery voltage   [V]',
                 'current draw    [mA]',
                 'consumption [mAh]']

gps_names = ['time',
             'num satellites',
             'latitude',
             'longitude',
             'fix',
             'HDOP',
             'altitude']

fsm_names = ['altitude [m]',
             'velocity [m/s]',
             'airbrake extension [%]',
             'flight phase',
             'timestamp [s]']


def data_struct():
    if sys.platform.startswith('win'):
        # imu_data_t = {'gyro_x': 'h',
        #               'gyro_y': 'h',
        #               'gyro_z': 'h',
        #               'acc_x': 'h',
        #               'acc_y': 'h',
        #               'acc_z': 'h',
        #               'ts': 'l'}
        #
        # baro_data_t = {'pressure': 'l',
        #                'temperature': 'l',
        #                'ts': 'l'}
        #
        # sb_data_t = {'baro': baro_data_t,
        #              'imu': imu_data_t,
        #              'checksum': 'B'}
        #
        # flight_phase_detection_t = {
        #     'flight_phase': 'B',
        #     'mach_regime': 'B',
        #     'mach_number': 'f',
        #     'num_samples_positive': 'b'
        # }

        gps_telemetry_t = {'hour': 'l',
                           'minute': 'l',
                           'second': 'l',
                           'satellite': 'B',
                           'lat_deg': 'B',
                           'lat_decimal': 'l',
                           'lon_deg': 'B',
                           'lon_decimal': 'l',
                           'fix': 'B',
                           'HDOP': 'h',
                           'altitude': 'h'}

        telemetry_battery_data_t = {'battery': 'h',
                                    'current': 'h',
                                    'consumption': 'h'}

        telemetry_sb_data_t = {'pressure': 'l',
                               'temp': 'l',
                               'gyro_x': 'h',
                               'gyro_y': 'h',
                               'gyro_z': 'h',
                               'acc_x': 'h',
                               'acc_y': 'h',
                               'acc_z': 'h'}

        telemetry_t = {'start': 'B',
                       'sb_data': telemetry_sb_data_t,
                       'battery': telemetry_battery_data_t,
                       'gps': gps_telemetry_t,
                       'altitude': 'l',
                       'velocity': 'l',
                       'airbrake_extension [%]': 'l',
                       'flight_phase': 'B',
                       'ts': 'l',
                       'cs': 'B'}

    else:
        # imu_data_t = {'gyro_x': 'h',
        #               'gyro_y': 'h',
        #               'gyro_z': 'h',
        #               'acc_x': 'h',
        #               'acc_y': 'h',
        #               'acc_z': 'h',
        #               'ts': 'i'}
        #
        # baro_data_t = {'pressure': 'i',
        #                'temperature': 'i',
        #                'ts': 'i'}

        # sb_data_t = {'baro': baro_data_t,
        #              'imu': imu_data_t,
        #              'checksum': 'B'}
        #
        # flight_phase_detection_t = {
        #     'flight_phase': 'B',
        #     'mach_regime': 'B',
        #     'mach_number': 'f',
        #     'num_samples_positive': 'b'
        # }

        gps_telemetry_t = {'hour': 'i',
                           'minute': 'i',
                           'second': 'i',
                           'satellite': 'B',
                           'lat_deg': 'B',
                           'lat_decimal': 'i',
                           'lon_deg': 'B',
                           'lon_decimal': 'i'}

        telemetry_battery_data_t = {'battery': 'h',
                                    'current': 'h',
                                    'consumption': 'h'}

        telemetry_sb_data_t = {'pressure': 'i',
                               'temp': 'i',
                               'gyro_x': 'h',
                               'gyro_y': 'h',
                               'gyro_z': 'h',
                               'acc_x': 'h',
                               'acc_y': 'h',
                               'acc_z': 'h'}

        telemetry_t = {'start': 'B',
                       'sb_data': telemetry_sb_data_t,
                       'battery': telemetry_battery_data_t,
                       'gps': gps_telemetry_t,
                       'altitude': 'i',
                       'velocity': 'i',
                       'airbrake_extension': 'i',
                       'flight_phase': 'B',
                       'ts': 'i',
                       'cs': 'B'}

    return telemetry_t
