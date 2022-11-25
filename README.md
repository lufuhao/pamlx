# PAMLX

## Description

A graphical user interface for [Phylogenetic Analysis by Maximum Likelihood (PAML)](http://abacus.gene.ucl.ac.uk/software/paml.html) program

Fit to Qt5 based on original version [pamlx v1.3.1](http://abacus.gene.ucl.ac.uk/software/pamlX1.3.1-src.tgz)

## Requiements

    [x] g++

    [x] Qt5

## Install

### Generate Makefile

```
#Generate Makefile
qmake
#add ' -lQt5Widgets -lQt5PrintSupport' to the LIBS in Makefile
#In my case: LIBS          = $(SUBLIBS) /usr/lib/x86_64-linux-gnu/libQt5Gui.so /usr/lib/x86_64-linux-gnu/libQt5Core.so -lGL -lpthread -lQt5Widgets -lQt5PrintSupport
perl -pi -e 'if (/^LIBS/){s/$/ -lQt5Widgets -lQt5PrintSupport/;}' Makefile
```

### Generate ui_*.h

```
for idv in *.ui; do echo $idv; uic $idv -o ui_${idv%.ui}.h; done
```

### make

```
#generate pamlx executable
make
```

## Notes

1. I am not familiar with Qt at all, so there are still some warnings when compiling

2. Not quite sure it's 100% working, but I can open it now; Need more testing

3. After opening the program, need to set PAML (where you can find bin/mamatree) in Tools > configuration

4. Good luck !!!

## Original author

    Bo Xu of Institute of Zoology, Chinese Academy of Sciences in Beijing

## Revision

    卢福浩(Fu-Hao Lu)
    
    Professor, PhD
    
    作物逆境适应与改良国家重点实验室，生命科学学院
    
    State Key Labortory of Crop Stress Adaptation and Improvement
    
    College of Life Science
    
    河南大学金明校区
    
    Jinming Campus, Henan University
    
    开封 475004， 中国
    
    Kaifeng 475004, P.R.China
    
    E-mail: LUFUHAO@HENU.EDU.CN
