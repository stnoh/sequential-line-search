#ifndef PREFERENCEREGRESSOR_H
#define PREFERENCEREGRESSOR_H

#include <vector>
#include <string>
#include <utility>
#include <Eigen/Core>
#include <sequential-line-search/regressor.h>
#include <sequential-line-search/preference.h>

namespace sequential_line_search
{
    /// \brief Class for performing regression based on preference data.
    /// \details See [Chu+, ICML 2005; Brochu+, NIPS 2007].
    class PreferenceRegressor : public Regressor
    {
    public:
        PreferenceRegressor(const Eigen::MatrixXd &X, const std::vector<Preference>& D, bool use_MAP_hyperparameters = false);
        PreferenceRegressor(const Eigen::MatrixXd &X, const std::vector<Preference>& D, const Eigen::VectorXd& w, bool use_MAP_hyperparameters = false);
        PreferenceRegressor(const Eigen::MatrixXd &X, const std::vector<Preference>& D, const Eigen::VectorXd& w, bool use_MAP_hyperparameters, const PreferenceRegressor* previous);
        
        double estimate_y(const Eigen::VectorXd& x) const override;
        double estimate_s(const Eigen::VectorXd& x) const override;
        
        const bool use_MAP_hyperparameters;
        
        /// \brief Find the data point that is likely to have the largest value from the so-far observed data points.
        Eigen::VectorXd find_arg_max();
        
        // Data
        Eigen::MatrixXd         X;
        std::vector<Preference> D;
        Eigen::VectorXd         w; ///< Weights for calculating the scales in the BTL model (default = ones), used in crowdsourcing settings
        
        // Derived by MAP estimation
        Eigen::VectorXd         y;
        double                  a; ///< ARD hyperparameter about signal level.
        double                  b; ///< ARD hyperparameter about noise level.
        Eigen::VectorXd         r; ///< ARD hyperparameter about length scales.
        
        // Can be derived after MAP estimation
        Eigen::MatrixXd         C;
        Eigen::MatrixXd         C_inv;
        
        // IO
        void dampData(const std::string& dirPath) const;
        
        // Getter
        const Eigen::MatrixXd& getX() const override { return X; }
        const Eigen::VectorXd& gety() const override { return y; }
        double geta() const override { return a; }
        double getb() const override { return b; }
        const Eigen::VectorXd& getr() const override { return r; }
        
        struct Params
        {
            Params() {}
            double a         = 0.500;
            double r         = 0.500;
            double b         = 0.005;
            double variance  = 0.250; ///< Used when hyperparameters are estimated via MAP
            double btl_scale = 0.010;
            static Params& getInstance() { static Params p; return p; }
        };
        
    private:
        void compute_MAP(const PreferenceRegressor* = nullptr);
    };
}

#endif // PREFERENCEREGRESSOR_H
