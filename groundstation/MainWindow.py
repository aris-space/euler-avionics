#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

import tkinter as tk
from tkinter import ttk, messagebox, Tk, Frame, scrolledtext, Canvas
from SerialConnection import SerialConnection, get_measurement_names, available_ports
import sys
import csv
import os.path
import matplotlib
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
from matplotlib import style
import logging
from Logs.LoggingHandler import LoggingHandler
from myutils import sb_names, gps_names, battery_names, fsm_names

matplotlib.use("TkAgg")
style.use('ggplot')

len_sb = len(sb_names)
len_gps = len(gps_names) + 2
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
altitude_data = [0]

fig_velocity = Figure(figsize=(4, 4), dpi=100)
sub_velocity = fig_velocity.add_subplot(111)

fig_altitude = Figure(figsize=(4, 4), dpi=100)
sub_altitude = fig_altitude.add_subplot(111)

sub_velocity.plot(velocity_data)
sub_altitude.plot(altitude_data)


class MainWindow(Frame):
    """
    MainWindow is the main window of the application.
    """

    def __init__(self, parent: Tk, gs_manager, *args, **kwargs):
        """
        Constructor.

        Parameters
        ----------
        parent : Tk
            Tkinter parent
        gs_manager : GroundStation
            Instance of current GroundStation
        *args
            Tkinter frame arguments, forwarded to superclass
        **kwargs
            Tkinter frame argument names, forwarded to superclass
        """
        super(MainWindow, self).__init__(parent, *args, **kwargs)
        self._gs_manager = gs_manager
        self.logger = logging.getLogger()

        parent.protocol('WM_DELETE_WINDOW', self.__on_close__)

        self.s = None
        self.connected = False
        self.recording = False
        self.is_receiving = False
        self.update_plot = False

        self._root = parent
        self._root.title("ARIS Groundstation")
        if sys.platform.startswith('win'):
            self._root.state("zoomed")
            self._root.wm_iconbitmap("aris.ico")
        else:
            self._root.state('normal')

        self._root.grid_rowconfigure(1, weight=1)
        self._root.grid_columnconfigure(1, weight=1)

        self.current_velocity = 0
        self.current_altitude = 0

        self.__setup__()

    def __setup__(self):
        """
        Setup main window
        """
        # ==============================================================================================================
        # Menu Bar
        # ==============================================================================================================
        self.menubar = tk.Menu(self._root)
        self._root.config(menu=self.menubar)

        self.file_menu = tk.Menu(self.menubar, tearoff=0)
        self.connection_menu = tk.Menu(self.menubar, tearoff=0)
        self.plot_menu = tk.Menu(self.menubar, tearoff=0)

        self.menubar.add_cascade(label="File", menu=self.file_menu)
        self.menubar.add_cascade(label="Connection", menu=self.connection_menu)
        self.menubar.add_cascade(label='Live plot', menu=self.plot_menu)

        self.file_menu.add_command(label="New", command=self.donothing)
        self.file_menu.add_command(label="Open", command=self.donothing)
        self.file_menu.add_command(label="Save", command=self.donothing)
        self.file_menu.add_command(label="Save as...", command=self.donothing)
        self.file_menu.add_separator()
        self.file_menu.add_command(label="Exit", command=self._root.quit)

        self.connection_menu.add_command(label="Serial connection", command=self.connection_window)
        self.connection_menu.add_command(label='Start reception', command=self.start_reception)
        self.connection_menu.add_command(label='Stop reception', command=self.stop_reception)

        # self.plotmenu.add_command(label='Settings', command=self.plot_setting_window)
        self.plot_menu.add_command(label='Start live plot', command=self.start_plot)
        self.plot_menu.add_command(label='Stop live plot', command=self.stop_plot)

        # ==============================================================================================================
        # create subframes
        # ==============================================================================================================
        self.frame_upper = tk.Frame(self._root, width=500, height=100)
        self.frame_lower = tk.Frame(self._root, width=150, height=100)
        self.frame_upper_left = tk.LabelFrame(self.frame_upper, text='Commands', font=6, width=10, height=100)
        self.frame_upper_middle = tk.LabelFrame(self.frame_upper, text='Velocity [m/s]', font=6, width=500, height=100)
        self.frame_upper_right = tk.LabelFrame(self.frame_upper, text='Altitude [m]', font=6, width=500, height=100)
        self.frame_log = tk.LabelFrame(self.frame_upper_left, text='Log', width=10, height=20)

        # ==============================================================================================================
        # create plots
        # ==============================================================================================================
        canvas_left = Canvas(self.frame_upper_middle, width=80, height=80)
        canvas_left.pack(side='left', fill='both', expand=True)

        self.canvas_left = FigureCanvasTkAgg(fig_velocity, canvas_left)
        self.canvas_left.draw()
        self.canvas_left.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

        canvas_right = Canvas(self.frame_upper_right, width=80, height=80)
        canvas_right.pack(side='left', fill='both', expand=True)

        self.canvas_right = FigureCanvasTkAgg(fig_altitude, canvas_right)
        self.canvas_right.draw()
        self.canvas_right.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

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
        self.button_stop_rec = tk.Button(self.frame_recording, text='Stop', font=4, command=self.stop_recording)
        self.entry_name = tk.Entry(self.frame_recording)

        # ==============================================================================================================
        # add frame for commands
        # ==============================================================================================================
        self.frame_command = tk.LabelFrame(self.frame_upper_left, text='Commands', width=40, height=10)

        self.button_sensor = tk.Button(self.frame_command,
                                       text='EST Reset',
                                       width=10,
                                       command=lambda: self.send_command('sensor'))
        self.button_airbrake = tk.Button(self.frame_command,
                                         text='Airbrake Test',
                                         width=10,
                                         command=lambda: self.send_command('airbrake'))
        self.button_sf = tk.Button(self.frame_command,
                                   text='High Sampling',
                                   width=10,
                                   command=lambda: self.send_command('frequency'))
        self.button_buzzer = tk.Button(self.frame_command,
                                       text='Buzzer',
                                       width=10,
                                       command=lambda: self.send_command('buzzer'))
        self.button_disable = tk.Button(self.frame_command,
                                        text='Disable Lock',
                                        width=10,
                                        command=lambda: self.send_command('disable'))

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
        # self.sep2_sb = ttk.Separator(self.frame_sb, orient='vertical')

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
        self.label_gps_name[6].grid(row=3, column=0, sticky='W')
        self.label_gps_name[7].grid(row=4, column=0, sticky='W')
        self.label_gps_name[8].grid(row=5, column=0, sticky='W')

        self.sep_gps.grid(row=1, column=2, rowspan=5, sticky='ns')

        self.label_gps_val[0].grid(row=1, column=1, padx=10)
        self.label_gps_val[1].grid(row=2, column=1, padx=10)
        self.label_gps_val[2].grid(row=1, column=4, padx=10)
        self.label_gps_val[3].grid(row=2, column=4, padx=10)
        self.label_gps_val[4].grid(row=1, column=5, padx=10)
        self.label_gps_val[5].grid(row=2, column=5, padx=10)
        self.label_gps_val[6].grid(row=3, column=1, padx=10)
        self.label_gps_val[7].grid(row=4, column=1, padx=10)
        self.label_gps_val[8].grid(row=5, column=1, padx=10)

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
        self.button_buzzer.grid(row=1, column=0, padx=5)
        self.button_disable.grid(row=1, column=1, padx=5)

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

        self.label_sb_name[0].grid(row=0, column=0, pady=(10, 0), sticky='W')
        self.label_sb_name[1].grid(row=1, column=0, sticky='W')
        self.label_sb_name[2].grid(row=0, column=3, pady=(10, 0), sticky='W')
        self.label_sb_name[3].grid(row=1, column=3, sticky='W')
        self.label_sb_name[4].grid(row=2, column=3, sticky='W')
        self.label_sb_name[5].grid(row=3, column=3, sticky='W')
        self.label_sb_name[6].grid(row=4, column=3, sticky='W')
        self.label_sb_name[7].grid(row=5, column=3, sticky='W')

        self.sep1_sb.grid(row=0, column=2, rowspan=6, sticky='ns')
        # self.sep2_sb.grid(row=0, column=5, rowspan=3, sticky='ns')

        self.label_sb_val[0].grid(row=0, column=1, padx=10, pady=(10, 0))
        self.label_sb_val[1].grid(row=1, column=1, padx=10)
        self.label_sb_val[2].grid(row=0, column=4, padx=10, pady=(10, 0))
        self.label_sb_val[3].grid(row=1, column=4, padx=10)
        self.label_sb_val[4].grid(row=2, column=4, padx=10)
        self.label_sb_val[5].grid(row=3, column=4, padx=10)
        self.label_sb_val[6].grid(row=4, column=4, padx=10)
        self.label_sb_val[7].grid(row=5, column=4, padx=10)

    def send_command(self, command):
        """
        Called when user presses one of the control buttons

        Parameters
        ----------
        command : str
            needs to be one of the following commands:
            sensor, airbrake, frequency
        """
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
        """
        Called when user closes the application.
        """
        if self.s is not None:
            self.s.close()
        self.connected = False
        self.update_plot = False
        self._root.destroy()

    def start_reception(self):
        """
        Starts reading data from serial line if the serial connection is established.
        """
        if self.connected:
            self.s.read_serial_start()
        else:
            messagebox.showinfo('Info', 'Serial connection is not established.')

    def stop_reception(self):
        """
        Stops reading data from serial line. Stops also live plot and recording.
        """
        if self.connected:
            self.s.isRun = False
            if self.update_plot:
                self.update_plot = False
                self.logger.info('Live plot stopped.')
            if self.recording:
                self.recording = False
                self.logger.info('Recording stopped.')
            self.logger.info('Stopped receiving data')

    def start_recording(self):
        """
        Starts recording data to csv file.
        """
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
                    writer.writerow(list(get_measurement_names())[1:])
                self.recording = True
                self.logger.info('Started recording.')
        else:
            with open(self.file_name, 'a') as outfile:
                writer = csv.writer(outfile)
                writer.writerow(list(get_measurement_names())[1:])
            self.recording = True
            self.logger.info('Started recording.')

    def stop_recording(self):
        """
        Stops recording data.
        """
        self.recording = False
        self.logger.info('Stopped recording.')

    def donothing(self):
        filewin = tk.Toplevel(self._root)
        button = tk.Button(filewin, text="Do nothing button")
        button.pack()

    def start_plot(self):
        """
        Starts live plot.
        """
        if self.is_receiving:
            self.update_plot = True
            self.update_canvas()
            self.logger.info('Live plot started')

    def stop_plot(self):
        """
        Stops live plot.
        """
        self.update_plot = False
        self.logger.info('Live plot stopped.')

    def update_canvas(self):
        """
        Updates canvas which contains the live plots.
        """
        velocity_data.append(self.current_velocity)
        altitude_data.append(self.current_altitude)
        sub_velocity.clear()
        sub_altitude.clear()
        sub_velocity.plot(velocity_data)
        sub_altitude.plot(altitude_data)
        self.canvas_left.draw()
        self.canvas_right.draw()
        if self.update_plot:
            self.frame_upper_middle.after(100, self.update_canvas)

    def connection_window(self):
        """
        Opens the serial connection settings window.
        """
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
        """
        Tries to establish serial connection.
        """
        port_name = self.entry1.get()
        baud_rate = int(self.entry2.get())
        self.s = SerialConnection(self, port_name, baud_rate)
        if not self.s.start_connection():
            messagebox.showerror('Error', "Could not establish serial connection.")
        else:
            self.connected = True
            self.root2.destroy()

    def update_values(self, data):
        """
        Updates the values in the main window and the data for live plots.

        Parameters
        ----------
        data : list
            contains the newest data received from the Xbee module.
        """
        # print(len(data))
        # print(data)
        self.is_receiving = True
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
            data = list(data[1:])
            sb_data = data[:len_sb]
            battery_data = data[len_sb:len_sb + len_battery]
            gps_data = data[len_sb + len_battery:len_sb + len_battery + len_gps]
            fsm_data = data[len_sb + len_gps + len_battery:len_sb + len_gps + len_battery + len_fsm]

            gps_time = str(gps_data[0]+2) + ':' + str(gps_data[1]) + ':' + str(gps_data[2])
            tmp = [0] * (len_gps - 2)
            tmp[0] = gps_time
            tmp[1:] = gps_data[3:]
            gps_data = tmp

            # sb data scaling
            sb_data[1] = sb_data[1]/100
            sb_data[2] = sb_data[2]/32.8
            sb_data[3] = sb_data[3]/32.8
            sb_data[4] = sb_data[4]/32.8
            sb_data[5] = sb_data[5]/1024*9.81
            sb_data[6] = sb_data[6] / 1024 * 9.81
            sb_data[7] = sb_data[7] / 1024 * 9.81


            # fsm data scaling
            fsm_data[0] = fsm_data[0]/1000
            fsm_data[1] = fsm_data[1]/1000
            fsm_data[2] = fsm_data[2]/10
            fsm_data[4] = fsm_data[4]/1000

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
                    writer.writerow(data[:-3])

            self.current_altitude = fsm_data[0]
            self.current_velocity = fsm_data[1]
