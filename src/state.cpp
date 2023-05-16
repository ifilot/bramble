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

#include "state.h"

State::State(const std::string& filename) {
    GeometryReader gr;
    gr.read_file(filename);

    this->cell = gr.get_unitcell();
    this->atoms = gr.get_atom_positions();
    this->elements = gr.get_elements();
    this->periodic = gr.is_periodic();

    this->create_supercell();

    std::cout << "Calculating interatomic distances..." << std::endl;
    this->calculate_distances();
    std::cout << "Completed calculation of interatomic distances." << std::endl;
}

/**
 * @brief      Creates a super cell (3x3x3 the normal cell)
 */
void State::create_supercell(){
    this->supercell = 3 * this->cell;

    this->atoms_supercell.resize(this->atoms.size() * 27);

    #pragma omp parallel for
    for(unsigned int i=0; i<this->atoms.size(); i++) {
        this->atoms_supercell[i] = this->atoms[i];
    }

    unsigned int row = this->atoms.size();
    for(float x=-1; x<=1; x++) {
        for(float y=-1; y<=1; y++) {
            for(float z=-1; z<=1; z++) {
                if(x==0 && y==0 && z==0){
                    continue;
                }
                Vec3 t = this->cell.transpose() * Vec3(x,y,z);
                for(unsigned int n=0; n<this->atoms.size(); n++){
                    this->atoms_supercell[row] = this->atoms[n] + t;
                    row++;
                }
            }
        }
    }
}

/**
 * @brief      calculate all distances between the atoms
 */
void State::calculate_distances() {
    // determine size
    const size_t nr_atoms_cell = this->get_nr_atoms();
    const size_t nr_atoms_supercell = this->get_nr_atoms_supercell();

    size_t dd = nr_atoms_supercell;
    if(nr_atoms_supercell > State::MAX_SIZE_ATOMS_UNITCELL || !this->is_periodic()) {
        dd = nr_atoms_cell;
    }

    unsigned long long memsize = dd * dd * sizeof(float);
    if(memsize > (unsigned long long)(17179869184)) {
        throw std::runtime_error("This operation requires more than 16GB of memory. For safety reasons, this operation is terminated");
    }

    // expand matrix
    this->distances.resize(dd, dd);

    // collect all distances
    #pragma omp parallel for
    for(unsigned int i=0; i<dd; i++) {
        const auto& pos1 = this->get_atom_pos_cartesian(i);
        for(unsigned int j=i; j<dd; j++) {
            const auto& pos2 = this->get_atom_pos_cartesian(j);
            const float dist = (pos1 - pos2).norm();
            this->distances(i,j) = dist;
            this->distances(j,i) = dist;
        }
    }
}
