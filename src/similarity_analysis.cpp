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

#include "similarity_analysis.h"

SimilarityAnalysis::SimilarityAnalysis(bool unsafe) {
    this->permutation_generators.resize(12);

    auto tstart = std::chrono::system_clock::now();
    std::cout << "Loading permutation vectors into memory ... this takes approx. 20 sec." << std::endl;

    #pragma omp parallel for schedule(dynamic)
    for(unsigned int i=1; i<=(unsafe ? 13 : 12); i++) {
        this->create_permutation(i);
    }

    auto tend = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = tend - tstart;
    std::cout << "Done constructing permutations in " << elapsed_seconds.count() << " seconds." << std::endl;

    std::cout << std::endl;

    // perform a self-evaluation of CPU vs GPU performance if the
    // CUDA module is enabled
    #ifdef MOD_CUDA
    tstart = std::chrono::system_clock::now();
    std::cout << "Executing selftests" << std::endl;

    for(unsigned int i=6; i<12; i++) {
        this->selftest(i);
    }

    std::cout << "Done executing selftests: all OK! " << std::endl;
    std::cout << std::endl;
    #endif // MOD_CUDA
}

void SimilarityAnalysis::analyze(const std::shared_ptr<State>& _state) {
    std::cout << "Start Similarity Analysis" << std::endl;

    // clear contents when doing new analysis
    this->distances.resize(0,0); // clear matrix contents
    this->neighbor_ids.clear();  // clear neighbors
    this->state = _state;

    // preallocate neigbor ids
    this->neighbor_ids.resize(this->state->get_nr_atoms());

    // calculate all distances
    std::cout << "Copying interatomic distances to similarity analysis object." << std::endl;
    this->distances = this->state->get_distances();

    // expand adjacency matrix vector
    this->distance_matrices.resize(this->state->get_nr_atoms());

    // construct local density matrices
    std::cout << "Constructing local density matrices...";
    #pragma omp parallel for
    for(unsigned int i=0; i<this->state->get_nr_atoms(); i++) {
        this->distance_matrices[i] = this->construct_distance_matrix(i, this->calculate_cutoff(i));
    }
    std::cout << " Done!" << std::endl;

    // perform similarity analysis
    this->distance_metric_matrix = MatrixXXf::Zero(this->state->get_nr_atoms(),this->state->get_nr_atoms());

    // create a list of jobs
    std::vector<std::pair<unsigned int, unsigned int>> jobs;
    for(unsigned int i=0; i<this->state->get_nr_atoms(); i++) {
        for(unsigned int j=i+1; j<this->state->get_nr_atoms(); j++) {
            jobs.emplace_back(i,j);
        }
    }

    // give number of threads
    #pragma omp parallel
    {
        int nrthreads = omp_get_num_threads();
        int threadnum = omp_get_thread_num();

        if(threadnum ==  0) {
            std::cout << "Executing similarity analysis using " << nrthreads << " threads." << std::endl;
        }
    }

    // execute jobs in parallel
    #ifdef MOD_CUDA
    for(unsigned int k=0; k<jobs.size(); k++) {
        std::cout << (k+1) << " / " << jobs.size() << std::endl;

        const unsigned int i = jobs[k].first;
        const unsigned int j = jobs[k].second;
        std::vector<unsigned int> permvec(12,0);
        for(unsigned int j=i+1; j<this->state->get_nr_atoms(); j++) {
            float mhsn = this->calculate_distance_metric_cuda(this->distance_matrices[i],
                                                              this->distance_matrices[j],
                                                              &permvec[0]);
            this->distance_metric_matrix(i,j) = this->distance_metric_matrix(j,i) = mhsn;
        }
    }
    #else
    #pragma omp parallel for schedule(dynamic)
    for(unsigned int k=0; k<jobs.size(); k++) {

        #pragma omp critical
        std::cout << (k+1) << " / " << jobs.size() << std::endl;

        const unsigned int i = jobs[k].first;
        const unsigned int j = jobs[k].second;
        std::vector<unsigned int> permvec(12,0);
        for(unsigned int j=i+1; j<this->state->get_nr_atoms(); j++) {
            float mhsn = this->calculate_distance_metric_single_thread(this->distance_matrices[i],
                                                                       this->distance_matrices[j],
                                                                       &permvec[0]);
            this->distance_metric_matrix(i,j) = this->distance_metric_matrix(j,i) = mhsn;
        }
    }
    #endif
}

