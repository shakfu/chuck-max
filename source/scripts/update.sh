#!/usr/bin/env bash

# update.sh
#
# This script updates to latest clones of chuck and chugins

CHUCK_REPO=https://github.com/ccrma/chuck.git
CHUGINS_REPO=https://github.com/ccrma/chugins.git
PROJECTS_DIR=source/projects
CHUCK_SRC=build/chuck-src
CHUCK_SRC_EXAMPLES=${CHUCK_SRC}/examples
PROJECT_SRC_EXAMPLES=source/projects/examples
PROJECT_EXAMPLES=examples

function update_chuck() {
	mkdir -p build &&
	git clone --depth=1 ${CHUCK_REPO} ${CHUCK_SRC} && \
	mkdir -p ${PROJECTS_DIR}/chuck-new && \
	mv ${CHUCK_SRC}/src/core ${PROJECTS_DIR}/chuck-new/ && \
	mv ${CHUCK_SRC}/src/host ${PROJECTS_DIR}/chuck-new/ && \
	cp ${PROJECTS_DIR}/chuck/CMakeLists.txt ${PROJECTS_DIR}/chuck-new/ && \
	cp ${PROJECTS_DIR}/chuck/core/CMakeLists.txt ${PROJECTS_DIR}/chuck-new/core/ && \
	cp ${PROJECTS_DIR}/chuck/host/CMakeLists.txt ${PROJECTS_DIR}/chuck-new/host/ && \
	cp -rf ${PROJECTS_DIR}/chuck/host_embed ${PROJECTS_DIR}/chuck-new/ &&  \
	mv ${PROJECTS_DIR}/chuck ${PROJECTS_DIR}/chuck-old && \
	mv ${PROJECTS_DIR}/chuck-new ${PROJECTS_DIR}/chuck
}



function move_to_new() {
	mv chugins-src/"$1" ${PROJECTS_DIR}/chugins-new/"$1"
}

