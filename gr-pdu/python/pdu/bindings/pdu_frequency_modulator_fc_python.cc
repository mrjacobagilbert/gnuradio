/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(pdu_frequency_modulator_fc.h) */
/* BINDTOOL_HEADER_FILE_HASH(135c18b3d338d2db6ada108bcf1d6f45)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/pdu/pdu_frequency_modulator_fc.h>
// pydoc.h is automatically generated in the build directory
#include <pdu_frequency_modulator_fc_pydoc.h>

void bind_pdu_frequency_modulator_fc(py::module& m)
{

    using pdu_frequency_modulator_fc = ::gr::pdu::pdu_frequency_modulator_fc;


    py::class_<pdu_frequency_modulator_fc,
               gr::block,
               gr::basic_block,
               std::shared_ptr<pdu_frequency_modulator_fc>>(
        m, "pdu_frequency_modulator_fc", D(pdu_frequency_modulator_fc))

        .def(py::init(&pdu_frequency_modulator_fc::make),
             py::arg("sensitivity"),
             py::arg("taps"),
             D(pdu_frequency_modulator_fc, make))


        .def("set_sensitivity",
             &pdu_frequency_modulator_fc::set_sensitivity,
             py::arg("sensitivity"),
             D(pdu_frequency_modulator_fc, set_sensitivity))


        .def("set_taps",
             &pdu_frequency_modulator_fc::set_taps,
             py::arg("taps"),
             D(pdu_frequency_modulator_fc, set_taps))

        ;
}
