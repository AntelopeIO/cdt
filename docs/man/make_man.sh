#!/bin/bash
for file in cdt-*.1.md; do
    pandoc "$file" -s -t man -o "${file%.md}"
done
