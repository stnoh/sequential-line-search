#include <sequential-line-search/data.h>
#include <map>

using Eigen::VectorXd;
using Eigen::MatrixXd;

namespace internal
{
    void MergeClosePoints(const double epsilon, Eigen::MatrixXd& X, std::vector<sequential_line_search::Preference>& D)
    {
        const double eps_squared = epsilon * epsilon;
        
        while (true)
        {
            bool dirty = false;
            
            const unsigned N = X.rows();
            const unsigned M = X.cols();
            
            // Distance matrix (upper triangle only)
            MatrixXd Dist(M, M);
            for (unsigned i = 0; i < M; ++ i)
            {
                for (unsigned j = i + 1; j < M; ++ j)
                {
                    Dist(i, j) = (X.col(i) - X.col(j)).squaredNorm();
                }
            }
            
            for (unsigned i = 0; i < M; ++ i)
            {
                for (unsigned j = i + 1; j < M; ++ j)
                {
                    if (!dirty && Dist(i, j) < eps_squared)
                    {
                        // Construct a mapping from the old indices to the new one
                        std::map<unsigned, unsigned> mapping;
                        unsigned new_index = 0;
                        for (unsigned old_index = 0; old_index < M; ++ old_index)
                        {
                            if (old_index != i && old_index != j)
                            {
                                mapping[old_index] = new_index ++;
                            }
                        }
                        mapping[i] = M - 2;
                        mapping[j] = M - 2;
                        
                        // Update the matrix
                        MatrixXd newX(N, M - 1);
                        for (unsigned old_index = 0; old_index < M; ++ old_index)
                        {
                            newX.col(mapping[old_index]) = X.col(old_index);
                        }
                        newX.col(M - 2) = 0.5 * (X.col(i) + X.col(j));
                        X = newX;
                        
                        // Update the indices in the preference pairs
                        for (sequential_line_search::Preference& p : D)
                        {
                            for (unsigned i = 0; i < p.size(); ++ i) p[i] = mapping[p[i]];
                        }
                        
                        dirty = true;
                    }
                }
            }
            if (!dirty) return;
        }
    }
}

namespace sequential_line_search
{
    void Data::MergeClosePoints(const double epsilon)
    {
        internal::MergeClosePoints(epsilon, X, D);
    }
    
    void Data::AddNewPoints(const Eigen::VectorXd&              x_preferable,
                            const std::vector<Eigen::VectorXd>& xs_other,
                            const bool                          merge_close_points)
    {
        if (X.rows() == 0)
        {
            // X
            const unsigned d = xs_other[0].rows();
            X = MatrixXd(d, xs_other.size() + 1);
            X.col(0) = x_preferable;
            for (unsigned i = 0; i < xs_other.size(); ++ i) { X.col(i + 1) = xs_other[i]; }
            
            // D
            std::vector<unsigned> indices(xs_other.size() + 1);
            for (unsigned i = 0; i < xs_other.size() + 1; ++ i) { indices[i] = i; }
            D.push_back(Preference(indices));
            
            return;
        }
        
        const unsigned d = X.rows();
        const unsigned N = X.cols();
        
        // X
        MatrixXd newX(d, N + xs_other.size() + 1);
        newX.block(0, 0, d, N) = X;
        newX.col(N) = x_preferable;
        for (unsigned i = 0; i < xs_other.size(); ++ i) { newX.col(N + i + 1) = xs_other[i]; }
        X = newX;
        
        // D
        std::vector<unsigned> indices(xs_other.size() + 1);
        for (unsigned i = 0; i < xs_other.size() + 1; ++ i) { indices[i] = N + i; }
        D.push_back(Preference(indices));
        
        // Merge
        if (merge_close_points) { MergeClosePoints(); }
    }
}