function update_new_chugin() {
	move_to_new "$1" && \
	cp ${PROJECTS_DIR}/chugins/"$1"/CMakeLists.txt ${PROJECTS_DIR}/chugins-new/"$1" && \
	rm -rf ${PROJECTS_DIR}/chugins-new/"$1"/makefile* && \
	rm -rf ${PROJECTS_DIR}/chugins-new/"$1"/*.dsw && \
	rm -rf ${PROJECTS_DIR}/chugins-new/"$1"/*.dsp && \
	rm -rf ${PROJECTS_DIR}/chugins-new/"$1"/*.xcodeproj && \
	rm -rf ${PROJECTS_DIR}/chugins-new/"$1"/*.vcxproj && \
	rm -rf ${PROJECTS_DIR}/chugins-new/"$1"/*.sln && \
	rm -rf ${PROJECTS_DIR}/chugins-new/"$1"/.gitignore
}


function update_chugins() {
	git clone ${CHUGINS_REPO} chugins-src && \
	mkdir -p ${PROJECTS_DIR}/chugins-new && \
	cp ${PROJECTS_DIR}/chugins/CMakeLists.txt ${PROJECTS_DIR}/chugins-new/ && \
	# non-chugins
	move_to_new chuck && \
	move_to_new chuginate && \
	move_to_new LICENSE && \
	move_to_new notes && \
	move_to_new README.md && \
	# move special cases
	mv ${PROJECTS_DIR}/chugins/Fauck ${PROJECTS_DIR}/chugins-new/Fauck && \
	mv ${PROJECTS_DIR}/chugins/WarpBuf ${PROJECTS_DIR}/chugins-new/WarpBuf && \
	# chugins
	update_new_chugin ABSaturator && \
	update_new_chugin AmbPan && \
	update_new_chugin Binaural && \
	update_new_chugin Bitcrusher && \
	update_new_chugin ConvRev && \
	update_new_chugin Elliptic && \
	update_new_chugin ExpDelay && \
	update_new_chugin ExpEnv && \
	update_new_chugin FIR && \
	update_new_chugin FluidSynth && \
	update_new_chugin FoldbackSaturator && \
	update_new_chugin GVerb && \
	update_new_chugin KasFilter && \
	update_new_chugin Ladspa && \
	update_new_chugin Line && \
	update_new_chugin MagicSine && \
	update_new_chugin Mesh2D && \
	update_new_chugin MIAP && \
	update_new_chugin Multicomb && \
	update_new_chugin NHHall && \
	update_new_chugin Overdrive && \
	update_new_chugin PanN && \
	update_new_chugin Patch && \
	update_new_chugin Perlin && \
	update_new_chugin PitchTrack && \
	update_new_chugin PowerADSR && \
	update_new_chugin Random && \
	update_new_chugin Range && \
	update_new_chugin RegEx && \
	update_new_chugin Sigmund && \
	# duplicate symbols between sigmund.c and sigmund-dsp.c
	mv ${PROJECTS_DIR}/chugins-new/Sigmund/sigmund.c ${PROJECTS_DIR}/chugins-new/Sigmund/sigmund.c.orig && \
	update_new_chugin Spectacle && \
	update_new_chugin WPDiodeLadder && \
	update_new_chugin WPKorg35 && \
	update_new_chugin Wavetable && \
	update_new_chugin WinFuncEnv && \
	update_new_chugin XML && \
	#rm -rf ${PROJECTS_DIR}/chugins-new/chuginate
	mv ${PROJECTS_DIR}/chugins ${PROJECTS_DIR}/chugins-old && \
	mv ${PROJECTS_DIR}/chugins-new ${PROJECTS_DIR}/chugins && \
	rm -rf chugins-src
}



function rm_bld() {
	rm -rf ${CHUCK_SRC_EXAMPLES}/$1
}

function cp_src_dst() {
	cp -rf ${PROJECT_SRC_EXAMPLES}/$1 ${PROJECT_EXAMPLES}
}

function update_examples() {
	rm_bld README
	rm_bld book
	rm_bld hanoi++.ck
	rm_bld hanoi.ck
	rm_bld hanoi2.ck
	rm_bld hanoi3.ck
	rm_bld help.ck
	rm_bld otf_01.ck
	rm_bld otf_02.ck
	rm_bld otf_03.ck
	rm_bld otf_04.ck
	rm_bld otf_05.ck
	rm_bld otf_06.ck
	rm_bld otf_07.ck
	rm_bld status.ck

	echo "replace current examples dir with new"
	rm -rf ${PROJECT_EXAMPLES}
	cp -rf ${CHUCK_SRC_EXAMPLES} ${PROJECT_EXAMPLES}

	cp_src_dst README.md
	cp_src_dst audiounit
	cp_src_dst chugins
	cp_src_dst convrev
	cp_src_dst fauck
	cp_src_dst faust
	cp_src_dst fluidsynth
	cp_src_dst hanoi
	cp_src_dst line
	cp_src_dst link
	cp_src_dst max
	cp_src_dst otf
	cp_src_dst test
	cp_src_dst util
	cp_src_dst warpbuf

	cp -f ${PROJECT_SRC_EXAMPLES}/data/amen.wav ${PROJECT_EXAMPLES}/data/amen.wav
	cp -f ${PROJECT_SRC_EXAMPLES}/data/nylon2.mp3 ${PROJECT_EXAMPLES}/data/nylon2.mp3
	cp -f ${PROJECT_SRC_EXAMPLES}/data/honkeytonk-algo3.ck ${PROJECT_EXAMPLES}/stk/honkeytonk-algo3.ck
	cp -rf ${PROJECT_SRC_EXAMPLES}/data/midi ${PROJECT_EXAMPLES}/midi/data
}

function update() {
	update_chuck
	update_examples
	update_chugins
	rm -rf ${CHUCK_SRC}
	rm -rf ${PROJECTS_DIR}/chuck-old
	rm -rf ${PROJECTS_DIR}/chugins-old
}



update


