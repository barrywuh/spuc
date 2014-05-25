#ifndef SPUC_FARROW
#define SPUC_FARROW

/*
    Copyright (C) 2014 Tony Kirke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
// from directory: spuc_templates
#include <spuc/spuc_types.h>
#include <spuc/complex.h>
#include <spuc/smart_array.h>
namespace SPUC {
//! \file
//! \brief  Template Class for Farrow implementation of a ploynomial interpolation using a FIR  filter. 
//
//! \brief  Template Class for Farrow implementation of a ploynomial interpolation using a FIR  filter. 
//
//!   Farrow implementation of a polynomial interpolation
//!   This covers 3 (orders) cases of interpolation
//!   1. Linear (not efficient using this method)
//!   2. Piecewise-Parabolic
//!   3. Cubic
//!   Although derived from the Lagrange coefficients the
//!   multiplication factor have been simplified to allow
//!   simple VLSI implementations. see
//!   Interpolation in Digital Modem - Part II, IEEE Trans.
//!   on Comms. June 1993.
//! \author Tony Kirke
//! \image html farrow.png
//!  \ingroup templates fir interpolation
template <class Numeric> class farrow
{
 public: 
  long num_taps;
  smart_array<Numeric> coeff;
 protected:
  smart_array<Numeric> z; 
  Numeric output;
      
 public: 
  //! Get current output
  Numeric out() { return(output); }
  //! Clock in new sample & compute current output
  Numeric check(long i) { 
	SPUC_ASSERT(i<num_taps+1);
	return(z[i]); }     	
  //! Need to cover different orders!
  farrow(char order) : num_taps(order), coeff(num_taps+1), z(num_taps+2) {
	int i;
	SPUC_ASSERT((order > 0) & (order < 5));
	for (i=0;i<=num_taps;i++) {
	  z[i] = (Numeric)0;
	  coeff[i] = (Numeric)0;  
	}
  } 
  //! reset
  void reset(void) { 	
	  int i;
	  for (i=0;i<=num_taps;i++) z[i] = (Numeric)0;
	  output = (Numeric)0;
  }
  ~farrow(void) { 
  }
  //! Only input new sample 
  void input(Numeric in) {
	int i; 
	//! Update history of inputs
	for (i=num_taps+1;i>0;i--) z[i] = z[i-1];  
	//! Add new input
	z[0] = in;   
  }

  //!:
  //!   Update => Clock in new input sample, recalculate coefficients and
  //!   determine output
  Numeric update(Numeric in, float_type offset) {
	int i; 
	//! Update history of inputs
	for (i=num_taps+1;i>0;i--) z[i] = z[i-1];  
	//! Add new input
	z[0] = in;   
	//! Calculate coefficients
	if (num_taps == 1) return(in);
	else if (num_taps == 2) calculate_coeff2();
	else if (num_taps == 3) calculate_coeff3a();
	else if (num_taps == 4) calculate_coeff4();
	//! Perform FIR
	return(fir(offset));
}
//! Calculate output for current coefficients. Offset is between 0 and 1
Numeric fir(float_type offset) {
	//! Perform FIR
	float_type gain = offset;
	output = coeff[0];
	for (int i=1;i<num_taps;i++) {
	  output += (Numeric)(offset*coeff[i]);
	  offset *= gain;
	}
	return(output);
}
//! Rephase => recalculate coefficients and output for new offset (for upsampling)
Numeric rephase(float_type offset) {  
  if (num_taps == 1) return(z[0]);
  else if (num_taps == 2) calculate_coeff2();
  else if (num_taps == 3) calculate_coeff3a();
  else if (num_taps == 4) calculate_coeff4();
  return(fir(offset));
}
//! Calculate coefficients for linear (not tested)
void calculate_coeff2(void) {
	//! Calculate coefficients
	coeff[1] = z[0] - z[1];
	coeff[0] = z[1];
}
//! Calculate coefficients for square
void calculate_coeff3(void) {
	//! Calculate coefficients
  coeff[2] = z[0] - ((Numeric)2)*z[1] + z[2];
  coeff[1] = z[0] - z[2];
  coeff[0] = ((Numeric)2)*z[1];
}
//! Calculate coefficients for parabolic
void calculate_coeff3a(void) {
	//! Calculate coefficients
	coeff[2] = (z[0] - z[1] - z[2] + z[3]);
	coeff[1] = -z[0] + ((Numeric)3)*z[1] - z[2] - z[3];
	coeff[0] = ((Numeric)2)*z[2];
}
//! Calculate coefficients for 4th order
void calculate_coeff4(void) {
	coeff[3] = -z[3] + ((Numeric)3)*z[2] - ((Numeric)3)*z[1] + z[0]; 
	coeff[2] = ((Numeric)3)*z[3] - ((Numeric)6)*z[2] + ((Numeric)3)*z[1]; 
	coeff[1] = ((Numeric)-2)*z[3] - ((Numeric)3)*z[2] + ((Numeric)6)*z[1] - z[0];	
	coeff[0] = ((Numeric)6)*z[2];
}
};    
} // namespace SPUC
#endif