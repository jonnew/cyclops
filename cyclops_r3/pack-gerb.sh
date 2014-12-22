#!/bin/bash

# This script packages the gerbers (using standard 4-layer file names required by OSH park) into a folter and compresses it

mkdir gerber 
mv ./cyclops_r3.GTL ./cyclops_r3.GTS ./cyclops_r3.GTO ./cyclops_r3.G2L ./cyclops_r3.G3L ./cyclops_r3.GBL ./cyclops_r3.GKO ./cyclops_r3.XLN ./cyclops_r3.GBO ./cyclops_r3.GBS gerber
zip gerber.zip ./gerber/*

mkdir stencil 
mv ./cyclops_r3.SST ./cyclops_r3.CST stencil
zip stencil.zip ./stencil/*
