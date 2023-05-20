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

#ifndef _ISORANK_CUDA_ANALYZER_H
#define _ISORANK_CUDA_ANALYZER_H

// mat1 and mat2 have predefined sizes equal to the largest matrix that they are supposed
// to hold, which is 12x12. For smaller matrices, the excess size is simply not used.
__device__ __constant__ float mat1[144];
__device__ __constant__ float mat2[144];

// matsize is equal to the dimension of the largest matrix, exsize is equal to the
// dimension of the smaller matrix
__device__ __constant__ unsigned int matsize;
__device__ __constant__ unsigned int exsize;

/**
 * @brief      calculate Frobenius norm
 *
 * @param      exchange  pointer to vector of exchange patterns
 * @param      results   result vector
 *
 * Note that the permutations are always performed on the second matrix, hence
 * this matrix should be the smaller one
 */
__global__ void isorank_analyze_cuda(uint8_t* exchange, float* results) {
    // calculate indices
    unsigned int ex_idx = blockIdx.x * exsize;
    unsigned int result_idx = blockIdx.x;

    // create local copy of data
    uint8_t ex[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
    memcpy(ex, &exchange[ex_idx], exsize * sizeof(uint8_t));

    // perform index exchange and calculate Frobenius norm
    float sum = 0.0f;
    for(unsigned int i=0; i<matsize; i++) { // rows
        for(unsigned int j=i+1; j<matsize; j++) { // columns
            sum += (mat1[i*matsize+j] - mat2[ex[i]*matsize+ex[j]]) * (mat1[i*matsize+j] - mat2[ex[i]*matsize+ex[j]]);
        }
    }

    results[result_idx] = sum * 2.0f;
}

#endif // _ISORANK_CUDA_ANALYZER_H
