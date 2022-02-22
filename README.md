# Micropolis-SDL2PP

This is a C++/SDL2 port of the SimCity/Micropolis code developed by Don Hopkins for Unix. See __README_OG for the original readme provided by Don Hopkins and EA.

## License
This project is licensed under the GPL as required by the original code. See COPYING for details. There are additional licensing stipulations required by EA, see "ADDITIONAL TERMS" in __README_OG.

## Goals
* Port the original code to C++
* Port all drawing/input/window management code to SDL2
* Eliminate dependency on TCL/Tk

## Why?
After recovering from a COVID-19 infection, I was left with serious long-term effects notably brain fog and fatigue. I couldn't focus on my other project (OPHD) because of the brain fog and decided I needed something to challenge my mind a bit and shake off the funk. I came across Micropolis while browsing the available apps in Snap for Manjaro and I thought it would be an interesting challenge to have this building and running without native GUI's on Windows and other platforms.

## Platforms
My main development machine is Windows based so the current development efforts are for Windows. With some minor changes (notably in the filesystem handling code) this code base should work on all major platforms.
