#!/bin/bash

gcc -rdynamic `pkg-config --cflags gtk+-3.0` -o gtk_main gtk_main.c  `pkg-config --libs gtk+-3.0`

