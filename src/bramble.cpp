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

#include <tclap/CmdLine.h>
#include <chrono>

#include "config.h"
#include "pattern_library.h"
#include "cna.h"
#include "similarity_analysis.h"
#include "state.h"

int main(int argc, char* argv[]) {
    try {
        TCLAP::CmdLine cmd("CNA algorithm");

        // input file
        TCLAP::ValueArg<std::string> arg_input_filename("i", "input", "Input file (i.e. POSCAR)", true, "", "path");

        // output file
        TCLAP::ValueArg<std::string> arg_output_filename("o", "output", "Output file (i.e. result.txt)", true, "", "path");

        // output file
        TCLAP::ValueArg<std::string> arg_fingerprint("p", "pattern", "Pattern file (i.e. fingerprints.json)", false, "", "path");

        // whether to perform a similarity analysis
        TCLAP::SwitchArg swarg_similarity("s","similarity","Perform a similarity analysis", cmd, false);

        // add and parse arguments
        cmd.add(arg_input_filename);
        cmd.add(arg_output_filename);
        cmd.add(arg_fingerprint);
        cmd.parse(argc, argv);

        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Executing Bramble v." << PROGRAM_VERSION << std::endl;
        std::cout << "Author: Ivo Filot <i.a.w.filot@tue.nl>" << std::endl;
        std::cout << "Documentation: https://bramble.imc-tue.nl" << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Git Hash: " << PROGRAM_GIT_HASH << std::endl;
        #ifdef MOD_CUDA
        std::cout << "Modules: CUDA" << std::endl;
        #endif // MOD_CUDA
        std::cout << "--------------------------------------------------------------" << std::endl;

        auto start = std::chrono::system_clock::now();

        //------------------------------------------------------------------------------------------
        // create fingerprints
        //------------------------------------------------------------------------------------------
        if(!arg_fingerprint.getValue().empty()) {
            // create pattern class
            const auto pl = std::make_shared<PatternLibrary>(arg_fingerprint.getValue());

            // read file and setup State
            const auto state = std::make_shared<State>(arg_input_filename.getValue());

            // calculate CNA fingerprint
            CNA cna(pl);
            cna.analyze(state);
            cna.write_analysis(arg_output_filename.getValue());
        }
        //------------------------------------------------------------------------------------------
        // perform similarity analysis
        //------------------------------------------------------------------------------------------
        else if(swarg_similarity.getValue()) {
            // construct similarity analysis class
            SimilarityAnalysis sl;

            // read file and setup State
            const auto state = std::make_shared<State>(arg_input_filename.getValue());

            // perform analysis
            sl.analyze(state);

            // write to file
            sl.write_analysis(arg_output_filename.getValue());
        }

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end-start;

        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Done in " << elapsed.count() << " seconds." << std::endl;

        return 0;

    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() <<
                     " for arg " << e.argId() << std::endl;
        return -1;
    }

    return 0;
}
