#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

from tkinter import Tk, Frame, Label, Entry, LabelFrame, IntVar, Radiobutton, Button, messagebox


class ConfigureGPSWindow(Frame):
    def __init__(self, parent: Tk, main):
        super(ConfigureGPSWindow, self).__init__(parent)
        self._root = parent

        self._main = main

        w = 300
        h = 150
        # get screen width and height
        ws = self._root.winfo_screenwidth()  # width of the screen
        hs = self._root.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        self._root.title('GPS setting')
        self._root.geometry('%dx%d+%d+%d' % (w, h, x, y))
        self._root.resizable(height=False, width=False)

        parent.protocol("WM_DELETE_WINDOW", self.__on_close__)
        self.__setup__()

    def __on_close__(self):
        self._root.destroy()

    def __setup__(self):
        frame_timezone = LabelFrame(self._root, text='Timezone')
        frame_timezone.pack()

        label1 = Label(frame_timezone, text='UTC+')
        label1.grid(row=0, column=0, padx=(5, 0), pady=5)
        self.entry1 = Entry(frame_timezone)
        self.entry1.grid(row=0, column=1, padx=(0, 5), pady=5)
        self.entry1.delete(0, 'end')
        self.entry1.insert(0, str(self._main.time_offset))

        frame_orientation = LabelFrame(self._root, text='Orientation')
        frame_orientation.pack()

        self.switch_variable = IntVar(value=self._main.gps_sign)
        east_button = Radiobutton(frame_orientation,
                                  text='East',
                                  variable=self.switch_variable,
                                  value=True,
                                  width=8)
        west_button = Radiobutton(frame_orientation,
                                  text='West',
                                  variable=self.switch_variable,
                                  value=False,
                                  width=8)
        east_button.grid(row=0, column=0)
        west_button.grid(row=0, column=1)

        save_button = Button(self._root, text='Save', command=self.save_gps)
        save_button.pack()

    def save_gps(self):
        try:
            tmp = int(self.entry1.get())
            self._main.time_offset = tmp
            self._root.destroy()
        except ValueError as e:
            messagebox.showinfo('ValueError', e)
            self.entry1.delete(0, 'end')
            self.entry1.insert(0, str(self._main.time_offset))
            self._root.lift()
        self._main.gps_sign = self.switch_variable.get()
