NAME=chuck~

dylibbundler -od -b \
	-x externals/${NAME}.mxo/Contents/MacOS/${NAME} \
	-d externals/${NAME}.mxo/Contents/Frameworks \
	-p @loader_path/../Frameworks
