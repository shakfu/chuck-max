RUN = chuck --chugin:../VST3.chug

.PHONY: all help \
	list basic-effect parameters dynamic-control bypass \
	instrument midi-simple multiple-effects parameter-sweep parameter-by-name \
	presets preset-by-name midi-control

# Default target: show help
help:
	@echo "VST3 Examples Makefile"
	@echo ""
	@echo "Usage: make <target>"
	@echo ""
	@echo "VST3 Examples (numbered series):"
	@echo "  list              - 00: List all VST3 plugins on the system"
	@echo "  basic-effect      - 01: Load and use a basic VST3 effect"
	@echo "  parameters        - 02: Parameter control demonstration"
	@echo "  dynamic-control   - 03: Real-time parameter modulation"
	@echo "  bypass            - 04: Bypass functionality"
	@echo "  instrument        - 05: MIDI instrument usage"
	@echo "  midi-simple       - 06: Simple MIDI example"
	@echo "  multiple-effects  - 07: Chaining multiple effects"
	@echo "  parameter-sweep   - 08: Parameter automation"
	@echo "  parameter-by-name - 09: Parameter control by name"
	@echo "  presets           - 10: Using factory presets"
	@echo "  preset-by-name    - 11: Loading presets by name"
	@echo "  midi-control      - 12: MIDI control change mapping"
	@echo ""
	@echo "Special targets:"
	@echo "  all               - Run all VST3 examples in order"
	@echo "  help              - Show this help message"

# Run all VST3 examples in order
all: list basic-effect parameters dynamic-control bypass \
	instrument midi-simple multiple-effects parameter-sweep \
	parameter-by-name presets preset-by-name midi-control

# VST3 Examples (00-12)
list:
	@echo "=== Running: 00-list-vst3.ck ==="
	@$(RUN) 00-list-vst3.ck

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

presets:
	@echo "=== Running: 10-presets.ck ==="
	@$(RUN) 10-presets.ck

preset-by-name:
	@echo "=== Running: 11-preset-by-name.ck ==="
	@$(RUN) 11-preset-by-name.ck

midi-control:
	@echo "=== Running: 12-midi-control.ck ==="
	@$(RUN) 12-midi-control.ck
