// The misty command lets you control the volume of output on a Raspberry Pi with a MIDI controller.
package main

import (
	"flag"
	"fmt"
	"github.com/bklimt/midi"
	"github.com/bklimt/volume"
)

var card string

func midiOn(note int) {
	if note == 72 {
		volume.SetVolume(card, 0)
	}

	// 48 -> 100, 72 -> 60
	vol := int(60.0 + 40.0*(1.0-((float64(note)-48.0)/(72.0-48.0))))
	fmt.Printf("Setting volume to %d%%\n", vol)
	volume.SetVolume(card, vol)
}

func midiControl(param, value int) {
	if param == 7 {
		volume.SetVolume(card, (value*100)/127)
	}
}

func main() {
	flag.StringVar(&card, "card", "default", "The card to set the volume for.")

	flag.Parse()

	c := make(chan interface{})
	midi.Listen(c)
	for event := range c {
		switch event := event.(type) {
		case midi.Controller:
			fmt.Printf("Controller event: %d %d\n", event.Param, event.Value)
			midiControl(event.Param, event.Value)
		case midi.NoteOn:
			fmt.Printf("Note on: %d\n", event.Note)
			midiOn(event.Note)
		case midi.NoteOff:
			fmt.Printf("Note off: %d\n", event.Note)
		}
	}
}
