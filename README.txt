Title: Please Look Up To The Sky, and Be Relax!

Interaction in the program:
	Interaction with the robot:

	The robot can jump, move, turn to different direction.
		click 'w' move forward, 's' move backward, 'a' move left, 'd' move 'right'.

	Jump and falling speed is according to gravitational acceleration:
		click 'j' to make the robot jump.

	Has third-person perspective and first-person perspective:
		click '1' to change to first-person view, click '2' to change for third-person view.
		Use mouth can change the direction of the view of the current X-person view.

	Interaction with the object:
		Collision system is applied in this program;
		After get close to the interactionable object, the call-word will appear on the adjacent object, please go check it on the demo video.
		click 'p' to make the object jump;
		click 'o' to make the object spin;

Object:
A track system of some objects, the objects is generating and disappearing at the same time.
Some objects are apply in the dynamaic system, the curve of some objects will gradually appear and gradually disappear in a cycle.
The generation place of the object is set in a random system, every time you start the program, the position of the different interactionable objects are setting in the different position to better satisfy the game theory！！！！！！！！

The floor use different kinds of the texture in different scenario.

3 big buildings, a house, a cola box and a hamburger.

The big house with different textures in different sides, and the cola box is inside the big house nearby.

A skybox is applied around the scene, since the scenario is too huge for the program, we disable the texture for temporily.

Cloud will move and automatically generate and disappear if too far on the sky.

difficulties in doing the tasks:
1. Performance Optimization:
	Ensuring the program ran smoothly without lag, even with numerous dynamic elements, required optimization of rendering techniques, efficient use of resources, and minimizing computational overhead.
2. Debugging and Error Handling:
	Debugging issues related to 3D rendering, physics calculations, and user interactions was time-consuming. Identifying and fixing bugs that caused unexpected behavior or crashes required extensive testing and iteration.
3. Handling User Inputs and Interactive Feedback:
	Designing a responsive system to handle various user inputs (keyboard and mouse) and providing immediate visual feedback was essential for a good user experience. This involved ensuring that all interactions were intuitive and accurately reflected in the 3D environment.
4. Collision Detection and Response:
	Developing a robust collision detection system was critical for interaction with objects. Ensuring that the robot and objects did not overlap or pass through each other required precise calculations and continuous adjustments to handle various edge cases and dynamic scenarios.
5. Random Object Generation and Placement:
	Creating a system to generate objects at random positions while ensuring they did not spawn in inaccessible or overlapping areas required a well-thought-out algorithm. Balancing randomness with playability was a delicate task.
6. Implementing Gravitational Physics for Jumping and Falling:
	Simulating realistic jump and fall mechanics required accurately modeling gravitational acceleration. This involved tweaking the physics calculations to ensure the robot's movements felt natural and responsive. Fine-tuning the jump height, velocity, and the impact of gravity proved to be challenging.
7. Implementing the Skybox:
	Adding a skybox to create an immersive environment was challenging due to the need for correctly mapping and aligning the textures on a large cube surrounding the scene. This required ensuring there were no visible seams or distortions in the textures, and managing the performance impact of rendering such a large element in the scene. Additionally, the size of the scenario necessitated the temporary disabling of textures, adding another layer of complexity to managing the skybox effectively.

| Name        | ID                   | Work                                                                    |
|-------------|-----------------|---------------------------------------------------------|
| 吕君豪      | 2130026100     | object interaction, floor, robot movement, demo  |
| 廖皆成      | 2130026083     | house building, floor, readme report, skybox         |
| 袁一琛      | 2130026190     | hamburger and cola building, demo                      |
| 何时         | 2130026041     | robot and cloud movement, view transition          |




