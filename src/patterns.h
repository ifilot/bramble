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

#ifndef _PATTERN_H
#define _PATTERN_H

#include "config.h"

#include <iostream>
#include <unordered_map>
#include <filesystem>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include <Eigen/Dense>
typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> MatrixXXb;

class Pattern {
private:
    std::string key;
    std::string pattern_name;
    std::string pattern;
    std::string color;

public:
    /**
     * @brief      Constructs the object.
     *
     * @param[in]  _key      JSON key
     * @param[in]  _name     name
     * @param[in]  _pattern  fingerprint
     * @param[in]  _color    color
     */
    Pattern(const std::string& _key, const std::string& _name,
        const std::string& _pattern, const std::string& _color);

    /**
     * @brief      Gets the key.
     *
     * @return     The key.
     */
    inline const std::string& get_key() const {
        return this->key;
    }

    /**
     * @brief      Gets the name.
     *
     * @return     The name.
     */
    inline const std::string& get_name() const {
        return this->pattern_name;
    }

    /**
     * @brief      Gets the fingerprint.
     *
     * @return     The fingerprint.
     */
    inline const std::string& get_fingerprint() const {
        return this->pattern;
    }

    /**
     * @brief      Gets the color.
     *
     * @return     The color.
     */
    inline const std::string& get_color() const {
        return this->color;
    }

};

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
    PatternLibrary(const std::string& filename);

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

private:
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
};

#endif // _PATTERN_H
