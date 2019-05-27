/* -*- c++ -*- */

#define DJILINK_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "djilink_swig_doc.i"

%{
#include "djilink/scenario.h"
#include "djilink/scenario_sender.h"
%}

%include "djilink/scenario.h"
GR_SWIG_BLOCK_MAGIC2(djilink, scenario);
%include "djilink/scenario_sender.h"
GR_SWIG_BLOCK_MAGIC2(djilink, scenario_sender);
