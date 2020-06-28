#!/usr/bin/env python

import logging
from MainWindow import MainWindow


class GroundStation:

    def __init__(self, root):
        self.logger = logging.getLogger()
        self._root = root

        self.main_window = MainWindow(self._root, self)
        self.logger.info('Initialised Groundstation')