#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

from tkinter import Tk, Frame, Label, Button, Entry, LabelFrame, IntVar, Checkbutton, messagebox


class ConfigurePlotWindow(Frame):

    def __init__(self, parent: Tk, main):
        super(ConfigurePlotWindow, self).__init__(parent)
        self._root = parent

        self._main = main
        w = 300
        h = 300
        # get screen width and height
        ws = self._root.winfo_screenwidth()  # width of the screen
        hs = self._root.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        self._root.title('Plot setting')
        self._root.geometry('%dx%d+%d+%d' % (w, h, x, y))
        self._root.resizable(height=False, width=False)

        parent.protocol("WM_DELETE_WINDOW", self.__on_close__)
        self.__setup__()

    def __on_close__(self):
        self._root.destroy()

    def __setup__(self):
        frame_color = LabelFrame(self._root, text='Color settings')
        frame_target_apogee = LabelFrame(self._root, text='Target apogee')
        frame_velocity = LabelFrame(self._root, text='Velocity reference line')

        frame_color.pack(side="top", fill="both", expand=True, padx=5)
        frame_target_apogee.pack(side="top", fill="both", expand=True, padx=5)
        frame_velocity.pack(side="top", fill="both", expand=True, padx=5, pady=5)

        # ==============================================================================================================
        # Color settings
        # ==============================================================================================================
        label_inner_color = Label(frame_color, text='Inner color')
        label_outer_color = Label(frame_color, text='Outer color')
        label_line_color = Label(frame_color, text='Line color')
        label_rgb = Label(frame_color, text='hex RGB')

        label_inner_color.grid(row=1, column=0, padx=10)
        label_outer_color.grid(row=2, column=0, padx=10)
        label_line_color.grid(row=3, column=0, padx=10, pady=(0, 5))
        label_rgb.grid(row=0, column=1, padx=(0, 10))

        self.entry_inner_color = Entry(frame_color, width=10)
        self.entry_inner_color.grid(row=1, column=1, padx=(0, 10))
        self.entry_inner_color.delete(0, 'end')
        self.entry_inner_color.insert(0, self._main.inner_color)

        self.entry_outer_color = Entry(frame_color, width=10)
        self.entry_outer_color.grid(row=2, column=1, padx=(0, 10))
        self.entry_outer_color.delete(0, 'end')
        self.entry_outer_color.insert(0, self._main.outer_color)

        self.entry_line_color = Entry(frame_color, width=10)
        self.entry_line_color.grid(row=3, column=1, padx=(0, 10), pady=(0, 5))
        self.entry_line_color.delete(0, 'end')
        self.entry_line_color.insert(0, self._main.line_color)

        # ==============================================================================================================
        # Target apogee
        # ==============================================================================================================
        label_target_apogee = Label(frame_target_apogee, text='Target apogee [m]')
        label_target_apogee.grid(row=0, column=0, padx=10)
        label_show_line_apogee = Label(frame_target_apogee, text='Show line')
        label_show_line_apogee.grid(row=1, column=0, padx=10)

        self.entry_target_apogee = Entry(frame_target_apogee, width=10)
        self.entry_target_apogee.grid(row=0, column=1, padx=(0, 10))
        self.entry_target_apogee.delete(0, 'end')
        self.entry_target_apogee.insert(0, self._main.target_apogee)

        self.show_line_apogee = IntVar(value=self._main.show_line_apogee)
        show_apogee_button = Checkbutton(frame_target_apogee,
                                         variable=self.show_line_apogee,
                                         onvalue=True,
                                         offvalue=False)
        show_apogee_button.grid(row=1, column=1)
        if self.show_line_apogee.get():
            show_apogee_button.select()

        # ==============================================================================================================
        # Velocity reference line
        # ==============================================================================================================
        label_reference_velocity = Label(frame_velocity, text='Reference velocity [m/s]')
        label_reference_velocity.grid(row=0, column=0)
        label_show_line_vel = Label(frame_velocity, text='Show line')
        label_show_line_vel.grid(row=1, column=0, padx=10)

        self.entry_reference_velocity = Entry(frame_velocity, width=10)
        self.entry_reference_velocity.grid(row=0, column=1, padx=(0, 10))
        self.entry_reference_velocity.delete(0, 'end')
        self.entry_reference_velocity.insert(0, self._main.reference_velocity)

        self.show_line_velocity = IntVar(value=self._main.show_line_velocity)
        show_velocity_button = Checkbutton(frame_velocity,
                                           variable=self.show_line_velocity,
                                           onvalue=True,
                                           offvalue=False)
        show_velocity_button.grid(row=1, column=1)
        if self.show_line_velocity.get():
            show_velocity_button.select()

        # Save
        save_button = Button(self._root, text='Save', command=self.save)
        save_button.pack()

    def save(self):

        try:
            self._main.target_apogee = int(self.entry_target_apogee.get())
            self._main.show_line_apogee = self.show_line_apogee.get()
            self._main.reference_velocity = int(self.entry_reference_velocity.get())
            self._main.show_line_velocity = self.show_line_velocity.get()
            self._main.change_color(self.entry_inner_color.get(),
                                    self.entry_outer_color.get(),
                                    self.entry_line_color.get())
            self._root.destroy()
        except ValueError as e:
            messagebox.showinfo('ValueError', e)
            self.entry_target_apogee.delete(0, 'end')
            self.entry_target_apogee.insert(0, str(self._main.target_apogee))
            self._root.lift()
