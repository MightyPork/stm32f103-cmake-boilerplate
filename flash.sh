#!/bin/bash

st-flash write build/*.bin 0x8000000 && echo "Flash OK"
