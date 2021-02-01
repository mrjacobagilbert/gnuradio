/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_PDU_TO_TAGGED_STREAM_H
#define INCLUDED_PDU_PDU_TO_TAGGED_STREAM_H

#include <gnuradio/pdu/api.h>
#include <gnuradio/pdu/pdu.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace pdu {

/*!
 * \brief Turns received PDUs into a tagged stream of items
 * \ingroup message_tools_blk
 */
class PDU_API pdu_to_tagged_stream : virtual public tagged_stream_block
{
public:
    // gr::pdu::pdu_to_tagged_stream::sptr
    typedef std::shared_ptr<pdu_to_tagged_stream> sptr;

    /*!
     * \brief Construct a pdu_to_tagged_stream block
     * \param type PDU type of pdu::vector_type
     * \param lengthtagname The name of the tag that specifies how long the packet is.
     *                      Defaults to 'packet_len'.
     */
    static sptr make(pdu::vector_type type,
                     const std::string& lengthtagname = "packet_len");
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_PDU_TO_TAGGED_STREAM_H */