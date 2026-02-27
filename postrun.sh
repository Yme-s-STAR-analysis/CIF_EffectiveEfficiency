#!/bin/bash

if [ $# -ne 2 ]; then
    echo "[WARNING] - $0: Usage: $0 <energy_tag> <action>"
    echo "==> An Example: $0 7 show"
    exit 1
fi

energy=$1
action=$2
xml_file="Xsubmit_${energy}.xml"

if [ ! -f "$xml_file" ]; then
    echo "[WARNING] - $0: File $xml_file not found!"
    exit 1
fi

target_path=$(grep -o 'file:[^"]*/out/' "$xml_file" | sed 's|file:\(.*\)/out/|\1|')

if [ -z "$target_path" ]; then
    echo "[WARNING] - $0: No valid path pattern found in $xml_file"
    exit 1
fi

case $action in
    show)
        echo "[LOG] - $0: output path: $target_path"
        ;;
    merge)
        output_file="${energy}GeV.hadd.root"
        echo "[LOG]- $0: merging files from $target_path/out/ to $output_file"
        hadd "$output_file" "$target_path"/out/*root
        ;;
    fmerge)
        output_file="${energy}GeV.hadd.root"
        echo "[LOG] - $0: merging files (force) from $target_path/out/ to $output_file"
        hadd -f "$output_file" "$target_path"/out/*root
        ;;
    *)
        echo "[WARNING]: invalid action ('$action'), please try: show, merge, fmerge"
        exit 1
        ;;
esac

