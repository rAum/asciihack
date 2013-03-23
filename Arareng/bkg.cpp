#include "apep_engine.hpp"

unsigned char rock[] = 
"                                          ____                         "
"                                         d8888b                        "
"                                         888888                        "
"            ___                        _d88888P                        "
"           d888b___                   d8888888_                        "
"           Y8888888b_____             888888888b                       "
"            8888888888888b_ __        Y888888888                       "
"          __888888888888888888b      _d88888888P                       "
"         d888888888888888888888_____d8888888_______        _____       "
"    ____d888888888888888888888888888888888888888888        88888b____  "
"___d88888888888888888888888888888888888888888888888b     _88888888888__"
"8888888888888888888888888888888888888888888888888888b __d88888888888888"
"88888888888888888888888888888888888888888888888888888888888888888888888"
"88888888888888888888888888888888888888888888888888888888888888888888888"
"88888888888888888888888888888888888888888888888888888888888888888888888"
"8888888888888888888888888888888888888888888888888888888888888888888888P";
unsigned char rockColor[sizeof(rock)];
unsigned char rockColor2[sizeof(rock)];

unsigned char cloud[] =
"       .-.       "
"    ,-(  _)-.    "
" .-(_    (_  )-. "
"(__            _)"
"   `-(______)-'  ";
unsigned char cloudColor[sizeof(cloud)]; 

unsigned char tree[] = 
"   ###o   "
" #o##/o#o "
"####\\###o#"
"##  {}  # "
"    {]    "
"    [}    "
"    []    ";
static const unsigned char g = FOREGROUND_GREEN | BACKGROUND_GREEN;
static const unsigned char k = FOREGROUND_RED;
unsigned char treeColor[] = {
    0, 0, 0, g, g, g, g, 0, 0, 0,
    0, g, g, g, g, k, g, g, g, 0,
    g, g, g, g, k, g, g, 0, g, g,
    g, g, 0, 0, k, k, 0, 0, g, 0,
    0, 0, 0, 0, k, k, 0, 0, 0, 0,
    0, 0, 0, 0, k, k, 0, 0, 0, 0, 
    0, 0, 0, 0, k, k, 0, 0, 0, 0
};

unsigned char tree2[] = 
"   ###o   "
" #o##/o#o "
"o##o1###o#"
"# ##{}0###"
"  # {] # #"
"    [}    "
"    []    ";
unsigned char treeColor2[] = {
    0, 0, 0, g, g, g, g, 0, 0, 0,
    0, g, g, g, g, k, g, g, g, 0,
    g, g, g, g, k, g, g, 0, g, g,
    g, 0, g, g, k, k, g, g, g, g,
    0, 0, g, 0, k, k, 0, g, 0, g,
    0, 0, 0, 0, k, k, 0, 0, 0, 0, 
    0, 0, 0, 0, k, k, 0, 0, 0, 0
};