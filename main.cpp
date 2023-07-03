// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"

// The uniform buffer objects data structures
// Remember to use the correct alignas(...) value
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)

struct MeshUniformBlock {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
};

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

// The vertices data structures
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

/*struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};*/

/* A16 */
/* Add the C++ data structure for the required vertex format */
/*struct VertexVColor {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 color;
};*/

/*struct VertexEnv {
	glm::vec3 pos;
	glm::vec3 norm;
};*/




// MAIN ! 
class A16 : public BaseProject {
	protected:

	// Current aspect ratio (used by the callback that resized the window)
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo;
	DescriptorSetLayout DSLMesh;
	//DescriptorSetLayout DSLOverlay;
	/* A16 */
	/* Add the variable that will contain the required Descriptor Set Layout */
	//DescriptorSetLayout DSLVColor;
	//DescriptorSetLayout DSLEnv;

	// Vertex formats
	VertexDescriptor VMesh;
	//VertexDescriptor VOverlay;
	/* A16 */
	/* Add the variable that will contain the required Vertex format definition */
	//VertexDescriptor VVColor;
	//VertexDescriptor VEnv;

	// Pipelines [Shader couples]
	Pipeline PMesh;
	//Pipeline POverlay;
	/* A16 */
	/* Add the variable that will contain the new pipeline */
	//Pipeline PVColor;
	//Pipeline PEnv;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	//Model<VertexMesh> MBody, MHandle, MWheel;
	/* A16 */
	/* Add the variable that will contain the model for the room */
	//Model<VertexVColor> MRoom;
	Model<VertexMesh> MEnv;

	//Model<VertexOverlay> MKey, MSplash;
	DescriptorSet DSGubo;
	//DescriptorSet DSBody, DSHandle, DSWheel1, DSWheel2, DSWheel3, DSKey, DSSplash;
	/* A16 */
	/* Add the variable that will contain the Descriptor Set for the room */
	//DescriptorSet DSRoom;
	DescriptorSet DSEnv;

	//Texture TBody, THandle, TWheel, TKey, TSplash;
	Texture TEnv;
	
	// C++ storage for uniform variables
	//MeshUniformBlock uboBody, uboHandle, uboWheel1, uboWheel2, uboWheel3;
	/* A16 */
	/* Add the variable that will contain the Uniform Block in slot 0, set 1 of the room */
	//MeshUniformBlock uboRoom;
	MeshUniformBlock uboEnv;

	GlobalUniformBlock gubo;
	//OverlayUniformBlock uboKey, uboSplash;

	// Other application parameters
	float CamH, CamRadius, CamPitch, CamYaw;
	//int gameState;


