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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "aircraft_impl.h"

namespace gr {
  namespace DJI {

    aircraft::sptr
    aircraft::make()
    {
      return gnuradio::get_initial_sptr
        (new aircraft_impl());
    }

    /*
     * The private constructor
     */
    aircraft_impl::aircraft_impl()
      : gr::block("aircraft",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      char timeoutString[] = "/home/sami/docs/phd/projects/03_hackathon_2018-03/code/dji_sample/UserConfig.txt"; char *argv[] = { timeoutString, timeoutString, timeoutString };

      linuxEnvironment = new LinuxSetup(2, argv);
      Vehicle *vehicle = linuxEnvironment->getVehicle();
      if (vehicle == NULL)
        std::cout << "Vehicle not initialized/exiting.\n";
      else
        manager = VehicleManager(vehicle);
      message_port_register_in(pmt::mp("in"));
      set_msg_handler(pmt::mp("in"), boost::bind( &aircraft_impl::handle_pdu, this, _1) );
    }

    /*
     * Our virtual destructor.
     */
    aircraft_impl::~aircraft_impl()
    {
      delete linuxEnvironment;
    }

    void aircraft_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {}

    int aircraft_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      consume_each (noutput_items);
      return noutput_items;
    }

    void aircraft_impl::handle_pdu(pmt::pmt_t pdu) 
    {
        pmt::pmt_t key = pmt::car(pdu);
        pmt::pmt_t scenarioId = pmt::cdr(pdu);

        lock.lock();
		if (scenarioId == pmt::mp("TAKE_OFF"))
			manager.makeSquare(10);
		else if(scenarioId == pmt::mp("BROADCAST"))
		{
			// getBroadcastData(vehicle, 1);
		}
		else
			std::cout<< "Error: unknown command" << std::endl;
		
		sleep(2);
        lock.unlock();
    }

  } /* namespace DJI */
} /* namespace gr */

