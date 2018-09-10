#!/bin/bash
# Run "python -m SimpleHTTPServer 8000" on server

time `for ((i=1;i<=1000;i++)); do   curl -s "http://127.0.0.1:8000" > /dev/null; done`
