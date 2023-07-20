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
#define MAX_LIGHTS 4

struct PointLight {
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec4 lightColor; //rgb + intensity
};

struct MeshUniformBlock {
    alignas(4) float amb;
    alignas(4) float gamma;
    alignas(16) glm::vec3 sColor;
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBlock {
    alignas(16) glm::vec3 DlightPos;
    alignas(16) glm::vec3 DlightDir;
    alignas(16) glm::vec3 DlightColor;
    alignas(16) glm::vec3 AmbLightColor;
    alignas(16) glm::vec3 eyePos;
	PointLight pointLights[MAX_LIGHTS];
};

struct VertexMesh {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 norm;
    alignas(8) glm::vec2 UV;
	alignas(16) glm::vec4 tan;
};

// MAIN !
class Dealership : public BaseProject {
    protected:

    // Current aspect ratio (used by the callback that resized the window
    float Ar;

    // Descriptor Layouts ["classes" of what will be passed to the shaders]
    DescriptorSetLayout DSLGubo, DSLEnv;
    DescriptorSetLayout DSLCar, DSLCar1, DSLCar2, DSLSpotlight;

    // Vertex formats
    VertexDescriptor VMesh;

    // Pipelines [Shader couples]
    Pipeline PMesh, PDoor, PCar1, PCar2, PCar, PSpotlight;

    DescriptorSet DSEnv, DSShow, DSGubo, DSDoor, DSSphere, DSSpotlight[MAX_LIGHTS];
    DescriptorSet DSCar1, DSCar2, DSCar3, DSCar4, DSCar5;

    // Models, textures and Descriptors (values assigned to the uniforms)
    Model<VertexMesh> MEnv, MShow, MSphere, MDoor, MSpotlight[MAX_LIGHTS];
    Model<VertexMesh> MCar1, MCar2, MCar3, MCar4, MCar5;

    // C++ storage for uniform variables
	MeshUniformBlock mubCar, mubEnv, mubSpotlight;

    GlobalUniformBlock gub;

    Texture TEnv, TEnv_1, TEnv_2;
    Texture TDoor, TDoor_1, TDoor_2;

    Texture TSpotlight_0, TSpotlight_1, TSpotlight_2, TSpotlight_3;

    Texture TCar1_0, TCar1_1, TCar1_2, TCar1_3;
    Texture TCar2_0, TCar2_1, TCar2_2, TCar2_3;
    Texture TCar3_0, TCar3_1, TCar3_2, TCar3_3;
    Texture TCar4_0, TCar4_1, TCar4_2, TCar4_3;
    Texture TCar5_0, TCar5_1, TCar5_2, TCar5_3;

    // Other application parameters
    int currCarModel = 0;
    int NumCars = 5;
    glm::mat4 ViewPrj;
    glm::vec3 Pos = glm::vec3(6.0f,1.0f,10.0f); // Initial spawn location
    glm::vec3 PrevPos = Pos;
    glm::vec3 CarSpawnPos = glm::vec3(6.0f,0.0f,6.0f);
    glm::vec3 cameraPos;
    // Initial camera angles
    float Yaw = glm::radians(0.0f);
    //float PrevYaw = Yaw;
    float Pitch = glm::radians(0.0f);
    float Roll = glm::radians(0.0f);
    float MOVE_SPEED = 2.0f;
    // Showcase platform parameters
    const float ShowRotSpeed = glm::radians(-5.0f);
    float ShowRot = 0.0f;

    // Here you set the main application parameters
    void setWindowParameters() override {
        // window size, title and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Car Dealership";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
        
        // Descriptor pool sizes
        /* Dealership */
        /* Update the requirements for the size of the pool */
        uniformBlocksInPool = 28;
        texturesInPool = 37;
        setsInPool = 28;
        
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
        DSLCar.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
            });

        DSLCar1.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                    {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                    {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                    {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
                });

        DSLCar2.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                    {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                });