float SimilarityAnalysis::calculate_distance_metric_single_thread(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec) const {
    // create local copy and make matrices equal in size
    MatrixXXf dm1c, dm2c;
    if(dm1.rows() < dm2.rows()) {
        dm1c = dm2;
        dm2c = dm1;
    } else {
        dm1c = dm1;
        dm2c = dm2;
    }

    size_t minsize = std::min(dm1c.rows(), dm2c.rows());
    size_t maxsize = std::max(dm1c.rows(), dm2c.rows());

    dm1c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));
    dm2c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));

    // build permutation generator and create pointer
    auto pg = this->permutation_generators[minsize-1].get();
    const auto& permutations = pg->get_permutation_vector();

    float lownorm = 1e6;
    size_t pid = 0;

    for(size_t i=0; i<pg->get_nr_perm(); i++) {
        auto perm = pg->get_permutation_single_vector(i);
        float norm = this->analyze_single(dm1c, dm2c, perm);

        if(norm < lownorm) {
            lownorm = norm;
            pid = i;
        }
    }

    for(unsigned int i=0; i<minsize; i++) {
        permvec[i] = permutations[pid * minsize + i];
    }

    return std::sqrt(lownorm);
}

float SimilarityAnalysis::calculate_distance_metric_openmp(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec) const {
    // create local copy and make matrices equal in size
    MatrixXXf dm1c, dm2c;
    if(dm1.rows() < dm2.rows()) {
        dm1c = dm2;
        dm2c = dm1;
    } else {
        dm1c = dm1;
        dm2c = dm2;
    }

    size_t minsize = std::min(dm1c.rows(), dm2c.rows());
    size_t maxsize = std::max(dm1c.rows(), dm2c.rows());

    dm1c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));
    dm2c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));

    // build permutation generator and create pointer
    auto pg = this->permutation_generators[minsize-1].get();
    const auto& permutations = pg->get_permutation_vector();

    int mxthread = omp_get_max_threads();
    std::vector<float> permnr(mxthread, 0);
    std::vector<float> lownorms(mxthread, 1e6);

    std::mutex mut;

    #pragma omp parallel
    {
        int nrthreads = omp_get_num_threads();
        int threadnum = omp_get_thread_num();
        size_t begin = pg->get_nr_perm() / nrthreads * threadnum;
        size_t end = pg->get_nr_perm() / nrthreads * (threadnum + 1) + pg->get_nr_perm() % nrthreads;

        for(size_t i=begin; i<end; i++) {
            const auto perm = pg->get_permutation_single_vector(i);

            float norm = this->analyze_single(dm1c, dm2c, perm);

            if(norm < lownorms[threadnum]) {
                lownorms[threadnum] = norm;
                permnr[threadnum] = i;
            }
        }
    }

    float lownorm = 1e6;
    size_t pid = 0;
    for(unsigned int i=0; i<mxthread; i++) {
        if(lownorms[i] < lownorm) {
            lownorm = lownorms[i];
            pid = permnr[i];
        }
    }

    for(unsigned int i=0; i<minsize; i++) {
        permvec[i] = permutations[pid * minsize + i];
    }

    return std::sqrt(lownorm);
}

#ifdef MOD_CUDA
/**
 * @brief      Calculates the distance metric between two distance matrices on the GPU
 *
 * @param[in]  dm1      distance matrix 1
 * @param[in]  dm2      distance matrix 2
 * @param      permvec  pointer to permutation vector
 *
 * @return     distance metric
 */
