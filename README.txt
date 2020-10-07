################################################################################

      IDS - Imaging Development Systems GmbH

      uEye RTSP Streaming library

################################################################################

(c) 2016, Imaging Development Systems GmbH

The uEye Streaming library provides a simple mechanism to setup RTSP server for
streaming images captured with uEye cameras. The uEye Streaming library supports
automatic scaling and encoding of images with video codecs like MJPEG and H.264.
A demo application is also included to setup a RTSP server and to stream images
from a uEye camera.

------------------
     SUMMARY
------------------

 1 - REQUIREMENTS
 2 - ARCHIVE FILE STRUCTURE
 3 - ARCHIVE EXTRACT INSTRUCTION
 4 - THE DEMONSTRATION PROGRAM
 5 - DOCUMENTATION
 6 - LICENSING


1 - REQUIREMENTS
----------------

SOFTWARE REQUIREMENTS

 - The IDS Software suite must be installed. (There is no special version
   dependency to the streamer application)
 - libc version 2.13 and libstdc++ version 6.0.17 have to be installed.
 - To build the example source codes CMake <https://cmake.org> and a C++11
   compatible compiler must be available. For the graphical demo application
   'uEyeStreamer' the Qt development toolkit must be available.
 - To run the binary 'ueye_streamer' under Linux the Qt Runtime (min. v4.5.2)
   is required.


OTHER REQUIREMENTS
 - If a firewall is active on your system, ensure that ports used for the RTSP
   server is not filtered.
- The H.264 encoder is not distributed with this tar archive. If you do like to
  use H.264 streaming the OpenH264 library must be obtained from
  <http://www.openh264.org>. Furthermore the shared library must be renamed to
  'libopenh264.lib' and must be available in the LD_LIBRARY_PATH path. To use
  H.264 with the demo application copy the library to the <extract dir>/lib/
  directory.


--------------------------------------------------------------------------------
2 - ARCHIVE FILE STRUCTURE
--------------------------------------------------------------------------------

The created files will be installed in following structure:
<extract dir>/src/uEyeStreamer/         Streamer demo source files
<extract dir>/src/SimpleStreamer/       Simple streamer application source files
<extract dir>/lib/                      uEye streaming shared library files
<extract dir>/lib/ffmpeg/               Prebuilt FFmpeg shared library files
<extract dir>/lib/live555/              Prebuilt LIVE555 shared library files
<extract dir>/licenses/                 License files
<extract dir>/README.txt                This readme file
<extract dir>/ueye_streamer.sh          Shell script to run the demo application
<extract dir>/bin/ueye_streamer         Demo application binary file
<extract dir>/doc/                      Library doxygen documentation
<extract dir>/include/                  Library include file

--------------------------------------------------------------------------------
4 - ARCHIVE EXTRACT INSTRUCTION
--------------------------------------------------------------------------------

1 - copy the tar archive into a directory on the hard disk
    (you need write access to decompress).

2 - go to the directory you copied the tar archive into and
    extract the archive files by typing (replace the
    wildcard with the actual version/arch identifier of the
    archive file)
    $> tar -xf ./ueye_stream*.tar.gz

3 - if extract has succeeded, one may start the streaming demo application by
    typing '<extract dir>/ueye_streamer.sh' (Qt runtime must be installed)


--------------------------------------------------------------------------------
5 - THE DEMONSTRATION PROGRAMS
--------------------------------------------------------------------------------

The demonstration program shows how to use the uEye streaming library in order
to make following actions:
- RTSP server setup
- Initializing and setup a uEye camera
- Initialize and setup a rtsp stream
- Streaming image data

To run the prebuilt demo application run the shell script
'<extract dir>/ueye_streamer.sh'. This shell scripts sets the 'LD_LIBRARY_PATH'
to use the prebuilt FFmpeg and LIVE555 libraries. The source could to the demo
application could be found in '<extract dir>/src/uEyeStreamer/'. Source code for
a simple console demo application is available in
'<extract dir>/src/SimpleStreamer/'.

Build applications from source
------------------------------

SimpleStreamer
$> 'change dir to build directory'
$> cmake '<extract dir>/src/SimpleStreamer'
$> make

uEyeStreamer
$> 'change dir to build directory'
$> cmake '<extract dir>/src/uEyeStreamer'
$> make

See CMakelist.txt for further configuration details.

--------------------------------------------------------------------------------
6 - DOCUMENTATION
--------------------------------------------------------------------------------

A doxygen documentation of the uEye Streaming library is available in HTML
format in the /ueye_stream/doc directory. Also see the example source code
available in the /src directory.

Notes
-----
- The RTSP server always binds to every network adapter. A possibility to only
  bind to a specific adapter is not available.
- The uEye Streaming library supports at the moment the video codecs MJPEG and
  H.264. Please keep in mind that the maximum resolution for H.264 is 4096 x
  2304 and for MJPEG 2040 x 2040 pixels. Furthermore the resolution (width,
  height) must be a multiple of 4.
- The submitted image buffers must be in a valid format for the already
  configured stream session (size, pixel format).
- Please consider that the transmission of the stream could be delayed by
  encoding, transmission, media player buffering,et cetera. If images are broken
  or could not be received, try reducing the image resolution, bitrate or
  increase the buffering of the media player software.
- If the streaming library is closed it is possible that the used port is not
  freed immediately. A reinitialization could therefore be denied until the port
  is freed by the OS.

What's new?
-----------
Version 2.0
Date: 2016/0/08

The uEye Streaming library has been reworked and methods, enumerations and
structures have been renamed to get the relationships more clearly. The live
media library has been updated to version 2016/04/21. The FFmpeg libraries has
been updated to version 2.8.7. The OpenH264 encoder is now used for H.264
encoding.

--------------------------------------------------------------------------------
7 - LICENSING
--------------------------------------------------------------------------------

Please see the files in the '<extract dir>/licenses/' directory.

FFmpeg is licensed under the GNU Lesser General Public License (LGPL) version
2.1. You may use, distribute and copy the FFmpeg library under the terms of
GNU Lesser General Public License version 2.1. This license makes reference to
the version 3 of the GNU General Public License, which you can find in the
LICENSE.GPLv3 file.

The distributed FFmpeg libraries version 2.8.7  are built with the following
configuration:

    --disable-static
    --enable-memalign-hack
    --disable-muxers
    --disable-filters
    --enable-pthreads
    --enable-shared
    --disable-programs
    --disable-doc
    --disable-network
    --disable-debug
    --enable-yasm
    --enable-asm

Used compiler:

gcc (Debian 4.7.2-5) 4.7.2
Copyright (C) 2012 Free Software Foundation, Inc.

The source code for this FFmpeg build can be found at:
<https://en.ids-imaging.com/open-source.html>

LIVE555 Streaming Media version 2016/04/21 is licensed under the GNU LGPL.
You may use, distribute and copy the Live555 library under the terms of
GNU Lesser General Public License version 2.1. This license makes reference to
the version 3 of the GNU General Public License, which you can find in the
LICENSE.GPLv3 file.

The source code for this LIVE555 build can be found at:
<https://en.ids-imaging.com/open-source.html>

The Qt Toolkit is Copyright (C) 2015 The Qt Company Ltd.
Contact: <http://www.qt.io/licensing/>
You may use, distribute and copy the Qt GUI Toolkit under the terms of
GNU Lesser General Public License version 3. This license makes reference to the
version 3 of the GNU General Public License, which you can find in the
LICENSE.GPLv3 file.

The source code for this Qt Toolkit build can be found at:
<https://en.ids-imaging.com/open-source.html>
