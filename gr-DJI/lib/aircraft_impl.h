/* -*- c++ -*- */
/* 
 * Copyright 2019 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DJI_AIRCRAFT_IMPL_H
#define INCLUDED_DJI_AIRCRAFT_IMPL_H

#include <mutex>
#include <DJI/aircraft.h>
#include "VehicleManager.h"
#include "./djiTools/dji_linux_helpers.hpp"

namespace gr {
  namespace DJI {

    class aircraft_impl : public aircraft
    {
     private:
      LinuxSetup* linuxEnvironment;
      VehicleManager manager;
      std::mutex lock;

     public:
      aircraft_impl();
      ~aircraft_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void handle_pdu(pmt::pmt_t pdu);
    };

  } // namespace DJI
} // namespace gr

#endif /* INCLUDED_DJI_AIRCRAFT_IMPL_H */