float SimilarityAnalysis::calculate_distance_metric_cuda(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec) const {
    // create local copy and make matrices equal in size
    MatrixXXf dm1c, dm2c;
    if(dm1.rows() < dm2.rows()) {
        dm1c = dm2;
        dm2c = dm1;
    } else {
        dm1c = dm1;
        dm2c = dm2;
    }

    size_t minsize = std::min(dm1c.rows(), dm2c.rows());
    size_t maxsize = std::max(dm1c.rows(), dm2c.rows());

    dm1c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));
    dm2c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));

    std::vector<float> dm1cv(maxsize * maxsize);
    std::vector<float> dm2cv(maxsize * maxsize);

    for(unsigned int i=0; i<maxsize; i++) {
        for(unsigned int j=i; j<maxsize; j++) {
            dm1cv[i * maxsize + j] = dm1cv[j * maxsize + i] = dm1c(i,j);
            dm2cv[i * maxsize + j] = dm2cv[j * maxsize + i] = dm2c(i,j);
        }
    }

    auto pg = this->permutation_generators[minsize-1].get();

    MetricAnalyzerCUDA mac;
    const size_t N = pg->get_nr_perm();

    float lowest_metric = 1e6;
    auto permutations = pg->get_permutation_vector();

    // the maximum size of a 32 bit unsigned integer is 4,294,967,295; the maximum index value
    // of 12! * 12 values equals 5,748,019,200; hence we have to submit 12! permutations in
    // multiple (here two) stages
    static const size_t increment = 39916800 * 6;

    for(size_t i=0; i<N; i+= increment) {
        std::vector<float> results(std::min(N,increment));
        mac.analyze_cuda(minsize, i, std::min(N,i+increment), permutations, dm1cv, dm2cv, results);

        // find minimum value
        unsigned int pid = 0;
        bool findbetter = false;
        for(unsigned int j=0; j<results.size(); j++) {
            if(results[j] < lowest_metric) {
                lowest_metric = results[j];
                pid = j;
                findbetter = true;
            }
        }

        // return permutation
        if(findbetter) {
            for(unsigned int j=0; j<minsize; j++) {
                permvec[j] = permutations[pid * minsize + j];
            }
        }
    }

    return std::sqrt(lowest_metric);
}
#endif // MOD_CUDA

void SimilarityAnalysis::create_permutation(unsigned int perm) {
    if(this->permutation_generators[perm-1].get() == nullptr) {
        this->permutation_generators[perm-1] = std::make_unique<PermutationGenerator>(perm);
    }
}

/**
 * @brief      calculate distance metric
 *
 * @param[in]  mat1  distance matrix 1
 * @param[in]  mat2  distance matrix 2
 * @param[in]  ex    exchange vector
 *
 * @return     distance metric
 */
float SimilarityAnalysis::analyze_single(const MatrixXXf& mat1, const MatrixXXf& mat2, const std::vector<size_t>& ex) const {
    float norm = 0.0f;

    // note that the second matrix is in principle the smaller one, but appended by zeros to match the size of the
    // first matrix. We only need to loop over the (original) size of the second matrix, which matches the size
    // of the exchange vector
    for(unsigned int i=0; i<ex.size(); i++) {
        for(unsigned int j=i+1; j<ex.size(); j++) {
            norm += (mat1(i,j) - mat2(ex[i], ex[j])) * (mat1(i,j) - mat2(ex[i], ex[j]));
        }
    }

    return norm * 2.0f;
}

float SimilarityAnalysis::calculate_adjacency_metric_perm(const MatrixXXb& am1, const MatrixXXb& am2, const unsigned int* permvec) const {
    // create local copy and make matrices equal in size
    MatrixXXb am1c, am2c;
    if(am1.rows() < am2.rows()) {
        am1c = am2;
        am2c = am1;
    } else {
        am1c = am1;
        am2c = am2;
    }

    size_t minsize = std::min(am1c.rows(), am2c.rows());
    size_t maxsize = std::max(am1c.rows(), am2c.rows());

    am1c.conservativeResizeLike(MatrixXXb::Zero(maxsize, maxsize));
    am2c.conservativeResizeLike(MatrixXXb::Zero(maxsize, maxsize));

    int anorm = 0;
    for(unsigned int i=0; i<minsize; i++) {
        for(unsigned int j=i+1; j<minsize; j++) {
            anorm += (am1c(i,j) - am2c(permvec[i], permvec[j])) * (am1c(i,j) - am2c(permvec[i], permvec[j]));
        }
    }

    return (float)anorm * 2.0f;
}

