#!/bin/bash

if ! command -v pdflatex &> /dev/null; then
    echo "Error: pdflatex is not installed"
    exit 1
fi

BUILD_DIR=$(mktemp -d)
pdflatex -interaction=nonstopmode -output-directory="$BUILD_DIR" checkpoint_report.tex
mv "$BUILD_DIR/checkpoint_report.pdf" .
rm -rf "$BUILD_DIR"
