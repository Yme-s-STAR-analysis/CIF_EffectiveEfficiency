#!/bin/bash

# New version: prepare.sh

if [ "$1" = "clean" ]; then
    if [ $# -gt 2 ]; then
        echo "[WARNING] - $0: Usage: $0 clean <submit_tag>"
        exit 1
    fi
    
    echo "[LOG] - $0: cleaning sandbox files..."
    rm -rf libZip*.zip libZip*.package
    
    if [ $# -eq 2 ]; then
        echo "[LOG] - $0: clearning temp. files for submit tag $2"
        rm -rf sched${2}.dataset sched${2}.session.xml
    fi
    exit 0
fi

if [ $# -lt 3 ]; then
    echo "[WARNING] - $0: Usage: $0 <target_path> <filelist_path> <energy_tag>[submit_flag]"
    exit 1
fi

target=$1
flist=$2
energy=$3
submit_flag=${4:-0} # default: 0, NOT to submit

echo "[LOG] - $0: target path: $target"
echo "[LOG] - $0: file list: $flist"
echo "[LOG] - $0: energy tag: $energy"
echo "[LOG] - $0: submit flag: $submit_flag"

new_xml="Xsubmit_${energy}.xml"

rm -f $new_xml
sed -e "s|TARGET|$target|g" -e "s|FLIST|$flist|g" .tpl.xml > $new_xml

mkdir -p $target/{report,csh,err,list,log,out}

rm -rf $target/report/* $target/csh/* $target/err/* $target/list/* $target/log/* $target/out/*

if [ "$submit_flag" = "1" ]; then
    echo "[LOG] - $0: submitting job with energy=$energy"
    star-submit-template-beta  -template $new_xml -entities ENGTAG=$energy
else
    echo "[LOG] - $0: I won't submit jobs, and you can do it manually: star-submit-template-beta -template $new_xml -entities ENGTAG=$energy"
fi