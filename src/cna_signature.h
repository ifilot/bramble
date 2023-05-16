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

#ifndef _CNA_SIGNATURE
#define _CNA_SIGNATURE

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <memory>
#include <vector>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/format.hpp>

#include "math.h"

typedef float Weight;
typedef boost::property<boost::vertex_name_t, std::string> NameProperty;
typedef boost::property <boost::edge_weight_t, int> WeightProperty;
typedef boost::property<boost::vertex_index_t, int> IndexProperty;
typedef boost::adjacency_list < boost::listS, boost::vecS, boost::undirectedS, NameProperty, WeightProperty > Graph;
typedef boost::graph_traits < Graph >::vertex_descriptor GVertex;
typedef boost::property_map < Graph, boost::vertex_index_t >::type IndexMap;
typedef boost::property_map < Graph, boost::vertex_name_t >::type NameMap;

typedef boost::iterator_property_map < GVertex*, IndexMap, GVertex, GVertex& > PredecessorMap;
typedef boost::iterator_property_map < Weight*, IndexMap, Weight, Weight& > DistanceMap;

class CNA_SIGNATURE {
private:
    unsigned int nr_neighbors;
    unsigned int nr_neighbor_edges;
    unsigned int longest_path;

public:
    /**
     * @brief      Constructs the signature object
     *
     * @param[in]  matrix  The adjacency matrix
     */
    CNA_SIGNATURE(const MatrixXXb& matrix, size_t atid);

    inline unsigned int get_nr_neighbors() const {
        return this->nr_neighbors;
    }

    inline unsigned int get_nr_neighbor_edges() const {
        return this->nr_neighbor_edges;
    }

    inline unsigned int get_longest_path() const {
        return this->longest_path;
    }

    std::string get_str() const;
};

std::ostream &operator<<(std::ostream &os, const CNA_SIGNATURE& m);

#endif // _CNA_SIGNATURE
