import sys


dict_commands = {'airbrake': bytearray([217]*4),
                 'sensor': bytearray([210]*4),
                 'frequency': bytearray([205]*4)}


sb_names = ["pressure",
            "temp",
            "gyro_x",
            "gyro_y",
            "gyro_z",
            "acc_x",
            "acc_y",
            "acc_z"]

battery_names = ['battery voltage',
                 'current draw',
                 'consumption']

gps_names = ['time',
             'num satellites',
             'latitude',
             'longitude',
             'degree',
             'decimal']

fsm_names = ['altitude',
             'velocity',
             'airbrake extension',
             'flight phase',
             'timestamp']


def data_struct():
    if sys.platform.startswith('win'):
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

        gps_telemetry_t = {'hour': 'l',
                           'minute': 'l',
                           'second': 'l',
                           'satellite': 'B',
                           'lat_deg': 'B',
                           'lat_decimal': 'l',
                           'lon_deg': 'B',
                           'lon_decimal': 'l'}

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
                       'airbrake_extension': 'l',
                       'flight_phase': 'B',
                       'ts': 'l',
                       'cs': 'B'}

    else:
        imu_data_t = {'gyro_x': 'h',
                      'gyro_y': 'h',
                      'gyro_z': 'h',
                      'acc_x': 'h',
                      'acc_y': 'h',
                      'acc_z': 'h',
                      'ts': 'i'}

        baro_data_t = {'pressure': 'i',
                       'temperature': 'i',
                       'ts': 'i'}

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
                       'height': 'i',
                       'velocity': 'i',
                       'ts': 'i',
                       'flight_phase': 'B',
                       'mach_regime': 'B'}

    return telemetry_t
