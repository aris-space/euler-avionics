#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""


from tkinter import Tk, Frame, Label, Entry, LabelFrame, Button, messagebox
from SerialConnection import available_ports, SerialConnection
import sys


class ConnectionWindow(Frame):
    def __init__(self, parent: Tk, main):
        super(ConnectionWindow, self).__init__(parent)
        self._root = parent

        self._main = main

        w = 400
        h = 200
        # get screen width and height
        ws = self._root.winfo_screenwidth()  # width of the screen
        hs = self._root.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        self._root.title('Serial connection setting')
        self._root.geometry('%dx%d+%d+%d' % (w, h, x, y))
        self._root.resizable(height=False, width=False)

        parent.protocol("WM_DELETE_WINDOW", self.__on_close__)
        self.__setup__()

    def __on_close__(self):
        self._root.destroy()

    def __setup__(self):
        frame1 = LabelFrame(self._root, text='Settings')
        frame2 = LabelFrame(self._root, text='Available ports')

        label_available_ports = Label(frame2, text=available_ports())

        label_port = Label(frame1, text='Port')
        label_baud = Label(frame1, text='Baud rate')

        self.entry1 = Entry(frame1)
        self.entry2 = Entry(frame1)

        button_connect = Button(frame1, text='Connect', command=self.connect_xbee)

        frame1.pack(side="left", fill="both", expand=True, padx=5, pady=5)
        frame2.pack(side="left", fill="both", expand=True, padx=5, pady=5)

        label_available_ports.grid(row=0, column=0)

        label_port.grid(row=0, column=0, padx=(10, 0))
        label_baud.grid(row=1, column=0, padx=(10, 0))
        self.entry1.grid(row=0, column=1)
        self.entry2.grid(row=1, column=1)
        button_connect.grid(row=3, column=0, columnspan=2, pady=10)

        self.entry1.delete(0, 'end')
        if sys.platform.startswith('win'):
            if self._main.s is not None:
                self.entry1.insert(0, self._main.s.port)
            else:
                self.entry1.insert(0, 'COM6')
        else:
            if self._main.s is not None:
                self.entry1.insert(0, self._main.s.port)
            else:
                self.entry1.insert(0, '/dev/ttyUSB0')
        self.entry2.delete(0, 'end')
        if self._main.s is not None:
            self.entry2.insert(0, self._main.s.baud)
        else:
            self.entry2.insert(0, 115200)

    def connect_xbee(self):
        """
        Tries to establish serial connection.
        """
        port_name = self.entry1.get()
        baud_rate = int(self.entry2.get())
        self._main.s = SerialConnection(self._main, port_name, baud_rate)
        if not self._main.s.start_connection():
            messagebox.showerror('Error', "Could not establish serial connection.")
            self._root.lift()
        else:
            self._main.connected = True
            self._root.destroy()
