@echo off
REM Clear the runway!
rm -rf build
rm -rf dist

REM Run py2exe to compile all the python scripts into an executable.
setup.py py2exe

REM Create the windows installer
howie.nsi