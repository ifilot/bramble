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

#include "geometry_reader.h"

GeometryReader::GeometryReader() {}

void GeometryReader::read_file(const std::string& filename,
                               bool centerstruc) {
    if(!std::filesystem::exists(filename)) {
        throw std::runtime_error("Cannot access file " + filename);
    }

    // capture extension and base filename
    const std::string ext = std::filesystem::path(filename).extension().string();
    const std::string fname = std::filesystem::path(filename).filename().string();

    if(ext == ".geo") {
        std::cout << "Loading " << filename << " as .geo file." << std::endl;
        this->read_geo(filename, centerstruc);
    } else if(ext == ".xyz") {
        std::cout << "Loading " << filename << " as .xyz file." << std::endl;
        this->read_xyz(filename, centerstruc);
    } else if(fname.substr(0,6) == "POSCAR" || fname.substr(0,7) == "CONTCAR") {
        std::cout << "Loading " << filename << " as POSCAR/CONTCAR file." << std::endl;
        this->read_poscar(filename, centerstruc);
        this->periodic = true;
    } else {
        throw std::runtime_error("Unknown file type: " + filename);
    }

    std::cout << "Geometry file contains " << this->get_nr_atoms() << " atoms." << std::endl;
}

/**
 * @brief      Read a .geo type of file
 *
 * @param[in]  filename  The filename of the .geo file
 */
void GeometryReader::read_geo(const std::string& filename,
                              bool centerstruc) {
    std::ifstream file (filename);

    if(!file.is_open()) {
        throw std::runtime_error("Could not open " + filename);
    }

    std::vector<std::string> lines;
    std::string line;
    while(getline(file,line)) {
        boost::algorithm::trim_all(line); // trim leading, trailing and middle spaces
        replace(line.begin(), line.end(), '\t', ' '); // trim middle tabs to single space
        lines.push_back(line);
    }

    std::vector<std::string> pieces; // temporary storage of split file line as string vector
    // find how many atoms are in the file
    int nr_atoms;
    for(unsigned int i=lines.size(); i>0; --i) {
        if(!lines[i].empty()) {
            nr_atoms = i-2; // minus 3 for the first 3 rows, which contain no atoms (but then +1 becuase that works)
            break;
        }
    }

    // get elements and atom positions
    this->elements.resize(nr_atoms);
    this->atom_positions.resize(nr_atoms);
    for(unsigned int i=0; i<nr_atoms; ++i) {
        boost::split(pieces,lines[i+3],boost::is_any_of("\t "));
        this->elements[i] = pieces[1];
        for(unsigned int j=2; j<pieces.size(); j++){
            this->atom_positions[i][j-2] = boost::lexical_cast<double>(pieces[j]);
        }
    }

    if(centerstruc) {
        Vec3 center = this->find_center_of_mass();

        for(unsigned int i=0; i<this->atom_positions.size(); i++) {
            this->atom_positions[i] -= center;
        }
    }

    // create unit cell
    this->unitcell.resize(3,3);
    Vec3 max = this->find_max_coordinates();

    for(int i=0; i<this->unitcell.rows(); ++i) {
        for(unsigned int j=0; j<3; j++) {
            this->unitcell(i,j) *= max[j] * 5;
        }
    }
}

/**
 * @brief      Constructs the object.
 *
 * @param[in]  filename  The filename of the .xyz file
 */
void GeometryReader::read_xyz(const std::string& filename,
                              bool centerstruc) {
    std::ifstream file (filename);

    if(!file.is_open()) {
        throw std::runtime_error("Could not open " + filename);
    }

    std::vector<std::string> lines;
    std::string line;
    while(getline(file,line)) {
        boost::algorithm::trim_all(line); // trim leading, trailing and middle spaces
        replace(line.begin(), line.end(), '\t', ' '); // trim middel tabs to single space
        lines.push_back(line);
    }

    int nr_atoms = std::stoi(lines[0]); // number of atoms that should be read from file

    // check if file has correct number of lines
    // if(lines.size() != nr_atoms+2) {
    //     throw std::runtime_error("Incorrect number of atoms found in " + filename);
    // }

    // get elements and atom positions
    std::vector<std::string> pieces; // temporary storage of split file line as string vector
    this->elements.resize(nr_atoms);
    this->atom_positions.resize(nr_atoms);
    for(unsigned int i=0; i<nr_atoms; i++) {
        boost::split(pieces,lines[i+2],boost::is_any_of(" "));
        this->elements[i] = pieces[0];
        for(unsigned int j=1; j<pieces.size(); j++){
            this->atom_positions[i][j-1] = std::stod(pieces[j]);
        }
    }

    if(centerstruc) {
        Vec3 center = this->find_center_of_mass();

        for(unsigned int i=0; i<this->atom_positions.size(); i++) {
            this->atom_positions[i] -= center;
        }
    }

    // get unit cell
    this->unitcell.resize(3,3);
    Vec3 max = this->find_max_coordinates();

    for(int i=0; i<this->unitcell.rows(); ++i) {
        for(unsigned int j=0; j<3; j++) {
            this->unitcell(i,j) *= max[j] * 5;
        }
    }
}

/**
 * @brief      Load structure from POSCAR file
 *
 * @param[in]  filename  The filename
 *
 * @return     Structure
 */
