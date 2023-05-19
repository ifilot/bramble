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

#ifndef _MATRIXMATH_H
#define _MATRIXMATH_H

#include <Eigen/Dense>

typedef Eigen::Matrix<float, 3, 3, Eigen::RowMajor> MatrixUnitcell;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> MatrixXXf;
typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> MatrixXXb;
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> MatrixXXi;
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> VectorXf;
typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> MatrixXXb;
typedef Eigen::Matrix<unsigned int, Eigen::Dynamic, Eigen::Dynamic> MatrixXXu;

typedef Eigen::Vector3f Vec3f;
typedef Vec3f Vec3;
typedef float fpt;  // general floating point type

// needed for sorting unordered maps based on second item
template <typename T1, typename T2>
struct greater_second {
    typedef std::pair<T1, T2> type;
    bool operator ()(type const& a, type const& b) const {
        return a.second > b.second;
    }
};

#endif // _MATRIXMATH_H
