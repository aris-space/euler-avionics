from time import sleep
import serial
from collections import deque
import threading
import socket


def read_serial(ser_port, baud_rate, q, timeout=4):
    with serial.Serial(ser_port, baud_rate, timeout=timeout) as ser:
        while True:
            line = ser.readline().decode()
            q.append(line)
            sleep(0.02)


def main():
    ser_port, baud_rate = 'COM5', 9600
    q = deque(maxlen=100)

    x = threading.Thread(target=read_serial, args=(ser_port, baud_rate, q))
    x.start()

    host = '127.0.0.1'  # Standard loopback interface address (localhost)
    port = 65432        # Port to listen on (non-privileged ports are > 1023)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        conn, addr = s.accept()
        with conn:
            print('Connected by', addr)
            while True:
                if q:
                    data = q.popleft()
                    print(repr(data))
                    conn.sendall(data.encode())
                sleep(0.02)


if __name__ == '__main__':
    main()