	// Here you set the main application parameters
	void setWindowParameters() override {
		// window size, title and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Car Dealership";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		/* A16 */
		/* Update the requirements for the size of the pool */
		uniformBlocksInPool = 9;
		texturesInPool = 7;
		setsInPool = 9;
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) override {
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Textures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() override {
		// Descriptor Layouts [what will be passed to the shaders]
		/*DSLMesh.init(this, {
					// this array contains the bindings:
					// first  element : the binding number
					// second element : the type of element (buffer or texture)
					//                  using the corresponding Vulkan constant
					// third  element : the pipeline stage where it will be used
					//                  with the corresponding Vulkan constant
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});*/

		DSLMesh.init(this, {
				// this array contains the binding:
				// first  element : the binding number
				// second element : the type of element (buffer or texture)
				// third  element : the pipeline stage where it will be used
				{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
				{1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
				{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
		});
				
		/*DSLOverlay.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});*/
		/* A16 */
		/* Init the new Data Set Layout */
		/*DSLVColor.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});*/
				
		DSLGubo.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});

		// Vertex descriptors
		VMesh.init(this, {
				  // this array contains the bindings
				  // first  element : the binding number
				  // second element : the stride of this binging
				  // third  element : whether this parameter change per vertex or per instance
				  //                  using the corresponding Vulkan constant
				  {0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  // this array contains the location
				  // first  element : the binding number
				  // second element : the location number
				  // third  element : the offset of this element in the memory record
				  // fourth element : the data type of the element
				  //                  using the corresponding Vulkan constant
				  // fifth  element : the size in byte of the element
				  // sixth  element : a constant defining the element usage
				  //                   POSITION - a vec3 with the position
				  //                   NORMAL   - a vec3 with the normal vector
				  //                   UV       - a vec2 with a UV coordinate
				  //                   COLOR    - a vec4 with an RGBA color
				  //                   TANGENT  - a vec4 with the tangent vector
				  //                   OTHER    - anything else
				  //
				  // ***************** DOUBLE CHECK ********************
				  //    That the Vertex data structure you use in the "offsetof" and
				  //	in the "sizeof" in the previous array, refers to the correct one,
				  //	if you have more than one vertex format!
				  // ***************************************************
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
				         sizeof(glm::vec2), UV}
				});

		/*VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
				         sizeof(glm::vec2), OTHER},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
				         sizeof(glm::vec2), UV}
				});*/
		/* A16 */
		/* Define the new Vertex Format */
		/*VVColor.init(this, {
				  {0, sizeof(VertexVColor), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexVColor, pos),
					 sizeof(glm::vec3), POSITION},
			  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexVColor, norm),
					 sizeof(glm::vec3), NORMAL},
			  {0, 2,  VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexVColor, color),
					 sizeof(glm::vec3), COLOR}
			});*/

		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on...
		//PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", {&DSLGubo, &DSLMesh});
		PMesh.init(this, &VMesh, "shaders/BlinnVert.spv", "shaders/BlinnFrag.spv", {&DSLMesh});
		PMesh.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);
		/*POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", {&DSLOverlay});
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);*/
		/* A16 */
		/* Create the new pipeline, using shaders "VColorVert.spv" and "VColorFrag.spv" */
		//PVColor.init(this, &VVColor, "shaders/VColorVert.spv", "shaders/VColorFrag.spv", { &DSLGubo, &DSLVColor });

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		/*MBody.init(this,   &VMesh, "Models/SlotBody.obj", OBJ);
		MHandle.init(this, &VMesh, "Models/SlotHandle.obj", OBJ);
		MWheel.init(this,  &VMesh, "Models/SlotWheel.obj", OBJ);*/
		/* A16 */
		/* load the mesh for the room, contained in OBJ file "Room.obj" */
		//MRoom.init(this, &VVColor, "Models/Room.obj", OBJ);
		createEnvMesh(MEnv.vertices, MEnv.indices);
		MEnv.initMesh(this, &VMesh);
		
		// Creates a mesh with direct enumeration of vertices and indices
		/*MKey.vertices = {{{-0.8f, 0.6f}, {0.0f,0.0f}}, {{-0.8f, 0.95f}, {0.0f,1.0f}},
						 {{ 0.8f, 0.6f}, {1.0f,0.0f}}, {{ 0.8f, 0.95f}, {1.0f,1.0f}}};
		MKey.indices = {0, 1, 2,    1, 2, 3};
		MKey.initMesh(this, &VOverlay);*/
		
		// Creates a mesh with direct enumeration of vertices and indices
		/*MSplash.vertices = {{{-1.0f, -0.58559f}, {0.0102f, 0.0f}}, {{-1.0f, 0.58559f}, {0.0102f,0.85512f}},
						 {{ 1.0f,-0.58559f}, {1.0f,0.0f}}, {{ 1.0f, 0.58559f}, {1.0f,0.85512f}}};
		MSplash.indices = {0, 1, 2,    1, 2, 3};
		MSplash.initMesh(this, &VOverlay);*/
		
		// Create the textures
		// The second parameter is the file name
		/*TBody.init(this,   "textures/SlotBody.png");
		THandle.init(this, "textures/SlotHandle.png");
		TWheel.init(this,  "textures/SlotWheel.png");
		TKey.init(this,    "textures/PressSpace.png");
		TSplash.init(this, "textures/SplashScreen.png");*/
		TEnv.init(this, "textures/TBs_20140623_1_02.png");
		
		// Init local variables
		CamH = 1.0f;
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(30.f);
		//gameState = 0;
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() override {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		/*POverlay.create();*/
		/* A16 */
		/* Create the new pipeline */
		//PVColor.create();
		
		// Here you define the data set
		DSEnv.init(this, &DSLMesh, {
				{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
				{1, UNIFORM, sizeof(GlobalUniformBlock), nullptr},
				{2, TEXTURE, 0, &TEnv}
		});


		/*DSBody.init(this, &DSLMesh, {
		// the second parameter, is a pointer to the Uniform Set Layout of this set
		// the last parameter is an array, with one element per binding of the set.
		// first  element : the binding number
		// second element : UNIFORM or TEXTURE (an enum) depending on the type
		// third  element : only for UNIFORMS, the size of the corresponding C++ object. For texture, just put 0
		// fourth element : only for TEXTURES, the pointer to the corresponding texture object. For uniforms, use nullptr
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBody}
				});
		DSHandle.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &THandle}
				});
		DSWheel1.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TWheel}
				});
		DSWheel2.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TWheel}
				});
		DSWheel3.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TWheel}
				});*/
		/* A16 */
		/* Define the data set for the room */
		/*DSRoom.init(this, &DSLVColor, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr}
			});*/

		/*DSKey.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TKey}
				});
		DSSplash.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TSplash}
				});*/
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
				});
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() override {
		// Cleanup pipelines
		PMesh.cleanup();
		/*POverlay.cleanup();*/
		/* A16 */
		/* cleanup the new pipeline */
		//PVColor.cleanup();

		// Cleanup datasets
		/*DSBody.cleanup();
		DSHandle.cleanup();
		DSWheel1.cleanup();
		DSWheel2.cleanup();
		DSWheel3.cleanup();*/
		/* A16 */
		/* cleanup the dataset for the room */
		//DSRoom.cleanup();
		DSEnv.cleanup();

		/*DSKey.cleanup();
		DSSplash.cleanup();*/
		DSGubo.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() override {
		// Cleanup textures
		/*TBody.cleanup();
		THandle.cleanup();
		TWheel.cleanup();
		TKey.cleanup();
		TSplash.cleanup();*/
		TEnv.cleanup();
		
		// Cleanup models
		/*MBody.cleanup();
		MHandle.cleanup();
		MWheel.cleanup();
		MKey.cleanup();
		MSplash.cleanup();*/
		/* A16 */
		/* Cleanup the mesh for the room */
		//MRoom.cleanup();
		MEnv.cleanup();
		
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		/*DSLOverlay.cleanup();*/
		/* A16 */
		/* Cleanup the new Descriptor Set Layout */
		//DSLVColor.cleanup();

		DSLGubo.cleanup();
		
		// Destroys the pipelines
		PMesh.destroy();
		/*POverlay.destroy();*/
		/* A16 */
		/* Destroy the new pipeline */
		//PVColor.destroy();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) override {
		// sets global uniforms (see below for parameters explanation)
		//DSGubo.bind(commandBuffer, PMesh, 0, currentImage);

		// binds the pipeline
		PMesh.bind(commandBuffer);
		// For a pipeline object, this command binds the corresponding pipeline to the command buffer passed in its parameter

		// binds the model
		//MBody.bind(commandBuffer);
		// For a Model object, this command binds the corresponding index and vertex buffer
		// to the command buffer passed in its parameter
		MEnv.bind(commandBuffer);
		DSEnv.bind(commandBuffer, PMesh, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MEnv.indices.size()), 1, 0, 0, 0);
		// binds the data set
		//DSBody.bind(commandBuffer, PMesh, 1, currentImage);
		// For a Dataset object, this command binds the corresponding dataset
		// to the command buffer and pipeline passed in its first and second parameters.
		// The third parameter is the number of the set being bound
		// As described in the Vulkan tutorial, a different dataset is required for each image in the swap chain.
		// This is done automatically in file Starter.hpp, however the command here needs also the index
		// of the current image in the swap chain, passed in its last parameter
					
		// record the drawing command in the command buffer
		//vkCmdDrawIndexed(commandBuffer,
		//		static_cast<uint32_t>(MBody.indices.size()), 1, 0, 0, 0);
		// the second parameter is the number of indexes to be drawn. For a Model object,
		// this can be retrieved with the .indices.size() method.

	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) override {
		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		// Integration with the timers and the controllers
		float deltaT;
		auto m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		// getSixAxis() is defined in Starter.hpp in the base class.
		// It fills the float point variable passed in its first parameter with the time
		// since the last call to the procedure.
		// It fills vec3 in the second parameters, with three values in the -1,1 range corresponding
		// to motion (with left stick of the gamepad, or WASD + RF keys on the keyboard)
		// It fills vec3 in the third parameters, with three values in the -1,1 range corresponding
		// to motion (with right stick of the gamepad, or Arrow keys + QE keys on the keyboard, or mouse)
		// If fills the last boolean variable with true if fire has been pressed:
		//          SPACE on the keyboard, A or B button on the Gamepad, Right mouse button

		// To debounce the pressing of the fire button, and start the event when the key is released
		static bool wasFire = false;
		bool handleFire = (wasFire && (!fire));
		wasFire = fire;
		
		// Parameters: wheels and handle speed and range
		const float HandleSpeed = glm::radians(90.0f);
		const float HandleRange = glm::radians(45.0f);
		const float WheelSpeed = glm::radians(180.0f);
		const float SymExtent = glm::radians(15.0f);	// size of one symbol on the wheel in angle rad.
		// static variables for current angles
		static float HandleRot = 0.0;
		static float Wheel1Rot = 0.0;
		static float Wheel2Rot = 0.0;
		static float Wheel3Rot = 0.0;
		static float TargetRot = 0.0;	// Target rotation
		
		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(90.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;
		const float rotSpeed = glm::radians(90.0f);
		const float movSpeed = 1.0f;

		CamH += m.y * movSpeed * deltaT;
		CamRadius -= m.z * movSpeed * deltaT;
		CamPitch -= r.x * rotSpeed * deltaT;
		CamYaw += m.x * rotSpeed * deltaT;
		
		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;
		glm::vec3 camTarget = glm::vec3(0,CamH,0);
		glm::vec3 camPos    = camTarget +
							  CamRadius * glm::vec3(cos(CamPitch) * sin(CamYaw),
													sin(CamPitch),
													cos(CamPitch) * cos(CamYaw));
		glm::mat4 View = glm::lookAt(camPos, camTarget, glm::vec3(0,1,0));

		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;

		// Writes value to the GPU
		//DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		/*glm::mat4 World = glm::mat4(1);
		uboBody.amb = 1.0f; uboBody.gamma = 180.0f; uboBody.sColor = glm::vec3(1.0f);
		uboBody.mvpMat = Prj * View * World;
		uboBody.mMat = World;
		uboBody.nMat = glm::inverse(glm::transpose(World));
		DSBody.map(currentImage, &uboBody, sizeof(uboBody), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.3f,0.5f,-0.15f)),
							HandleRot, glm::vec3(1,0,0));
		uboHandle.amb = 1.0f; uboHandle.gamma = 180.0f; uboHandle.sColor = glm::vec3(1.0f);
		uboHandle.mvpMat = Prj * View * World;
		uboHandle.mMat = World;
		uboHandle.nMat = glm::inverse(glm::transpose(World));
		DSHandle.map(currentImage, &uboHandle, sizeof(uboHandle), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-0.15f,0.93f,-0.15f)),
							Wheel1Rot, glm::vec3(1,0,0));
		uboWheel1.amb = 1.0f; uboWheel1.gamma = 180.0f; uboWheel1.sColor = glm::vec3(1.0f);
		uboWheel1.mvpMat = Prj * View * World;
		uboWheel1.mMat = World;
		uboWheel1.nMat = glm::inverse(glm::transpose(World));
		DSWheel1.map(currentImage, &uboWheel1, sizeof(uboWheel1), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.93f,-0.15f)),
							Wheel2Rot, glm::vec3(1,0,0));
		uboWheel2.amb = 1.0f; uboWheel2.gamma = 180.0f; uboWheel2.sColor = glm::vec3(1.0f);
		uboWheel2.mvpMat = Prj * View * World;
		uboWheel2.mMat = World;
		uboWheel2.nMat = glm::inverse(glm::transpose(World));
		DSWheel2.map(currentImage, &uboWheel2, sizeof(uboWheel2), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.15f,0.93f,-0.15f)),
							Wheel3Rot, glm::vec3(1,0,0));
		uboWheel3.amb = 1.0f; uboWheel3.gamma = 180.0f; uboWheel3.sColor = glm::vec3(1.0f);
		uboWheel3.mvpMat = Prj * View * World;
		uboWheel3.mMat = World;
		uboWheel3.nMat = glm::inverse(glm::transpose(World));
		DSWheel3.map(currentImage, &uboWheel3, sizeof(uboWheel3), 0);*/
		/* A16 */
		/* fill the uniform block for the room. Identical to the one of the body of the slot machine */
		/*World = glm::mat4(1);
		uboRoom.amb = 1.0f; uboRoom.gamma = 180.0f; uboRoom.sColor = glm::vec3(1.0f);
		uboRoom.mvpMat = Prj * View * World;
		uboRoom.mMat = World;
		uboRoom.nMat = glm::inverse(glm::transpose(World));
		DSRoom.map(currentImage, &uboRoom, sizeof(uboRoom), 0);*/
		/* map the uniform data block to the GPU */

		uboEnv.mMat = glm::scale(glm::mat4(1), glm::vec3(3));
		uboEnv.mvpMat = Prj * View * uboEnv.mMat;
		uboEnv.nMat = glm::inverse(glm::transpose(uboEnv.mMat));


		//auto World = glm::mat4(1);
		//uboEnv.amb = 1.0f; uboEnv.gamma = 180.0f; uboEnv.sColor = glm::vec3(1.0f);
		//uboEnv.mvpMat = Prj * View * World;
		//uboEnv.mMat = World;
		//uboEnv.nMat = glm::inverse(glm::transpose(World));
		DSEnv.map(currentImage, &uboEnv, sizeof(uboEnv), 0);
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		/*uboKey.visible = (gameState == 1) ? 1.0f : 0.0f;
		DSKey.map(currentImage, &uboKey, sizeof(uboKey), 0);

		uboSplash.visible = (gameState == 0) ? 1.0f : 0.0f;
		DSSplash.map(currentImage, &uboSplash, sizeof(uboSplash), 0);*/
	}

	void createEnvMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
};

#include "EnvMesh.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    A16 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}