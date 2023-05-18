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

#include "pattern.h"

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

std::ostream &operator<<(std::ostream &os, const Pattern& m) {
    os << m.get_fingerprint();
    return os;
}
