from tkinter import Tk, Frame, Label
from tkinter.ttk import Separator
from myutils import button_text, dict_commands


class CommandSettingsWindow(Frame):
    def __init__(self, parent: Tk, main):
        super(CommandSettingsWindow, self).__init__(parent)
        self._root = parent

        self._main = main

        w = 300
        h = 250
        # get screen width and height
        ws = self._root.winfo_screenwidth()  # width of the screen
        hs = self._root.winfo_screenheight()  # height of the screen

        # calculate x and y coordinates for the Tk window
        x = (ws / 2) - (w / 2)
        y = (hs / 2) - (h / 2)

        self._root.title('Command settings')
        # root.geometry('{}x{}'.format(300, 250))
        self._root.geometry('%dx%d+%d+%d' % (w, h, x, y))
        self._root.resizable(height=False, width=False)

        frame1 = Frame(self._root)
        frame1.pack(fill="both", expand=True)

        label_buttons = Label(frame1, text='Button names')
        label_commands = Label(frame1, text='Transmitted data')
        label_buttons.grid(row=0, column=0, padx=10)
        label_commands.grid(row=0, column=2, padx=10)

        button_list = []
        for i in range(len(button_text)):
            button_list.append(Label(frame1,
                                     text=button_text[i],
                                     borderwidth=2,
                                     relief='sunken',
                                     width=15).grid(row=i + 2, column=0, pady=5, padx=15))

        command_list = []
        for i in range(len(dict_commands)):
            text = ''.join(format(x, '02X') for x in list(dict_commands.values())[i])
            command_list.append(Label(frame1,
                                      text='0x' + text).grid(row=i + 2, column=2, padx=10))

        sep1 = Separator(frame1, orient='vertical')
        sep1.grid(row=0, column=1, rowspan=9, sticky='ns', pady=(5, 0))

        sep2 = Separator(frame1, orient='horizontal')
        sep2.grid(row=1, column=0, columnspan=3, sticky='we', padx=5)
