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

#define BOOST_TEST_MODULE test_fingerprint
#include <boost/test/unit_test.hpp>

#include "pattern_library.h"

// basic check to see if the test module works
BOOST_AUTO_TEST_CASE(test_fingerprint) {
    BOOST_TEST_MESSAGE( "Testing fingerprints..." );

    PatternLibrary pl;

    // check that all patterns in file are valid
    for(const auto& item : pl.get_patterns()) {
        BOOST_TEST(pl.is_valid_pattern(item.second.get_fingerprint()));
    }

    // test that these patterns are invalid
    BOOST_TEST(!pl.is_valid_pattern("invalid"));
    BOOST_TEST(!pl.is_valid_pattern("0(1,2,3)"));
    BOOST_TEST(!pl.is_valid_pattern("12(1,2,0)(1,2,0)"));
    BOOST_TEST(!pl.is_valid_pattern("12(1,2,0)0"));
    BOOST_TEST(!pl.is_valid_pattern("12(1,2,0)a12(1,2,0)"));
    BOOST_TEST(!pl.is_valid_pattern("12(1,2,0) 12(1,2,0)"));

    // also test keys
    for(const auto& item : pl.get_patterns()) {
        BOOST_TEST(pl.is_valid_key(item.second.get_key()));
    }

    // and colors
    for(const auto& item : pl.get_patterns()) {
        BOOST_TEST(pl.is_valid_colorcode(item.second.get_color()));
    }
}
