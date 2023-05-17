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

#ifndef _GEOMETRY_READER
#define _GEOMETRY_READER

#include "config.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <exception>
#include <filesystem>

#include <Eigen/Dense>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/math/constants/constants.hpp>

#include "math.h"

class GeometryReader {
private:
    MatrixUnitcell unitcell;                // unit cell matrix
    std::vector<std::string> elements;      // atomic elements
    std::vector<Vec3> atom_positions;       // position of the atoms
    bool periodic = false;                  // whether unit cell is periodic

public:
    GeometryReader();

    void read_file(const std::string& filename, bool centerstruc = true);

    void read_geo(const std::string& filename, bool centerstruc = true);

    void read_xyz(const std::string& filename, bool centerstruc = true);

    void read_poscar(const std::string& filename, bool centerstruc = true);

    inline const auto& get_unitcell() const {
        return this->unitcell;
    }

    inline const std::vector<std::string>& get_elements() const {
        return this->elements;
    }

    inline const auto& get_atom_positions() const {
        return this->atom_positions;
    }

    inline unsigned int get_nr_atoms() const {
        return this->atom_positions.size();
    }

    inline bool is_periodic() const {
        return this->periodic;
    }

private:
    /**
     * @brief      find the center of mass of the particle (assume equal mass)
     *
     * @return     center of mass of the particle
     */
    Vec3 find_center_of_mass();

    /**
     * @brief      find larges absolute x, y and z value
     *
     * @return     largest absolute x,y,z as vector
     */
    Vec3 find_max_coordinates();

};
#endif // _GEOMETRY_READER
