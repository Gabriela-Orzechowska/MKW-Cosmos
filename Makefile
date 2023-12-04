.SUFFIXES:

BUILD		:=	build
SOURCES		:=	code
DATA		:=	data  
INCLUDES	:=	include
CPPFILES := $(shell find $(SOURCEDIR) -name '*.cpp')

.PHONY:

