from flask import Flask, render_template

from flask_socketio import SocketIO
from threading import Thread, Event
from time import sleep

from datetime import datetime

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

time_thread = Thread()
time_thread_stop_event = Event()

countdown_thread = Thread()
countdown_thread_stop_event = Event()

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


@socketio.on('countdown_restart')
def countdown_restart(json, a):
    global countdown_thread, countdown
    if countdown_thread.isAlive():
        print("Restarting Countdown")
        print(json)
        countdown.set_seconds(json['sign'], json['minutes'], json['seconds'])
    else:
        countdown_start(json)


def time_updater():
    while not time_thread_stop_event.isSet():
        current_time = datetime.now().strftime('%d/%m/%Y %H:%M:%S.%f')[:-4]
        socketio.emit('time_update', {'current_time': current_time})
        sleep(0.1)


@socketio.on('start_time')
def start_time():
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
