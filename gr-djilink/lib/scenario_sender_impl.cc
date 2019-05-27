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
#include "scenario_sender_impl.h"

namespace gr {
  namespace djilink {

    scenario_sender::sptr
    scenario_sender::make()
    {
      return gnuradio::get_initial_sptr
        (new scenario_sender_impl());
    }

    /*
     * The private constructor
     */
    scenario_sender_impl::scenario_sender_impl()
      : gr::block("scenario_sender",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("scenario_msg2"));
      message_port_register_out(pmt::mp("scenario_msg"));

      set_msg_handler(pmt::mp("scenario_msg2"), boost::bind( &scenario_sender_impl::handle_pdu, this, _1) );
    }

    /*
     * Our virtual destructor.
     */
    scenario_sender_impl::~scenario_sender_impl()
    {
    }

    void
    scenario_sender_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    scenario_sender_impl::general_work (int noutput_items,
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
   scenario_sender_impl::handle_pdu(pmt::pmt_t pdu)
   {
      pmt::pmt_t meta = pmt::intern("SCENARIO_ID");
      pmt::pmt_t msg = pmt::mp("BROADCAST");
      message_port_pub(pmt::mp("scenario_msg"), pmt::cons(meta, msg));
      std::cout << "Message sent from sender" << std::endl;
   }

  } /* namespace djilink */
} /* namespace gr */

