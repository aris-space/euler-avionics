#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

from tkinter import Tk, Frame, Label, Button, Entry, LabelFrame, IntVar, Checkbutton, messagebox



class LargeWindow(Frame):

    def __init__(self, parent: Tk, main):
        super(LargeWindow, self).__init__(parent)
        self._root = parent
        self._root.tk.call('tk', 'scaling', 2.0)

        self._main = main
        w = 300
        h = 300
        # get screen width and height
        ws = self._root.winfo_screenwidth()  # width of the screen
        hs = self._root.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        self._root.title('Live View')
        self._root.geometry('%dx%d+%d+%d' % (w, h, x, y))
        self._root.resizable(height=False, width=False)

        parent.protocol("WM_DELETE_WINDOW", self.__on_close__)
        self.__setup__()

    def __on_close__(self):
        self._root.destroy()

    def __setup__(self):
        label1 = Label(self._root, text='test').pack()