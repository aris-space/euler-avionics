#!/usr/bin/env python

import tkinter as tk
from tkinter import ttk, messagebox, Tk, Frame, scrolledtext
from SerialConnection import SerialConnection, get_measurement_names, available_ports
import threading
import time
import sys
import csv
import os.path
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib.figure import Figure
import matplotlib.animation as animation
from matplotlib.animation import FuncAnimation
from matplotlib import style
style.use('ggplot')
import logging
from Logs.LoggingHandler import LoggingHandler
from View.PlotControl import PlotControl

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

len_sb = len(sb_names)
len_gps = len(gps_names)+2
len_battery = len(battery_names)
len_fsm = len(fsm_names)

flight_phase = {0: '---------',
                1: 'IDLE',
                2: 'AIRBRAKE TEST',
                3: 'THRUSTING',
                4: 'COASTING',
                5: 'DESCENT',
                6: 'RECOVERY'}

mach_regime = {0: '----------',
               1: 'SUBSONIC',
               2: 'TRANSONIC',
               3: 'SUPERSONIC'}

velocity_data = [0]
height_data = [0]

fig_speed = Figure(figsize=(4, 4), dpi=100)
sub_speed = fig_speed.add_subplot(111)

fig_height = Figure(figsize=(4, 4), dpi=100)
sub_height = fig_height.add_subplot(111)

sub_speed.plot([2]*10)
sub_height.plot([3]*10)


def animate():
    global velocity_data
    global height_data
    while True:
        sub_speed.clear()
        sub_height.clear()

        # velocity_data = [3]*10
        # height_data = [4]*10
        print('update')
        print(velocity_data)
        velocity_data = velocity_data + [1]
        height_data = height_data + [-1]
        sub_speed.plot(velocity_data)
        sub_height.plot(height_data)
        fig_height.canvas.draw()
        fig_speed.canvas.draw()
        plt.show()
        time.sleep(1)


# def animation_thread():
#    ani1 = animation.FuncAnimation(fig_speed, animate, interval=10)


