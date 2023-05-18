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

#ifndef _PATTERN_LIBRARY_H
#define _PATTERN_LIBRARY_H

#include "config.h"
#include "math.h"

#include <iostream>
#include <unordered_map>
#include <filesystem>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

#include "pattern.h"

class PatternLibrary {
private:
    std::unordered_map<std::string, Pattern> patterns;               // patterns map sorted by fingerprint
    std::unordered_map<std::string, Pattern> patterns_labelkey;      // patterns map sorted by key
    boost::property_tree::ptree patterns_json_root;                  // json data

    std::unordered_map<std::string, MatrixXXb> adjacency_matrices;   // map of adjacency_matrices

public:
    /**
     * @brief      pattern library constructor
     */
    PatternLibrary();

    /**
     * @brief      pattern library constructor
     *
     * @param[in]  filename  json file wherein patterns are stored
     */
    PatternLibrary(const std::string& filename);

    /**
     * @brief      Stores a pattern library.
     *
     * @param[in]  filename  The filename
     */
    void store_pattern_library(const std::string& filename) const;

    /**
     * @brief      get the patterns
     *
     * @return     The patterns map
     */
    inline const auto& get_patterns() const {
        return this->patterns_labelkey;
    }

    /**
     * @brief      identify the pattern based on the fingerprint
     *
     * @param[in]  pattern  fingerprint of the pattern
     *
     * @return     name of the pattern
     */
    const std::string& identify_pattern(const std::string& pattern) const;

    /**
     * @brief      get pattern using fingerprint
     *
     * @param[in]  pattern  fingerprint of the pattern
     *
     * @return     Pattern object
     */
    const Pattern& get_pattern(const std::string& pattern) const;

    /**
     * @brief      get pattern using key
     *
     * @param[in]  pattern key
     *
     * @return     Pattern object
     */
    const Pattern& get_pattern_by_key(const std::string& key) const;

    /**
     * @brief      add pattern to library
     *
     * @param[in]  key      JSON key
     * @param[in]  pattern  fingerprint
     * @param[in]  name     name or label of the pattern
     * @param[in]  color    color
     */
    void add_pattern(const std::string& key, const std::string& pattern,
                     const std::string& name, const std::string& color);

    /**
     * @brief      edit a pattern in library
     *
     * @param[in]  key      JSON key
     * @param[in]  pattern  fingerprint
     * @param[in]  name     name or label of the pattern
     * @param[in]  color    color
     */
    void edit_pattern(const std::string& key, const std::string& pattern,
                      const std::string& name, const std::string& color);

    /**
     * @brief      remove a pattern in library
     *
     * @param[in]  key      JSON key
     * @param[in]  pattern  fingerprint
     */
    void remove_pattern(const std::string& key, const std::string& pattern);

    /**
     * @brief      remove a pattern in library
     *
     * @param[in]  key      JSON key
     */
    void delete_pattern_by_key(const std::string& key);

    /**
     * @brief      Determines whether the specified pattern is valid pattern.
     *
     * @param[in]  pattern  The pattern
     *
     * @return     True if the specified pattern is valid pattern, False otherwise.
     */
    bool is_valid_pattern(const std::string& pattern) const;

    /**
     * @brief      Determines whether the specified key is valid key.
     *
     * @param[in]  key  The key
     *
     * @return     True if the specified key is valid key, False otherwise.
     */
    bool is_valid_key(const std::string& key) const;

    /**
     * @brief      Determines whether the specified colorcode is valid colorcode.
     *
     * @param[in]  colorcode  The colorcode
     *
     * @return     True if the specified colorcode is valid colorcode, False otherwise.
     */
    bool is_valid_colorcode(const std::string& colorcode) const;

private:
};

#endif // _PATTERN_LIBRARY_H
