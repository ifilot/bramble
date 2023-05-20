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

SimilarityAnalysis::SimilarityAnalysis(bool unsafe,
                                       bool lazy_init) {
    this->permutation_generators.resize(12);

    if(!lazy_init) {
        auto tstart = std::chrono::system_clock::now();
        std::cout << "Loading permutation vectors into memory ... this takes approx. 20 sec." << std::endl;

        #pragma omp parallel for schedule(dynamic)
        for(unsigned int i=1; i<=(unsafe ? 13 : 12); i++) {
            this->create_permutation(i);
        }

        auto tend = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = tend - tstart;
        std::cout << "Done constructing permutations in " << elapsed_seconds.count() << " seconds." << std::endl;
    }

    std::cout << std::endl;

    // perform a self-evaluation of CPU vs GPU performance if the
    // CUDA module is enabled
    #ifdef MOD_CUDA
    if(!lazy_init) {
        std::cout << "Executing selftests" << std::endl;

        for(unsigned int i=6; i<12; i++) {
            this->selftest(i);
        }

        std::cout << "Done executing selftests: all OK! " << std::endl;
        std::cout << std::endl;
    }
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
    this->distance_metric_matrix = MatrixXXf::Ones(this->state->get_nr_atoms(),this->state->get_nr_atoms()) * -1;

    // create a list of jobs
    std::cout << "Constructing jobs and pre-allocating permutation vectors." << std::endl;
    std::vector<std::pair<unsigned int, unsigned int>> jobs;
    for(unsigned int i=0; i<this->state->get_nr_atoms(); i++) {
        for(unsigned int j=i+1; j<this->state->get_nr_atoms(); j++) {
            jobs.emplace_back(i,j);
            unsigned int minsize = std::min(this->distance_matrices[i].rows(),
                                            this->distance_matrices[j].rows());
            if(!this->permutation_generators[minsize-1]) {
                this->create_permutation(minsize);
            }
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

    // show progress bar
    ProgressBar progress(jobs.size(), "");
    std::vector<bool> jobdone(jobs.size(), false);
    this->jobtimes = MatrixXXf::Ones(this->state->get_nr_atoms(),this->state->get_nr_atoms()) * -1;

    #ifdef MOD_CUDA
    omp_set_nested(1);
    #pragma omp parallel for num_threads(2) schedule(dynamic)
    #endif // MOD_CUDA
    for(unsigned int k=0; k<jobs.size(); k++) {

        // keep track of time per job
        auto tstart = std::chrono::system_clock::now();

        const unsigned int i = jobs[k].first;
        const unsigned int j = jobs[k].second;
        std::vector<unsigned int> permvec(12,0);
        float mhsn = -1;

        // refuse to execute job when number of atoms is larger than 12
        if(this->distance_matrices[i].rows() > 12 || this->distance_matrices[j].rows() > 12) {
            this->distance_metric_matrix(i,j) = this->distance_metric_matrix(j,i) = mhsn;
            continue;
        }

        #ifdef MOD_CUDA
        if(omp_get_thread_num == 0) { // run these on GPU
            mhsn = this->calculate_distance_metric_cuda(this->distance_matrices[i],
                                                        this->distance_matrices[j],
                                                        &permvec[0]);
        } else { // all other jobs are run on CPU
            mhsn = this->calculate_distance_metric_openmp(this->distance_matrices[i],
                                                          this->distance_matrices[j],
                                                          &permvec[0]);
        }
        #else
        mhsn = this->calculate_distance_metric_openmp(this->distance_matrices[i],
                                                      this->distance_matrices[j],
                                                      &permvec[0]);
        #endif // MOD_CUDA

        // store time
        auto tend = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = tend - tstart;
        jobdone[k] = true;
        this->jobtimes(i,j) = this->jobtimes(j,i) = elapsed_seconds.count();
        this->distance_metric_matrix(i,j) = this->distance_metric_matrix(j,i) = mhsn;

        #pragma omp critical
        {
            size_t done = std::count(jobdone.begin(), jobdone.end(), true);
            progress.updateLastPrintedMessage(
                    "Jobs remaining: " + std::to_string(jobs.size() - done));
            ++progress;
        }
    }
}

float SimilarityAnalysis::calculate_distance_metric_single_thread(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec) {
    // create local copy and make matrices equal in size
    MatrixXXf dm1c, dm2c;
    if(dm1.rows() < dm2.rows()) {
        dm1c = dm2;
        dm2c = dm1;
    } else {
        dm1c = dm1;
        dm2c = dm2;
    }

    size_t maxsize = std::max(dm1c.rows(), dm2c.rows());

    dm1c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));
    dm2c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));

    if(!this->permutation_generators[maxsize-1]) {
        this->create_permutation(maxsize);
    }

    // build permutation generator and create pointer
    auto pg = this->permutation_generators[maxsize-1].get();
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

    for(unsigned int i=0; i<maxsize; i++) {
        permvec[i] = permutations[pid * maxsize + i];
    }

    return std::sqrt(lownorm);
}

