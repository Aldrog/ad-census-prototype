TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS +=                   \
    core                     \
    adcensus

CONFIG += c++11

core.file                     = core/core.pro
adcensus.file = adcensus/adcensus.pro
