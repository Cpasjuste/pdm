#!/bin/bash
#arg1 : executable
#arg2 : working directory
echo "exec set to $1"
echo "cwd set to $2"
cd $2
$1


