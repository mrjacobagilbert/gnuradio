/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qtgui_freq_sink_c.h>
#include <gr_io_signature.h>
#include <string.h>

#include <QTimer>
#include <volk/volk.h>

qtgui_freq_sink_c_sptr
qtgui_make_freq_sink_c(int fftsize, int wintype,
		       double fc, double bw,
		       const std::string &name,
		       QWidget *parent)
{
  return gnuradio::get_initial_sptr
    (new qtgui_freq_sink_c(fftsize, wintype,
			   fc, bw, name,
			   parent));
}

qtgui_freq_sink_c::qtgui_freq_sink_c(int fftsize, int wintype,
				     double fc, double bw,
				     const std::string &name,
				     QWidget *parent)
  : gr_sync_block("freq_sink_c",
		  gr_make_io_signature(1, -1, sizeof(gr_complex)),
		  gr_make_io_signature(0, 0, 0)),
    d_fftsize(fftsize),
    d_wintype((gr_firdes::win_type)(wintype)),
    d_center_freq(fc), d_bandwidth(bw), d_name(name),
    d_parent(parent)
{
  d_main_gui = NULL;

  // Perform fftshift operation;
  // this is usually desired when plotting
  d_shift = true;

  d_fft = new gri_fft_complex(d_fftsize, true);

  d_nconnections = 1;

  d_index = 0;
  for(int i = 0; i < d_nconnections; i++) {
    d_residbufs.push_back(new gr_complex[d_fftsize]);
    d_magbufs.push_back(new double[d_fftsize]);
  }

  buildwindow();

  initialize();
}

qtgui_freq_sink_c::~qtgui_freq_sink_c()
{
  for(int i = 0; i < d_nconnections; i++) {
    delete [] d_residbufs[i];
    delete [] d_magbufs[i];
  }
  delete d_fft;
}

void
qtgui_freq_sink_c::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned int ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++) {
    ninput_items_required[i] = std::min(d_fftsize, 8191);
  }
}

void
qtgui_freq_sink_c::initialize()
{
  if(qApp != NULL) {
    d_qApplication = qApp;
  }
  else {
    int argc=0;
    char **argv = NULL;
    d_qApplication = new QApplication(argc, argv);
  }

  d_main_gui = new FreqDisplayForm(1, d_parent);

  // initialize update time to 10 times a second
  set_update_time(0.1);
  d_last_time = 0;
}

void
qtgui_freq_sink_c::exec_()
{
  d_qApplication->exec();
}

QWidget*
qtgui_freq_sink_c::qwidget()
{
  return d_main_gui;
}

PyObject*
qtgui_freq_sink_c::pyqwidget()
{
  PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
  PyObject *retarg = Py_BuildValue("N", w);
  return retarg;
}

void
qtgui_freq_sink_c::set_frequency_range(const double centerfreq,
				       const double bandwidth)
{
  d_center_freq = centerfreq;
  d_bandwidth = bandwidth;
  d_main_gui->SetFrequencyRange(d_center_freq,
				-d_bandwidth/2.0,
				d_bandwidth/2.0);
}

void
qtgui_freq_sink_c::set_fft_power_db(double min, double max)
{
  d_main_gui->SetFrequencyAxis(min, max);
}

void
qtgui_freq_sink_c::set_update_time(double t)
{
  //convert update time to ticks
  gruel::high_res_timer_type tps = gruel::high_res_timer_tps();
  d_update_time = t * tps;
  d_main_gui->setUpdateTime(t);
}

void
qtgui_freq_sink_c::set_title(int which, const std::string &title)
{
  d_main_gui->setTitle(which, title.c_str());
}

void
qtgui_freq_sink_c::set_color(int which, const std::string &color)
{
  d_main_gui->setColor(which, color.c_str());
}

void
qtgui_freq_sink_c::set_resize(int width, int height)
{
  d_main_gui->resize(QSize(width, height));
}

void
qtgui_freq_sink_c::fft(float *data_out, const gr_complex *data_in, int size)
{
  if(d_window.size()) {
    volk_32fc_32f_multiply_32fc_a(d_fft->get_inbuf(), data_in, &d_window.front(), size);
  }
  else {
    memcpy(d_fft->get_inbuf(), data_in, sizeof(gr_complex)*size);
  }

  d_fft->execute();     // compute the fft
  volk_32fc_s32f_x2_power_spectral_density_32f_a(data_out, d_fft->get_outbuf(), size, 1.0, size);
}

void
qtgui_freq_sink_c::windowreset()
{
  //gr_firdes::win_type newwintype = (gr_firdes::win_type)d_main_gui->GetWindowType();
  //if(d_wintype != newwintype) {
  //  d_wintype = newwintype;
  //  buildwindow();
  //}
}

void
qtgui_freq_sink_c::buildwindow()
{
  d_window.clear();
  if(d_wintype != 0) {
    d_window = gr_firdes::window(d_wintype, d_fftsize, 6.76);
  }
}

void
qtgui_freq_sink_c::fftresize()
{
  int newfftsize = d_fftsize;

  if(newfftsize != d_fftsize) {

    // Resize residbuf and replace data
    for(int i = 0; i < d_nconnections; i++) {
      delete [] d_residbufs[i];
      delete [] d_magbufs[i];

      d_residbufs.push_back(new gr_complex[newfftsize]);
      d_magbufs.push_back(new double[newfftsize]);
    }

    // Set new fft size and reset buffer index 
    // (throws away any currently held data, but who cares?) 
    d_fftsize = newfftsize;
    d_index = 0;

    // Reset window to reflect new size
    buildwindow();

    // Reset FFTW plan for new size
    delete d_fft;
    d_fft = new gri_fft_complex(d_fftsize, true);
  }
}

int
qtgui_freq_sink_c::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  int j=0;
  const gr_complex *in = (const gr_complex*)input_items[0];

  // Update the FFT size from the application
  fftresize();
  windowreset();

  for(int i=0; i < noutput_items; i+=d_fftsize) {
    unsigned int datasize = noutput_items - i;
    unsigned int resid = d_fftsize-d_index;

    // If we have enough input for one full FFT, do it
    if(datasize >= resid) {

      // Fill up residbuf with d_fftsize number of items
      memcpy(d_residbufs[0]+d_index, &in[j], sizeof(gr_complex)*resid);
      float *fbuf = new float[d_fftsize];
      fft(fbuf, d_residbufs[0], d_fftsize);
      for(int x=0; x < d_fftsize; x++)
	d_magbufs[0][x] = (double)fbuf[x];
      
      if(gruel::high_res_timer_now() - d_last_time > d_update_time) {
	d_last_time = gruel::high_res_timer_now();
	d_qApplication->postEvent(d_main_gui,
				  new FreqUpdateEvent(d_magbufs, d_fftsize));
      }

      d_index = 0;
      j += resid;
    }
    // Otherwise, copy what we received into the residbuf for next time
    else {
      memcpy(d_residbufs[0]+d_index, &in[j], sizeof(gr_complex)*datasize);
      d_index += datasize;
      j += datasize;
    }
  }

  return noutput_items;
}
