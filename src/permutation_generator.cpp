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

#include "permutation_generator.h"

/**
 * @brief      construct permutation generator object
 *
 * @param[in]  _sz   matrix dimension of permutation matrix
 */
PermutationGenerator::PermutationGenerator(unsigned int _sz) :
    sz(_sz) {
    if(this->sz > 13) {
        throw std::runtime_error("Exchange patterns larger than 12 are not allowed");
    }

    this->generate_permutations();
    this->get_permutation_vector();
}

/**
 * @brief      get the permutation matrix
 *
 * @param[in]  pid   permutation index
 *
 * @return     the permutation matrix
 */
MatrixXXu PermutationGenerator::get_permutation_matrix(unsigned int pid, unsigned int _sz) const {
    MatrixXXu ans = MatrixXXu::Zero(_sz, _sz);

    for(unsigned int i=0; i<this->sz; i++) {
        size_t idx = 0;
        if(i%2 == 0) {
            idx = this->exchanges[pid][i/2] >> 4;   // get upper four bits
        } else {
            idx = this->exchanges[pid][i/2] & 15;   // get lower four bits
        }

        ans(i,idx) = 1;
        ans(idx,i) = 1;
    }

    return ans;
}

/**
 * @brief      Gets the permutation vector.
 *
 * @return     The permutation vector.
 */
const std::vector<uint8_t>& PermutationGenerator::get_permutation_vector() {
    if(this->permvec.size() != 0) {
        return this->permvec;
    }

    for(unsigned int i=0; i<this->exchanges.size(); i++) {
        for(unsigned int j=0; j<this->sz; j++) {
            uint8_t idx = 0;
            if(j%2 == 0) {
                idx = this->exchanges[i][j/2] >> 4;   // get upper four bits
            } else {
                idx = this->exchanges[i][j/2] & 15;   // get lower four bits
            }
            this->permvec.push_back(idx);
        }
    }

    return this->permvec;
}

/**
 * @brief      Gets the permutation vector.
 *
 * @return     The permutation vector.
 */
std::vector<size_t> PermutationGenerator::get_permutation_single_vector(int pid) const {
    std::vector<size_t> permutations;

    for(unsigned int i=0; i<this->sz; i++) {
        size_t idx = 0;
        if(i%2 == 0) {
            idx = this->exchanges[pid][i/2] >> 4;   // get upper four bits
        } else {
            idx = this->exchanges[pid][i/2] & 15;   // get lower four bits
        }

        permutations.push_back(idx);
    }

    return permutations;
}

/**
 * @brief      get permutation exchange pattern
 *
 * @param[in]  pid   permutation index
 *
 * @return     permutation exchange pattern
 */
std::string PermutationGenerator::get_exchange_pattern_str(unsigned int pid) {
    std::stringstream str;

    for(unsigned int i=0; i<this->sz; i++) {
        str << (boost::format("%02i  ") % i).str();
    }
    str << std::endl;

    for(unsigned int i=0; i<this->sz; i++) {
        size_t idx = 0;
        if(i%2 == 0) {
            idx = this->exchanges[pid][i/2] >> 4;   // get upper four bits
        } else {
            idx = this->exchanges[pid][i/2] & 15;   // get lower four bits
        }
        str << (boost::format("%02i  ") % idx).str();
    }

    return str.str();
}

/**
 * @brief      generate all permutations
 */
void PermutationGenerator::generate_permutations() {
    std::vector<uint8_t> myints(this->sz);
    for(unsigned int i=0; i<this->sz; i++) {
        myints[i] = i;
    }

    do {
        std::array<uint8_t, 6> ex = {0,0,0,0,0,0};
        for(unsigned int i=0; i<this->sz; i++) {
            if(i%2 == 0) {
                ex[i/2] |= myints[i] << 4;
            } else {
                ex[i/2] |= myints[i];
            }
        }
        this->exchanges.push_back(ex);

    } while (std::next_permutation(myints.begin(),myints.end()));
}
