import random
import sys
import time

import frontend

# List the default values for the INI file.  This should be a dictionary
# with keys of the form "<fe>.<entry>", where <fe> is the name of the
# frontend (e.g. "aim", "msn"), and entry is the name of the configuration
# variable (e.g. "username", "password").  All values should be strings.
#
# Each frontend must define an "active" entry, whose possible values
# are "yes" and "no", which indicates whether that frontend should
# be activated.
configDefaults = {
    "tty.active":       "yes"
    }	

class FrontEndTTY(frontend.IFrontEnd):
    """
    A butt-simple class demonstrating the bare minimum needed to
    implement a new front-end for Howie.
    """    
    def go(self):
        while True:
            input = raw_input(">>> ")
            response = self.submit(input, "localhost")
            #time.sleep(random.random() * 4)
            self.display(response, "localhost")
    
    def display(self, output, user):
        print output