# Command to build:
from distutils.core import setup
import glob
import py2exe

setup(name="Howie",
	author="Cort Stratton",
	author_email="cort@users.sourceforge.net",
	description="Howie The Chatterbot",
	url="http://howie.sourceforge.net/",
	scripts=["runme.py"],
	data_files=[
		("standard", glob.glob("standard\\*.aiml")),
		(".", ("j-alice.ini", "std-startup.xml", "README.txt", "substitutions.xml", "howie.ini")),

		# This is a HACK.  Currently, the code in core.py requires that these
		# actual source files exist in order for the frontends to work.
		("howie", glob.glob("howie\\*.py")),
		("howie\\frontends", glob.glob("howie\\frontends\\*.py")),
		("howie\\backends", glob.glob("howie\\backends\\*.py")),		  
	],
)
