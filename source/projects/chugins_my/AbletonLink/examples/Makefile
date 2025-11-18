RUN = chuck --chugin:../AbletonLink.chug

.PHONY: tempo sequencer sync latency drum metronome

tempo:
	@$(RUN) tempo_control.ck

seq:
	@$(RUN) sequencer.ck

quantum:
	@$(RUN) quantum_sync.ck

latency:
	@$(RUN) latency_test.ck

drum:
	@$(RUN) drum_machine.ck

metronome:
	@$(RUN) basic_metronome.ck