

function download_sf2() {
	EXAMPLES_DIR=examples
	sf2_dir="${EXAMPLES_DIR}/fluidsynth/sf2"
	if [[ ! -a ${sf2_dir}/FluidR3_GM.sf2 ]]; then
		wget -P ${sf2_dir} https://github.com/pianobooster/fluid-soundfont/releases/download/v3.1/FluidR3_GM.sf2
	fi
	if [[ ! -a ${sf2_dir}/FluidR3_GS.sf2 ]]; then
		wget -P ${sf2_dir} https://github.com/pianobooster/fluid-soundfont/releases/download/v3.1/FluidR3_GS.sf2
	fi
}

download_sf2
