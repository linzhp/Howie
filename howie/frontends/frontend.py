"""
Abstract base class upon which all front-ends are built.
"""

class IFrontEnd:
    def __init__(self):
        pass

    def go(self):
        print "OVERLOAD ME!"
        pass

    def submit(self, input, user):
        "Submits a line of input for Howie to process, from a particular user."
        # must delay this import until now to prevent circular references
        import howie.core
        return howie.core.submit(input, user)

    def display(self, output, user):
        "Displays output for the specified user."
        pass
    