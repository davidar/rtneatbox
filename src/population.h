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

#ifndef POPULATION_H
#define POPULATION_H

#include <Box2D.h>
#include <NEAT/genome.h>
#include <NEAT/organism.h>
#include <NEAT/population.h>

class Level;
class Organism;

class Population {
public:
    /** Should evolution occur? */
    bool evolve;
    
    Population(Level *level, int lifetime);
    ~Population();
    void setLifetime(int lifetime);
    void spawn();
    void step();
    Organism *find(b2Body *body);
    
protected:
    /** Number of offspring born */
    int m_numOffspring;
    /** Number of ticks between evolution */
    int m_evolutionSpacing;
    /** Number of ticks since the last evolution */
    int m_ticksSinceEvolution;
    /** Array of organisms in this population */
    Organism **m_organisms;
    /** Level this population lives in */
    Level *m_level;
    /** The rtNEAT population */
    NEAT::Population *m_population;
    
    void generatePopulation(NEAT::Genome *starterGenome);
    void evolvePopulation();
    NEAT::Organism *reproduce();
    void replaceOrganism(NEAT::Organism *oldOrganism,
                         NEAT::Organism *newOrganism);
    void reassignSpecies();
};

#endif
