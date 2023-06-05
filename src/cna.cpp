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

#include "cna.h"

/**
 * @brief      Constructs the CNA object
 */
CNA::CNA(const std::shared_ptr<PatternLibrary>& _pl) :
    pl(_pl) {}

/**
 * @brief      analyze dataset
 *
 * @param[in]  _state  pointer to state class
 */
void CNA::analyze(const std::shared_ptr<State>& _state) {

    if(!_state) {
        throw std::runtime_error("NULLPTR detected in state pointer: " +
            (boost::format("%i") % __LINE__).str() + " in " +
            std::string(__FILE__) + ".");
    }

    std::cout << "Start common neighbor analysis..." << std::endl;

    // clear contents when doing new analysis
    this->distances.resize(0,0); // clear matrix contents
    this->neighbor_ids.clear();  // clear neighbors
    this->fingerprints.clear();  // clear fingerprint pattern
    this->state = _state;

    // preallocate neigbor ids
    this->neighbor_ids.resize(this->state->get_nr_atoms());

    // calculate all distances
    this->distances = this->state->get_distances();

    // expand adjacency matrix vector
    this->adjacency_matrices.resize(this->state->get_nr_atoms());

    // collect fingerprint for each atom
    this->fingerprints.resize(this->state->get_nr_atoms());

    #pragma omp parallel for
    for(unsigned int i=0; i<this->state->get_nr_atoms(); i++) {
        this->fingerprints[i] = this->calculate_fingerprint(i);
    }

    std::cout << "Common neighbor analysis completed." << std::endl;
}

/**
 * @brief      calculates fingerprint for an atom
 *
 * @param[in]  atid  atom identifier
 *
 * @return     The fingerprint.
 */
std::string CNA::calculate_fingerprint(size_t atid) {
    // construct reduced adjacency matrix from distance matrix
    this->adjacency_matrices[atid] = this->construct_adjancy_matrix(atid, this->calculate_cutoff(atid));

    return this->calculate_fingerprint_from_adjacency_matrix(this->adjacency_matrices[atid]);
}

/**
 * @brief      Calculates the fingerprint from adjacency matrix.
 *
 * @param[in]  am    adjacency matrix
 *
 * @return     the fingerprint
 */
std::string CNA::calculate_fingerprint_from_adjacency_matrix(const MatrixXXb& am) {
    // construct signature from adjacency matrix
    std::vector<CNATriplet> cna_triplets;
    for(unsigned int i=0; i<am.rows(); i++) {
        cna_triplets.push_back(CNATriplet(am, i));
    }

    // count number of similar CNA cna_triplets
    std::map<std::string, int, std::greater<std::string>> map;
    for(const auto& sign : cna_triplets) {
        auto got = map.find(sign.get_str());
        if(got != map.end()) {
            got->second++;
        } else {
            map.emplace(sign.get_str(), 1);
        }
    }

    // reduce set of CNA triplets to reduce on fingerprint string size
    std::stringstream str;
    for(auto p : map) {
        str << p.second << p.first;
    }

    return str.str();
}

/**
 * @brief      construct reduced adjacency matrix from distance matrix
 *
 * @param[in]  atid    atom id of interest
 * @param[in]  cutoff  cutoff distance (smaller assumes bond)
 *
 * @return     reduced adjacency matrix
 */
MatrixXXb CNA::construct_adjancy_matrix(size_t atid, float cutoff) {
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

    // construct adjacency matrix
    MatrixXXb am(this->neighbor_ids[atid].size(), this->neighbor_ids[atid].size());
    for(unsigned int i=0; i<this->neighbor_ids[atid].size(); i++) {
        for(unsigned int j=i; j<this->neighbor_ids[atid].size(); j++) {
            if(i == j) {
                am(i,j) = false;
                continue;
            }

            unsigned int id1 = this->neighbor_ids[atid][i];
            unsigned int id2 = this->neighbor_ids[atid][j];

            if(this->distances(id1, id2) < cutoff) {
                am(i,j) = am(j,i) = true;
            } else {
                am(i,j) = am(j,i) = false;
            }
        }
    }

    return am;
}

/**
 * @brief      calculate cutoff distance
 *
 * @param[in]  atid  atom id
 *
 * @return     cutoff distance
 */
float CNA::calculate_cutoff(size_t atid) {
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

/**
 * @brief      Writes the CNA analysis to a file
 *
 * @param[in]  filename  The filename
 */
void CNA::write_analysis(const std::string& filename) {
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
    outfile << "Number of atoms: " << this->fingerprints.size() << std::endl;
    outfile << dashed_lines << std::endl;

    // print header
    outfile << "#     atom           x             y             z   color       pattern  fingerprint" << std::endl;
    outfile << dashed_lines << std::endl;

    // collecting statistics
    std::unordered_map<std::string, unsigned int> abundancies;

    // print finger prints and analysis
    for(unsigned int i=0; i<this->fingerprints.size(); ++i) {
        const auto& atompos = this->state->get_atom_pos_cartesian(i);
        const auto& element = this->state->get_elements()[i];
        const std::string fingerprintstr = this->fingerprints[i];

        if(abundancies.find(fingerprintstr) != abundancies.end()) {
            abundancies.find(fingerprintstr)->second++;
        } else {
            abundancies.emplace(fingerprintstr, 1);
        }

        outfile << boost::format("%04i  %2s  %12.6f  %12.6f  %12.6f  %6s  %12s  %s\n")
                    % (i+1)
                    % element
                    % atompos[0]
                    % atompos[1]
                    % atompos[2]
                    % this->pl->get_pattern(fingerprintstr).get_color()
                    % this->pl->identify_pattern(fingerprintstr)
                    % fingerprintstr;
    }

    outfile << dashed_lines << std::endl << std::endl;

    // dump abundancy map into a vector and sort it
    std::vector<std::pair<std::string, unsigned int> > abundancies_v(abundancies.begin(), abundancies.end());
    std::sort(abundancies_v.begin(), abundancies_v.end(), greater_second<std::string, unsigned int>());

    // output statistics
    outfile << "Statistics" << std::endl;
    outfile << dashed_lines << std::endl;
    outfile << " #atom    perc      pattern fingerprint" << std::endl;
    outfile << dashed_lines << std::endl;
    for(const auto& item : abundancies_v) {
        outfile << boost::format("%6i  %5.2f%% %12s %s\n")
                    % item.second
                    % ((double)item.second / (double)this->fingerprints.size() * 100.)
                    % this->pl->identify_pattern(item.first)
                    % item.first;
    }
    outfile << std::endl;

    // print adjacency matrices
    outfile << dashed_lines << std::endl;
    outfile << "ADJACENCY MATRICES" << std::endl;
    outfile << dashed_lines << std::endl << std::endl;
    for(unsigned int i=0; i<this->adjacency_matrices.size(); i++) {
        outfile << dashed_lines << std::endl;
        outfile << "Atom " << (i+1) << " ( ";
        for(auto id : this->neighbor_ids[i]) {
            if(id > this->state->get_nr_atoms()) {
                outfile << "/" << ((id+1) % this->state->get_nr_atoms()) << "/" << " ";
            } else {
                outfile << (id+1) << " ";
            }
        }
        outfile << ")" << std::endl;
        outfile << dashed_lines << std::endl;
        outfile << std::fixed << std::setprecision(1) << this->adjacency_matrices[i] << std::endl;
        outfile << std::endl;
    }

    outfile << dashed_lines << std::endl;
    outfile << "Done" << std::endl;

    outfile.close();
}
