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

#ifndef ORGANISM_H
#define ORGANISM_H

#include <Box2D.h>
#include <NEAT/organism.h>

#define ORGANISM_NUM_INPUTS 14 /* number of inputs, including bias */
#define ORGANISM_NUM_OUTPUTS 1

class Level;

class Organism {
public:
    /** Inputs to the organism's sensors */
    double *inputs;
    /** Score of the organism for this run */
    double score;
    
    Organism(NEAT::Organism *organism, Level *level);
    ~Organism();
    void step(bool respawn);
    void spawn();
    b2Vec2 position();
    b2Vec2 velocity();
    NEAT::Organism *getNEATOrganism();
    void setNEATOrganism(NEAT::Organism *organism);
    b2Body *getBody();
    
protected:
    /** The rtNEAT organism */
    NEAT::Organism *m_organism;
    /** The organism's physical body */
    b2Body *m_body;
    /** The level the organism lives in */
    Level *m_level;
    
    void age(bool respawn);
    void act(std::vector<NEAT::NNode*> outputs);
    void kill();
    void construct(b2Vec2 position);
    double raycast(double angle, double range = 50.0);
};

#endif
