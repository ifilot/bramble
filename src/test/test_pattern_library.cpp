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

#define BOOST_TEST_MODULE pattern_library
#include <boost/test/unit_test.hpp>

#include "pattern_library.h"

// check that we can modify pattern library
BOOST_AUTO_TEST_CASE(test_pattern_library) {
    BOOST_TEST_MESSAGE( "Testing pattern library..." );

    PatternLibrary pl;  // create empty library

    // test finding default pattern
    auto got = pl.get_pattern_by_key("unknown");
    BOOST_CHECK(got.get_name() == "Unknown");

    // test adding pattern
    pl.add_pattern("fcc_bulk", "12(4,2,1)", "FCC Bulk", "0b3d05");
    auto fccbulkpattern = pl.get_pattern("12(4,2,1)");
    auto fccbulkkey = pl.get_pattern_by_key("fcc_bulk");
    BOOST_CHECK(fccbulkpattern.get_name() == fccbulkkey.get_name());
    BOOST_CHECK(fccbulkpattern.get_color() == fccbulkkey.get_color());
    BOOST_CHECK(fccbulkpattern.get_fingerprint() == fccbulkkey.get_fingerprint());

    // test editing a pattern
    pl.edit_pattern("fcc_bulk", "12(4,2,2)", "FCC BULK", "0b3d08");
    fccbulkpattern = pl.get_pattern("12(4,2,2)");
    fccbulkkey = pl.get_pattern_by_key("fcc_bulk");

    BOOST_CHECK(fccbulkpattern.get_name() == fccbulkkey.get_name());
    BOOST_CHECK(fccbulkpattern.get_color() == fccbulkkey.get_color());
    BOOST_CHECK(fccbulkpattern.get_fingerprint() == fccbulkkey.get_fingerprint());
    BOOST_CHECK(fccbulkpattern.get_name() == "FCC BULK");
    BOOST_CHECK(fccbulkpattern.get_color() == "0b3d08");
    BOOST_CHECK(fccbulkpattern.get_fingerprint() == "12(4,2,2)");

    // test throw error for false pattern
    BOOST_CHECK_THROW(pl.add_pattern("fcc_bulk", "12(4,2,1)a", "FCC Bulk", "0b3d05"), std::runtime_error);
    BOOST_CHECK_THROW(pl.edit_pattern("fcc_bulk", "12(4,2,1)a", "FCC Bulk", "0b3d05"), std::runtime_error);
    BOOST_CHECK_THROW(pl.edit_pattern("non_existing_key", "12(4,2,1)", "FCC Bulk", "0b3d05"), std::runtime_error);
    BOOST_CHECK_THROW(pl.add_pattern("invalid key", "12(4,2,1)", "FCC Bulk", "0b3d05"), std::runtime_error);
    BOOST_CHECK_THROW(pl.add_pattern("invalid key", "12(4,2,1)", "FCC Bulk", "40b3d05"), std::runtime_error);
    BOOST_CHECK_THROW(pl.add_pattern("valid_key", "12(4,2,1)", "FCC Bulk", "40b3d05"), std::runtime_error);
    BOOST_CHECK_THROW(pl.add_pattern("valid_key", "12(4,2,1)", "FCC Bulk", "0b305"), std::runtime_error);
}