float SimilarityAnalysis::calculate_distance_metric_openmp(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec) {
    // create local copy and make matrices equal in size
    MatrixXXf dm1c, dm2c;
    if(dm1.rows() < dm2.rows()) {
        dm1c = dm2;
        dm2c = dm1;
    } else {
        dm1c = dm1;
        dm2c = dm2;
    }

    size_t maxsize = std::max(dm1c.rows(), dm2c.rows());

    dm1c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));
    dm2c.conservativeResizeLike(MatrixXXf::Zero(maxsize, maxsize));

    if(!this->permutation_generators[maxsize-1]) {
        this->create_permutation(maxsize);
    }

    // build permutation generator and create pointer
    auto pg = this->permutation_generators[maxsize-1].get();
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

    for(unsigned int i=0; i<maxsize; i++) {
        permvec[i] = permutations[pid * maxsize + i];
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
float SimilarityAnalysis::calculate_distance_metric_cuda(const MatrixXXf& dm1, const MatrixXXf& dm2, unsigned int* permvec) {
    // create local copy and make matrices equal in size
    MatrixXXf dm1c, dm2c;
    if(dm1.rows() < dm2.rows()) {
        dm1c = dm2;
        dm2c = dm1;
    } else {
        dm1c = dm1;
        dm2c = dm2;
    }

    size_t maxsize = std::max(dm1c.rows(), dm2c.rows());

    if(!this->permutation_generators[maxsize-1]) {
        this->create_permutation(maxsize);
    }

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

    auto pg = this->permutation_generators[maxsize-1].get();

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
        mac.analyze_cuda(maxsize, i, std::min(N,i+increment), permutations, dm1cv, dm2cv, results);

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
            for(unsigned int j=0; j<maxsize; j++) {
                permvec[j] = permutations[pid * maxsize + j];
            }
        }
    }

    return std::sqrt(lowest_metric);
}
#endif // MOD_CUDA

void SimilarityAnalysis::create_permutation(unsigned int perm) {
    std::cout << "Building permutations: " << perm << std::endl;
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
float SimilarityAnalysis::analyze_single(const MatrixXXf& mat1,
                                         const MatrixXXf& mat2,
                                         const std::vector<size_t>& ex) const {
    float norm = 0.0f;

    // note that the second matrix is in principle the smaller one, but appended by zeros to match the size of the
    // first matrix. We only need to loop over the (original) size of the larger matrix.
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
 * @brief      Write result of similarity analysis
 *
 * @param[in]  filename  Output file
 */
void SimilarityAnalysis::write_analysis(const std::string& filename) {
    std::ofstream outfile(filename);

    std::string dashed_lines;
    dashed_lines.resize(100, '-');

    outfile << dashed_lines << std::endl;
    outfile << "Executing Bramble v." << PROGRAM_VERSION << std::endl;
    outfile << "Author: Ivo Filot <i.a.w.filot@tue.nl>" << std::endl;
    outfile << "Documentation: https://bramble.imc-tue.nl" << std::endl;
    outfile << dashed_lines << std::endl;
    outfile << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
    outfile << "Git Hash: " << PROGRAM_GIT_HASH << std::endl;
    outfile << dashed_lines << std::endl;
    outfile << "Number of atoms: " << this->distance_matrices.size() << std::endl;
    outfile << dashed_lines << std::endl;

    for(unsigned int i=0; i<this->state->get_nr_atoms(); i++) {
        for(unsigned int j=0; j<this->state->get_nr_atoms(); j++) {
            if(i == j) {
                outfile << (boost::format("%04i  %04i  %02i  %02i  %12s  %6s") %
                       (i+1) %
                       (j+1) %
                       this->distance_matrices[i].rows() %
                       this->distance_matrices[j].rows() %
                       "N/A" %
                       "N/A").str()
                    << std::endl;
            } else {
                outfile << (boost::format("%04i  %04i  %02i  %02i  %12.6f  %6.2f s") %
                       (i+1) %
                       (j+1) %
                       this->distance_matrices[i].rows() %
                       this->distance_matrices[j].rows() %
                       this->distance_metric_matrix(i,j) %
                       this->jobtimes(i,j)).str()
                    << std::endl;
            }
        }
    }
    outfile << std::endl;

    // print distance matrices
    outfile << dashed_lines << std::endl;
    outfile << "DISTANCE MATRICES" << std::endl;
    outfile << dashed_lines << std::endl << std::endl;;
    for(unsigned int i=0; i<this->distance_matrices.size(); i++) {
        outfile << dashed_lines << std::endl;
        outfile << "Atom " << (i+1) << std::endl;
        outfile << dashed_lines << std::endl;
        outfile << std::fixed << std::setprecision(5) << this->distance_matrices[i] << std::endl;
        outfile << std::endl;
    }
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
    MatrixXXf m1 = MatrixXXf::Random(sz,sz);
    MatrixXXf m2 = MatrixXXf::Random(sz,sz);

    // create symmetric distance matrices
    MatrixXXf dm1 = 0.5f * (m1 + m1.transpose()).eval();
    MatrixXXf dm2 = 0.5f * (m2 + m2.transpose()).eval();

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
