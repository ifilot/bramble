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

#ifndef _CNA_H
#define _CNA_H

#include "config.h"

#include <memory>
#include <vector>
#include <iostream>

#include "math.h"
#include "state.h"
#include "patterns.h"
#include "cna_signature.h"

class CNA {

private:
    MatrixXXf distances;
    std::vector<MatrixXXb> adjacency_matrices;
    std::vector<std::string> fingerprints;
    std::vector<std::vector<size_t> > neighbor_ids;
    std::shared_ptr<PatternLibrary> pl;
    std::shared_ptr<State> state;

public:
    /**
     * @brief      Constructs the CNA object
     */
    CNA(const std::shared_ptr<PatternLibrary>& _pl);

    /**
     * @brief      analyze dataset
     *
     * @param[in]  _dataset  link to shared_ptr of dataset
     */
    void analyze(const std::shared_ptr<State>& _state);

    /**
     * @brief      Gets the fingerprint.
     *
     * @param[in]  atid  The atom id
     *
     * @return     The fingerprint.
     */
    inline const std::string& get_fingerprint(size_t atid) const {
        return this->fingerprints[atid];
    }

    /**
     * @brief      Gets the neighbor identifiers.
     *
     * @param[in]  atid  The atom  id
     *
     * @return     The neighbor identifiers.
     */
    inline const std::vector<size_t>& get_neighbor_ids(size_t atid) const {
        return this->neighbor_ids[atid];
    }

    /**
     * @brief      Gets the neighbor identifiers vector.
     *
     * @return     The neighbor identifiers vector.
     */
    inline const auto& get_neighbor_ids_vec() const {
        return this->neighbor_ids;
    }

    /**
     * @brief      Gets the adjacency matrix.
     *
     * @param[in]  atid  the atom id
     *
     * @return     The adjacency matrix.
     */
    inline const auto& get_adjacency_matrix(size_t atid) const {
        return this->adjacency_matrices[atid];
    }

    /**
     * @brief      Calculates the fingerprint from adjacency matrix.
     *
     * @param[in]  am    adjacency matrix
     *
     * @return     the fingerprint
     */
    std::string calculate_fingerprint_from_adjacency_matrix(const MatrixXXb& am);

    /**
     * @brief      Writes the CNA analysis to a file
     *
     * @param[in]  filename  The filename
     */
    void write_analysis(const std::string& filename);

private:
    /**
     * @brief      calculates fingerprint for an atom
     *
     * @param[in]  atid  atom identifier
     *
     * @return     The fingerprint.
     */
    std::string calculate_fingerprint(size_t atid);

    /**
     * @brief      construct reduced adjacency matrix from distance matrix
     *
     * @param[in]  atid    atom id of interest
     * @param[in]  cutoff  cutoff distance (smaller assumes bond)
     *
     * @return     reduced adjacency matrix
     */
    MatrixXXb construct_adjancy_matrix(size_t atid, float cutoff);

    /**
     * @brief      calculate cutoff distance
     *
     * @param[in]  atid  atom id
     *
     * @return     cutoff distance
     */
    float calculate_cutoff(size_t atid);
};

#endif // _CNA_H
