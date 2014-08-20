Misty
=====

A go app for controlling the volume of output on a Raspberry Pi with a MIDI controller.

# Setup

Depends on my [MIDI package](https://github.com/bklimt/midi), so make sure to install the dependencies for that. Specifically, you'll need to make sure you install the ALSA asound development libraries.

Run the app to listen for MIDI events to control the volume:

    $GOPATH/bin/misty

While `misty` is running, you can see it as a MIDI receiver on your system by running:

    aconnect -loi

You'll need to use `aconnect` to bind the MIDI controller you want to use to `misty`, something like:

    aconnect 20 128
