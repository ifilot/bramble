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

#ifndef _SIMILARITY_ANALYSIS
#define _SIMILARITY_ANALYSIS

#include <memory>
#include <chrono>
#include <mutex>
#include <boost/math/special_functions/factorials.hpp>

#include "state.h"
#include "permutation_generator.h"
#include "metric_analyzer_cuda.h"
#include "progress_bar.h"

class SimilarityAnalysis {
private:
    std::shared_ptr<State> state;
    std::vector<std::unique_ptr<PermutationGenerator>> permutation_generators;
    std::vector<MatrixXXf> distance_matrices;
    std::vector<std::vector<size_t> > neighbor_ids;
    MatrixXXf jobtimes;
    MatrixXXf distances;
    MatrixXXf distance_metric_matrix;

public:
    SimilarityAnalysis(bool unsafe = false, bool lazy_init = true);

    void analyze(const std::shared_ptr<State>& _state);

    float calculate_distance_metric_single_thread(const MatrixXXf& dm1,
                                                  const MatrixXXf& dm2,
                                                  unsigned int* permvec);

    float calculate_distance_metric_openmp(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec);

    #ifdef MOD_CUDA
    float calculate_distance_metric_cuda(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec);
    #endif

    float calculate_adjacency_metric_perm(const MatrixXXb& am1, const MatrixXXb& am2, const unsigned int* permvec) const;

    void create_permutation(unsigned int perm);

    void write_analysis(const std::string& filename);

private:
    float analyze_single(const MatrixXXf& mat1, const MatrixXXf& mat2, const std::vector<size_t>& ex) const ;

    /**
     * @brief      construct local distance matrix
     *
     * @param[in]  atid    atom id of interest
     * @param[in]  cutoff  cutoff distance (smaller assumes bond)
     *
     * @return     distance matrix
     */
    MatrixXXf construct_distance_matrix(size_t atid, float cutoff);

    /**
     * @brief      calculate cutoff distance
     *
     * @param[in]  atid  atom id
     *
     * @return     cutoff distance
     */
    float calculate_cutoff(size_t atid);

    #ifdef MOD_CUDA
    void selftest(unsigned int sz);
    #endif
};


#endif // _SIMILARITY_ANALYSIS
