
COLOR_BOLD_YELLOW="\033[1;33m"
COLOR_BOLD_BLUE="\033[1;34m"
COLOR_BOLD_MAGENTA="\033[1;35m"
COLOR_BOLD_CYAN="\033[1;36m"
COLOR_BOLD_RED="\033[1;31m"
COLOR_BOLD_WHITE="\033[1m"
COLOR_RESET="\033[m"



function section {
    echo
    echo $COLOR_BOLD_MAGENTA$1 $COLOR_RESET
    echo "----------------------------------------------------------------------------"
}

function warn {
    echo
    echo $COLOR_BOLD_RED"WARNING: "$COLOR_RESET$1
    echo
}

function finished {
    echo
    echo $COLOR_BOLD_WHITE"[DONE]"$COLOR_RESET
    echo
}

function info {
    echo
    echo "$COLOR_BOLD_CYAN$1 $COLOR_RESET"
    echo
}

function runtime {
    echo
    echo "----------------------------------------------------------"
    echo "=> $COLOR_BOLD_CYAN RUNTIME:$COLOR_RESET $COLOR_BOLD_WHITE$1$COLOR_RESET seconds"
    echo "----------------------------------------------------------"
    echo
}

