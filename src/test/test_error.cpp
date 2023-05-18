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

#define BOOST_TEST_MODULE test_error
#include <boost/test/unit_test.hpp>

#include "geometry_reader.h"
#include "pattern_library.h"

// check that an error is thrown when trying to read a file that does not exist
BOOST_AUTO_TEST_CASE(test_error) {
    BOOST_TEST_MESSAGE( "Testing error throwing..." );

    // check whether an error is thrown for non-existent file for geometry reader
    GeometryReader gr;
    BOOST_CHECK_THROW(gr.read_file("nonexistent_file"), std::runtime_error);

    // check whether an error is thrown for non-existent file for pattern library
    BOOST_CHECK_THROW(PatternLibrary pl("nonexistent_file"), std::runtime_error);
}
