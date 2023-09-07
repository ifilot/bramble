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

#ifndef _METRIC_ANALYZER_CUDA_H
#define _METRIC_ANALYZER_CUDA_H

#include <iostream>
#include <vector>
#include <chrono>
#include <stdint.h>
#include <omp.h>

class MetricAnalyzerCUDA {
private:

public:
    MetricAnalyzerCUDA();

    /**
     * @brief      calculate distance metric using GPU
     *
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
    float analyze_cuda(int device_id,
                       size_t psize,
                       size_t start,
                       size_t stop,
                       const std::vector<uint8_t>& _exchanges,
                       const std::vector<float>& _mat1,
                       const std::vector<float>& _mat2,
                       std::vector<float>& _results);

private:

};


#endif // _METRIC_ANALYZER_CUDA_H
