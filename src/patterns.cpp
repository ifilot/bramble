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

#include "patterns.h"

/**
 * @brief      Constructs the object.
 *
 * @param[in]  _key      JSON key
 * @param[in]  _name     name
 * @param[in]  _pattern  fingerprint
 */
Pattern::Pattern(const std::string& _key,
                 const std::string& _name,
                 const std::string& _pattern,
                 const std::string& _color) :
key(_key),
pattern_name(_name),
pattern(_pattern),
color(_color)
{

}

/**
 * @brief      pattern library constructor
 */
PatternLibrary::PatternLibrary(const std::string& filename) {
    if(!std::filesystem::exists(filename)) {
        throw std::runtime_error("Cannot access " + filename);
    }

    // read JSON file
    boost::property_tree::read_json(filename, this->patterns_json_root);

    // parse patterns
    auto node = this->patterns_json_root.get_child("patterns");
    for(const auto& ptn : node) {   // loop over children
        const std::string key = ptn.first;
        const std::string fingerprint = this->patterns_json_root.get<std::string>("patterns."+key+".fingerprint");
        const std::string label = this->patterns_json_root.get<std::string>("patterns."+key+".label");
        const std::string colstr = this->patterns_json_root.get<std::string>("patterns."+key+".color");

        this->add_pattern(key, fingerprint, label, colstr);
    }
}

/**
 * @brief      identify the pattern based on the fingerprint
 *
 * @param[in]  pattern  fingerprint of the pattern
 *
 * @return     name of the pattern
 */
const std::string& PatternLibrary::identify_pattern(const std::string& pattern) const {
    return this->get_pattern(pattern).get_name();
}

/**
 * @brief      get pattern using fingerprint
 *
 * @param[in]  pattern  fingerprint of the pattern
 *
 * @return     Pattern object
 */
const Pattern& PatternLibrary::get_pattern(const std::string& pattern) const {
    auto got = this->patterns.find(pattern);

    if(got != this->patterns.end()) {
        return got->second;
    } else {
        return this->get_pattern("");
    }
}

/**
 * @brief      get pattern using key
 *
 * @param[in]  pattern key
 *
 * @return     Pattern object
 */
const Pattern& PatternLibrary::get_pattern_by_key(const std::string& key) const {
    auto got = this->patterns_labelkey.find(key);

    if(got != this->patterns_labelkey.end()) {
        return got->second;
    } else {
        throw std::runtime_error("Unknown pattern key: " + key);
    }
}

/**
 * @brief      add pattern to library
 *
 * @param[in]  key      JSON key
 * @param[in]  pattern  fingerprint
 * @param[in]  name     name or label of the pattern
 */
void PatternLibrary::add_pattern(const std::string& key, const std::string& pattern,
    const std::string& name, const std::string& color) {
    auto got = this->patterns.find(pattern);

    if(got == this->patterns.end()) {
        this->patterns.emplace(pattern, Pattern(key, name, pattern, color));
        this->patterns_labelkey.emplace(key, Pattern(key, name, pattern, color));
    } else {
        std::cerr << "Pattern {" << pattern << "} already exists in library." << std::endl;
    }
}
