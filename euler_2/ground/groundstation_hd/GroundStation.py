#!/usr/bin/env python3

"""
Author(s): Imre Kertesz
"""

import logging
from MainWindow import MainWindow


class GroundStation:
    """GroundStation is called upon start of the application by
        main.py and will remain alive until the program quits

        Attributes
        ----------
        main_window : Frame
            Tkinter main Frame.
        logger : logging
            for logging
        """
    def __init__(self, root):
        self.logger = logging.getLogger()
        self._root = root

        self.main_window = MainWindow(self._root, self)
        self.logger.info('Initialised Groundstation')

        self.about_text = "ARIS Ground-station Laptop interface\n" \
                          "Author: Imre Kertesz\n" \
                          "Version: 2.0\n" \
                          "Git: https://github.com/aris-space/aris-euler-avionics\n\n" \
                          "Avionics team:\n" \
                          "- Jonas Binz\n" \
                          "- Luca Jost\n" \
                          "- Imre Kertesz\n" \
                          "- Nemanja Stojoski"
