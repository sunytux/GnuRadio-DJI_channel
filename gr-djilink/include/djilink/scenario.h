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


#ifndef INCLUDED_DJILINK_SCENARIO_H
#define INCLUDED_DJILINK_SCENARIO_H

#include <djilink/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace djilink {

    /*!
     * \brief <+description of block+>
     * \ingroup djilink
     *
     */
    class DJILINK_API scenario : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<scenario> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of djilink::scenario.
       *
       * To avoid accidental use of raw pointers, djilink::scenario's
       * constructor is in a private implementation
       * class. djilink::scenario::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace djilink
} // namespace gr

#endif /* INCLUDED_DJILINK_SCENARIO_H */

