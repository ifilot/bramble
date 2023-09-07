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

#include "metric_analyzer_cuda.h"
#include "isorank_analyze_cuda.h"

#define gpu_err_chk(ans) { gpu_assert((ans), __FILE__, __LINE__); }

inline void gpu_assert(cudaError_t code, const char *file, int line, bool abort=true) {
    if(code != cudaSuccess) {
        fprintf(stderr,"GPU assert: %s %s %d\n", cudaGetErrorString(code), file, line);
        if (abort) {
            exit(code);
        }
    }
}

MetricAnalyzerCUDA::MetricAnalyzerCUDA() {
}

/**
 * @brief      calculate distance metric using GPU
 *
 * @param[in]  cuda_device which GPU to perform this calculation on
 * @param[in]  psize       number of atom indices
 * @param[in]  start       starting permutation index
 * @param[in]  stop        stopping permutation index
 * @param[in]  _exchanges  vector holding exchanges
 * @param[in]  _mat1       distance matrix of atom 1
 * @param[in]  _mat2       distance matrix of atom 2
 * @param      _results    result vector (distance of each permutation)
 *
 * @return     lowest permutation
 */
float MetricAnalyzerCUDA::analyze_cuda(int cuda_device,
                                       size_t psize,
                                       size_t start,
                                       size_t stop,
                                       const std::vector<uint8_t>& _exchanges,
                                       const std::vector<float>& _mat1,
                                       const std::vector<float>& _mat2,
                                       std::vector<float>& _results) {
    uint8_t *exchanges;
    float *results;
    const size_t N = (stop - start);

    // specify cuda device
    gpu_err_chk(cudaSetDevice(cuda_device));

    // Allocate Unified Memory – accessible from CPU or GPU
    gpu_err_chk(cudaMalloc(&exchanges, N * psize * sizeof(uint8_t)));
    gpu_err_chk(cudaMalloc(&results, N * sizeof(float)));

    // determine sizes
    unsigned int vecsize = _mat1.size();
    unsigned int _matsize = std::sqrt(vecsize);

    // copy to device
    gpu_err_chk(cudaMemcpyToSymbol(mat1, &_mat1[0], vecsize * sizeof(float)));
    gpu_err_chk(cudaMemcpyToSymbol(mat2, &_mat2[0], vecsize * sizeof(float)));
    gpu_err_chk(cudaMemcpyToSymbol(matsize, &_matsize, sizeof(unsigned int)));
    gpu_err_chk(cudaMemcpyToSymbol(exsize, &psize, sizeof(unsigned int)));

    // copy exchange patterns
    gpu_err_chk(cudaMemcpy(exchanges, &_exchanges[start*psize], N * psize * sizeof(uint8_t), cudaMemcpyHostToDevice));

    // execute kernel
    isorank_analyze_cuda<<<N, 1>>>(exchanges, results);
    gpu_err_chk(cudaDeviceSynchronize());

    // copy back to host
    gpu_err_chk(cudaMemcpy(&_results[0], results, N * sizeof(float), cudaMemcpyDeviceToHost));

    // Free memory
    gpu_err_chk(cudaFree(exchanges));
    gpu_err_chk(cudaFree(results));

    return 0;
}
