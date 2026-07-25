# Karkino

The Karkinolution creature simulator focuses on simulating creatures from an individual perspective. In other words, the main question of interest is always shifted towards: "How is the individual doing? What is it going to do? How will it behave?"

This is the main focus of what Karkinolution actually simulates.

# Inspirations

One of the sandbox simulators that clearly brought the most inspiration for this project was WorldBox, a general-purpose simulator focused on simulating both the world itself and human societies. Although it still follows a more _macro_ approach rather than being purely focused on individuals, it was one of the projects that most motivated me to create this simulator.

# Processing Style: Why Sequential?

I chose that the creature processing cycle would not work like this:

Creature A sends an action  
Creature B sends an action  
Creature C sends an action  

And at the end, a judge evaluates which actions will actually occur through a Queue analysis.

Instead, I chose an approach that focuses more on the random sequentiality of creatures:

Creature A acts  
Creature B acts  
Creature C acts  

Clearly, this type of processing is highly non-deterministic, making debugging more difficult. However, I believe it creates emergent behavior that feels much more alive compared to other alternatives.