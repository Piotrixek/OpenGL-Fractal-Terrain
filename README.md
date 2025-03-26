# OpenGL Fractal Terrain Generator

## Overview
This project is an OpenGL-based fractal terrain generator that creates realistic landscapes using procedural generation techniques. The application allows importing terrain from `.obj` files, rendering it with OpenGL, and exporting the generated terrain back to `.obj` format.

## Features
- **Procedural Terrain Generation**: Uses fractal algorithms such as Perlin noise or Diamond-Square to create realistic landscapes.
- **OBJ Import/Export**: Load terrain data from `.obj` files and export generated terrain back to `.obj` format.
- **Real-time Rendering**: Utilizes OpenGL for smooth and interactive 3D visualization.
- **Adjustable Parameters**: Customize terrain properties such as height, detail level, and noise scale.
- **Camera Controls**: Navigate through the terrain with adjustable perspective and movement controls.

## Dependencies
- OpenGL
- GLFW
- GLAD
- GLM
- Assimp (for `.obj` file handling)

## Usage
- **Import Terrain**: Load a `.obj` file to modify an existing terrain.
- **Generate Terrain**: Run the fractal generation algorithm to create a new landscape.
- **Export Terrain**: Save the generated terrain to an `.obj` file.
- **Modify Parameters**: Adjust settings such as terrain roughness and scale.

## Future Improvements
- Add support for additional file formats (e.g., `.png` heightmaps)
- Implement GPU-based terrain generation for better performance
- Enhance texture mapping and lighting effects
- Add terrain erosion simulation for more realistic landscapes

## Contributing
Contributions are welcome! Feel free to open issues or submit pull requests to improve the project.

## Author
Piotrixek / Veni 
GitHub: https://github.com/Piotrixek

