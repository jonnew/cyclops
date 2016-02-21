#!/bin/bash

# This script packages the gerbers (using standard 4-layer file names required by OSH park) into a folter and compresses it

mkdir gerber 
mv ./pcb/cyclops.GTL ./pcb/cyclops.GTS ./pcb/cyclops.GTO ./pcb/cyclops.G2L ./pcb/cyclops.G3L ./pcb/cyclops.GBL ./pcb/cyclops.GKO ./pcb/cyclops.XLN ./pcb/cyclops.GBO ./pcb/cyclops.GBS gerber
zip gerber.zip ./gerber/*

mkdir stencil 
rm cyclops.SST
mv ./pcb/cyclops.CST stencil
zip stencil.zip ./stencil/*
