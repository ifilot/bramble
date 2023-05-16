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

#define BOOST_TEST_MODULE test_poscar
#include <boost/test/unit_test.hpp>

#include "geometry_reader.h"

// check that we can read .geo files
BOOST_AUTO_TEST_CASE(test_poscar) {
    BOOST_TEST_MESSAGE( "Testing reading POSCAR file..." );
    GeometryReader gr;
    gr.read_file("POSCAR");
    BOOST_CHECK(gr.get_nr_atoms() != 42);
    BOOST_TEST(gr.get_nr_atoms(), 65);

    // check that unit cell is valid
    const auto& unitcell = gr.get_unitcell();
    BOOST_TEST(unitcell(0,0) == 7.4640000000000004,
        boost::test_tools::tolerance(1e-7));
    BOOST_TEST(unitcell(1,0) == 3.7320000000000002,
        boost::test_tools::tolerance(1e-7));
    BOOST_TEST(unitcell(1,1) == 6.4638200000000001,
        boost::test_tools::tolerance(1e-7));
    BOOST_TEST(unitcell(2,2) == 27.1883699999999990,
        boost::test_tools::tolerance(1e-7));

    // check one of the atoms
    const auto& pos = gr.get_atom_positions()[58];
    BOOST_TEST(pos[0] == 3.696113,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos[1] == 2.134227,
        boost::test_tools::tolerance(1e-4));
    BOOST_TEST(pos[2] == 19.743038,
        boost::test_tools::tolerance(1e-4));

    // check if gr flags this input file as periodic
    BOOST_CHECK(gr.is_periodic());
}
