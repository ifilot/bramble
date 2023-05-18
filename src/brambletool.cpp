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

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "config.h"
#include "pattern_library.h"

int main(int argc, char* argv[]) {
    try {
        TCLAP::CmdLine cmd("CNA algorithm");

        // pattern file
        TCLAP::ValueArg<std::string> arg_fingerprint("p", "pattern", "Pattern file (i.e. fingerprints.json)", true, "", "path");

        // list fingerprints
        TCLAP::SwitchArg swarg_list("l","list","List all fingerprints in pattern file", cmd, false);

        // create new empty patterns file
        TCLAP::SwitchArg swarg_create("c","create","Create a new empty patterns file", cmd, false);

        // add a new pattern
        TCLAP::ValueArg<std::string> arg_newpattern("a", "add", "Add a new pattern to the file", false, "", "patterndata");

        // find pattern by label
        TCLAP::ValueArg<std::string> arg_find_label("l", "label", "Find pattern by label", false, "", "labelname");

        // add and parse arguments
        cmd.add(arg_fingerprint);
        cmd.add(arg_newpattern);
        cmd.parse(argc, argv);

        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Running bramble-tool v." << PROGRAM_VERSION << std::endl;
        std::cout << "Author: Ivo Filot <i.a.w.filot@tue.nl>" << std::endl;
        std::cout << "Documentation: https://bramble.imc-tue.nl" << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Git Hash: " << PROGRAM_GIT_HASH << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;

        auto start = std::chrono::system_clock::now();

        // create a new empty patterns file
        if(swarg_create.getValue()) {
            if(std::filesystem::exists(arg_fingerprint.getValue())) {
                std::cout << "Are you sure you want to overwrite existing pattern file (Y/N)?" << std::endl;
                char ans = 'N';
                std::cin >> ans;

                if(!(ans == 'Y' || ans == 'y')) {
                    std::cout << "Aborting!" << std::endl;
                    return -1;
                }
            }

            PatternLibrary pl;
            pl.store_pattern_library(arg_fingerprint.getValue());

            std::cout << "Storing pattern file as: " << arg_fingerprint.getValue() << std::endl;

            std::cout << "--------------------------------------------------------------" << std::endl;
            std::cout << "Done." << std::endl;
            return 0;
        }

        // if no new pattern library is requested, try to load the existing one
        PatternLibrary pl;
        try {
            pl = PatternLibrary(arg_fingerprint.getValue());
        } catch(const std::exception& e) {
            std::cerr << "A problem occurred reading the pattern file." << std::endl;
            std::cerr << "Error: " << e.what() << std::endl;
            return -1;
        }


        // add a new pattern to the file
        if(!arg_newpattern.getValue().empty()) {
            std::vector<std::string> pieces;

            std::string patternstr = arg_newpattern.getValue();
            boost::algorithm::trim_all(patternstr);
            boost::split(pieces, patternstr, boost::is_any_of(";"));

            // check if number of items is correct
            if(pieces.size() != 4) {
                throw std::runtime_error("Invalid pattern description: " + patternstr);
            }

            // clean pieces
            for(auto& piece : pieces) {
                boost::algorithm::trim_all(piece);
            }

            // add key - pattern - name - color
            pl.add_pattern(pieces[0], pieces[1], pieces[2], pieces[3]);

            std::cout << "Adding pattern:" << std::endl;
            std::cout << "    Key: " << pieces[0] << std::endl;
            std::cout << "    Pattern: " << pieces[1] << std::endl;
            std::cout << "    Name: " << pieces[2] << std::endl;
            std::cout << "    Color: " << pieces[3] << std::endl;

            pl.store_pattern_library(arg_fingerprint.getValue());
            std::cout << "Storing pattern file as: " << arg_fingerprint.getValue() << std::endl;

            std::cout << "--------------------------------------------------------------" << std::endl;
            std::cout << "Done." << std::endl;
            return 0;
        }

        // print list of patterns, stop after printing
        if(swarg_list.getValue()) {
            std::cout << "Printing list of patterns:" << std::endl;
            for(const auto& item : pl.get_patterns()) {
                std::cout << (boost::format("%22s %8s %s")
                                % item.second.get_name()
                                % item.second.get_color()
                                % item.second.get_fingerprint()).str() << std::endl;
            }
            std::cout << "--------------------------------------------------------------" << std::endl;
            std::cout << "Done." << std::endl;
            return 0;
        }

        // find a pattern
        if(!arg_find_label.getValue().empty()) {
            std::cout << "Searching for pattern: " << arg_find_label.getValue() << std::endl;
            auto pattern = pl.get_pattern(arg_find_label.getValue());
        }

        return 0;

    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() <<
                     " for arg " << e.argId() << std::endl;
        return -1;
    }

    return 0;
}
