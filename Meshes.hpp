void Dealership::createEnvMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
	// The primitive built here is a box centered in the origin, with proportions respecting the texture.

	// Top wall & Bottom wall
	vDef.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.0f}});	// vertex 0 - Position and Normal
	vDef.push_back({{4.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}});	// vertex 1 - Position and Normal
	vDef.push_back({{0.0f, 2.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});	// vertex 2 - Position and Normal
	vDef.push_back({{4.0f, 2.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}});   // vertex 3 - Position and Normal

	vDef.push_back({{0.0f, 0.0f, 4.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}});	// vertex 4 - Position and Normal
	vDef.push_back({{4.0f, 0.0f, 4.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.5f}});	// vertex 5 - Position and Normal
	vDef.push_back({{0.0f, 2.0f, 4.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.5f}});	// vertex 6 - Position and Normal
	vDef.push_back({{4.0f, 2.0f, 4.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});   // vertex 7 - Position and Normal

	// Top wall
	vIdx.push_back(2); vIdx.push_back(3); vIdx.push_back(7);	// First triangle
	vIdx.push_back(2); vIdx.push_back(6); vIdx.push_back(7);	// Second triangle

	// Bottom wall
	vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(5);	// First triangle
	vIdx.push_back(0); vIdx.push_back(4); vIdx.push_back(5);	// Second triangle

	// Left wall & Right wall
	vDef.push_back({{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.5f}});	// vertex 8 - Position and Normal
	vDef.push_back({{4.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.5f}});	// vertex 9 - Position and Normal
	vDef.push_back({{0.0f, 2.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.0f}});	// vertex 10 - Position and Normal
	vDef.push_back({{4.0f, 2.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});   // vertex 11 - Position and Normal

	vDef.push_back({{0.0f, 0.0f, 4.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.5f}});	// vertex 12 - Position and Normal
	vDef.push_back({{4.0f, 0.0f, 4.0f}, {-1.0f, 0.0f, 0.0f}, {0.5f, 0.5f}});	// vertex 13 - Position and Normal
	vDef.push_back({{0.0f, 2.0f, 4.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});	// vertex 14 - Position and Normal
	vDef.push_back({{4.0f, 2.0f, 4.0f}, {-1.0f, 0.0f, 0.0f}, {0.5f, 0.0f}});   // vertex 15 - Position and Normal

	// Left wall
	vIdx.push_back(8); vIdx.push_back(10); vIdx.push_back(14);	// First triangle
	vIdx.push_back(8); vIdx.push_back(12); vIdx.push_back(14);	// Second triangle

	// Right wall
	vIdx.push_back(9); vIdx.push_back(11); vIdx.push_back(15);	// First triangle
	vIdx.push_back(9); vIdx.push_back(13); vIdx.push_back(15);	// Second triangle

	// Far wall
	vDef.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.5f}});	// vertex 16 - Position and Normal
	vDef.push_back({{4.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}});	// vertex 17 - Position and Normal
	vDef.push_back({{0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});	// vertex 18 - Position and Normal
	vDef.push_back({{4.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.0f}});   // vertex 19 - Position and Normal

	// Far wall
	vIdx.push_back(16); vIdx.push_back(17); vIdx.push_back(18);	// First triangle
	vIdx.push_back(17); vIdx.push_back(18); vIdx.push_back(19);	// Second triangle

	// Close wall
	vDef.push_back({{0.0f, 0.0f, 4.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.5f}});	// vertex 20 - Position and Normal
	vDef.push_back({{4.0f, 0.0f, 4.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.5f}});	// vertex 21 - Position and Normal
	vDef.push_back({{0.0f, 2.0f, 4.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.0f}});	// vertex 22 - Position and Normal
	vDef.push_back({{4.0f, 2.0f, 4.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});   // vertex 23 - Position and Normal

	// Close wall
	vIdx.push_back(20); vIdx.push_back(21); vIdx.push_back(22);	// First triangle
	vIdx.push_back(21); vIdx.push_back(22); vIdx.push_back(23);	// Second triangle
}

void Dealership::createShowcaseMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
    uint32_t numV = 64;
    float initialAngle = 360.0f / (float)numV;
    float currentAngle = 0;
    vDef.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});	// vertex 0 - center
    vDef.push_back({{0.0f, 0.1f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});	// vertex 0 - center

    for (uint32_t i = 0; i < numV; i++) {
        vDef.push_back({{3.5*cos(glm::radians(currentAngle)),
                              0.0f,
                              3.5*sin(glm::radians(currentAngle))},
                        {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});	// bottom circumference, y normal
        vDef.push_back({{3.5*cos(glm::radians(currentAngle)),
                              0.0f,
                              3.5*sin(glm::radians(currentAngle))},
                        {cos(glm::radians(currentAngle)),
                              0.0f,
                              sin(glm::radians(currentAngle))}, {0.0f, 0.0f}});
        vDef.push_back({{3.5*cos(glm::radians(currentAngle)),
                              0.1f,
                              3.5*sin(glm::radians(currentAngle))},
                        {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});	// top circumference, y normal
        vDef.push_back({{3.5*cos(glm::radians(currentAngle)),
                              0.1f,
                              3.5*sin(glm::radians(currentAngle))},
                        {cos(glm::radians(currentAngle)),
                              0.0f,
                              sin(glm::radians(currentAngle))}, {0.0f, 0.0f}});
        currentAngle += initialAngle;
    }

    for (uint32_t i = 0; i < numV*4-4; i=i+4) {
        vIdx.push_back(0); vIdx.push_back(i+2); vIdx.push_back(i+6);	// First triangle
        vIdx.push_back(1); vIdx.push_back(i+4); vIdx.push_back(i+8);	// First triangle

        vIdx.push_back(i+3); vIdx.push_back(i+5); vIdx.push_back(i+7);	// First triangle
        vIdx.push_back(i+7); vIdx.push_back(i+9); vIdx.push_back(i+5);	// First triangle
    }

    vIdx.push_back(0); vIdx.push_back(vDef.size()-4); vIdx.push_back(2);
    vIdx.push_back(1); vIdx.push_back(vDef.size()-2); vIdx.push_back(4);

    vIdx.push_back(vDef.size()-3); vIdx.push_back(vDef.size()-1); vIdx.push_back(3);
    vIdx.push_back(5); vIdx.push_back(vDef.size()-1); vIdx.push_back(3);
}

void Dealership::createSphereMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
	// The primitive built here is a sphere of radius 1, centered in the origin, on which the Mars texture is applied seamless.

	uint32_t numRings = 32, numV = 256, firstVertex, initialVertexIdx = 0;
	float uCoordStep = 1.0f / (float)numV;
	float vCoordStep = 1.0f / (float)numRings;
	float x, y ,z;
	float InitHeightAngle = glm::radians(180.0f / (float)numRings);   // inclination
	float InitRingAngle = glm::radians(360.0f / (float)numV);       // azimuth
	float currentRingAngle = 0, currentHeightAngle = InitHeightAngle;
	float currentUCoord = 0, currentVCoord = vCoordStep;

	//vDef.push_back({{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});	// vertex 0 - Position, Normal and uv
	for (uint32_t i = 0; i < numV+1; i++) {
		x = sin(currentHeightAngle) * cos(currentRingAngle);
		z = sin(currentHeightAngle) * sin(currentRingAngle);
		y = cos(currentHeightAngle);
		vDef.push_back({{x, y, z},
		                {x,  y, z},
		                {currentUCoord, currentVCoord}});    // vertex 0 - Position, Normal and uv
		currentRingAngle += InitRingAngle;
		currentUCoord += uCoordStep;
	}

	// internal rings
	for (uint32_t ring = 0; ring < numRings-2; ring++) {
		currentHeightAngle += InitHeightAngle;
		currentRingAngle = 0;
		currentVCoord += vCoordStep;
		currentUCoord = 0;
		firstVertex = vDef.size();

		for (uint32_t i = 0; i < numV+1; i++) {
			x = sin(currentHeightAngle) * cos(currentRingAngle);
			z = sin(currentHeightAngle) * sin(currentRingAngle);
			y = cos(currentHeightAngle);
			vDef.push_back({{x, y, z},
			                {x,  y, z},
			                {currentUCoord, currentVCoord}});    // vertex 0 - Position, Normal and uv
			currentRingAngle += InitRingAngle;
			currentUCoord += uCoordStep;
		}

		for (uint32_t t = 0; t < numV; t++){
			vIdx.push_back(firstVertex + t); vIdx.push_back(firstVertex-numV-1 + t); vIdx.push_back(firstVertex-numV + t);
			vIdx.push_back(firstVertex + t); vIdx.push_back(firstVertex + t + 1); vIdx.push_back(firstVertex-numV + t);
		}
	}

	currentUCoord = 0;
	for (uint32_t t = 0; t < numV; t++){
		vDef.push_back({{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {currentUCoord, 0.0f}});	// vertex 0 - Position, Normal and uv
		vIdx.push_back(vDef.size()-1); vIdx.push_back(initialVertexIdx + t); vIdx.push_back(initialVertexIdx + t + 1);
		vDef.push_back({{0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {currentUCoord, 1.0f}});	// vertex 0 - Position, Normal and uv
		vIdx.push_back(vDef.size()-1); vIdx.push_back(firstVertex + t); vIdx.push_back(firstVertex + t + 1);
		currentUCoord += uCoordStep;
	}
}