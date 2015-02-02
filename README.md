Sensorino firmware
==================

See further down for general information about the Sensorino project.  This repository contains Arduino-compatible libraries that implement the _Sensorino node_'s logic -- both the remote node's and the central _Base node_.  They implement enough logic that the final sketch may be limited to just a few lines of code that initializes node-specific services, sets their addresses and identifiers.

The Sensorino subdirectory contains the remote node library and the Base subdirectory contains the base node library.  Both can be installed by copying/symlinking to Arduino's _"Libraries"_ directory or from within the IDE.  Example sketches for both node types can be found in subdirectories named sketch0, sketch1, etc.

The Base library has one dependency, the excellent _aJson_ library for JSON parsing and generation on Arduino.  It can be installed from its github repository (`git clone https://github.com/interactive-matter/aJson.git`) or from a submodule of the Sensorino repository.  We also maintain a copy of the _RadioHead_ library which is however optional.  By default Sensorino uses its own minimal nRF24L01+ radio driver.  You can switch to RadioHead to experiment with options such as mesh networking.

A little more documentation [is available on this project's wiki](https://github.com/Sensorino/Sensorino/wiki).

The Sensorino Project
=====================

Sensorino is a set of opensource software and open hardware designs that form a base for cheap, small-scale sensor networks / home-automation / remote-control setups.  The project is based around Arduino-compatible boards and other cheap technologies, aiming at great experience at less than $5 for a basic network node.  It integrates with some other technologies and the software side is intentionally flexible to allow integrating newly emerging technologies.

Documentation of the system is located in the github wikis corresponding to the subprojects:
* [Sensorino firmware and general information wiki](https://github.com/Sensorino/Sensorino/wiki).

* [Sensorino smarthome server wiki](https://github.com/Sensorino/sensorino-smarthome/wiki).

Contact
-------

sensorino@googlegroups.com is the open mailing list for the project.  You
can also submit specific feedback through the github Issues for each
specific subproject.

Components
----------

This is a quick overview of the project's elements.

* Hardware -- any Atmega328-based board can be used for a base or remote Sensorino node.  This includes Arduino Pro Mini, Arduino Uno, Arduino Duemilanove, and compatible boards, including cheap clones.  For a remote node, a 5V power supply, an nRF24L01+ radio module and a choice of sensors or actuators are needed.  https://github.com/Sensorino/SensorinoHardware contains specialised, minimal board designs that can be used instead of piecing a node together from individual off-the-shelf boards.
* Sensorino firmware to be used on the remote- and base-nodes, which relays data between the sensors/actuators and the central node, and executes automation rules.  https://github.com/Sensorino/Sensorino contains the firmware's source code.
* The modified optiboot bootloader at https://github.com/balrog-kun/optiboot can be used together with the Sensorino firmware to provide over-the-air flashing support for remote nodes.
* Sensorino Server -- software that runs on a Linux-capable machine to provide an HTTP API to the whole Sensorino network and a web User Interface to give the user full control of the network.  The Arduino-based Base-node may be connected to the same machine or to a different computer and connect through TCP/IP.  The Server can also control devices that use some other radio technologies and protocols, such as some consumer Bluetooth Low-Energy (BLE) devices, to provide a unified interface to all installed devices.  https://github.com/Sensorino/sensorino-smarthome contains the server implementation.

![Sensorino software diagram](https://rawgit.com/Sensorino/Sensorino/master/doc/diagram-software.0.svg)
