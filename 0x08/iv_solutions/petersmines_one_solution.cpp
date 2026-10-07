// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Peters Mines', see ../tasks.md, with the members of the extension.

#include <iostream>
#include <cstddef>
#include <cstdlib>

using std::cout, std::ptrdiff_t;

class vehicle_base {
public:
    explicit vehicle_base(const int seats) : seats_{seats} {
        cout << "vehicle_base(" << seats << ")\n";
    }

    int seats() const { return seats_; }

private:
    int seats_;
};

class vehicle {
public:
    explicit vehicle(const int max_speed) : max_speed_{max_speed} {
        cout << "vehicle(" << max_speed << ")\n";
    }

    int max_speed() const { return max_speed_; }

private:
    int max_speed_;
};

// `virtual vehicle_base`: shared with every other class in the object that inherits it virtually. `vehicle` is not
// shared - an amphibian has two of them.
class car : public virtual vehicle_base, public vehicle {
public:
    car(const int seats, const int max_speed, const int wheels)
        : vehicle_base{seats}, vehicle{max_speed}, wheels_{wheels} {
        cout << "car(" << seats << ", " << max_speed << ", " << wheels << ")\n";
    }

    int wheels() const { return wheels_; }

private:
    int wheels_;
};

class boat : public virtual vehicle_base, public vehicle {
public:
    boat(const int seats, const int max_speed, const int draught_cm)
        : vehicle_base{seats}, vehicle{max_speed}, draught_cm_{draught_cm} {
        cout << "boat(" << seats << ", " << max_speed << ", " << draught_cm << ")\n";
    }

    int draught_cm() const { return draught_cm_; }

private:
    int draught_cm_;
};

// The most derived class constructs the virtual base: `vehicle_base{seats}` here is the one that runs - first of all.
// The `vehicle_base{seats}` in the initializer lists of `car` and `boat` are skipped when they are parts of an
// amphibian; they count only for a plain car or boat.
class amphibian : public car, public boat {
public:
    amphibian(const int seats, const int road_speed, const int water_speed)
        : vehicle_base{seats}, car{seats, road_speed, 4}, boat{seats, water_speed, 60} {
        cout << "amphibian(" << seats << ", " << road_speed << ", " << water_speed << ")\n";
    }
};

ptrdiff_t offset_of(const void* part, const void* object) {
    return static_cast<const unsigned char*>(part) - static_cast<const unsigned char*>(object);
}

int main() {
    const amphibian a{4, 120, 15};
    cout << "seats " << a.seats() << ", on the road " << a.car::max_speed() << ", on the water " << a.boat::max_speed()
         << ", wheels " << a.wheels() << ", draught " << a.draught_cm() << " cm\n";

    // gcc and clang, 64-bit: 4, 4, 24, 24, 40. A `car` has a vptr, although it has no virtual function: the table holds
    // the offset of its `vehicle_base`, which depends on the object the car is part of.
    cout << "sizeof: vehicle_base " << sizeof(vehicle_base) << ", vehicle " << sizeof(vehicle) << ", car "
         << sizeof(car) << ", boat " << sizeof(boat) << ", amphibian " << sizeof(amphibian) << '\n';

    // The layout, with gcc and clang:
    //   0  vptr of the car part          car
    //   8  max_speed_ (road)             car's vehicle
    //  12  wheels_                       car
    //  16  vptr of the boat part         boat
    //  24  max_speed_ (water)            boat's vehicle
    //  28  draught_cm_                   boat
    //  32  seats_                        the one vehicle_base, at the end
    const car& c{a};
    const boat& b{a};
    const vehicle_base& vb{a};
    const vehicle& road{c};
    const vehicle& water{b};
    cout << "offsets: car " << offset_of(&c, &a) << ", boat " << offset_of(&b, &a) << ", car's vehicle "
         << offset_of(&road, &a) << ", boat's vehicle " << offset_of(&water, &a) << ", vehicle_base "
         << offset_of(&vb, &a) << '\n';

    // Extension, without `virtual`: two `vehicle_base` parts, two numbers of seats. `a.seats()` does not compile -
    // gcc: "request for member 'seats' is ambiguous", clang: "non-static member 'seats' found in multiple base-class
    // subobjects of type 'vehicle_base'" - and `vehicle_base{seats}` in the initializer list of `amphibian` is refused:
    // `vehicle_base` is no longer a direct base. `a.car::seats()` works. `sizeof(amphibian)` is 24: 12 for each part -
    // no vptrs any more.

    return EXIT_SUCCESS;
}
