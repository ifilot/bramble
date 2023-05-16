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

#define BOOST_TEST_MODULE test_cna
#include <boost/test/unit_test.hpp>

#include "cna.h"

// check that we can read .geo files
BOOST_AUTO_TEST_CASE(test_cna) {
    BOOST_TEST_MESSAGE( "Testing executing CNA..." );

    // create input pattern class
    const auto pl = std::make_shared<PatternLibrary>("patterns.json");

    // read file and setup State for different file formats
    const auto state = std::make_shared<State>("POSCAR");

    // calculate CNA fingerprint
    CNA cna(pl);
    cna.analyze(state);

    for(unsigned int id : {5,6,7,8,54,55,56,57}) {
        BOOST_CHECK(cna.get_fingerprint(id) == "3(4,2,1)6(3,1,1)");
    }

    for(unsigned int id = 9; id < 54; id++) {
        BOOST_CHECK(cna.get_fingerprint(id) == "12(4,2,1)");
    }
}
