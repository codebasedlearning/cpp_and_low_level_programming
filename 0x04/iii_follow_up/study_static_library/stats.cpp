// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* --- `stats.cpp` --- Part of the library: the statistics, and the one definition of `values_seen`. */

#include "weather.hpp"

#include <cmath>

namespace weather {

    int values_seen{0};                     // the definition belonging to the `extern` declaration

    double mean(const std::vector<double>& values) {
        double sum{0.0};
        for (const double v : values) {
            sum += v;
        }
        values_seen += static_cast<int>(values.size());
        return sum / static_cast<double>(values.size());
    }

    double spread(const std::vector<double>& values) {
        const double m{mean(values)};
        double sum{0.0};
        for (const double v : values) {
            sum += (v - m) * (v - m);
        }
        return std::sqrt(sum / static_cast<double>(values.size()));
    }

}
