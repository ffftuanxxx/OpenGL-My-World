# Please Look Up To The Sky, and Be Relax!

## Interaction in the Program

### Interaction with the Robot

- **Movement**: The robot can jump, move, and turn in different directions.
  - Click `w` to move forward.
  - Click `s` to move backward.
  - Click `a` to move left.
  - Click `d` to move right.

- **Jumping**: The robot's jump and fall speed is determined by gravitational acceleration.
  - Click `j` to make the robot jump.

- **Perspective**: The program offers both third-person and first-person perspectives.
  - Click `1` to switch to the first-person view.
  - Click `2` to switch to the third-person view.
  - Use the mouse to change the view direction in the current perspective.

### Interaction with Objects

- **Collision System**: The program includes a collision detection system.
  - When you approach an interactive object, a call-word will appear on the adjacent object. Refer to the demo video for more details.
  - Click `p` to make the object jump.
  - Click `o` to make the object spin.

## Object Dynamics

- **Track System**: Objects are continuously generated and disappear over time.
- **Dynamic System**: Some objects follow a dynamic system where their curves appear and disappear cyclically.
- **Random Generation**: The generation points of objects are randomized. Each time you start the program, interactive objects are placed in different positions to enhance the gameplay experience.

## Environment Details

- **Floor Textures**: Different textures are used for the floor in various scenarios.
- **Buildings and Objects**: The scene includes three large buildings, a house, a cola box, and a hamburger.
  - The large house has different textures on different sides.
  - The cola box is located inside the large house.

- **Skybox**: A skybox surrounds the scene to create an immersive environment.
  - Due to the large size of the scenario, textures are temporarily disabled.
  - Clouds move and automatically generate and disappear when too far away.

## Challenges Faced

1. **Performance Optimization**: Ensuring smooth performance without lag despite numerous dynamic elements required optimizing rendering techniques, efficient resource usage, and minimizing computational overhead.
2. **Debugging and Error Handling**: Debugging 3D rendering, physics calculations, and user interactions was time-consuming. Extensive testing and iteration were needed to fix bugs causing unexpected behavior or crashes.
3. **Handling User Inputs and Interactive Feedback**: Designing a responsive system to handle keyboard and mouse inputs while providing immediate visual feedback was crucial for a good user experience. Ensuring intuitive and accurate interactions in the 3D environment was essential.
4. **Collision Detection and Response**: Developing a robust collision detection system to ensure the robot and objects didn't overlap or pass through each other required precise calculations and continuous adjustments to handle edge cases and dynamic scenarios.
5. **Random Object Generation and Placement**: Creating a system to generate objects at random positions while avoiding inaccessible or overlapping areas required a well-thought-out algorithm. Balancing randomness with playability was a delicate task.
6. **Implementing Gravitational Physics for Jumping and Falling**: Simulating realistic jump and fall mechanics involved accurately modeling gravitational acceleration. Fine-tuning jump height, velocity, and gravity's impact was challenging.
7. **Implementing the Skybox**: Adding a skybox for an immersive environment involved correctly mapping and aligning textures on a large cube surrounding the scene. Managing performance impact and ensuring no visible seams or distortions in textures were significant challenges. Additionally, the scenario size required temporary texture disabling, adding complexity.

---

We hope you enjoy interacting with the robot and exploring the dynamic environment we've created. Please refer to the demo video for a comprehensive guide to all interactions and features.
