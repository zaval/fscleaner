#!/bin/bash

# This script converts the icon theme to a format that can be used by the system.
# It uses the `convert` command to convert the SVG files to PNG format.
# It also creates a symlink to the current version of the theme.
# The script assumes that the `convert` command is available in the PATH.
# It also assumes that the `inkscape` command is available in the PATH.
# The script takes one argument, which is the path to the icon theme directory.
# The script will create a new directory called `icons` in the current directory,
# and will copy the SVG files from the icon theme directory to the `icons` directory.

file_name=$(basename "$1")
echo "Converting $file_name to PNG format..."

mkdir 32x32@2 || true
mkdir 32x32 || true
mkdir 16x16 || true
mkdir 20x20 || true
mkdir 24x24 || true

convert "$1"  -resize 64x64 32x32@2/$file_name
convert "$1"  -resize 32x32 32x32/$file_name
convert "$1"  -resize 16x16 16x16/$file_name
convert "$1"  -resize 20x20 20x20/$file_name
convert "$1"  -resize 256x256 256x256/$file_name
convert "$1"  -resize 24x24 24x24/$file_name
