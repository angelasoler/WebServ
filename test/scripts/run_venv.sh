#!/bin/bash
echo "execute: source test/script/run_venv.sh"

python3 -m venv test/venv

. test/venv/bin/activate

test/venv/bin/python -m pip install -r test/venv/requirements.txt

