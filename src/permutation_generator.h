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

#ifndef _PERMUTATION_GENERATOR_H
#define _PERMUTATION_GENERATOR_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <boost/format.hpp>

#include "math.h"

/**
 * @brief      Class for generating all possible permutations
 */
class PermutationGenerator {
private:
    unsigned int sz;    //!< number of exchangeable indices
    std::vector<std::array<uint8_t, 6>> exchanges;
    std::vector<uint8_t> permvec;

public:
    /**
     * @brief      construct permutation generator object
     *
     * @param[in]  _sz   matrix dimension of permutation matrix
     */
    PermutationGenerator(unsigned int _sz);

    /**
     * @brief      get the permutation matrix
     *
     * @param[in]  pid   permutation index
     * @param[in]  _sz   dimension of the matrix
     *
     * @return     the permutation matrix
     */
    MatrixXXu get_permutation_matrix(unsigned int pid, unsigned int _sz) const;

    /**
     * @brief      get number of permutations generated
     *
     * @return     number of permutations
     */
    inline size_t get_nr_perm() const {
        return this->exchanges.size();
    }

    /**
     * @brief      Gets the size.
     *
     * @return     The size.
     */
    inline unsigned int get_sz() const {
        return this->sz;
    }

    /**
     * @brief      Gets the permutation vector.
     *
     * @return     The permutation vector.
     */
    const std::vector<uint8_t>& get_permutation_vector();

    /**
     * @brief      Gets the permutation single vector.
     *
     * @param[in]  pid   The pid
     *
     * @return     The permutation single vector.
     */
    std::vector<size_t> get_permutation_single_vector(int pid) const;

    /**
     * @brief      get permutation exchange pattern
     *
     * @param[in]  pid   permutation index
     *
     * @return     permutation exchange pattern
     */
    std::string get_exchange_pattern_str(unsigned int pid);

private:
    /**
     * @brief      generate all permutations
     */
    void generate_permutations();

};

#endif // _PERMUTATION_GENERATOR
