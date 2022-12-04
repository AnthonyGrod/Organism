#ifndef ORG_ORGANISM_H
#define ORG_ORGANISM_H

#include <optional>
#include <tuple>

template<typename T>
concept is_equality_comparable = requires {
    std::equality_comparable<T>;
};


template<is_equality_comparable species_t, bool can_eat_meat, bool can_eat_plants>
class Organism {
public:
    constexpr Organism(species_t const &species, uint64_t vitality)
            : species(species), vitality(vitality) {}

    constexpr const species_t &get_species() const {
        return species;
    }

    constexpr uint64_t get_vitality() const {
        return vitality;
    }

    constexpr bool is_dead() const {
        return vitality == 0;
    }

    template<is_equality_comparable species_t_o2, bool can_eat_meat_o2, bool can_eat_plants_o2>
    constexpr bool will_mate(Organism<species_t_o2, can_eat_meat_o2, can_eat_plants_o2> o) const {
        return species == o.get_species() && can_eat_meat == can_eat_meat_o2 && can_eat_plants == can_eat_plants_o2;
    }

    template<is_equality_comparable species_t_o2, bool can_eat_meat_o2, bool can_eat_plants_o2>
    constexpr bool will_eat(Organism<species_t_o2, can_eat_meat_o2, can_eat_plants_o2> o) const {
        return (can_eat_meat && (can_eat_meat_o2 || can_eat_plants_o2)) ||
               (can_eat_plants && (!can_eat_meat_o2 && !can_eat_plants_o2));
    }

private:
    species_t species;
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

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>,
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
eat(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
    Organism<species_t, sp2_eats_m, sp2_eats_p> organism2, uint64_t v1, uint64_t v2) {
    Organism<species_t, sp1_eats_m, sp1_eats_p> first(organism1.get_species(), v1);
    Organism<species_t, sp2_eats_m, sp2_eats_p> second(organism2.get_species(), v2);
    return std::make_tuple(first, second, std::nullopt);
}

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>,
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {

    // two plants
    static_assert(sp1_eats_m || sp1_eats_p || sp2_eats_m || sp2_eats_p);

    // one is dead
    if (organism1.is_dead() || organism2.is_dead()) {
        return std::make_tuple(organism1, organism2, std::nullopt);
    }

    // will mate
    if (organism1.will_mate(organism2)) {
        Organism<species_t, sp1_eats_m, sp1_eats_p>
                offspring(organism1.get_species(), (organism1.get_vitality() + organism2.get_vitality()) / 2);
        return std::make_tuple(organism1, organism2, offspring);
    }

    // can't eat each other
    if (!organism1.will_eat(organism2) && !organism2.will_eat(organism1)) {
        return std::make_tuple(organism1, organism2, std::nullopt);
    }

    // can eat each other
    if (organism1.will_eat(organism2) && organism2.will_eat(organism1)) {
        if (organism1.get_vitality() > organism2.get_vitality()) {
            return eat(organism1, organism2, organism1.get_vitality() + organism2.get_vitality() / 2, 0);
        } else if (organism1.get_vitality() < organism2.get_vitality()) {
            return eat(organism1, organism2, 0, organism1.get_vitality() / 2 + organism2.get_vitality());
        } else {
            return eat(organism1, organism2, 0, 0);
        }
    }

    // one side is a plant that can be eaten
    if (sp1_eats_p && !sp2_eats_m && !sp2_eats_p) {
        return eat(organism1, organism2, organism1.get_vitality() + organism2.get_vitality(), 0);
    }

    if (!sp1_eats_p && !sp1_eats_m && sp2_eats_p) {
        return eat(organism1, organism2, 0, organism1.get_vitality() + organism2.get_vitality());
    }

    // only one side can eat the other (not a plant)
    if (sp1_eats_m) {
        if (organism1.get_vitality() <= organism2.get_vitality()) {
            return {organism1, organism2, std::nullopt};
        } else {
            return eat(organism1, organism2, organism1.get_vitality() + organism2.get_vitality() / 2, 0);
        }
    }

    if (sp2_eats_m) {
        if (organism1.get_vitality() >= organism2.get_vitality()) {
            return {organism1, organism2, std::nullopt};
        } else {
            return eat(organism1, organism2, 0, organism1.get_vitality() / 2 + organism2.get_vitality());
        }
    }

    return std::make_tuple(organism1, organism2, std::nullopt);
}

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1) {
    return organism1;
}

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p, typename... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
                 Organism<species_t, sp2_eats_m, sp2_eats_p> organism2,
                 Args... args) {
    return encounter_series(std::get<0>(encounter(organism1, organism2)), args...);
}

#endif //ORG_ORGANISM_H
