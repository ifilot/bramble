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

#define BOOST_TEST_MODULE test_card_manager
#include <boost/test/unit_test.hpp>

#include "card_manager.h"

BOOST_AUTO_TEST_CASE(test_card_manager) {
    BOOST_TEST_MESSAGE( "Testing CUDA card manager..." );

    CardManager cm;
    const int num_gpus = cm.get_num_gpus();
    BOOST_CHECK_GE(num_gpus, 0);

    if(num_gpus > 0) {
        BOOST_CHECK_GT(cm.get_memory_device(0), 0);
    } else {
        BOOST_CHECK_EQUAL(cm.get_memory_device(0), -1);
    }

    BOOST_CHECK_EQUAL(cm.get_memory_device(static_cast<unsigned int>(num_gpus)), -1);
}
