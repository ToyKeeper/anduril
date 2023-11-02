#!/usr/bin/env python3
# Use with Python3, make sure to have the pymcuprog module installed (via pip)

# Read out the pymcuprog version
from pymcuprog.version import VERSION as pymcuprog_version
print("pymcuprog version {}".format(pymcuprog_version))


# List out the available ports
import serial.tools.list_ports as ls;
#ports = ls.comports()
ports = []
for p in ls.comports():
    if "COM" in p.device or "USB" in p.device:
        ports.append(p)

if len(ports) == 0:
    print("No serial ports found, exiting")
    exit()
elif len(ports) == 1:
    print("Found one serial port:", ports[0].device, "-", ports[0].description)
    port = ports[0].device
else: # found more than one serial port
    print("Found multiple serial ports:")
    for p in ports:
        print(" *", p.device, "-", p.description)
    print("Which serial port would you like to use? (default: " + ports[0].device + ") ", end="")
    port = input()
    if not port:
        port = ports[0].device


import sys
args = sys.argv
if len(args) == 1: # only the program name, no arguements: ask for the hex file
    print("Which hex file would you like to flash? ", end="")
    hexfile = input()
else:
    hexfile = args[1]
    

# pymcuprog uses the Python logging module
import logging
logging.basicConfig(format="%(levelname)s: %(message)s", level=logging.WARNING)

# Configure the session
from pymcuprog.backend import SessionConfig
sessionconfig = SessionConfig("attiny1616")

# Instantiate Serial transport (only 1 tool connected)
from pymcuprog.toolconnection import ToolSerialConnection
transport = ToolSerialConnection(serialport=port)

# Instantiate backend
from pymcuprog.backend import Backend
backend = Backend()

# Connect to tool using transport
backend.connect_to_tool(transport)

# Start the session
backend.start_session(sessionconfig)

# Read the target device_id
device_id = backend.read_device_id()
print("Device ID is {0:06X}".format(int.from_bytes(device_id, byteorder="little")))

# Erase the device, write the hexfile, and verify the write
backend.erase()
print("Memories erased.")
print("Writing hex file to the device... ")
backend.write_hex_to_target(hexfile)
print("Writing complete.")
print("Verifying the write... ")
verify_status = backend.verify_hex(hexfile)
if verify_status is True:
    print("Verification successful!")

