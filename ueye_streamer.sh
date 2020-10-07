#!/bin/sh

# This scripts sets the library path to use the prebuilt FFmpeg and LIVE555 libraries.

LD_LIBRARY_PATH="./lib/;./lib/ffmpeg/;./lib/live555" ./bin/ueye_streamer
