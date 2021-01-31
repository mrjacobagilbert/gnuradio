#!/usr/bin/env python
#
# Copyright 2021 NTESS LLC.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt
import time
try:
    from gnuradio import pdu
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import pdu

class qa_pdu_split (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_split (self):
        split = pdu.pdu_split()
        d1 = blocks.message_debug()
        d2 = blocks.message_debug()

        self.tb.msg_connect((split, 'dict'), (d1, 'store'))
        self.tb.msg_connect((split, 'data'), (d2, 'store'))

        in_meta1 = pmt.dict_add(pmt.make_dict(), pmt.intern('num'), pmt.from_long(4))
        in_meta2 = pmt.dict_add(pmt.make_dict(), pmt.intern('n'), pmt.from_long(99))
        in_pdu = pmt.cons(in_meta1, pmt.init_u8vector(6, range(6)))

        self.tb.start()
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), pmt.intern("MALFORMED PDU"))
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(2, range(2))))
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), pmt.cons(in_meta2, pmt.init_u8vector(0, [])))
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), in_pdu)
        split.to_basic_block()._post(
            pmt.intern("system"), pmt.cons(
                pmt.intern("done"), pmt.from_long(1)))
        self.tb.wait()

        self.assertTrue(pmt.equal(d1.get_message(0), in_meta2))
        self.assertTrue(pmt.equal(d1.get_message(1), in_meta1))
        self.assertTrue(pmt.equal(d2.get_message(0), pmt.init_u8vector(2, range(2))))
        self.assertTrue(pmt.equal(d2.get_message(1), pmt.init_u8vector(6, range(6))))


    def test_002_pass_empty (self):
        split = pdu.pdu_split(True)
        d1 = blocks.message_debug()
        d2 = blocks.message_debug()
        
        self.tb.msg_connect((split, 'dict'), (d1, 'store'))
        self.tb.msg_connect((split, 'data'), (d2, 'store'))

        in_meta1 = pmt.dict_add(pmt.make_dict(), pmt.intern('num'), pmt.from_long(4))
        in_meta2 = pmt.dict_add(pmt.make_dict(), pmt.intern('n'), pmt.from_long(99))
        in_pdu = pmt.cons(in_meta1, pmt.init_u8vector(6, range(6)))

        self.tb.start()
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), pmt.intern("MALFORMED PDU"))
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(2, range(2))))
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), pmt.cons(in_meta2, pmt.init_u8vector(0, [])))
        time.sleep(.001)
        split.to_basic_block()._post(pmt.intern("pdus"), in_pdu)
        split.to_basic_block()._post(
            pmt.intern("system"), pmt.cons(
                pmt.intern("done"), pmt.from_long(1)))
        self.tb.wait()

        self.assertTrue(pmt.equal(d1.get_message(0), pmt.PMT_NIL))
        self.assertTrue(pmt.equal(d1.get_message(1), in_meta2))
        self.assertTrue(pmt.equal(d1.get_message(2), in_meta1))
        self.assertTrue(pmt.equal(d2.get_message(0), pmt.init_u8vector(2, range(2))))
        self.assertTrue(pmt.equal(d2.get_message(1), pmt.init_u8vector(0, [])))
        self.assertTrue(pmt.equal(d2.get_message(2), pmt.init_u8vector(6, range(6))))


if __name__ == '__main__':
    gr_unittest.run(qa_pdu_split)
