#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

from tkinter import Tk, Frame, Label, CENTER
from PIL import ImageTk, Image
import os.path
from myutils import fontStyle_medium


class AboutWindow(Frame):
    def __init__(self, parent: Tk, main):
        super(AboutWindow, self).__init__(parent)
        self._root = parent

        self._main = main

        w = 300
        h = 500
        # get screen width and height
        ws = self._root.winfo_screenwidth()  # width of the screen
        hs = self._root.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        self._root.title('About')
        self._root.geometry('%dx%d+%d+%d' % (3*w, 3*h, x, y))
        self._root.resizable(height=False, width=False)

        cwd = os.path.dirname(os.path.abspath(__file__))

        frame1 = Frame(self._root)
        frame2 = Frame(self._root)

        frame1.pack(side="top", fill="both", expand=True)
        frame2.pack(side="bottom", fill="both", expand=True)

        img = Image.open(os.path.join(cwd, 'aris_logo.png'))
        img = img.resize((900, 300), Image.ANTIALIAS)
        img = ImageTk.PhotoImage(img)

        img2 = Image.open(os.path.join(cwd, 'euler_launch.jpg'))
        img2 = img2.resize((900, 1200), Image.ANTIALIAS)
        img2 = ImageTk.PhotoImage(img2)

        background_label = Label(frame2, image=img2,
                                 text=self._main._gs_manager.about_text,
                                 font=fontStyle_medium,
                                 compound=CENTER)
        background_label.image = img2
        background_label.configure(fg="white")
        background_label.pack()

        label_img = Label(frame1, image=img)
        label_img.configure(bg="white")
        label_img.image = img
        label_img.pack()
