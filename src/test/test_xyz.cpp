/**************************************************************************
 *                                                                        *
 *   Author: Ivo Filot <i.a.w.filot@tue.nl>                               *
 *                                                                        *
 *   BRAMBLE is free software:                                            *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   BRAMBLE is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#define BOOST_TEST_MODULE test_xyz
#include <boost/test/unit_test.hpp>

#include "geometry_reader.h"

// check that we can read .xyz files
BOOST_AUTO_TEST_CASE(test_xyz) {
    BOOST_TEST_MESSAGE( "Testing reading .xyz file..." );
    GeometryReader gr;
    gr.read_file("co_np.xyz", false);
    BOOST_CHECK(gr.get_nr_atoms() == (unsigned int)1728);
    BOOST_TEST(gr.get_atom_positions()[0][0] == 40.0068,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(gr.get_atom_positions()[0][1] == 46.932,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(gr.get_atom_positions()[0][2] == 37.4955,
        boost::test_tools::tolerance(1e-4));

    // check if gr flags this input file *not* as periodic
    BOOST_CHECK(!gr.is_periodic());
}