class MainWindow(Frame):

    def __init__(self, parent: Tk, gs_manager, *args, **kwargs):

        super(MainWindow, self).__init__(parent, *args, **kwargs)
        self._gs_manager = gs_manager
        self.logger = logging.getLogger()

        parent.protocol('WM_DELETE_WINDOW', self.__on_close__)

        self.s = None
        self.connected = False
        self.recording = False

        self._root = parent
        self._root.title("ARIS Groundstation")
        if sys.platform.startswith('win'):
            self._root.state("zoomed")
            self._root.wm_iconbitmap("aris.ico")
        else:
            self._root.state('normal')


        self._root.grid_rowconfigure(1, weight=1)
        self._root.grid_columnconfigure(1, weight=1)

        self.thread1 = threading.Thread(target=animate)
        self.__setup__()


        # ani2 = animation.FuncAnimation(fig_height, animate, interval=10)



    def __setup__(self):

        # ==============================================================================================================
        # Menu Bar
        # ==============================================================================================================
        self.menubar = tk.Menu(self._root)
        self.filemenu = tk.Menu(self.menubar, tearoff=0)

        self.filemenu.add_command(label="New", command=self.donothing)
        self.filemenu.add_command(label="Open", command=self.donothing)
        self.filemenu.add_command(label="Save", command=self.donothing)
        self.filemenu.add_command(label="Save as...", command=self.donothing)

        self.filemenu.add_separator()

        self.filemenu.add_command(label="Exit", command=self._root.quit)
        self.menubar.add_cascade(label="File", menu=self.filemenu)

        self.connectionmenu = tk.Menu(self.menubar, tearoff=0)
        self.connectionmenu.add_command(label="Serial connection", command=self.connection_window)
        self.connectionmenu.add_command(label='Start reception', command=self.start_reception)
        self.connectionmenu.add_command(label='Stop reception', command=self.stop_reception)

        self.menubar.add_cascade(label="Connection", menu=self.connectionmenu)

        # ==============================================================================================================
        # create subframes
        # ==============================================================================================================
        self.frame_upper = tk.Frame(self._root, width=500, height=100)
        self.frame_lower = tk.Frame(self._root, width=150, height=100)
        self.frame_upper_left = tk.LabelFrame(self.frame_upper, text='Commands', font=6, width=10, height=100)
        self.frame_upper_middle = tk.LabelFrame(self.frame_upper, text='Velocity', font=6, width=500, height=100)
        self.frame_upper_right = tk.LabelFrame(self.frame_upper, text='Altitude', font=6, width=500, height=100)
        self.frame_log = tk.LabelFrame(self.frame_upper_left, text='Log', width=10, height=20)

        # ==============================================================================================================
        # create plots
        # ==============================================================================================================
        # self.fig1 = Figure(figsize=(5, 5), dpi=100)
        # self.sub = self.fig1.add_subplot(111)
        # self.sub.plot(self.velocity_list)

        # canvas_left = FigureCanvasTkAgg(fig_speed, self.frame_upper_middle)
        # canvas_left.draw()
        # canvas_left.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)
        #
        # toolbar_left = NavigationToolbar2Tk(canvas_left, self.frame_upper_middle)
        # toolbar_left.update()
        # canvas_left._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True)
        #
        # canvas_right = FigureCanvasTkAgg(fig_height, self.frame_upper_right)
        # canvas_right.draw()
        # canvas_right.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)
        #
        # toolbar_right = NavigationToolbar2Tk(canvas_right, self.frame_upper_right)
        # toolbar_right.update()
        # canvas_right._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        self.plot_velocity = PlotControl(self.frame_upper_middle, add_toolbar=True, figsize=(5, 5), polling_time=.5)
        # self.plot_velocity.title = 'Velocity'
        self.plot_velocity.grid(row=0, column=0, sticky='nswe')

        self.plot_height = PlotControl(self.frame_upper_right, add_toolbar=True, figsize=(5, 5), polling_time=.5)
        self.plot_height.grid(row=0, column=0, sticky='nswe')

        # ==============================================================================================================
        # add separator lines
        # ==============================================================================================================
        self.sep1 = ttk.Separator(self._root, orient="horizontal")
        self.sep2 = ttk.Separator(self.frame_lower, orient="vertical")

        # ==============================================================================================================
        # add frame for recording
        # ==============================================================================================================
        self.frame_recording = tk.LabelFrame(self.frame_upper_left, text='Recording', width=40, height=10)
        self.label_file_name = tk.Label(self.frame_recording, text='file name')
        self.button_start_rec = tk.Button(self.frame_recording, text='Start', font=4, command=self.start_recording)
        self.button_stop_rec = tk.Button(self.frame_recording, text='Stop', font=4)
        self.entry_name = tk.Entry(self.frame_recording)

        # ==============================================================================================================
        # add frame for commands
        # ==============================================================================================================
        self.frame_command = tk.LabelFrame(self.frame_upper_left, text='Commands', width=40, height=10)
        self.button_sensor = tk.Button(self.frame_command,
                                       text='Sensor calibration',
                                       command=lambda: self.send_command('sensor'))
        self.button_airbrake = tk.Button(self.frame_command,
                                         text='Airbrake test',
                                         command=lambda: self.send_command('airbrake'))
        self.button_sf = tk.Button(self.frame_command,
                                   text='Sampling frequency',
                                   command=lambda: self.send_command('frequency'))

        # ==============================================================================================================
        # add sensorboard frame
        # ==============================================================================================================
        self.frame_sb = tk.LabelFrame(self.frame_lower, text='Sensorboard data', font=6, width=250, height=75)

        self.label_sb_val = []
        for i in range(len(sb_names)):
            self.label_sb_val.append(tk.Label(self.frame_sb,
                                              text="--------",
                                              borderwidth=2,
                                              relief="sunken",
                                              width=10))

        self.label_sb_name = []
        for i in range(len(sb_names)):
            self.label_sb_name.append(tk.Label(self.frame_sb, text=sb_names[i]))

        self.sep1_sb = ttk.Separator(self.frame_sb, orient='vertical')
        self.sep2_sb = ttk.Separator(self.frame_sb, orient='vertical')

        # ==============================================================================================================
        # add GPS frame
        # ==============================================================================================================
        self.frame_gps = tk.LabelFrame(self.frame_lower, text='GPS', font=6, width=200, height=75)
        self.label_gps_val = []
        self.label_gps_name = []
        for i in range(len(gps_names)):
            self.label_gps_val.append(tk.Label(self.frame_gps,
                                               text='--------',
                                               borderwidth=2,
                                               relief='sunken',
                                               width=10))

            self.label_gps_name.append(tk.Label(self.frame_gps, text=gps_names[i]))

        self.sep_gps = ttk.Separator(self.frame_gps, orient='vertical')

        self.label_gps_name[0].grid(row=1, column=0, sticky='W')
        self.label_gps_name[1].grid(row=2, column=0, sticky='W')
        self.label_gps_name[2].grid(row=1, column=3, sticky='W')
        self.label_gps_name[3].grid(row=2, column=3, sticky='W')
        self.label_gps_name[4].grid(row=0, column=4)
        self.label_gps_name[5].grid(row=0, column=5)

        self.sep_gps.grid(row=0, column=2, rowspan=3, sticky='ns')

        self.label_gps_val[0].grid(row=1, column=1, padx=10)
        self.label_gps_val[1].grid(row=2, column=1, padx=10)
        self.label_gps_val[2].grid(row=1, column=4, padx=10)
        self.label_gps_val[3].grid(row=2, column=4, padx=10)
        self.label_gps_val[4].grid(row=1, column=5, padx=10)
        self.label_gps_val[5].grid(row=2, column=5, padx=10)

        # ==============================================================================================================
        # add battery monitoring frame
        # ==============================================================================================================
        self.frame_battery = tk.LabelFrame(self.frame_lower, text='Power', font=6, width=100, height=75)
        self.label_battery_val = []
        self.label_battery_names = []
        for i in range(len(battery_names)):
            self.label_battery_val.append(tk.Label(self.frame_battery,
                                                   text='--------',
                                                   borderwidth=2,
                                                   relief='sunken',
                                                   width=15))

            self.label_battery_names.append(tk.Label(self.frame_battery, text=battery_names[i]))

            if i == 0:
                self.label_battery_names[i].grid(row=i, column=0, padx=(20, 0), pady=(10, 0), sticky='W')
                self.label_battery_val[i].grid(row=i, column=1, padx=10, pady=(10, 0))
            else:
                self.label_battery_names[i].grid(row=i, column=0, padx=(20, 0), sticky='W')
                self.label_battery_val[i].grid(row=i, column=1, padx=10)

        # ==============================================================================================================
        # add FSM frame
        # ==============================================================================================================
        self.frame_fsm = tk.LabelFrame(self.frame_lower, text='FSM data', font=6, width=200, height=75)
        self.label_fsm_val = []
        self.label_fsm_names = []
        for i in range(len(fsm_names)):
            self.label_fsm_val.append(tk.Label(self.frame_fsm,
                                               text='--------',
                                               borderwidth=2,
                                               relief='sunken',
                                               width=15))

            self.label_fsm_names.append(tk.Label(self.frame_fsm, text=fsm_names[i]))

            if i == 0:
                self.label_fsm_names[i].grid(row=i, column=0, padx=(20, 0), pady=(10, 0), sticky='W')
                self.label_fsm_val[i].grid(row=i, column=1, padx=10, pady=(10, 0))
            else:
                self.label_fsm_names[i].grid(row=i, column=0, padx=(20, 0), sticky='W')
                self.label_fsm_val[i].grid(row=i, column=1, padx=10)

        # ==============================================================================================================
        # pack/grid all elements
        # ==============================================================================================================
        self.frame_upper.pack(side="top", fill="both", expand=True)
        self.sep1.pack(side="top", fill="both")
        self.frame_lower.pack(side="top", fill="both", expand=True)

        self.frame_upper_left.pack(side='left', fill='both', expand=True, pady=(0, 10))
        self.frame_upper_middle.pack(side='left', fill='both', expand=True, pady=(0, 10))
        self.frame_upper_right.pack(side='left', fill='both', expand=True, pady=(0, 10))

        self.frame_recording.pack(side='top', fill='both', expand=True)
        self.frame_command.pack(side='top', fill='both', expand=True)
        self.frame_log.pack(side='top', fill='both', expand=True)

        st = scrolledtext.ScrolledText(self.frame_log, state='disabled', width=40, height=20)
        st.configure(font='TkFixedFont')
        st.grid(row=0, column=0)

        self.button_airbrake.grid(row=0, column=0, padx=5)
        self.button_sensor.grid(row=0, column=1, padx=5)
        self.button_sf.grid(row=0, column=2, padx=5)

        self.logger.addHandler(LoggingHandler(st))

        self.frame_sb.pack(side="left", fill="both", expand=True, padx=5, pady=10)
        # self.sep2.pack(side="left", fill="both")
        self.frame_gps.pack(side="left", fill="both", expand=True, padx=5, pady=10)
        self.frame_battery.pack(side="left", fill="both", expand=True, padx=5, pady=10)
        self.frame_fsm.pack(side="left", fill="both", expand=True, padx=5, pady=10)

        self.label_file_name.grid(row=0, column=0, pady=20, padx=20)
        self.entry_name.grid(row=0, column=1, pady=20)
        self.button_start_rec.grid(row=1, column=0)
        self.button_stop_rec.grid(row=1, column=1)

        self.entry_name.delete(0, 'end')
        self.entry_name.insert(0, 'recording.csv')

        self.label_sb_name[0].grid(row=0, column=0)
        self.label_sb_name[1].grid(row=1, column=0)
        self.label_sb_name[2].grid(row=0, column=3)
        self.label_sb_name[3].grid(row=1, column=3)
        self.label_sb_name[4].grid(row=2, column=3)
        self.label_sb_name[5].grid(row=0, column=6)
        self.label_sb_name[6].grid(row=1, column=6)
        self.label_sb_name[7].grid(row=2, column=6)

        self.sep1_sb.grid(row=0, column=2, rowspan=3, sticky='ns')
        self.sep2_sb.grid(row=0, column=5, rowspan=3, sticky='ns')

        self.label_sb_val[0].grid(row=0, column=1, padx=10)
        self.label_sb_val[1].grid(row=1, column=1, padx=10)
        self.label_sb_val[2].grid(row=0, column=4, padx=10)
        self.label_sb_val[3].grid(row=1, column=4, padx=10)
        self.label_sb_val[4].grid(row=2, column=4, padx=10)
        self.label_sb_val[5].grid(row=0, column=7, padx=10)
        self.label_sb_val[6].grid(row=1, column=7, padx=10)
        self.label_sb_val[7].grid(row=2, column=7, padx=10)

        self._root.config(menu=self.menubar)

    def send_command(self, command):
        if self.s is not None:
            if command == 'airbrake':
                answer = messagebox.askquestion('Warning', 'Airbrakes will extend. Make sure travel'
                                                           'range is cleared. Do you want to continue?')
                if answer == 'yes':
                    self.s.send(command)
                else:
                    self.logger.info('Airbrake test aborted.')
            else:
                self.s.send(command)
        else:
            messagebox.showinfo('Info', 'Serial connection is not established.')

    def __on_close__(self):
        if self.s is not None:
            self.s.close()
        self.connected = False
        self._root.destroy()

    def start_reception(self):
        if self.connected:
            self.s.read_serial_start()
        else:
            messagebox.showinfo('Info', 'Serial connection is not established.')

    def stop_reception(self):
        if self.connected:
            self.s.isRun = False
            self.logger.info('Stopped receiving data')

    def start_recording(self):
        self.file_name = self.entry_name.get()
        if not self.connected:
            messagebox.showinfo('Warning', 'Serial connection is not established yet. Cannot start recording.')
        elif self.file_name[-4:] != '.csv':
            messagebox.showinfo('Warning', 'Invalid filename. Needs to end on .csv')
        elif os.path.exists(self.file_name):
            answer = messagebox.askquestion('Warning', 'File already exists. Continue overwriting?')
            if answer == 'yes':
                with open(self.file_name, 'w') as outfile:
                    writer = csv.writer(outfile)
                    writer.writerow(get_measurement_names())
                self.recording = True
        else:
            with open(self.file_name, 'a') as outfile:
                writer = csv.writer(outfile)
                writer.writerow(get_measurement_names())
            self.recording = True

    def stop_recording(self):
        self.recording = False

    def donothing(self):
        filewin = tk.Toplevel(self._root)
        button = tk.Button(filewin, text="Do nothing button")
        button.pack()

    def connection_window(self):
        self.root2 = tk.Toplevel(self._root)
        self.root2.title('Xbee serial connection setting')
        self.root2.geometry('{}x{}'.format(400, 200))

        self.frame1 = tk.LabelFrame(self.root2, text='Settings')
        self.frame2 = tk.LabelFrame(self.root2, text='Available ports')

        self.label_available_ports = tk.Label(self.frame2, text=available_ports())

        self.label_port = tk.Label(self.frame1, text='Port')
        self.label_baud = tk.Label(self.frame1, text='Baud rate')

        self.entry1 = tk.Entry(self.frame1)
        self.entry2 = tk.Entry(self.frame1)

        self.button_connect = tk.Button(self.frame1, text='Connect', command=self.connect_xbee)

        self.frame1.pack(side="left", fill="both", expand=True, padx=5, pady=5)
        self.frame2.pack(side="left", fill="both", expand=True, padx=5, pady=5)

        self.label_available_ports.grid(row=0, column=0)

        self.label_port.grid(row=0, column=0, padx=(10, 0))
        self.label_baud.grid(row=1, column=0, padx=(10, 0))
        self.entry1.grid(row=0, column=1)
        self.entry2.grid(row=1, column=1)
        self.button_connect.grid(row=3, column=0, columnspan=2, pady=10)

        self.entry1.delete(0, 'end')
        if sys.platform.startswith('win'):
            self.entry1.insert(0, 'COM6')
        else:
            self.entry1.insert(0, '/dev/ttyUSB0')
        self.entry2.delete(0, 'end')
        self.entry2.insert(0, 115200)

    def connect_xbee(self):
        port_name = self.entry1.get()
        baud_rate = int(self.entry2.get())
        self.s = SerialConnection(self, port_name, baud_rate)
        if not self.s.start_connection():
            messagebox.showerror('Error', "Could not establish serial connection.")
        else:
            self.connected = True
            self.root2.destroy()

    def update_values(self, data):
        # print(len(data))
        # print(data)
        if data == 0:
            for i in range(len(self.label_sb_val)):
                self.label_sb_val[i].config(text='-----')

            for i in range(len(self.label_gps_val)):
                self.label_gps_val[i].config(text='-----')

            for i in range(len(self.label_battery_val)):
                self.label_battery_val[i].config(text='-----')

            for i in range(len(self.label_fsm_val)):
                self.label_fsm_val[i].config(text='-----')
        else:
            sb_data = data[:len_sb]
            battery_data = data[len_sb:len_sb + len_battery]
            gps_data = data[len_sb + len_battery:len_sb + len_battery + len_gps]
            fsm_data = data[len_sb+len_gps+len_battery:len_sb+len_gps+len_battery+len_fsm]

            gps_time = str(gps_data[0])+':'+str(gps_data[1])+':'+str(gps_data[2])
            tmp = [0]*(len_gps-2)
            tmp[0] = gps_time
            tmp[1:] = gps_data[3:]
            gps_data = tmp

            for i in range(len(self.label_sb_val)):
                self.label_sb_val[i].config(text=sb_data[i])

            for i in range(len(self.label_gps_val)):
                self.label_gps_val[i].config(text=gps_data[i])

            for i in range(len(self.label_battery_val)):
                self.label_battery_val[i].config(text=battery_data[i])

            for i in range(len(self.label_fsm_val)):
                if i != 3:
                    self.label_fsm_val[i].config(text=fsm_data[i])

            # print(fsm_data)
            if fsm_data[3] in range(len(flight_phase)):
                self.label_fsm_val[3].config(text=flight_phase[fsm_data[3]])
            else:
                self.label_fsm_val[3].config(text=fsm_data[3])

            if self.recording:
                with open(self.file_name, 'a') as outfile:
                    writer = csv.writer(outfile)
                    writer.writerow(data[:40])

            height_data.append(fsm_data[0])
            velocity_data.append(fsm_data[1])

