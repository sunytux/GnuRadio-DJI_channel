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
#include "sender_impl.h"

namespace gr {
  namespace DJI {

    sender::sptr
    sender::make()
    {
      return gnuradio::get_initial_sptr
        (new sender_impl());
    }

    /*
     * The private constructor
     */
    sender_impl::sender_impl()
      : gr::block("sender",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("in"));
      message_port_register_out(pmt::mp("out"));

      set_msg_handler(pmt::mp("in"), boost::bind( &sender_impl::handle_pdu, this, _1) );
    }

    /*
     * Our virtual destructor.
     */
    sender_impl::~sender_impl()
    {}

    void sender_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {}

    int sender_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      consume_each (noutput_items);
      return noutput_items;
    }

    void sender_impl::handle_pdu(pmt::pmt_t pdu)
    {
      pmt::pmt_t meta = pmt::intern("SCENARIO_ID");
      pmt::pmt_t msg = pmt::mp("TAKE_OFF");
      message_port_pub(pmt::mp("out"), pmt::cons(meta, msg));
    }

  } /* namespace DJI */
} /* namespace gr */

