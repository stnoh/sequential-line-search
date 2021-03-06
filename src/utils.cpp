#include <sequential-line-search/utils.h>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <Eigen/Core>

using Eigen::VectorXd;
using Eigen::MatrixXd;

namespace sequential_line_search
{
    namespace utils
    {
        Eigen::VectorXd generateRandomVector(unsigned n)
        {
            return 0.5 * (Eigen::VectorXd::Random(n) + Eigen::VectorXd::Ones(n));
        }
        
        void exportMatrixToCsv(const std::string& filePath, const Eigen::MatrixXd& X)
        {
            std::ofstream file(filePath);
            Eigen::IOFormat format(Eigen::StreamPrecision, Eigen::DontAlignCols, ",");
            file << X.format(format);
        }
        
        std::string getCurrentTimeInString()
        {
            const std::time_t t = std::time(nullptr);
            std::stringstream s; s << std::put_time(std::localtime(&t), "%Y%m%d%H%M%S");
            return s.str();
        }
    }
}
