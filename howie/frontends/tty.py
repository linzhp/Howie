import random
import sys
import time

import frontend

# This string needs to be defined for each front end.  It should
# contain the name of the front-end class defined in this module.
frontEndClass = "FrontEndTTY"

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
        self._sessionID = "localhost@TTY"
        import howie.core
        howie.core.kernel.setPredicate("secure", "yes", self._sessionID)
        while True:
            input = raw_input(">>> ")
            response = self.submit(input, self._sessionID)
            #time.sleep(random.random() * 4)
            self.display(response, self._sessionID)
    
    def display(self, output, user):
        print output