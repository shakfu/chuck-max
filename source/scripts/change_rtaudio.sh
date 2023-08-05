#!/usr/bin/env bash


echo "changing kAudioObjectPropertyElementMaster -> kAudioObjectPropertyElementMain"
rpl --match-case --whole-words --fixed-strings --recursive \
	kAudioObjectPropertyElementMaster \
	kAudioObjectPropertyElementMain \
	source/projects/chuck~/host/RtAudio

rpl --match-case --whole-words --fixed-strings --recursive \
	kAudioObjectPropertyElementMaster \
	kAudioObjectPropertyElementMain \
	source/projects/chuck~/host_embed/RtAudio
