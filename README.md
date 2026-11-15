# Mission-Architect
A desktop rocket-design and mission-planning game built with C++ and SFML 3. Build and configure a rocket, check whether it can carry its payload, manage your budget, and move through the mission phases.

Overview
The game challenges you to assemble a rocket that can carry a selected payload while balancing factors such as rocket capacity, cost, and reliability. The available build includes rocket and component choices, payload/capacity checks, budget feedback, and a mission simulation flow.
The program also evaluates mission outcomes using science and communications-related information, including downlink performance and science points returned.
Features
- Rocket selection and editing — choose a rocket and edit its configuration.
- Component choices — the build includes selectable bus, power, communications, and propulsion options.
- Payload and mass checks — see whether the selected rocket can lift the configured mass and whether the payload is within its capacity.
- Budget feedback — receive warnings when the design exceeds the available budget.
- Reliability feedback — rocket reliability is presented as a factor in mission risk.
- Mission phases — proceed through the mission using the in-game phase controls.
- Mission results — the game includes science-point and downlink-related feedback.
Run the game
1. Download or extract the ZIP archive.
2. Keep nasa.exe and all three SFML DLL files together in the same folder.
3. Double-click nasa.exe to launch the game.

The game provides contextual feedback while configuring a rocket. For example:
- A message when the selected mass fits the rocket.
- A warning when the design is too heavy for the selected rocket.
- A warning when the design is over budget.
- A warning when the payload exceeds the rocket's capacity.
- A prompt to continue to the next mission phase once the rocket is ready.
Follow the instructions and status messages displayed in the game window.
Built With
- C++
- SFML 3 — graphics, window, and system runtime libraries.

me’s exact scoring formula and full mission rules are not described in the supplied package.
Credits
Developed as a C++ game project using SFML 3.