        DSLEnv.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                    {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                    {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                });

		DSLSpotlight.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
					{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
					{3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
					{4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

        DSLGubo.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
            });


        // Vertex descriptors
        VMesh.init(this, {
                  {0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
                }, {
                  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
                         sizeof(glm::vec3), POSITION},
                  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
                         sizeof(glm::vec3), NORMAL},
                  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, tan),
		                  sizeof(glm::vec4), TANGENT},
                  {0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
                         sizeof(glm::vec2), UV},
                });

        // Pipelines [Shader couples]
        PCar1.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/Car1ShaderFrag.spv", {&DSLGubo, &DSLCar1});
        PCar2.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/Car2ShaderFrag.spv", {&DSLGubo, &DSLCar2});
        PCar.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/CarShaderFrag.spv", {&DSLGubo, &DSLCar});

        //PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/BlinnNormMapFrag.spv", {&DSLGubo, &DSLEnv});
	    PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/BlinnNormMapFrag.spv", {&DSLGubo, &DSLEnv});
        PMesh.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

	    PDoor.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/DoorFrag.spv", {&DSLGubo, &DSLEnv});

		PSpotlight.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/SpotlightShaderFrag.spv", {&DSLGubo, &DSLSpotlight});

        /* Models */
        createEnvMesh(MEnv.vertices, MEnv.indices);
        MEnv.initMesh(this, &VMesh);

        createShowcaseMesh(MShow.vertices, MShow.indices);
        MShow.initMesh(this, &VMesh);

        createSphereMesh(MSphere.vertices, MSphere.indices);
        MSphere.initMesh(this, &VMesh);

        MCar1.init(this, &VMesh, "Models/Car1.gltf", GLTF);
        MCar2.init(this, &VMesh, "Models/Car2.obj", OBJ);
        MCar3.init(this, &VMesh, "Models/Car3.obj", OBJ);
        MCar4.init(this, &VMesh, "Models/Car4.obj", OBJ);
        MCar5.init(this, &VMesh, "Models/Car5.obj", OBJ);

        MDoor.init(this, &VMesh, "models/door/door.gltf", GLTF);


		for (auto & i : MSpotlight)
		{
			i.init(this, &VMesh, "models/Spotlight.obj", OBJ);
		}
		
        /* Textures */
        TCar1_0.init(this, "textures/Car1/cb_car_baseColor.png");
        TCar1_1.init(this, "textures/Car1/cb_car_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
        TCar1_2.init(this, "textures/Car1/cb_car_metallicRoughness.png", VK_FORMAT_R8G8B8A8_UNORM);
        TCar1_3.init(this, "textures/Car1/cb_car_emissive.png", VK_FORMAT_R8G8B8A8_UNORM);

        TCar2_0.init(this, "textures/Car2/baseColor.png");
        TCar2_1.init(this, "textures/Car2/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);

        TCar3_0.init(this, "textures/Car3/baseColor.png");

        TCar4_0.init(this, "textures/Car4/baseColor.png");

        TCar5_0.init(this, "textures/Car5/baseColor.png");

		TSpotlight_0.init(this, "textures/Spotlight/baseColor.png");
        TSpotlight_1.init(this, "textures/Spotlight/normal.png", VK_FORMAT_R8G8B8A8_UNORM);
        TSpotlight_2.init(this, "textures/Spotlight/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
        TSpotlight_3.init(this, "textures/Spotlight/emission.png", VK_FORMAT_R8G8B8A8_UNORM);

        TEnv.init(this, "textures/TextureRoom2.jpg");
        TEnv_1.init(this, "textures/TextureRoom_norm.jpg", VK_FORMAT_R8G8B8A8_UNORM);
        TEnv_2.init(this, "textures/TextureRoom_roughness.jpg", VK_FORMAT_R8G8B8A8_UNORM);

        TDoor.init(this, "textures/door/Door_baseColor.jpeg");
        TDoor_1.init(this, "textures/door/Door_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
        TDoor_2.init(this, "textures/door/Door_metallicRoughness.png", VK_FORMAT_R8G8B8A8_UNORM);
    }
    
    // Here you create your pipelines and Descriptor Sets!
    void pipelinesAndDescriptorSetsInit() override {
        // This creates a new pipeline (with the current surface), using its shaders
        PMesh.create();
        PCar1.create();
        PCar2.create();
        PCar.create();
		PDoor.create();
        PSpotlight.create();

        // Here you define the data set
        DSEnv.init(this, &DSLEnv, {
                {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                {1, TEXTURE, 0, &TEnv},
                {2, TEXTURE, 0, &TEnv_1},
                {3, TEXTURE, 0, &TEnv_2}
        });

        DSShow.init(this, &DSLEnv, {
                {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                {1, TEXTURE, 0, &TEnv},
                {2, TEXTURE, 0, &TEnv_1},
                {3, TEXTURE, 0, &TEnv_2}
        });

        DSDoor.init(this, &DSLEnv, {
               {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
               {1, TEXTURE, 0, &TDoor},
               {2, TEXTURE, 0, &TDoor_1},
               {3, TEXTURE, 0, &TDoor_2}
        });

        DSSphere.init(this, &DSLEnv, {
                {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                {1, TEXTURE, 0, &TEnv},
                {2, TEXTURE, 0, &TEnv_1},
                {3, TEXTURE, 0, &TEnv_2}
        });

		for (auto & i : DSSpotlight)
		{
			i.init(this, &DSLSpotlight, {
				{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
				{1, TEXTURE, 0, &TSpotlight_0},
				{2, TEXTURE, 0, &TSpotlight_1},
				{3, TEXTURE, 0, &TSpotlight_2},
				{4, TEXTURE, 0, &TSpotlight_3},
			});
		}

        DSCar1.init(this, &DSLCar1, {
                    {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TCar1_0},
                    {2, TEXTURE, 0, &TCar1_1},
                    {3, TEXTURE, 0, &TCar1_2},
                    {4, TEXTURE, 0, &TCar1_3}

        });

        DSCar2.init(this, &DSLCar2, {
                    {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TCar2_0},
                    {2, TEXTURE, 0, &TCar2_1},

        });

        DSCar3.init(this, &DSLCar, {
                    {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TCar3_0}

        });

        DSCar4.init(this, &DSLCar, {
                    {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TCar4_0}

        });
        
        DSCar5.init(this, &DSLCar, {
                    {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TCar5_0}

        });

        DSGubo.init(this, &DSLGubo, {
                    {0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
        });
    }

    // Here you destroy your pipelines and Descriptor Sets!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    void pipelinesAndDescriptorSetsCleanup() override {
        // Cleanup pipelines
        PMesh.cleanup();
        PCar1.cleanup();
        PCar2.cleanup();
        PCar.cleanup();
		PDoor.cleanup();
        PSpotlight.cleanup();

        DSCar1.cleanup();
        DSCar2.cleanup();
        DSCar3.cleanup();
        DSCar4.cleanup();
        DSCar5.cleanup();
        DSGubo.cleanup();
        DSDoor.cleanup();
        DSEnv.cleanup();
        DSShow.cleanup();
        DSSphere.cleanup();
		for (auto & i : DSSpotlight)
		{
			i.cleanup();
		}
    }

    // Here you destroy all the Models, Texture and Desc. Set Layouts you created!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    // You also have to destroy the pipelines: since they need to be rebuilt, they have two different
    // methods: .cleanup() recreates them, while .destroy() delete them completely
    void localCleanup() override {
        // Cleanup textures
        TCar1_0.cleanup();
        TCar1_1.cleanup();
        TCar1_2.cleanup();
        TCar1_3.cleanup();

        TCar2_0.cleanup();
        TCar2_1.cleanup();

        TCar3_0.cleanup();

        TCar4_0.cleanup();

        TCar5_0.cleanup();

        TEnv.cleanup();
        TEnv_1.cleanup();
        TEnv_2.cleanup();

        TSpotlight_0.cleanup();
        TSpotlight_1.cleanup();
        TSpotlight_2.cleanup();
        TSpotlight_3.cleanup();

        TDoor.cleanup();
        TDoor_1.cleanup();
        TDoor_2.cleanup();

        // Cleanup models
        MCar1.cleanup();
        MCar2.cleanup();
        MCar3.cleanup();
        MCar4.cleanup();
        MCar5.cleanup();
        MEnv.cleanup();
        MShow.cleanup();
        MDoor.cleanup();
        MSphere.cleanup();
		for (auto & i : MSpotlight)
		{
			i.cleanup();
		}

        // Cleanup descriptor set layouts
        DSLCar1.cleanup();
        DSLCar2.cleanup();
        DSLCar.cleanup();
        DSLGubo.cleanup();
        DSLEnv.cleanup();
        DSLSpotlight.cleanup();
        
        // Destroys the pipelines
        PMesh.destroy();
        PCar1.destroy();
        PCar2.destroy();
        PCar.destroy();
		PDoor.destroy();
        PSpotlight.destroy();
    }
    
    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) override {

        PMesh.bind(commandBuffer);

        DSGubo.bind(commandBuffer, PMesh, 0, currentImage);

        MEnv.bind(commandBuffer);
        DSEnv.bind(commandBuffer, PMesh, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MEnv.indices.size()), 1, 0, 0, 0);


        MShow.bind(commandBuffer);
        DSShow.bind(commandBuffer, PMesh, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MShow.indices.size()), 1, 0, 0, 0);

        MSphere.bind(commandBuffer);
        DSSphere.bind(commandBuffer, PMesh, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MSphere.indices.size()), 1, 0, 0, 0);

		PDoor.bind(commandBuffer);
        MDoor.bind(commandBuffer);
        DSDoor.bind(commandBuffer, PDoor, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
            static_cast<uint32_t>(MDoor.indices.size()), 1, 0, 0, 0);

		PSpotlight.bind(commandBuffer);
		DSGubo.bind(commandBuffer, PSpotlight, 0, currentImage);
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			MSpotlight[i].bind(commandBuffer);		
			DSSpotlight[i].bind(commandBuffer, PSpotlight, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MSpotlight[i].indices.size()), 1, 0, 0, 0);
		}

        switch(currCarModel) {
            case 0:
                PCar1.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar1, 0, currentImage);
                MCar1.bind(commandBuffer);
                DSCar1.bind(commandBuffer, PCar1, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar1.indices.size()), 1, 0, 0, 0);
                break;
            case 1:
                PCar2.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar2, 0, currentImage);
                MCar2.bind(commandBuffer);
                DSCar2.bind(commandBuffer, PCar2, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar2.indices.size()), 1, 0, 0, 0);
                break;
            case 2:
                PCar.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar, 0, currentImage);
                MCar3.bind(commandBuffer);
                DSCar3.bind(commandBuffer, PCar, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar3.indices.size()), 1, 0, 0, 0);
                break;
            case 3:
                PCar.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar, 0, currentImage);
                MCar4.bind(commandBuffer);
                DSCar4.bind(commandBuffer, PCar, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar4.indices.size()), 1, 0, 0, 0);
                break;
            case 4:
                PCar.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar, 0, currentImage);
                MCar5.bind(commandBuffer);
                DSCar5.bind(commandBuffer, PCar, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar5.indices.size()), 1, 0, 0, 0);
                break;
        }
    }

    void GameLogic() {
        // Parameters
        // Camera FOV-y, Near Plane and Far Plane
        const float FOVy = glm::radians(60.0f);
        const float nearPlane = 0.1f;
        const float farPlane = 100.f;
        // Camera target height and distance
        const float camHeight = 1.5;
        const float camDist = 1; /*1.5;*/
        // Camera Pitch limits
        const float minPitch = glm::radians(-60.0f);
        const float maxPitch = glm::radians(60.0f);
        const float minRoll = glm::radians(-30.0f);
        const float maxRoll = glm::radians(30.0f);
        // Rotation and motion speed
        const float ROT_SPEED = glm::radians(120.0f);
        // Car showcase radius
        const float RADIUS = 3.5f;

        // Integration with the timers and the controllers
        float deltaT;
        auto m = glm::vec3(0.0f), r = glm::vec3(0.0f);
        bool fire = false;
        getSixAxis(deltaT, m, r, fire);

        // Game Logic implementation

        // Showcase platform rotation
        ShowRot += ShowRotSpeed * deltaT;

        ViewPrj = glm::mat4(1);
        glm::mat4 World = glm::mat4(1);

        // World
        // Position
        glm::vec3 ux = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
        glm::vec3 uz = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);

        // Collision checks with walls
        Pos = Pos + MOVE_SPEED * m.x * ux * deltaT;
        if (Pos.x < 1.0f) Pos.x = 1.0f;
        else if (Pos.x > 11.0f) Pos.x = 11.0f;
        //Pos = Pos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
        //Pos.y = Pos.y < 0.0f ? 0.0f : Pos.y;
        Pos.y = 0.5f;
        Pos = Pos + MOVE_SPEED * m.z * uz * deltaT;
        if (Pos.z < 1.0f) Pos.z = 1.0f;
        else if (Pos.z > 11.0f) Pos.z = 11.0f;

        // Collision check with the car showcase
        if (glm::distance(CarSpawnPos, Pos) < RADIUS) Pos = PrevPos;

        //if (Pos != PrevPos) std::cout << "Pos = (" << Pos.x << ", " << Pos.y << ", " << Pos.z << ")" << std::endl;
        PrevPos = Pos;

        // Rotation
        Yaw = Yaw - ROT_SPEED * deltaT * r.y;
        /*if (Yaw != PrevYaw) std::cout << "Yaw = " << Yaw << std::endl;
        PrevYaw = Yaw;*/

        Pitch = Pitch + ROT_SPEED * deltaT * r.x;
        Pitch  =  Pitch < minPitch ? minPitch :
                  (Pitch > maxPitch ? maxPitch : Pitch);
        Roll   = Roll - ROT_SPEED * deltaT * -r.z;
        Roll   = Roll < minRoll ? minRoll :
                 (Roll > maxRoll ? maxRoll : Roll);

        // Final world matrix computation
        World = glm::translate(glm::mat4(1), Pos) * glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0,1,0));

        // Projection
        glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
        Prj[1][1] *= -1;

        // View
        // Target
        glm::vec3 target = Pos + glm::vec3(0.0f, camHeight, 0.0f);

        // Camera position, depending on Yaw parameter, but not character direction
        cameraPos = World * glm::vec4(0.0f, camHeight + camDist * sin(Pitch), camDist * cos(Pitch), 1.0);

        // Damping of camera
        glm::mat4 View = glm::rotate(glm::mat4(1.0f), -Roll, glm::vec3(0,0,1)) *
                         glm::lookAt(cameraPos, target, glm::vec3(0,1,0));

        ViewPrj = Prj * View;
    }

    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) override {
        // Standard procedure to quit when the ESC key is pressed
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Lighting parameters
        const auto lightStep = 0.2f;
        const auto colorStep = 10.0f;
        static auto colorX = 1.0f;
        static auto colorY = 1.0f;
        static auto colorZ = 1.0f;
        static float* currentColorPtr = &colorX;
        static auto lightPos = glm::vec3(6.0f, 5.8f, 6.0f);
        static auto lightPosOld = lightPos;
        static auto colorOld = glm::vec3(colorX, colorY, colorZ);


		gub.pointLights[0].lightColor = glm::vec4(glm::vec3(1.0f, 10.0f, 1.0f), 1.0f);
		gub.pointLights[1].lightColor = glm::vec4(glm::vec3(1.0f, 1.0f, 10.0f), 1.0f);
		gub.pointLights[2].lightColor = glm::vec4(glm::vec3(10.0f, 1.0f, 1.0f), 1.0f);
		gub.pointLights[3].lightColor = glm::vec4(glm::vec3(1.0f, 10.0f, 1.0f), 1.0f);
		const float radius = 4.0f; // Radius of the circle
		auto circleCenter = glm::vec3(6.0f, 5.5f, 6.0f); // Center position of the circle
		auto targetPoint = glm::vec3(6.0f, 1.0f, 6.0f); // Target point in the center

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			float angle = i * (2 * glm::pi<float>()) / MAX_LIGHTS; // Calculate the angle for each light
			gub.pointLights[i].lightPos = circleCenter + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
		}

        // Key press parameters
        static bool debounce = false;
        static int curDebounce = 0;
        static bool showNormal = false;
        static bool showUV = false;

        // Switch currCarModel on specific key press
        if(glfwGetKey(window, GLFW_KEY_N)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_N;
                currCarModel = (currCarModel+1 == NumCars) ? 0 : currCarModel+1;
                std::cout << "Scene : " << currCarModel << "\n";
                RebuildPipeline();
            }
        } else {
            if((curDebounce == GLFW_KEY_N) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_P)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_P;
                currCarModel = (currCarModel-1 < 0) ? NumCars-1 : currCarModel-1;
                std::cout << "Scene : " << currCarModel << "\n";
                RebuildPipeline();
            }
        } else {
            if((curDebounce == GLFW_KEY_P) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        // Hold SHIFT to run
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_LEFT_SHIFT;
                MOVE_SPEED = 5.0f;
            }
        } else {
            if((curDebounce == GLFW_KEY_LEFT_SHIFT) && debounce) {
                debounce = false;
                curDebounce = 0;
                MOVE_SPEED = 2.0f;
            }
        }

        // Hold ALT to change the light color, or don't hold to change light position
        if(glfwGetKey(window, GLFW_KEY_LEFT_ALT)) {
            // While holding ALT, we can modify the light color
            if (glfwGetKey(window, GLFW_KEY_J)){
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_J;
                    if (currentColorPtr == &colorX) {
                        currentColorPtr = &colorZ;
                        std::cout << "Switched to B." << std::endl;
                    }
                    else if (currentColorPtr == &colorY) {
                        currentColorPtr = &colorX;
                        std::cout << "Switched to R." << std::endl;
                    }
                    else {
                        currentColorPtr = &colorY;
                        std::cout << "Switched to G." << std::endl;
                    }
                }
            } else {
                if((curDebounce == GLFW_KEY_J) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_L)){
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_L;
                    if (currentColorPtr == &colorX) {
                        currentColorPtr = &colorY;
                        std::cout << "Switched to G." << std::endl;
                    }
                    else if (currentColorPtr == &colorY) {
                        currentColorPtr = &colorZ;
                        std::cout << "Switched to B." << std::endl;
                    }
                    else {
                        currentColorPtr = &colorX;
                        std::cout << "Switched to R." << std::endl;
                    }
                }
            } else {
                if((curDebounce == GLFW_KEY_L) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_I)) {
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_I;
                    *currentColorPtr += colorStep;
                    if (*currentColorPtr > 255.0f) {
                        *currentColorPtr = 255.0f;
                        std::cout << "Maximum reached." << std::endl;
                    }
                }
            } else {
                if((curDebounce == GLFW_KEY_I) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_K)) {
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_K;
                    *currentColorPtr -= colorStep;
                    if (*currentColorPtr < 1.0f) {
                        *currentColorPtr = 1.0f;
                        std::cout << "Minimum reached." << std::endl;
                    }
                }
            } else {
                if((curDebounce == GLFW_KEY_K) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }

            // Press R to reset the light's color
            if(glfwGetKey(window, GLFW_KEY_R)) {
                if(!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_R;
                    colorX = 1.0f;
                    colorY = 1.0f;
                    colorZ = 1.0f;
                }
            } else {
                if((curDebounce == GLFW_KEY_R) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
        } else {
            // If not holding ALT, modify the light's position
            if (glfwGetKey(window, GLFW_KEY_J)) {
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_J;
                    lightPos.x -= lightStep;
                }
            } else {
                if ((curDebounce == GLFW_KEY_J) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_L)) {
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_L;
                    lightPos.x += lightStep;
                }
            } else {
                if ((curDebounce == GLFW_KEY_L) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_I)) {
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_I;
                    lightPos.z -= lightStep;
                }
            } else {
                if ((curDebounce == GLFW_KEY_I) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_K)) {
                if (!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_K;
                    lightPos.z += lightStep;
                }
            } else {
                if ((curDebounce == GLFW_KEY_K) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }

            // Press R to reset the light's position
            if(glfwGetKey(window, GLFW_KEY_R)) {
                if(!debounce) {
                    debounce = true;
                    curDebounce = GLFW_KEY_R;
                    lightPos = glm::vec3(6.0f, 5.8f, 6.0f);
                }
            } else {
                if((curDebounce == GLFW_KEY_R) && debounce) {
                    debounce = false;
                    curDebounce = 0;
                }
            }
        }

        // Light position limits
        if (lightPos.x < 0.2f) lightPos.x = 0.2f;
        else if (lightPos.x > 11.8f) lightPos.x = 11.8f;
        if (lightPos.z < 0.2f) lightPos.z = 0.2f;
        else if (lightPos.z > 11.8f) lightPos.z = 11.8f;



        if (colorOld.x != colorX || colorOld.y != colorY || colorOld.z != colorZ) std::cout << "Color = (" << colorX << ", " << colorY << ", " << colorZ << ")" << std::endl;
        colorOld = glm::vec3(colorX, colorY, colorZ);

        GameLogic();

        // Car Point light
        gub.DlightPos = lightPos;
        gub.DlightDir = glm::normalize(lightPos - targetPoint);
        gub.DlightColor = glm::vec4(colorX, colorY, colorZ, 1.0f);
        gub.AmbLightColor = glm::vec3(0.2f);
        gub.eyePos = cameraPos;

	    if (lightPosOld != lightPos) {
		    std::cout << "lightPos = (" << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ")" << std::endl;
		    std::cout << gub.DlightDir.x << " " << gub.DlightDir.y << " " << gub.DlightDir.z << std::endl;
	    }
	    lightPosOld = lightPos;

        DSGubo.map((int)currentImage, &gub, sizeof(gub), 0);

        mubEnv.amb = 1.0f; mubEnv.gamma = 180.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::scale(glm::mat4(1), glm::vec3(3));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));

        // Mapping of the room
        DSEnv.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

        mubEnv.amb = 1.0f; mubEnv.gamma = 180.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::rotate(glm::translate(glm::mat4(1.0f),
                                   glm::vec3(6.0f, 0.0f, 6.0f)),ShowRot,
                                   glm::vec3(0,1,0));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));

        // Mapping of the Showcase platform
        DSShow.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

        mubEnv.amb = 1.0f; mubEnv.gamma = 180.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::rotate(glm::rotate(glm::scale(
            glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 1.65f, 12.0f)),
            glm::vec3(1.5)), glm::radians(-90.0f), glm::vec3(1, 0, 0)),
            glm::radians(-90.0f), glm::vec3(0, 0, 1));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));

        // Mapping of the Door
        DSDoor.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

        mubEnv.amb = 1.0f; mubEnv.gamma = 180.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.2));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));

        // Mapping of the sphere
        DSSphere.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

	    mubSpotlight.amb = 1.0f;
	    mubSpotlight.gamma = 1000.0f;
	    mubSpotlight.sColor = 0.9f * glm::vec3(1.0f);

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			glm::vec3 direction = -glm::normalize(targetPoint - gub.pointLights[i].lightPos); // Calculate the direction towards the target point
			glm::vec3 up = glm::vec3(0, -1, -1.1f); // Define the up direction

			glm::mat4 rotationMat = glm::lookAt(glm::vec3(0.0f), direction, up); // Create a rotation matrix based on the direction and up vectors

			mubSpotlight.mMat = glm::scale(glm::translate(glm::mat4(1.0f), gub.pointLights[i].lightPos) * rotationMat, glm::vec3(2));
			mubSpotlight.mvpMat = ViewPrj * mubSpotlight.mMat;
			mubSpotlight.nMat = glm::inverse(glm::transpose(mubSpotlight.mMat));
			DSSpotlight[i].map((int)currentImage, &mubSpotlight, sizeof(mubSpotlight), 0);
		}
		//to test gamma **************************REMOVE********
		static float gamma = 1000.0f;
		if (glfwGetKey(window, GLFW_KEY_KP_4)) {
			gamma = gamma + 1.0f;
			std::cout << "gamma= "<< gamma << std::endl;
		}
		if (glfwGetKey(window, GLFW_KEY_KP_1)) {
			gamma = glm::max(gamma - 1.0f, 0.0f);
			std::cout << "gamma= " << gamma << std::endl;
		}
		//to test metallic factor 
		static float Ms_factor = 0.9f;
		if (glfwGetKey(window, GLFW_KEY_KP_5)) {
			Ms_factor = Ms_factor + 1.0f;
			std::cout << "Ms_factor= "<< Ms_factor << std::endl;
		}
		if (glfwGetKey(window, GLFW_KEY_KP_2)) {
			Ms_factor = glm::max(Ms_factor - 1.0f, 0.0f);
			std::cout << "Ms_factor= " << Ms_factor << std::endl;
		}
		//*********************************************************
        switch(currCarModel) {
            case 0:
                mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f),
                                                                    glm::vec3(6.0f, 0.1f, 6.0f)),
                                                     glm::vec3(0.012, 0.012, 0.012)), ShowRot,
                                          glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar1.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
                break;
            case 1:
				mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(glm::scale(
                        glm::translate(glm::mat4(1.0f),glm::vec3(6.0f, 0.1f, 6.0f)),
                         glm::vec3(0.0115, 0.0115, 0.0115)), ShowRot, glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar2.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
                break;
            case 2:
				mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(glm::scale(
                        glm::rotate(
                        glm::rotate(
                        glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.5f, 6.0f)),
                      glm::radians(90.0f), glm::vec3(-1, 0, 0)),
                      glm::radians(90.0f), glm::vec3(0, 0, 1)),
                         glm::vec3(0.08, 0.08, 0.08)),
                                          ShowRot, glm::vec3(0,0,1));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar3.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
                break;
            case 3:
				mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(
                        glm::scale(
                        glm::rotate(
                        glm::translate(
                        glm::mat4(1.0f),
                         glm::vec3(6.0f, 1.2f, 6.0f)),
                      glm::radians(90.0f),
                         glm::vec3(0, 1, 0)),
                         glm::vec3(0.035, 0.035, 0.035)),
                      ShowRot, glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar4.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
                break;
            case 4:
				mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(glm::scale(
                          glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.1f, 6.0f)),
                           glm::vec3(1.3, 1.3, 1.3)),
                                          ShowRot, glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar5.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
                break;
        }
    }

    static void createEnvMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
    static void createShowcaseMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
    static void createSphereMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
	static void generate_tangents(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
};

#include "Meshes.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    Dealership app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
