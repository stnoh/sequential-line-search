#include "core.h"
#include <iostream>
#include <cmath>
#include <sequential-line-search/sequential-line-search.h>
#include "mainwindow.h"

using namespace sequential_line_search;

using Eigen::MatrixXd;
using Eigen::VectorXd;

Core::Core() : use_MAP_hyperparameters(false)
{
    data.X = MatrixXd::Zero(0, 0);
    data.D.clear();
    
    x_max = VectorXd::Zero(0);
    y_max = NAN;
}

void Core::proceedOptimization()
{
    // Add new preference data
    const VectorXd x = computeParametersFromSlider(mainWindow->obtainSliderPosition());
    data.AddNewPoints(x, { slider->orig_0, slider->orig_1 });
    
    // Compute regression
    computeRegression();
    
    // Check the current best
    x_max = regressor->find_arg_max();
    y_max = regressor->estimate_y(x_max);
    
    // Update slider ends
    updateSliderEnds();
}

double Core::evaluateObjectiveFunction(const Eigen::VectorXd& x) const
{
    assert(x.rows() == 2);
#if 0
    return 0.4 * std::max(0.0, std::sin(x(0)) + x(0) / 3.0 + std::sin(x(0) * 12.0) + std::sin(x(1)) + x(1) / 3.0 + std::sin(x(1) * 12.0) - 1.0);
#elif 0
    return 1.0 - std::abs(x(0) - 0.5) - std::abs(x(1) - 0.5);
#elif 0
    return std::exp(- (x - VectorXd::Constant(x.rows(), 0.5)).squaredNorm() / (0.5 * 0.5));
#elif 0
    double sum = 0.0;
    for (unsigned i = 0; i < x.rows(); ++ i)
    {
        sum += std::sin(x(i)) + x(i) / 3.0 + std::sin(12.0 * x(i));
    }
    return sum;
#elif 1
    auto lambda = [](const Eigen::VectorXd& x, const Eigen::VectorXd& mu, const double sigma)
    {
        return std::exp(- (x - mu).squaredNorm() / (sigma * sigma));
    };
    
    return lambda(x, Eigen::Vector2d(0.3, 0.3), 0.3) + 1.5 * lambda(x, Eigen::Vector2d(0.7, 0.7), 0.4);
#else
    auto lambda = [](const Eigen::VectorXd& x, const Eigen::VectorXd& mu, const double sigma)
    {
        return std::exp(- (x - mu).squaredNorm() / (2.0 * sigma * sigma));
    };
    
    return lambda(x, Eigen::Vector2d(0.5, 0.5), 0.5);
#endif
}

void Core::computeRegression()
{
    regressor = std::make_shared<PreferenceRegressor>(data.X, data.D, use_MAP_hyperparameters);
}

void Core::updateSliderEnds()
{
    // If this is the first time...
    if (slider.get() == nullptr)
    {
#if 0
        slider = std::make_shared<Slider>(Utility::generateRandomVector(2), Utility::generateRandomVector(2), false);
#else
        const Eigen::Vector2d x0(0.1, 0.9);
        const Eigen::Vector2d x1(0.4, 0.1);
        slider = std::make_shared<Slider>(x0, x1, false);
#endif
        return;
    }
    
    const VectorXd x_1 = regressor->find_arg_max();
    const VectorXd x_2 = acquisition_function::FindNextPoint(*regressor);
    
    slider = std::make_shared<Slider>(x_1, x_2, true);
}

VectorXd Core::computeParametersFromSlider(int sliderValue, int minValue, int maxValue) const
{
    const double t = static_cast<double>(sliderValue - minValue) / static_cast<double>(maxValue - minValue);
    return computeParametersFromSlider(t);
}

VectorXd Core::computeParametersFromSlider(double value) const
{
    assert(slider.get() != nullptr);
    return slider->getValue(value);
}
