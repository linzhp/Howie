"""
A parent class for all of Howie's backend modules.
"""

class IBackEnd:
	def __init__(self):
		pass
	
	def respondTo(self, input):
		"""
		Returns an appropriate response to the given input, or None if
		nothing comes to mind.
		"""
		return None