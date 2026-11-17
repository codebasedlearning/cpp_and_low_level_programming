// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

#ifndef CBL_TRACER_HPP
#define CBL_TRACER_HPP

#include <iostream>                         // cout
#include <string>                           // string

/*
 * An object that reports its birth and its death, with its address - a probe for the snippets about memory:
 *
 *      tracer* t{new tracer{"t"}};         // prints ' T| - 't' constructed at 0x...'
 *      delete t;                           // prints ' T| - 't' destroyed at 0x...'
 *
 * The lines start with ' T|' - a label of its own, so that the letters ' a|', ' b|', ... stay free for the snippet.
 *
 * It can be neither copied nor moved: the compiler refuses both. A generated copy would print nothing, and its
 * destructor would report a second death of the same name - so here, every 'destroyed' line belongs to exactly one
 * 'constructed' line. Where copies and moves are the topic, a snippet writes a tracer of its own that reports them.
 */
class tracer {
public:
    explicit tracer(const std::string& name) : name_{name} {
        std::cout << " T| - '" << name_ << "' constructed at " << this << '\n';
    }

    ~tracer() {
        std::cout << " T| - '" << name_ << "' destroyed at " << this << '\n';
    }

    tracer(const tracer&) = delete;                     // and so no move either: it falls back to the deleted copy
    tracer& operator=(const tracer&) = delete;

    [[nodiscard]] const std::string& name() const { return name_; }

private:
    std::string name_;
};

#endif // CBL_TRACER_HPP
