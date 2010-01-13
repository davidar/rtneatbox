/*
* Copyright (c) 2010 David Roberts <d@vidr.cc>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "population.h"
#include "level.h"
#include "organism.h"

#include <cassert>
#include <fstream>
#include <vector>
#include <cstdio>

#include <NEAT/species.h>

#define INELIGIBLE_PROPORTION 0.5
#define NUM_SPECIES_TARGET 4
#define COMPATIBILITY_THRESHOLD_DELTA 0.1

/**
 * Create a new population.
 * 
 * @param world              the world the population lives in
 * @param lifetime           the lifetime of the organisms in this population
 */
Population::Population(Level *level, int lifetime)
    : evolve(false), m_numOffspring(0), m_ticksSinceEvolution(0),
      m_level(level) {
    generatePopulation(new NEAT::Genome(
        ORGANISM_NUM_INPUTS, ORGANISM_NUM_OUTPUTS, 0, 0));
    setLifetime(lifetime);
}

Population::~Population() {
    for(int i = 0; i < NEAT::pop_size; i++)
        delete m_organisms[i];
    delete [] m_organisms;
    delete m_population;
}

/**
 * Set the lifetime of the organisms in the population.
 * 
 * @param lifetime  the new lifetime (must be positive)
 */
void Population::setLifetime(int lifetime) {
    assert(lifetime > 0);
    NEAT::time_alive_minimum = lifetime;
    m_evolutionSpacing =
        (double) lifetime / (INELIGIBLE_PROPORTION * NEAT::pop_size);
}

/**
 * Spawn all organisms.
 */
void Population::spawn() {
    for(int i = 0; i < NEAT::pop_size; i++)
        m_organisms[i]->spawn();
}

/**
 * Step the population forward by one timestep.
 */
void Population::step() {
    for(int i = 0; i < NEAT::pop_size; i++)
        m_organisms[i]->step(evolve);
    if(evolve && ++m_ticksSinceEvolution >= m_evolutionSpacing)
        evolvePopulation();
}

/**
 * Find the organism with the given body.
 * 
 * @param body  the body
 * @return      the corresponding organism if found, NULL otherwise
 */
Organism *Population::find(b2Body *body) {
    for(int i = 0; i < NEAT::pop_size; i++)
        if(m_organisms[i]->getBody() == body)
            return m_organisms[i];
    return NULL;
            
}

/**
 * Generate an rtNEAT population from the given starter genome.
 * 
 * @param starterGenome  the starter genome
 */
void Population::generatePopulation(NEAT::Genome *starterGenome) {
    m_population = new NEAT::Population(starterGenome, NEAT::pop_size);
    assert(m_population->verify());
    m_organisms = new Organism* [NEAT::pop_size];
    for(int i = 0; i < NEAT::pop_size; i++)
        m_organisms[i] = new Organism(m_population->organisms[i], m_level);
}

/**
 * Evolve the population.
 */
void Population::evolvePopulation() {
    m_ticksSinceEvolution = 0;
    NEAT::Organism *deadOrganism = m_population->remove_worst();
    if(!deadOrganism) return; // no mature organisms
    printf("%d species\n", m_population->species.size());
    for(std::vector<NEAT::Species*>::iterator
        i = m_population->species.begin(), e = m_population->species.end();
        i != e; i++)
        printf("species #%d:\tsize=%3d,\taverage=%f\n",
            (*i)->id, (*i)->organisms.size(), (*i)->average_est);
    NEAT::Organism *newOrganism = reproduce();
    reassignSpecies();
    replaceOrganism(deadOrganism, newOrganism);
}

/**
 * Reproduce an organism.
 * 
 * @return  the organism
 */
NEAT::Organism *Population::reproduce() {
    for(std::vector<NEAT::Species*>::iterator
        i = m_population->species.begin(), e = m_population->species.end();
        i != e; i++) // re-estimate average fitness of all species
        (*i)->estimate_average();
    fprintf(stderr, "producing offspring #%d\n", m_numOffspring);
    return m_population->choose_parent_species()->reproduce_one(
        m_numOffspring++, m_population, m_population->species);
}

/**
 * Replace an rtNEAT organism.
 * 
 * @param oldOrganism  the rtNEAT organism to replace
 * @param newOrganism  the new rtNEAT organism
 */
void Population::replaceOrganism(NEAT::Organism *oldOrganism,
                                 NEAT::Organism *newOrganism) {
    for(int i = 0; i < NEAT::pop_size; i++) {
        Organism *organism = m_organisms[i];
        if(organism->getNEATOrganism() == oldOrganism) {
            organism->setNEATOrganism(newOrganism);
            organism->spawn();
        }
    }
}

/**
 * Reassign the organisms to different species if necessary.
 */
void Population::reassignSpecies() {
    if(m_numOffspring % (NEAT::pop_size/8) != 0) return;
    int numSpecies = m_population->species.size();
    if(numSpecies < NUM_SPECIES_TARGET)
        NEAT::compat_threshold -= COMPATIBILITY_THRESHOLD_DELTA;
    else if(numSpecies > NUM_SPECIES_TARGET)
        NEAT::compat_threshold += COMPATIBILITY_THRESHOLD_DELTA;
    if(NEAT::compat_threshold < 0.3) NEAT::compat_threshold = 0.3;
    for(std::vector<NEAT::Organism*>::iterator
        i = m_population->organisms.begin(), e = m_population->organisms.end();
        i != e; i++)
        m_population->reassign_species(*i);
}
