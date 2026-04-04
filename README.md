# TEMPLATE RUN - OpenGL Edition

An exciting endless runner game built with C++ and FreeGLUT, featuring smooth physics, power-ups, and dynamic difficulty.

##  Game Overview

TEMPLATE RUN is a fast-paced endless runner where you control a character running through an obstacle-filled landscape. The game gets progressively harder as your distance increases, testing your reflexes and timing.

##  Play the Web Version Now!

**[⚡ PLAY TEMPLATE RUN ONLINE ⚡](https://robiul-hasan-jisan.github.io/Runner-game/)**

The web version features:
- **Tap/Space controls** - Perfect for mobile and desktop
- **Golden shield power-ups** - Collect for protection
- **Progressive difficulty** - Speed increases as you run
- **High score tracking** - Beat your best distance

##  Features

- **Smooth Character Controls**: Responsive jump mechanics with coyote time and jump buffering
- **Dynamic Difficulty**: Game speed increases as you progress
- **Power-up System**: Collect shield power-ups for temporary invincibility
- **Visual Effects**: Particle system, screen shake, and shield aura effects
- **Progressive Scoring**: Earn points by passing obstacles
- **High Score Tracking**: Best score persists during gameplay session
- **Multiple Input Methods**: Keyboard, arrow keys, and mouse support

##  Controls

| Action | Input |
|--------|-------|
| Jump | Space, W, Up Arrow, or Left Mouse Click |
| Restart | R |
| Quit | ESC |

##  Gameplay Mechanics

### Core Mechanics
- **Jumping**: Press jump to avoid obstacles
- **Coyote Time**: Brief window to jump after leaving a platform
- **Jump Buffering**: Jump input is stored briefly for precise timing

### Scoring System
- **+12 points** for each obstacle successfully passed
- Score increases with distance traveled
- Dynamic speed scaling affects difficulty

### Power-ups
- **Shield**: Provides temporary invincibility (glowing aura effect)
- Protects against one collision
- Visual indicator shows active shield status

### Obstacles
- **Blocks**: Standard obstacles of varying heights
- **Spikes**: Dangerous obstacles with triangular tops (rare spawn)

##  Technical Details

### Requirements
- C++ compiler with OpenGL support
- FreeGLUT library installed
- GLU library

### Compilation

**Linux/Mac:**
```bash
g++ -o templaterun main.cpp -lglut -lGLU -lGL
```

**Windows (MinGW):**

```bash
g++ -o templaterun.exe main.cpp -lfreeglut -lopengl32 -lglu32
```
---

## Game Constants
- Window Size: 900x550 pixels

- Base Speed: 5.2 units/frame

- Max Speed: 12.8 units/frame

- Gravity: 0.85 units/frame²

- Jump Power: -11.8 units/frame

## Visual Design

**Visual Elements**
- Dynamic Sky: Gradient skybox with twinkling stars

- Moon: Decorative celestial element

- Detailed Ground: Multi-layered ground with animated markings

- Character Design: Stylized runner with scarf and expressive features

**Particle Effects: Dust clouds on jump, collision sparks, and collection effects**

**Color Palette**
- Sky: Deep blue to purple gradient

- Ground: Earth tones with warm highlights

- Character: Teal body with warm skin tone and red scarf

- Obstacles: Rustic brown with orange accents

## Game Systems

- Physics Engine
  
- Gravity-based jump physics

- Collision detection system

- Velocity-based movement

## Spawn System

- Dynamic obstacle spawning based on current speed

- Power-up spawns every ~3-5 seconds

- Adaptive spawn rates for balanced difficulty

## Particle System
- Jump dust effects

- Score pop particles

- Shield collection effects

- Collision spark particles

## HUD Elements
- SCORE: Current run score

- BEST: Highest score achieved

- DIST: Distance traveled

- SPEED: Current game speed

- SHIELD STATUS: Power-up availability indicator

## Strategy Tips
- Timing is Key: Use the jump buffer to your advantage

- Watch Your Speed: The game gets faster - adjust your timing accordingly

- Shield Management: Save shields for when you need them most

- Practice Makes Perfect: Learn obstacle patterns and jump timing

## Known Issues
- Screen shake might affect visibility during intense moments

- Particle effects may impact performance on older systems

##  Contributing
- Feel free to fork and enhance this project! Potential improvements:

- Additional power-up types

- More obstacle varieties

- Sound effects and background music

- Different character skins

- Level progression system

## License
**This project is open source and available for educational purposes.**

## Credits
- Created as a FreeGLUT project demonstrating:

- OpenGL rendering techniques

- Game physics implementation

- Real-time input handling

- Particle system design

- Dynamic difficulty scaling

 ## 👨‍💻 Author
**Robiul Hasan Jisan**

- **Portfolio:** [robiulhasanjisan.vercel.app](https://robiulhasanjisan.vercel.app/)
- **GitHub:** [@RoBiul-Hasan-Jisan](https://github.com/RoBiul-Hasan-Jisan)

