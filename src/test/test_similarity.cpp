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

#define BOOST_TEST_MODULE test_similarity
#include <boost/test/unit_test.hpp>

#include "similarity_analysis.h"
#ifdef MOD_CUDA
#include "card_manager.h"
#endif

// check that we can read .geo files
BOOST_AUTO_TEST_CASE(test_similarity) {
    BOOST_TEST_MESSAGE( "Test similarity analysis..." );

    static const unsigned int matsize = 8;

    std::vector<unsigned int> permvec(matsize,0);
    SimilarityAnalysis sa;
    float ans = -1;

    // fixed matrices
    MatrixXXf dm1 = MatrixXXf::Identity(matsize,matsize);
    MatrixXXf dm2 = MatrixXXf::Identity(matsize,matsize);
    ans = sa.calculate_distance_metric_single_thread(dm1, dm2, &permvec[0]);
    BOOST_CHECK(ans == 0.0);
    for(unsigned int i=0; i<matsize; i++) {
        BOOST_CHECK(permvec[i] == i);
    }

    // random matrices
    MatrixXXf m1 = MatrixXXf::Random(matsize,matsize) + MatrixXXf::Ones(matsize,matsize);
    MatrixXXf m2 = MatrixXXf::Random(matsize,matsize) + MatrixXXf::Ones(matsize,matsize);
    MatrixXXf dm3 = 0.5f * (m1 + Eigen::Transpose(m1)).eval();
    MatrixXXf dm4 = 0.5f * (m2 + Eigen::Transpose(m2)).eval();

    BOOST_CHECK(dm3.isApprox(dm3.transpose(), 1e-7));
    BOOST_CHECK(dm4.isApprox(dm4.transpose(), 1e-7));

    float ans2 = sa.calculate_distance_metric_single_thread(dm3, dm4, &permvec[0]);
    float ans3 = sa.calculate_distance_metric_openmp(dm3, dm4, &permvec[0]);

    BOOST_TEST(ans2 == ans3, boost::test_tools::tolerance(1e-7));

    #ifdef MOD_CUDA
    CardManager cm;
    if(cm.get_num_gpus() > 0 && cm.get_memory_device(0) >= 8589934592) {
        float ans4 = sa.calculate_distance_metric_cuda(0, dm3, dm4, &permvec[0]);
        BOOST_TEST(ans2 == ans4, boost::test_tools::tolerance(1e-7));
        BOOST_TEST(ans3 == ans4, boost::test_tools::tolerance(1e-7));
    }
    #endif // MOD_CUDA

    //-------------------------------------------------------------------------
    // Test specific matrix situation
    //-------------------------------------------------------------------------

    static const unsigned int testmatsize = 8;
    MatrixXXf dm5(testmatsize,testmatsize);
    std::ifstream infile("mat1.txt");
    for(unsigned int i=0; i<testmatsize; i++) {
        std::vector<std::string> pieces;
        std::string line;
        getline(infile, line);
        boost::split(pieces,line,boost::is_any_of("\t "));
        for(unsigned int j=0; j<testmatsize; j++) {
            dm5(i,j) = boost::lexical_cast<float>(pieces[j]);
        }
    }
    infile.close();

    MatrixXXf dm6(testmatsize,testmatsize);
    infile = std::ifstream("mat2.txt");
    for(unsigned int i=0; i<testmatsize; i++) {
        std::vector<std::string> pieces;
        std::string line;
        getline(infile, line);
        boost::split(pieces,line,boost::is_any_of("\t "));
        for(unsigned int j=0; j<testmatsize; j++) {
            dm6(i,j) = boost::lexical_cast<float>(pieces[j]);
        }
    }

    float ans5 = sa.calculate_distance_metric_single_thread(dm5, dm6, &permvec[0]);
    float ans6 = sa.calculate_distance_metric_openmp(dm5, dm6, &permvec[0]);

    BOOST_CHECK(ans5 < 1e-4);
    BOOST_TEST(ans5 == ans6, boost::test_tools::tolerance(1e-7));
}
