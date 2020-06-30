import sys


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

        telemetry_t = {'sb1': sb_data_t,
                       'sb2': sb_data_t,
                       'sb3': sb_data_t,
                       # 'height': 'l',
                       # 'velocity': 'l',
                       # 'ts': 'l',
                       # 'flight_phase': 'B',
                       # 'mach_regime': 'B'
                       }

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
                       # 'height': 'i',
                       # 'velocity': 'i',
                       # 'ts': 'i',
                       # 'flight_phase': 'B',
                       # 'mach_regime': 'B'
                       }

    return telemetry_t
