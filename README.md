## 📘 3D Cube with Transformation Toggles using OpenGL :
🔧 Overview

For My Social Media Account -> @ahmedmaherplus && @ahmedrefaat

This C++ program renders a 3D cube using OpenGL with support for applying various geometric transformations in real-time. It uses GLFW for window/context creation, GLAD for loading OpenGL functions, and GLM for mathematics.
🧱 Features

    Camera movement using W, A, S, D

    Mouse look only when the left mouse button is held down

    Toggle transformations using number keys:

        1: Apply Translation

        2: Apply Rotation

        3: Apply Scaling

        4: Apply Shearing

        5: Apply Reflection

        0: Reset (disable all transformations)

🧠 Technical Details
🎮 Controls
Key	Action
W/A/S/D	Move camera
Mouse Drag (LMB)	Rotate camera
1	Toggle Translation
2	Toggle Rotation
3	Toggle Scaling
4	Toggle Shearing
5	Toggle Reflection
0	Disable all transformations
🧱 Transformations

    Translation: Moves cube along the x-axis.

    Rotation: Rotates around an arbitrary axis.

    Scaling: Scales based on sine wave for dynamic effect.

    Shearing: Applies shear along x-axis over time.

    Reflection: Reflects across the yz-plane.

🧩 Structure

    main(): Initializes context, compiles shaders, sets up buffers.

    processInput(): Handles keyboard interaction.

    mouse_callback(): Updates camera direction (only when left mouse is pressed).

    Rendering Loop: Applies selected transformations and draws the cube.

📦 Dependencies

    OpenGL 3.3

    GLAD

    GLFW

    GLM
