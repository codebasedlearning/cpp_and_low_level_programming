// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* --- `report.cpp` --- Part of the library: a line of text, built with the functions of `stats.cpp`. */

#include "weather.hpp"

#include <sstream>
#include <iomanip>

namespace weather {

    std::string report(const std::vector<double>& values) {
        std::ostringstream out{};
        out << std::fixed << std::setprecision(decimals) << "mean " << mean(values) << " C, spread " << spread(values)
            << " C";
        return out.str();
    }

}
