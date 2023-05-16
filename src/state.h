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

#ifndef _STATE_H
#define _STATE_H

#include <iostream>
#include <string>
#include <vector>

#include "geometry_reader.h"
#include "math.h"

class State {
private:
    MatrixUnitcell cell;                  // matrix spanning the unit cell
    MatrixUnitcell supercell;             // cell with 3x3x3 the size of the normal cell
    std::vector<Vec3> atoms;              // atom positions in the unit cell
    std::vector<Vec3> atoms_supercell;    // atom positions in the super cell (starts with the positions of the original cell)
    std::vector<std::string> elements;    // atomic elements

    MatrixXXf distances; // distances between all atom

    bool periodic;

public:
    static const size_t MAX_SIZE_ATOMS_UNITCELL = 1000000000;

    State(const std::string& filename);

    inline bool is_periodic() const {
        return this->periodic;
    }

    inline unsigned int get_nr_atoms() const {
        return this->atoms.size();
    }

    inline unsigned int get_nr_atoms_supercell() const {
        return this->atoms_supercell.size();
    }

    /**
     * @brief      Gets the atom position (cartesian)
     *
     * @param[in]  n     id of atom
     *
     * @return     The atom position
     */
    inline const Vec3& get_atom_pos_cartesian(unsigned int n) const {
        return this->atoms_supercell[n];
    }

    inline const auto& get_distances() {
        return this->distances;
    }

    inline const auto& get_elements() {
        return this->elements;
    }

private:
    void create_supercell();
    void calculate_distances();

};
#endif //_STATE_H
