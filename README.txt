Howie - A Conversational AI Bot
(http://howie.sourceforge.net/)
(c) 2003 Cort Stratton (cort@andrew.cmu.edu)

ABOUT HOWIE
-----------
Howie is a conversational AI -- a computer program you can talk to.  He
is designed to be simple to install, configure and (above all) extend.

Howie is Friendly.
Currently, Howie is willing to discuss religion, sports, sex (reluctantly),
and philosophy.  He even knows a little German!

Howie is Helpful.
Howie is able to answer most questions of the form "Who/what/where/when
is/was/are/were X?" -- though his answers are occasionally more
entertaining than accurate.  He will provide definitions or synonyms for any
word in the dictionary.  He can even perform a few simple remote
system-administration tasks!

Howie is Connected.
You can communicate with Howie in a number of different ways: AIM,
MSN Messenger, IRC, or locally on the command-line.  His knowledge
base is made up of a wide variety of Internet resources.  If you find
something that Howie can't do, it's usually pretty straightforward to
teach him!

INSTALLATION
------------
1. Run the installer (you've done that).
2. Edit the howie.ini file, found in the directory where you installed Howie.
   This file lets you set up the interfaces presented to visitors (AIM, MSN,
   etc.) as well as customize Howie's behavior.  Each section has an "active"
   parameter, which indicates whether that module is active ("yes") or not.
3. Run the "Howie\Run Howie" shortcut, found in your start menu.

CHANGELOG
---------
11/19/2003: "Return Of The Howie" (Dev-20031119) is released
 - Updated Jalice to version 0.5.  See the Jalice homepage for summary
   of new features.
5/30/2003: "Finding Howie" (Dev-20030530) is released
 - Fixed Jalice bug which caused Howie to repeat his first response
   over and over.
 - Massive code reorganization makes adding new frontends a breeze!
 - Connection parameters are read from the new configuration file,
   howie.ini.
 - Added command-line interface (force command-line only with the -l flag).
5/15/2003: "Howie Reloaded" (Dev-20030515) is released
 - initial pre-pre-alpha version, Windows installer only.
 - AIM only


ACKNOWLEDGEMENTS
----------------
The Win32 installer was produced using the Nullsoft Install System
(http://www.nullsoft.com/free/nsis/).

Howie's conversational back-end is provided by J-Alice (http://j-alice.org/),
a C++ implementation of Dr. Richard S. Wallace's A.L.I.C.E. system.  For
more information about the A.L.I.C.E. Foundation (including information on
AIML, the Artificial Intelligence Markup Language used to program Howie's
responses), visit http://www.alicebot.org/.

Howie's AIM front-end is provided by Py-TOC (http://www.jamwt.com/Py-TOC/).

Howie is named after Howie Day (http://www.howieday.com/), for no particular
reason.  If it weren't for Amy Kalson, he would be named something truly boring
like PyBot or PyBorg.

Hooray for Python! (http://www.python.org/)