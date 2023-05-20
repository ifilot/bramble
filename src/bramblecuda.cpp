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

#include <iostream>

#include "config.h"
#include "card_manager.h"

int main(int argc, char* argv[]) {
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "Running bramble-tool v." << PROGRAM_VERSION << std::endl;
    std::cout << "Author: Ivo Filot <i.a.w.filot@tue.nl>" << std::endl;
    std::cout << "Documentation: https://bramble.imc-tue.nl" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Git Hash: " << PROGRAM_GIT_HASH << std::endl;
    #ifdef MOD_CUDA
    std::cout << "Modules: CUDA" << std::endl;
    #endif // MOD_CUDA
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "Testing CUDA..." << std::endl;

    CardManager cm;
    cm.probe_cards();

    return 0;
}
