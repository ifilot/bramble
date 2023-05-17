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

#define BOOST_TEST_MODULE test_geo
#include <boost/test/unit_test.hpp>

#include "geometry_reader.h"

// check that we can read .geo files
BOOST_AUTO_TEST_CASE(test_geo) {
    BOOST_TEST_MESSAGE( "Testing reading .geo file..." );
    GeometryReader gr;
    gr.read_file("co_np.geo", false);
    BOOST_TEST(gr.get_nr_atoms(), 3375);

    // check that unit cell is valid
    const auto& unitcell = gr.get_unitcell();
    BOOST_TEST(unitcell(0,0) == 100.0000,
        boost::test_tools::tolerance(1e-7));
    BOOST_TEST(unitcell(1,1) == 100.0000,
        boost::test_tools::tolerance(1e-7));
    BOOST_TEST(unitcell(2,2) == 100.0000,
        boost::test_tools::tolerance(1e-7));

    // check some atoms
    const auto& pos1 = gr.get_atom_positions()[0];
    BOOST_TEST(pos1[0] == 35.16227056697313,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos1[1] == 32.75539286329577,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos1[2] == 37.79392834721789,
        boost::test_tools::tolerance(1e-4));

    const auto& pos2 = gr.get_atom_positions()[57];
    BOOST_TEST(pos2[0] == 35.65895658526478,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos2[1] == 42.03564094395121,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos2[2] == 58.94019105246535,
        boost::test_tools::tolerance(1e-4));

    const auto& pos3 = gr.get_atom_positions()[3374];
    BOOST_TEST(pos3[0] == 54.26085294185137,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos3[1] == 60.13996067611079,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos3[2] == 62.75244677076046,
        boost::test_tools::tolerance(1e-4));

    // check if gr flags this input file as periodic
    BOOST_CHECK(!gr.is_periodic());
}