/**
 * @brief      construct reduced adjacency matrix from distance matrix
 *
 * @param[in]  atid    atom id of interest
 * @param[in]  cutoff  cutoff distance (smaller assumes bond)
 *
 * @return     reduced adjacency matrix
 */
MatrixXXf SimilarityAnalysis::construct_distance_matrix(size_t atid, float cutoff) {
    // determine number of atoms
    size_t dist = this->state->get_nr_atoms_supercell();

    if(dist > State::MAX_SIZE_ATOMS_UNITCELL || !this->state->is_periodic()) {
        dist = this->state->get_nr_atoms();
    }

    // determine nearest neighbors
    for(unsigned int i=0; i<dist; i++) {
        if(i == atid) {
            continue;
        }

        if(this->distances(atid, i) < cutoff) {
            this->neighbor_ids[atid].push_back(i);
        }
    }

    // construct distance matrix
    MatrixXXf dm(this->neighbor_ids[atid].size(), this->neighbor_ids[atid].size());
    for(unsigned int i=0; i<this->neighbor_ids[atid].size(); i++) {
        for(unsigned int j=i; j<this->neighbor_ids[atid].size(); j++) {
            if(i == j) {
                dm(i,j) = 0.0;
                continue;
            }

            unsigned int id1 = this->neighbor_ids[atid][i];
            unsigned int id2 = this->neighbor_ids[atid][j];
            dm(i,j) = dm(j,i) = this->distances(id1, id2);
        }
    }

    return dm;
}

/**
 * @brief      calculate cutoff distance
 *
 * @param[in]  atid  atom id
 *
 * @return     cutoff distance
 */
float SimilarityAnalysis::calculate_cutoff(size_t atid) {
    // sort distances
    std::vector<float> vec;
    const auto atom_distances = this->distances.row(atid);
    for(unsigned int i=0; i<atom_distances.size(); i++) {
        if(i != atid) {
            vec.push_back(atom_distances(i));
        }
    }
    std::sort(std::begin(vec), std::end(vec));

    // calculate proper cutoff
    float sum = 0.0f;
    for(unsigned int i=0; i<6; i++) {
        sum += vec[i];
    }

    return sum / 6.0f * (1.0f + std::sqrt(2.0f)) / 2.0f;
}

#ifdef MOD_CUDA
void SimilarityAnalysis::selftest(unsigned int sz) {
    MatrixXXf dm1 = MatrixXXf::Zero(sz,sz);
    MatrixXXf dm2 = MatrixXXf::Zero(sz,sz);

    for(unsigned int i=0; i<sz; i++) {
        for(unsigned int j=i+1; j<sz; j++) {
            dm1(i,j) = dm1(j,i) = (i+1) * (j+1);
            dm2(sz - 1 - i,sz - 1 - j) = dm2(sz - 1 - j,sz - 1 - i) = (i+1) * (j+1) + 1;
        }
    }

    unsigned int permvec[12];

    // perform gpu calculation
    auto tstart = std::chrono::system_clock::now();
    const float gpu_ans = this->calculate_distance_metric_cuda(dm1, dm2, permvec);
    auto tend = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds_gpu = tend - tstart;


    // perform cpu calculation
    tstart = std::chrono::system_clock::now();
    const float cpu_ans = this->calculate_distance_metric_openmp(dm1, dm2, permvec);
    tend = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds_cpu = tend - tstart;

    // calculate correct number
    const float ans = std::sqrt(sz * sz - sz);

    if(std::fabs(gpu_ans - cpu_ans) < 1e-10 && std::fabs(gpu_ans - ans) < 1e-10) {
        std::cout << boost::format("T%02i: CPU %12.6f | GPU %12.6f")
                        % sz
                        % elapsed_seconds_cpu.count()
                        % elapsed_seconds_gpu.count() << std::endl;
    } else {
        throw std::runtime_error("Critical selftest failed; terminating.");
    }
}
#endif
