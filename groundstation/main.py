#!/home/imre/anaconda3/bin/python

from tkinter import Tk
from GroundStation import GroundStation
import logging

if __name__ == '__main__':

    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    # create file handler which logs even debug messages
    fh = logging.FileHandler('Logs/Groundstation.log')
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
    gs = GroundStation(root)
    gs.main_window.mainloop()

    logger.info('==========================================================================')
    logger.info('                        ARIS Groundstation closed                         ')
    logger.info('==========================================================================')
