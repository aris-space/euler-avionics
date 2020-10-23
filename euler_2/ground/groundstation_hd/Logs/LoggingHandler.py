import logging
import tkinter as tk


class LoggingHandler(logging.Handler):

    def __init__(self, text_frame):
        logging.Handler.__init__(self)
        self.text_frame = text_frame
        self.setLevel('INFO')
        formatter = logging.Formatter('%(filename)s:%(funcName)s:\n%(message)s')
        self.setFormatter(formatter)
        self.counter = 0

    def emit(self, record):
        msg = self.format(record)
        msg = msg.replace('\n', '\n ')

        try:
            self.text_frame.configure(state='normal')

            if self.counter > 500:
                self.text_frame.delete('1.0', '3.0')
                self.counter -= 1

            self.text_frame.insert(tk.END, msg + '\n')
            self.counter += 1
            self.text_frame.configure(state='disabled')

            self.text_frame.yview(tk.END)
        except tk.TclError:
            pass
