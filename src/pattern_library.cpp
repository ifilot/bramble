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

#include "pattern_library.h"

PatternLibrary::PatternLibrary() {
    // at least add the empty pattern

    // add key - pattern - name - color
    this->add_pattern("unknown", "", "Unknown", "000000");
}

/**
 * @brief      pattern library constructor
 *
 * @param[in]  filename  json file wherein patterns are stored
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

    // check that the pattern library contains at least one entry with the item
    // unknown, if not, throw an error
    try {
        auto res = this->get_pattern("");
        if(res.get_key() != "unknown" || res.get_name() != "Unknown") {
            std::cerr << "    Key name: " << res.get_key() << std::endl;
            std::cerr << "    Label name: " << res.get_name() << std::endl;
            throw std::runtime_error("Incorrect label encountered for empty pattern.");
        }
    } catch(const std::exception& e) {
        if(std::string(e.what()) == "Unknown pattern key: ") {
            std::cerr << "Unknown pattern is missing in library" << std::endl;
            std::cerr << "Every pattern library should contain the 'unknown' element." << std::endl;
            throw std::runtime_error("Invalid or incorrectly formatted patterns file.");
        } else {
            throw;
        }
    }
}

/**
 * @brief      Stores a pattern library.
 *
 * @param[in]  filename  The filename
 */
void PatternLibrary::store_pattern_library(const std::string& filename) const {
    std::ofstream out(filename);

    static const std::string tab4("    ");
    static const std::string tab8 = tab4 + tab4;
    static const std::string tab12 = tab8 + tab4;

    // header
    out << "{" << std::endl;
    out << tab4 << "\"patterns\":" << std::endl;
    out << tab4 << "{" << std::endl;

    // all patterns
    for(auto item = this->patterns.begin(); item != this->patterns.end(); item++) {
        out << tab8 << "\"" << item->second.get_key() << "\":" << std::endl;
        out << tab8 << "{" << std::endl;
        out << tab12 << "\"label\": \"" << item->second.get_name() << "\"," << std::endl;
        out << tab12 << "\"fingerprint\": \"" << item->second.get_fingerprint() << "\"," << std::endl;
        out << tab12 << "\"color\": \"" << item->second.get_color() << "\"" << std::endl;

        if(std::next(item) == this->patterns.end()) {
            out << tab8 << "}" << std::endl;
        } else {
            out << tab8 << "}," << std::endl;
        }
    }

    // footer
    out << tab4 << "}" << std::endl;
    out << "}" << std::endl;
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
    // check if pattern is valid
    if(!this->is_valid_pattern(pattern)) {
        throw std::runtime_error("Pattern \"" + pattern + "\" is invalid!");
    }

    // check if key is valid
    if(!this->is_valid_key(key)) {
        throw std::runtime_error("Key \"" + key + "\" is invalid!");
    }

    // check if color code is valid
    if(!this->is_valid_colorcode(color)) {
        throw std::runtime_error("Color code \"" + color + "\" is invalid!");
    }

    auto got = this->patterns.find(pattern);

    if(got == this->patterns.end()) {
        this->patterns.emplace(pattern, Pattern(key, name, pattern, color));
        this->patterns_labelkey.emplace(key, Pattern(key, name, pattern, color));
    } else {
        throw std::runtime_error("Pattern {" + pattern + "} already exists in library.");
    }
}

/**
 * @brief      edit a pattern in library
 *
 * @param[in]  key      JSON key
 * @param[in]  pattern  fingerprint
 * @param[in]  name     name or label of the pattern
 * @param[in]  color    color
 */
void PatternLibrary::edit_pattern(const std::string& key, const std::string& pattern,
                                  const std::string& name, const std::string& color) {

    // find key
    const auto& got = this->get_pattern_by_key(key);
    std::string oldpattern = got.get_fingerprint();

    // check if pattern is valid
    if(!this->is_valid_pattern(pattern)) {
        throw std::runtime_error("Pattern \"" + pattern + "\" is invalid!");
    }

    // check if color code is valid
    if(!this->is_valid_colorcode(color)) {
        throw std::runtime_error("Color code \"" + color + "\" is invalid!");
    }

    try {
        this->remove_pattern(key, oldpattern);
    } catch(const std::exception& e) {
        throw std::runtime_error("Unable to remove key: " + key);
    }

    this->add_pattern(key, pattern, name, color);
}

/**
 * @brief      remove a pattern in library
 *
 * @param[in]  key      JSON key
 * @param[in]  pattern  fingerprint
 */
void PatternLibrary::remove_pattern(const std::string& key, const std::string& pattern) {
    this->patterns.erase(pattern);
    this->patterns_labelkey.erase(key);
}

/**
 * @brief      remove a pattern in library
 *
 * @param[in]  key      JSON key
 */
void PatternLibrary::delete_pattern_by_key(const std::string& key) {
    auto got = this->get_pattern_by_key(key);
    this->remove_pattern(got.get_key(), got.get_fingerprint());
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
    } else if(pattern.empty() && got == this->patterns.end()) {
        throw std::logic_error("Library does not contain the empty pattern.");
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
 * @brief      Determines whether the specified pattern is valid pattern.
 *
 * @param[in]  pattern  The pattern
 *
 * @return     True if the specified pattern is valid pattern, False otherwise.
 */
bool PatternLibrary::is_valid_pattern(const std::string& pattern) const {
    const boost::regex regex_cna_pattern("^([1-9]+\\([0-9]+,[0-9]+,[0-9]+\\))*$");
    boost::smatch what;
    return boost::regex_match(pattern, what, regex_cna_pattern);
}

/**
 * @brief      Determines whether the specified key is valid key.
 *
 * @param[in]  key  The key
 *
 * @return     True if the specified key is valid key, False otherwise.
 */
bool PatternLibrary::is_valid_key(const std::string& key) const {
    const boost::regex regex_key("^[A-Za-z0-9_-]+$");
    boost::smatch what;
    return boost::regex_match(key, what, regex_key);
}


/**
 * @brief      Determines whether the specified colorcode is valid colorcode.
 *
 * @param[in]  colorcode  The colorcode
 *
 * @return     True if the specified colorcode is valid colorcode, False otherwise.
 */
bool PatternLibrary::is_valid_colorcode(const std::string& colorcode) const {
    const boost::regex regex_color_code("^[A-Fa-f0-9]{6}$");
    boost::smatch what;
    return boost::regex_match(colorcode, what, regex_color_code);
}
