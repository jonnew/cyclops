#!/bin/bash

# This script packages the gerbers (using standard 4-layer file names required by OSH park) into a folter and compresses it

mkdir gerber 
mv ./pcb/cyclops_r3.GTL ./pcb/cyclops_r3.GTS ./pcb/cyclops_r3.GTO ./pcb/cyclops_r3.G2L ./pcb/cyclops_r3.G3L ./pcb/cyclops_r3.GBL ./pcb/cyclops_r3.GKO ./pcb/cyclops_r3.XLN ./pcb/cyclops_r3.GBO ./pcb/cyclops_r3.GBS gerber
zip gerber.zip ./gerber/*

mkdir stencil 
mv ./pcb/cyclops_r3.SST ./pcb/cyclops_r3.CST stencil
zip stencil.zip ./stencil/*
