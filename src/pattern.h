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

std::ostream &operator<<(std::ostream &os, const Pattern& m);

#endif //_PATTERN_H
