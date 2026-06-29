# Karkinolution

A creature ecosystem simulator with emergent behaviors, 
built in Python with a data-oriented architecture.

## Overview

Karkinolution simulates an ecosystem where creatures with unique genomes 
interact, reproduce, and compete for resources across different terrains. 
Emergent behavior arises from simple rules governing each entity — no 
hard-coded behavior scripts.

The architecture is data-oriented: creatures are composed of independent 
components (genome, diet, combat stats), making the addition of new 
behaviors and entity types surgical, without breaking existing systems.

## Features

- Creatures inherit traits from parents with random mutation, influencing 
  diet, aggressiveness, and reproductive capacity
- Reactive combat system — creatures attack based on proximity and 
  internal state, not fixed scripts
- Multiple terrain types affect mobility and resource availability
- Full life cycle: birth, growth, reproduction, and death