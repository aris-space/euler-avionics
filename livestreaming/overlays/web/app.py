from flask import Flask, render_template

from flask_socketio import SocketIO
from threading import Thread, Event
from time import sleep

from datetime import datetime

import socket

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

time_thread = Thread()
time_thread_stop_event = Event()

countdown_thread = Thread()
countdown_thread_stop_event = Event()

# params for speed and altitude
HOST = '127.0.0.1'
PORT = 65432

position_thread = Thread()
position_thread_stop_event = Event()

getsec = lambda minutes, seconds: minutes * 60 + seconds


class Countdown:
    def __init__(self, sign, minutes, seconds, hold):
        """ Minutes and seconds are negative if the launch is in the future."""
        self.seconds = getsec(minutes, seconds) if (sign == '+') else (- getsec(minutes, seconds))
        self.hold = hold

    def set_seconds(self, sign, minutes, seconds):
        self.seconds = getsec(minutes, seconds) if (sign == '+') else (- getsec(minutes, seconds))

    def set_hold(self, hold):
        self.hold = hold

    def run(self):
        while not countdown_thread_stop_event.isSet():
            if not self.hold:
                self.seconds += 1
            minutes, seconds = divmod(abs(self.seconds), 60)
            socketio.emit('countdown_update', {'minutes': minutes, 'seconds': seconds, 'negative': self.seconds < 0})
            sleep(1)


class Event:
    def __init__(self, id, name, sign, t):
        self.id = id
        self.name = name
        self.sign = sign
        self.t = t


events = []


@socketio.on('add_event')
def add_event(json):
    events.append(Event(json["id"], json["name"], json["sign"], json["t"]))


# TODO: don't init here
countdown = Countdown('-', 0, 0, True)


@socketio.on('countdown_start')
def countdown_start(json):
    global countdown_thread, countdown
    print('received json: ' + str(json))
    if not countdown_thread.isAlive():
        print("Starting Countdown")
        print(json)
        countdown = Countdown(json['sign'], json['minutes'], json['seconds'], json['hold'])
        countdown_thread = socketio.start_background_task(countdown.run)


@socketio.on('countdown_set')
def countdown_set(json):
    global countdown_thread, countdown
    if not countdown_thread.isAlive():
        print("Starting Countdown")
        print(json)
        countdown = Countdown(json['sign'], json['minutes'], json['seconds'], json['hold'])
        countdown_thread = socketio.start_background_task(countdown.run)
    else:
        print("Updating Countdown")
        print(json)
        countdown.set_seconds(json['sign'], json['minutes'], json['seconds'])
        countdown.set_hold(json['hold'])


class SocketClient(object):
    '''Taken from
    https://stackoverflow.com/questions/9959616/multiple-writes-get-handled-as-single-one?noredirect=1&lq=1'''

    def __init__(self):
        self.buffer = ''
        self.sock = None

    def connect(self, address):
        self.buffer = ''
        self.sock = socket.socket()
        self.sock.connect(address)

    def get_msg(self):
        '''Append raw data to buffer until sentinel is found,
           then strip off the message, leaving the remainder
           in the buffer.
        '''
        while '\n' not in self.buffer:
            data = self.sock.recv(64)
            if not data:
                return ''
            self.buffer += data.decode()
        # print(repr(self.buffer))
        sentinel = self.buffer.index('\n') + 1
        msg, self.buffer = self.buffer[:sentinel], self.buffer[sentinel:]
        # if len(self.buffer) != 0:
        #     print(f'Buf: {self.buffer}')
        return msg

    def close(self):
        self.sock.close()


def position_updater():
    while not position_thread_stop_event.isSet():
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            try:
                c = SocketClient()
                c.connect((HOST, PORT))
                # s.sendall(b'Hello, world')
                while True:
                    data = c.get_msg()
                    # print(data)
                    # speed 0, altitude 1
                    position_vals = data.split()
                    if len(position_vals) >= 2:
                        socketio.emit('position_update', {'speed': f'{float(position_vals[0]):.2f}', 'altitude': position_vals[1]})
                    elif len(position_vals) > 2:
                        print(position_vals)
                    sleep(0.02)
            except socket.error:
                sleep(1)


@socketio.on('position_start')
def position_start():
    global position_thread
    if not position_thread.isAlive():
        print('Starting Position')
        position_thread = socketio.start_background_task(position_updater)


def time_updater():
    while not time_thread_stop_event.isSet():
        current_time = datetime.now().strftime('%d/%m/%Y %H:%M:%S.%f')[:-4]
        socketio.emit('time_update', {'current_time': current_time})
        sleep(0.1)


@socketio.on('time_start')
def time_start():
    global time_thread
    if not time_thread.isAlive():
        print('Starting Time')
        time_thread = socketio.start_background_task(time_updater)


@socketio.on('countdown_hold')
def countdown_hold():
    global countdown
    print('Hold')
    countdown.set_hold(True)


@socketio.on('countdown_unhold')
def countdown_unhold():
    global countdown
    print('Unhold')
    countdown.set_hold(False)


@socketio.on('connect')
def connect():
    print('Client connected')


@socketio.on('disconnect')
def disconnect():
    print('Client disconnected')


@app.route('/')
def main():
    return render_template('main.html')


@app.route('/admin')
def admin():
    return render_template('admin.html')


if __name__ == '__main__':
    socketio.run(app)
