#!/bin/bash

magick appicon.png -resize 16x16 16-icon.png
magick appicon.png -resize 32x32 32-icon.png
magick appicon.png -resize 64x64 64-icon.png
magick appicon.png -resize 128x128 128-icon.png
magick appicon.png -resize 256x256 256-icon.png
magick appicon.png -resize 512x512 512-icon.png
magick appicon.png -resize 1024x1024 1024-icon.png