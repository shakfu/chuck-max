RUN = chuck --chugin:../CLAP.chug

.PHONY: all help \
	list basic-effect parameters dynamic-control bypass \
	instrument midi-simple multiple-effects parameter-sweep parameter-by-name \
	multi-plugin \
	demo clap-test tal midi melody

# Default target: show help
help:
	@echo "CLAP2 Examples Makefile"
	@echo ""
	@echo "Usage: make <target>"
	@echo ""
	@echo "CLAP2 Examples (numbered series):"
	@echo "  list              - 00: List all CLAP plugins on the system"
	@echo "  basic-effect      - 01: Load and use a basic effect"
	@echo "  parameters        - 02: Parameter control demonstration"
	@echo "  dynamic-control   - 03: Real-time parameter modulation"
	@echo "  bypass            - 04: Bypass functionality"
	@echo "  instrument        - 05: MIDI instrument usage"
	@echo "  midi-simple       - 06: Simple MIDI example"
	@echo "  multiple-effects  - 07: Chaining multiple effects"
	@echo "  parameter-sweep   - 08: Parameter automation"
	@echo "  parameter-by-name - 09: Parameter control by name"
	@echo "  multi-plugin      - 10: Multi-plugin bundle support"
	@echo ""
	@echo "Legacy CLAP Examples:"
	@echo "  demo              - CLAP MIDI demo"
	@echo "  clap-test         - CLAP test suite"
	@echo "  tal               - TAL plugin test"
	@echo "  midi              - MIDI test"
	@echo "  melody            - MIDI melody test"
	@echo ""
	@echo "Special targets:"
	@echo "  all               - Run all CLAP2 examples in order"
	@echo "  legacy            - Run all legacy CLAP examples"
	@echo "  help              - Show this help message"

# Run all CLAP2 examples in order
all: list basic-effect parameters dynamic-control bypass \
	instrument midi-simple multiple-effects parameter-sweep \
	parameter-by-name multi-plugin

# Run all legacy CLAP examples
legacy: demo clap-test tal midi melody

# CLAP2 Examples (00-10)
list:
	@echo "=== Running: 00-list-clap.ck ==="
	@$(RUN) 00-list-clap.ck

basic-effect:
	@echo "=== Running: 01-basic-effect.ck ==="
	@$(RUN) 01-basic-effect.ck

parameters:
	@echo "=== Running: 02-parameters.ck ==="
	@$(RUN) 02-parameters.ck

dynamic-control:
	@echo "=== Running: 03-dynamic-control.ck ==="
	@$(RUN) 03-dynamic-control.ck

bypass:
	@echo "=== Running: 04-bypass.ck ==="
	@$(RUN) 04-bypass.ck

instrument:
	@echo "=== Running: 05-instrument.ck ==="
	@$(RUN) 05-instrument.ck

midi-simple:
	@echo "=== Running: 06-midi-simple.ck ==="
	@$(RUN) 06-midi-simple.ck

multiple-effects:
	@echo "=== Running: 07-multiple-effects.ck ==="
	@$(RUN) 07-multiple-effects.ck

parameter-sweep:
	@echo "=== Running: 08-parameter-sweep.ck ==="
	@$(RUN) 08-parameter-sweep.ck

parameter-by-name:
	@echo "=== Running: 09-parameter-by-name.ck ==="
	@$(RUN) 09-parameter-by-name.ck

multi-plugin:
	@echo "=== Running: 10-multi-plugin.ck ==="
	@$(RUN) 10-multi-plugin.ck

# Legacy CLAP Examples
demo:
	@echo "=== Running: Clap-midi-demo.ck ==="
	@$(RUN) Clap-midi-demo.ck

clap-test:
	@echo "=== Running: Clap-test.ck ==="
	@$(RUN) Clap-test.ck

tal:
	@echo "=== Running: TAL-test.ck ==="
	@$(RUN) TAL-test.ck

midi:
	@echo "=== Running: test-midi.ck ==="
	@$(RUN) test-midi.ck

melody:
	@echo "=== Running: test-midi-melody.ck ==="
	@$(RUN) test-midi-melody.ck
