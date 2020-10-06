#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

from tkinter import Tk
from GroundStation import GroundStation
import logging
from datetime import datetime

if __name__ == '__main__':

    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)

    now = datetime.now()
    dt_string = now.strftime("%d-%m-%Y_%H_%M")
    # create file handler which logs even debug messages
    fh = logging.FileHandler('Logs/' + dt_string + '_Groundstation.log')
    fh.setLevel(logging.INFO)
    # create console handler with a higher log level
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    # create formatter and add it to the handlers
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    # add the handlers to the logger
    logger.addHandler(fh)
    logger.addHandler(ch)

    logger.info('==========================================================================')
    logger.info('                        ARIS Groundstation started                        ')
    logger.info('==========================================================================')

    root = Tk()
    # root.tk.call('tk', 'scaling', .2)
    gs = GroundStation(root)
    gs.main_window.mainloop()

    logger.info('==========================================================================')
    logger.info('                        ARIS Groundstation closed                         ')
    logger.info('==========================================================================')
