/* -*- c++ -*- */

#define DJI_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "DJI_swig_doc.i"

%{
#include "DJI/sender.h"
#include "DJI/aircraft.h"
%}


%include "DJI/sender.h"
GR_SWIG_BLOCK_MAGIC2(DJI, sender);
%include "DJI/aircraft.h"
GR_SWIG_BLOCK_MAGIC2(DJI, aircraft);
