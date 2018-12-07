#!/bin/env bash

# Generates gource video: https://gource.io/
# To run it from the root directory
# Needs gource and ffmpeg for the video
# Usage:
#
# cd repository
# .ci/create_gource
#
# or, to save it into a video:
#
# create_gource --video
#


function is_installed () {
    is_installed=$(which $1 2> /dev/null)
    if [[ $? != 0 ]]; then
        echo " ${1} is not available. Try installing it with"
        echo "     brew install ${1}"
        exit 1
    fi
}

pic_dir=".git/avatar"

# name,github
cat <<EOF > names.csv
David Pérez-Suárez,dpshelio
Gary Macindoe,garymacindoe
Ilektra Christidi,ilectra
Jason McEwen,jasonmcewen
Jens H Nielsen,jenshnielsen
Luke Pratley,Luke-Pratley
Mayeul d'Avezac,mdavezac
Ole Streicher,olebole
Rafael Carrillo,rafael-carrillo
rc-softdev-admin,rc-softdev-admin
Roland Guichard,UCLGuichard
EOF

mkdir -p ${pic_dir}
url="https://github.com/username.png"
n_images=$(ls ${pic_dir} | wc -l)
n_names=$(wc -l names.csv)
if [[ ${n_images[1]} < ${n_names[1]} ]]; then
    while IFS="," read fullname username; do
        curl -L ${url/username/${username}} > "${pic_dir}/${fullname}.png"
    done < names.csv
fi

is_installed gource || exit 1


if [[ ${1} == "--video" ]]; then
    is_installed ffmpeg || exit 1
    gource --user-image-dir ${pic_dir} --seconds-per-day 0.05 -1280x720 -o - | ffmpeg -y -r 60 -f image2pipe -vcodec ppm -i - -vcodec libx264 -preset ultrafast -pix_fmt yuv420p -crf 1 -threads 0 -bf 0 gource.mp4
else
    gource --user-image-dir ${pic_dir} --seconds-per-day 0.05 -1280x720
fi
