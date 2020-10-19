#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

import sys

dict_commands = {'airbrake': bytearray([217] * 4),
                 'est_reset': bytearray([73] * 4),
                 'sampling_rate': bytearray([13] * 4),
                 'buzzer': bytearray([113] * 4),
                 'disable_lock': bytearray([251] * 4),
                 'camera_on': bytearray([222] * 4),
                 'camera_off': bytearray([123] * 4)}

dict_command_msg = {'airbrake': 'Airbrakes will extend. Make sure travel range is cleared. Do you want to continue?',
                    'est_reset': 'Estimator will be reset. Do you want to continue?',
                    'sampling_rate': 'High sampling rate will be turned on. Do you want to continue?',
                    'buzzer': 'Buzzer will be turned on. Do you want to continue?',
                    'disable_lock': 'Lock will be disabled. Do you want to continue?',
                    'camera_on': 'Camera will be turned on. Do you want to continue?',
                    'camera_off': 'Camera will be turned off. Do you want to continue?'}

button_text = ['Airbrake Test', 'EST Reset', 'High Sampling', 'Buzzer', 'Disable Lock', 'Camera on', 'Camera off']

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
             'timestamp [s]',
             'Buzzer',
             'Camera']

rf_names = ['Packets total',
            'Corrupted packets',
            'Correct packets',
            'RSSI']


def data_struct():
    if sys.platform.startswith('win'):
        # for windows
        gps_telemetry_t = {'hour': 'l',
                           'minute': 'l',
                           'second': 'l',
                           'lat_decimal': 'l',
                           'lon_decimal': 'l',
                           'HDOP': 'H',
                           'altitude': 'H',
                           'satellite': 'B',
                           'lat_deg': 'B',
                           'lon_deg': 'B',
                           'fix': 'B'}

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

        telemetry_t = {'alignment1': 'l',
                       'ts': 'l',
                       'sb_data': telemetry_sb_data_t,
                       'battery': telemetry_battery_data_t,
                       'alignment2': 'l',
                       'gps': gps_telemetry_t,
                       'altitude': 'l',
                       'velocity': 'l',
                       'airbrake_extension [%]': 'l',
                       'flight_phase': 'B',
                       'alignment3': 'l'
                       }

    else:
        # for Linux
        gps_telemetry_t = {'hour': 'i',
                           'minute': 'i',
                           'second': 'i',
                           'lat_decimal': 'i',
                           'lon_decimal': 'i',
                           'HDOP': 'h',
                           'altitude': 'h',
                           'satellite': 'B',
                           'lat_deg': 'B',
                           'lon_deg': 'B',
                           'fix': 'B'}

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

        telemetry_t = {'alignment1': 'L',
                       'ts': 'i',
                       'sb_data': telemetry_sb_data_t,
                       'battery': telemetry_battery_data_t,
                       'alignment2': 'L',
                       'gps': gps_telemetry_t,
                       'altitude': 'i',
                       'velocity': 'i',
                       'airbrake_extension': 'i',
                       'flight_phase': 'B',
                       'alignment3': 'L'}

    return telemetry_t
