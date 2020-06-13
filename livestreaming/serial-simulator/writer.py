from time import sleep
import serial


def main():
    with serial.Serial('COM3', 9600, timeout=4) as ser:
        while True:
            for i in range(1, 1000):
                ser.write(f'{i/100} {i}\n'.encode())
                sleep(0.02)


if __name__ == '__main__':
    main()
