#ifndef ORG_ORGANISM_H
#define ORG_ORGANISM_H

#include <iostream>


template<typename T>
concept is_equality_comparable = requires {
    std::equality_comparable<T>;
};


template<is_equality_comparable species_t, bool can_eat_meat, bool can_eat_plants>
class Organism {
public:
    constexpr Organism(species_t const &species, uint64_t vitality)
        : specie (species)
        , vitality (vitality)
    {}

    inline const constexpr species_t &get_species() const {
        return specie;
    }

    inline constexpr uint64_t get_vitality() const {
        return vitality;
    }

private:
    species_t specie;
    uint64_t vitality;
};


template<is_equality_comparable species_t>
using Carnivore = Organism<species_t, true, false>;

template<is_equality_comparable species_t>
using Omnivore = Organism<species_t, true, true>;

template<is_equality_comparable species_t>
using Herbivore = Organism<species_t, false, true>;

template<is_equality_comparable species_t>
using Plant = Organism<species_t, false, false>;


#endif //ORG_ORGANISM_H
