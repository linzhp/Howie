# Command to build:
from distutils.core import setup
import glob
import py2exe

setup(name="Howie",
    version="0.6.0",
    author="Cort Stratton",
    author_email="cort@users.sourceforge.net",
    description="Howie The Chatterbot",
    url="http://howie.sourceforge.net/",
    console=["runme.py"],
    data_files=[
        #("scripts", glob.glob("scripts\\*.py")),
        ("standard", glob.glob("standard\\std-*.aiml")),
        ("standard", glob.glob("standard\\dev-*.aiml")),
        (".", ("std-startup.xml", "README.txt", "CHANGES.txt", "my-howie.aiml", "howie.ini")),	  
    ],
)