void GeometryReader::read_poscar(const std::string& filename,
                                 bool centerstruc) {
    std::ifstream infile(filename);

    if(!infile.is_open()) {
        throw std::runtime_error("Could not open " + filename);
    }

    std::string line;

    // skip first line (name of system)
    std::getline(infile, line);

    // read scaling factor
    std::getline(infile, line);
    boost::trim(line);
    double scalar = boost::lexical_cast<double>(line);

    // read matrix
    std::vector<std::string> pieces;
    for(unsigned int j=0; j<3; j++) {
        std::getline(infile, line);
        boost::trim(line);
        boost::split(pieces, line, boost::is_any_of(" \t"), boost::token_compress_on);
        for(unsigned int i=0; i<3; i++) {
            this->unitcell(j,i) = boost::lexical_cast<double>(pieces[i]);
        }
    }
    unitcell *= scalar;

    // assume that POSCARS are POSCAR5
    std::getline(infile, line);
    const boost::regex regex_els("^.*[A-Za-z]+.*$"); // if the line contains even a single non-numeric character, it is a line containing elements
    boost::smatch what;
    if(!boost::regex_match(line, what, regex_els)) {
        throw std::runtime_error("This file is probably a VASP4 POSCAR file. You can only load VASP5+ POSCAR files");
    }
    std::vector<std::string> elements;
    boost::trim(line);
    boost::split(elements, line, boost::is_any_of(" \t"), boost::token_compress_on);

    // get the number for each element
    std::getline(infile, line);
    boost::trim(line);
    std::vector<unsigned int> nr_elements;
    boost::split(pieces, line, boost::is_any_of(" \t"), boost::token_compress_on);
    for(unsigned int i=0; i<pieces.size(); i++) {
        nr_elements.push_back(boost::lexical_cast<unsigned int>(pieces[i]));
    }
    if(nr_elements.size() != elements.size()) {
        throw std::runtime_error("Array size for element types does not match array size for number for each element type.");
    }

    // check if next line is selective dynamics, if so, skip
    bool selective_dynamics = false;
    std::getline(infile, line);
    const boost::regex regex_sd("^\\s*[Ss].*$");
    if(boost::regex_match(line, what, regex_sd)) {
        selective_dynamics = true;
        std::getline(infile, line);
    }

    // direct or cartesian
    bool direct = (line[0] == 'D' || line[0] == 'd') ? true : false;

    // collect atoms
    static const boost::regex regex_double3("^\\s*([0-9e.-]+)\\s+([0-9e.-]+)\\s+([0-9e.-]+)\\s*(.*)$");
    static const boost::regex regex_double3_bool3("^\\s*([0-9e.-]+)\\s+([0-9e.-]+)\\s+([0-9e.-]+)\\s+([TF])\\s+([TF])\\s+([TF])\\s*(.*)$");

    for(unsigned int i=0; i<elements.size(); i++) {
        for(unsigned int j=0; j<nr_elements[i]; j++) {
            std::getline(infile, line);
            boost::smatch what;

            if(selective_dynamics) {
                if(boost::regex_match(line, what, regex_double3_bool3)) {
                    float x = boost::lexical_cast<float>(what[1]);
                    float y = boost::lexical_cast<float>(what[2]);
                    float z = boost::lexical_cast<float>(what[3]);
                    bool sx = (what[4] == "F" ? false : true);
                    bool sy = (what[5] == "F" ? false : true);
                    bool sz = (what[6] == "F" ? false : true);

                    Vec3 position(x,y,z);

                    // build coordinates
                    if(direct) {
                        Vec3 cartesian = unitcell.transpose() * position;
                        this->atom_positions.push_back(cartesian);
                    } else {
                        this->atom_positions.push_back({x,y,z});
                    }
                    this->elements.push_back(elements[i]);
                }
            } else {
                if(boost::regex_match(line, what, regex_double3)) {
                    float x = boost::lexical_cast<float>(what[1]);
                    float y = boost::lexical_cast<float>(what[2]);
                    float z = boost::lexical_cast<float>(what[3]);
                    Vec3 position(x,y,z);

                    // build coordinates
                    if(direct) {
                        Vec3 cartesian = unitcell.transpose() * position;
                        this->atom_positions.push_back(cartesian);
                    } else {
                        this->atom_positions.push_back({x,y,z});
                    }
                    this->elements.push_back(elements[i]);
                }
            }
        }
    }
}


/**
 * @brief      find the center of mass of the particle (assume equal mass)
 *
 * @return     center of mass of the particle
 */
Vec3 GeometryReader::find_center_of_mass() {
    Vec3 center(0, 0, 0);

    for(unsigned int i=0; i<this->atom_positions.size(); i++) {
        center += this->atom_positions[i];
    }
    center /= (double)this->atom_positions.size();

    return center;
}

/**
 * @brief      find larges absolute x, y and z value
 *
 * @return     largest absolute x,y,z as vector
 */
Vec3 GeometryReader::find_max_coordinates() {
    Vec3 max;

    for(unsigned int i=0; i<this->atom_positions.size(); ++i) {
        max = max.cwiseMax(this->atom_positions[i].cwiseAbs());
    }

    return max;
}
