/* -*- c++ -*- */
/* 
 * Copyright 2018 gr-djilink author.
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
#include "scenario_impl.h"

using namespace DJI::OSDK; 

namespace gr {
  namespace djilink {

    scenario::sptr
    scenario::make()
    {
      return gnuradio::get_initial_sptr
        (new scenario_impl());
    }

    /*
     * The private constructor
     */
    scenario_impl::scenario_impl()
      : gr::block("scenario",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("scenario_msg"));
      set_msg_handler(pmt::mp("scenario_msg"), boost::bind( &scenario_impl::handle_pdu, this, _1) );

      char timeoutString[] = "/home/sami/docs/phd/projects/03_hackathon_2018-03/code/dji_sample/UserConfig.txt";
      char *argv[] = { timeoutString, timeoutString, timeoutString}; // WTF?

      // LinuxSetup linuxEnvironment(2, argv); 
      // vehicle = linuxEnvironment.getVehicle();
      vehicle = NULL;

      std::cout << vehicle << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    scenario_impl::~scenario_impl()
    {
    }

    void
    scenario_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    scenario_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      // const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
      // <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

   void 
   scenario_impl::handle_pdu(pmt::pmt_t pdu)
   {
        pmt::pmt_t key = pmt::car(pdu);
        pmt::pmt_t scenarioId = pmt::cdr(pdu);
      

        std::cout << "Received message:\n\t- key: " << key << "\n\t- msg: " << scenarioId << std::endl;
        
        // std::cout << vehicle << std::endl;
        // getBroadcastData(vehicle, 100);


        if (scenarioId == pmt::mp("TAKE_OFF")){
          /* code */
        }else if(scenarioId == pmt::mp("BROADCAST")){
          // getBroadcastData(vehicle, 1);
          std::cout << "Executing Broadcast" << std::endl;  
        }else{
          std::cout<< "Error: unknown command" << std::endl;
        }
    }

  } /* namespace djilink */
} /* namespace gr */

