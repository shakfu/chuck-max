#!/usr/bin/env bash

# update.sh
#
# This script updates to latest clones of chuck and chugins

CHUCK_REPO=https://github.com/ccrma/chuck.git
CHUGINS_REPO=https://github.com/ccrma/chugins.git
PROJECTS_DIR=source/projects


function update_chuck() {
	git clone ${CHUCK_REPO} chuck-src && \
	mkdir -p ${PROJECTS_DIR}/chuck-new && \
	mv chuck-src/src/core ${PROJECTS_DIR}/chuck-new/ && \
	mv chuck-src/src/host ${PROJECTS_DIR}/chuck-new/ && \
	rm -rf chuck-src && \
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
	mv ${PROJECTS_DIR}/chugins/MIAP ${PROJECTS_DIR}/chugins-new/MIAP && \
	mv ${PROJECTS_DIR}/chugins/WarpBuf ${PROJECTS_DIR}/chugins-new/WarpBuf && \
	# chugins
	update_new_chugin ABSaturator && \
	update_new_chugin AmbPan && \
	update_new_chugin Binaural && \
	update_new_chugin Bitcrusher && \
	update_new_chugin Elliptic && \
	update_new_chugin ExpDelay && \
	update_new_chugin ExpEnv && \
	update_new_chugin FIR && \
	update_new_chugin FoldbackSaturator && \
	update_new_chugin GVerb && \
	update_new_chugin KasFilter && \
	update_new_chugin MagicSine && \
	update_new_chugin Mesh2D && \
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
	#rm -rf ${PROJECTS_DIR}/chugins-new/chuginate
	mv ${PROJECTS_DIR}/chugins ${PROJECTS_DIR}/chugins-old && \
	mv ${PROJECTS_DIR}/chugins-new ${PROJECTS_DIR}/chugins && \
	rm -rf chugins-src
}



function update() {
	update_chuck
	update_chugins
	rm -rf ${PROJECTS_DIR}/chuck-old
	rm -rf ${PROJECTS_DIR}/chugins-old
}

update
