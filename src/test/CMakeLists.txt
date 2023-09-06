 #*************************************************************************
 #   CMakeLists.txt  --  This file is part of BRAMBLE.                    *
 #                                                                        *
 #   Author: Ivo Filot <i.a.w.filot@tue.nl>                               *
 #                                                                        *
 #   BRAMBLE is free software: you can redistribute it and/or modify      *
 #   it under the terms of the GNU General Public License as published    *
 #   by the Free Software Foundation, either version 3 of the License,    *
 #   or (at your option) any later version.                               *
 #                                                                        *
 #   BRAMBLE is distributed in the hope that it will be useful,           *
 #   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 #   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 #   See the GNU General Public License for more details.                 *
 #                                                                        *
 #   You should have received a copy of the GNU General Public License    *
 #   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 #                                                                        *
 #*************************************************************************/

# auto-generate main function
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

#######################################################
# define tests
#######################################################
SET(TESTS
    test_basic
    test_geo
    test_xyz
    test_error
    test_poscar
    test_cna
    test_fingerprint
    test_pattern_library
    test_similarity
)

foreach(testexec ${TESTS})
    add_executable(${testexec} ${testexec}.cpp)
    target_link_libraries(${testexec}
                          bramblesources
                          ${Boost_LIBRARIES}
                          ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                          )
    target_include_directories(${testexec} PRIVATE ${Boost_INCLUDE_DIRS})
    add_test(NAME ${testexec} COMMAND ${testexec} --run_test=${testexec} --catch_system_error=yes)
    message("Adding test: ${testexec}")
endforeach()

# some test require linking to cuda files
if(MOD_CUDA)
    target_link_libraries(test_similarity
                          bramblesources
                          cuda_files
                          ${Boost_LIBRARIES}
                          ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                          )
endif()

#######################################################
# add testinput files
#######################################################
configure_file(data/co_np.xyz co_np.xyz COPYONLY)
configure_file(data/co_np.geo co_np.geo COPYONLY)
configure_file(data/POSCAR POSCAR COPYONLY)
configure_file(data/POSCAR_Rh111 POSCAR_Rh111 COPYONLY)
configure_file(data/mat1.txt mat1.txt COPYONLY)
configure_file(data/mat2.txt mat2.txt COPYONLY)
configure_file(../../patterns/patterns.json patterns.json COPYONLY)
