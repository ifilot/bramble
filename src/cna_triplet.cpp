#include "cna_triplet.h"

/**
 * @brief      Constructs the signature object
 *
 * @param[in]  matrix  The adjacency matrix
 */
CNATriplet::CNATriplet(const MatrixXXb& matrix, size_t atid) :
    nr_neighbors(0),
    nr_neighbor_edges(0),
    longest_path(0)
{
    // identify neighbors
    std::vector<unsigned int> neighbor_ids;
    for(unsigned int i=0; i<matrix.rows(); i++) {
        if(matrix(i, atid)) {
            neighbor_ids.push_back(i);
            this->nr_neighbors++;
        }
    }

    // calculate number of edges between neighbors
    // first construct a graph
    Graph g(this->nr_neighbors);
    for(unsigned int i=0; i<neighbor_ids.size(); i++) {
        const size_t atid1 = neighbor_ids[i];
        for(unsigned int j=i+1; j<neighbor_ids.size(); j++) {
            const size_t atid2 = neighbor_ids[j];
            if(matrix(atid1, atid2)) {
                boost::add_edge(i, j, 1, g);
                this->nr_neighbor_edges++;
            }
        }
    }

    // early exit if atom has no neighbors
    if(neighbor_ids.size() == 0) {
        this->longest_path = 0;
        return;
    }

    // calculate all possible path between points and calculate shortest distance
    MatrixXXi path_dist = MatrixXXi::Zero(neighbor_ids.size(), neighbor_ids.size());
    for(unsigned int i=0; i<neighbor_ids.size(); i++) {
        // Create things for Dijkstra
        std::vector<GVertex> predecessors(num_vertices(g)); // To store parents
        std::vector<Weight> distances(num_vertices(g)); // To store distances

        IndexMap indexMap = boost::get(boost::vertex_index, g);
        PredecessorMap predecessorMap(&predecessors[0], indexMap);
        DistanceMap distanceMap(&distances[0], indexMap);

        // use Dijkstra algorithm
        boost::dijkstra_shortest_paths(g, i, boost::distance_map(distanceMap).predecessor_map(predecessorMap));

        for(unsigned int j=0; j<neighbor_ids.size(); j++) {
            path_dist(i,j) = distanceMap[j];
        }
    }

    this->longest_path = path_dist.maxCoeff();
}

/**
 * @brief      Get the string representation of the CNA triplet
 *
 * @return     String representation
 */
std::string CNATriplet::get_str() const {
    return (boost::format("(%i,%i,%i)") % this->nr_neighbors % this->nr_neighbor_edges % this->longest_path).str();
}

/**
 * @brief      Output ostream representation of the CNA triplet
 *
 * @return     ostream representation of CNA triplet
 */
std::ostream &operator<<(std::ostream &os, const CNATriplet& m) {
    os << "{" << m.get_nr_neighbors() << "," << m.get_nr_neighbor_edges() << "," << m.get_longest_path() << "}";
    return os;
}
