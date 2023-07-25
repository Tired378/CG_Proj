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

struct OverlayUniformBlock {
    alignas(4) float visible;
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

struct VertexOverlay {
    glm::vec2 pos;
    glm::vec2 UV;
};

// MAIN !
class Dealership : public BaseProject {
    protected:

    // Current aspect ratio (used by the callback that resized the window
    float Ar;

    // Descriptor Layouts ["classes" of what will be passed to the shaders]
    DescriptorSetLayout DSLGubo, DSLEnv;
    DescriptorSetLayout DSLCar, DSLCar1, DSLCar2, DSLSpotlight, DSLOverlay;

    // Vertex formats
    VertexDescriptor VMesh;
    VertexDescriptor VOverlay;

    // Pipelines [Shader couples]
    Pipeline PMesh, PShow, PDoor, PSpotlight;
	Pipeline PCar1_0, PCar1_1, PCar1_2, PCar2, PCar;
    Pipeline POverlay;

	// Descriptors
    DescriptorSet DSEnv, DSShow, DSGubo, DSDoor, DSSphere, DSSpotlight[MAX_LIGHTS];
    DescriptorSet DSCar1, DSCar2, DSCar3, DSCar4, DSCar5;
    DescriptorSet DSCommands, DSOCar1, DSOCar2, DSOCar3, DSOCar4, DSOCar5;

    // Models
    Model<VertexMesh> MEnv, MShow, MSphere, MDoor, MSpotlight[MAX_LIGHTS];
    Model<VertexMesh> MCar1, MCar2, MCar3, MCar4, MCar5;
    Model<VertexOverlay> Commands, OCar1, OCar2, OCar3, OCar4, OCar5;

    // Uniform variables
	MeshUniformBlock mubCar, mubEnv, mubSpotlight;

    GlobalUniformBlock gub;
    OverlayUniformBlock uboCommands, uboOverCar;

	// Textures
    Texture TEnv, TEnv_1, TEnv_2;
    Texture TDoor, TDoor_1, TDoor_2;
	Texture TSphere;
    Texture TSpotlight_0, TSpotlight_1, TSpotlight_2, TSpotlight_3;
    Texture TCar1_0, TCar1_1, TCar1_2, TCar1_3;
    Texture TCar2_0, TCar2_1;
    Texture TCar3_0;
    Texture TCar4_0;
    Texture TCar5_0;
    Texture TCommands, TOCar1, TOCar2, TOCar3, TOCar4, TOCar5;

    // Other application parameters
    int currCarModel = 0, currPipeShader = 0, currLightSetup = 0;
    int NumCars = 5, NumShaders = 3, NumLightSetup = 3;
    glm::mat4 ViewPrj;
    glm::vec3 Pos = glm::vec3(6.0f,1.0f,10.0f); // Initial spawn location
    glm::vec3 PrevPos = Pos;
    glm::vec3 CarSpawnPos = glm::vec3(6.0f,0.0f,6.0f);
    glm::vec3 cameraPos;
    // Initial camera angles
    float Yaw = glm::radians(0.0f);
    float Pitch = glm::radians(0.0f);
    float Roll = glm::radians(0.0f);
    float MOVE_SPEED = 2.0f;
    // Showcase platform parameters
    const float ShowRotSpeed = glm::radians(-5.0f);
    float ShowRot = 0.0f;
    float OCarVisible = 0.0f;

    // Here you set the main application parameters
    void setWindowParameters() override {
        // Window size, title and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Car Dealership";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
        
        // Descriptor pool sizes
        uniformBlocksInPool = 20;
        texturesInPool = 43;
        setsInPool = 20;
        
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
        
        DSLOverlay.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
                });
        
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
        
        VOverlay.init(this, {
                  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
                }, {
                  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
                         sizeof(glm::vec2), OTHER},
                  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
                         sizeof(glm::vec2), UV}
                });

        // Pipelines [Shader couples]
        PCar1_0.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/Car1BlinnShaderFrag.spv", {&DSLGubo, &DSLCar1});
	    PCar1_1.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/Car1PhongShaderFrag.spv", {&DSLGubo, &DSLCar1});
	    PCar1_2.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/Car1GGXShaderFrag.spv", {&DSLGubo, &DSLCar1});
        PCar2.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/Car2ShaderFrag.spv", {&DSLGubo, &DSLCar2});
        PCar.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/CarShaderFrag.spv", {&DSLGubo, &DSLCar});

		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", {&DSLOverlay});
        POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

	    PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/BlinnNormMapFrag.spv", {&DSLGubo, &DSLEnv});
        PMesh.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

        PShow.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/ShowFrag.spv", {&DSLGubo, &DSLEnv});
        PShow.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

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
        
        Commands.vertices = {{{-0.8f, 0.6f}, {0.0f,0.0f}}, {{-0.8f, 0.95f}, {0.0f,1.0f}},
                         {{ 0.8f, 0.6f}, {1.0f,0.0f}}, {{ 0.8f, 0.95f}, {1.0f,1.0f}}};
        Commands.indices = {0, 1, 2,    1, 2, 3};
        Commands.initMesh(this, &VOverlay);
        
        OCar1.vertices = {{{-0.8f, -0.55f}, {0.0f,0.0f}}, {{-0.8f, 0.25f}, {0.0f,1.0f}},
                         {{ 0.8f, -0.55f}, {1.0f,0.0f}}, {{ 0.8f, 0.25f}, {1.0f,1.0f}}};
        OCar1.indices = {0, 1, 2,    1, 2, 3};
        OCar1.initMesh(this, &VOverlay);
        
        OCar2.vertices = {{{-0.8f, -0.55f}, {0.0f,0.0f}}, {{-0.8f, 0.25f}, {0.0f,1.0f}},
                         {{ 0.8f, -0.55f}, {1.0f,0.0f}}, {{ 0.8f, 0.25f}, {1.0f,1.0f}}};
        OCar2.indices = {0, 1, 2,    1, 2, 3};
        OCar2.initMesh(this, &VOverlay);
        
        OCar3.vertices = {{{-0.8f, -0.55f}, {0.0f,0.0f}}, {{-0.8f, 0.25f}, {0.0f,1.0f}},
                         {{ 0.8f, -0.55f}, {1.0f,0.0f}}, {{ 0.8f, 0.25f}, {1.0f,1.0f}}};
        OCar3.indices = {0, 1, 2,    1, 2, 3};
        OCar3.initMesh(this, &VOverlay);
        
        OCar4.vertices = {{{-0.8f, -0.55f}, {0.0f,0.0f}}, {{-0.8f, 0.25f}, {0.0f,1.0f}},
                         {{ 0.8f, -0.55f}, {1.0f,0.0f}}, {{ 0.8f, 0.25f}, {1.0f,1.0f}}};
        OCar4.indices = {0, 1, 2,    1, 2, 3};
        OCar4.initMesh(this, &VOverlay);
        
        OCar5.vertices = {{{-0.8f, -0.55f}, {0.0f,0.0f}}, {{-0.8f, 0.25f}, {0.0f,1.0f}},
                         {{ 0.8f, -0.55f}, {1.0f,0.0f}}, {{ 0.8f, 0.25f}, {1.0f,1.0f}}};
        OCar5.indices = {0, 1, 2,    1, 2, 3};
        OCar5.initMesh(this, &VOverlay);
        
        
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

        TSphere.init(this, "textures/TextureSphere.png");
        
        TCommands.init(this, "textures/PressSpace.png");
        TOCar1.init(this, "textures/COverlay1.png");
        TOCar2.init(this, "textures/COverlay2.png");
        TOCar3.init(this, "textures/COverlay3.png");
        TOCar4.init(this, "textures/COverlay4.png");
        TOCar5.init(this, "textures/COverlay5.png");
    }
    
    // Here you create your pipelines and Descriptor Sets!
    void pipelinesAndDescriptorSetsInit() override {
        // Pipeline creation
        PMesh.create();
        PCar1_0.create();
	    PCar1_1.create();
	    PCar1_2.create();
        PCar2.create();
        PCar.create();
        PDoor.create();
        PShow.create();
        PSpotlight.create();
        POverlay.create();

        // Descriptor sets init
        DSEnv.init(this, &DSLEnv, {
                {0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
                {1, TEXTURE, 0, &TEnv},
                {2, TEXTURE, 0, &TEnv_1},
                {3, TEXTURE, 0, &TEnv_2}
        });
        
        DSCommands.init(this, &DSLOverlay, {
                    {0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TCommands}
                });
        
        DSOCar1.init(this, &DSLOverlay, {
                    {0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TOCar1}
                });
        
        DSOCar2.init(this, &DSLOverlay, {
                    {0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TOCar2}
                });
        
        DSOCar3.init(this, &DSLOverlay, {
                    {0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TOCar3}
                });
        
        DSOCar4.init(this, &DSLOverlay, {
                    {0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TOCar4}
                });
        
        DSOCar5.init(this, &DSLOverlay, {
                    {0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
                    {1, TEXTURE, 0, &TOCar5}
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
                {1, TEXTURE, 0, &TSphere},
                {2, TEXTURE, 0, &TSphere},
                {3, TEXTURE, 0, &TSphere}
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
        PCar1_0.cleanup();
	    PCar1_1.cleanup();
	    PCar1_2.cleanup();
		PCar2.cleanup();
        PCar.cleanup();
        PDoor.cleanup();
        PShow.cleanup();
        PSpotlight.cleanup();
        POverlay.cleanup();

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
        DSCommands.cleanup();
        DSOCar1.cleanup();
        DSOCar2.cleanup();
        DSOCar3.cleanup();
        DSOCar4.cleanup();
        DSOCar5.cleanup();
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
        Commands.cleanup();
        OCar1.cleanup();
        OCar2.cleanup();
        OCar3.cleanup();
        OCar4.cleanup();
        OCar5.cleanup();
        
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

        TSphere.cleanup();
        
        TCommands.cleanup();
        TOCar1.cleanup();
        TOCar2.cleanup();
        TOCar3.cleanup();
        TOCar4.cleanup();
        TOCar5.cleanup();

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
	    DSLOverlay.cleanup();
        
        // Destroys the pipelines
        PMesh.destroy();
        PCar1_0.destroy();
	    PCar1_1.destroy();
	    PCar1_2.destroy();
        PCar2.destroy();
        PCar.destroy();
        PDoor.destroy();
        PShow.destroy();
        PSpotlight.destroy();
        POverlay.destroy();
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

        MSphere.bind(commandBuffer);
        DSSphere.bind(commandBuffer, PMesh, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MSphere.indices.size()), 1, 0, 0, 0);

        PShow.bind(commandBuffer);
        MShow.bind(commandBuffer);
        DSShow.bind(commandBuffer, PShow, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MShow.indices.size()), 1, 0, 0, 0);

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

	    POverlay.bind(commandBuffer);
	    Commands.bind(commandBuffer);
	    DSCommands.bind(commandBuffer, POverlay, 0, currentImage);
	    vkCmdDrawIndexed(commandBuffer,
	                     static_cast<uint32_t>(Commands.indices.size()), 1, 0, 0, 0);

        switch(currCarModel) {
            case 0:
				switch(currPipeShader){
					case 0:
		                PCar1_0.bind(commandBuffer);
		                DSGubo.bind(commandBuffer, PCar1_0, 0, currentImage);
		                MCar1.bind(commandBuffer);
		                DSCar1.bind(commandBuffer, PCar1_0, 1, currentImage);
						break;
					case 1:
						PCar1_1.bind(commandBuffer);
						DSGubo.bind(commandBuffer, PCar1_1, 0, currentImage);
						MCar1.bind(commandBuffer);
						DSCar1.bind(commandBuffer, PCar1_1, 1, currentImage);
						break;
					case 2:
						PCar1_2.bind(commandBuffer);
						DSGubo.bind(commandBuffer, PCar1_2, 0, currentImage);
						MCar1.bind(commandBuffer);
						DSCar1.bind(commandBuffer, PCar1_2, 1, currentImage);
						break;
				}
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar1.indices.size()), 1, 0, 0, 0);

				POverlay.bind(commandBuffer);
		        OCar1.bind(commandBuffer);
		        DSOCar1.bind(commandBuffer, POverlay, 0, currentImage);
		        vkCmdDrawIndexed(commandBuffer,
		                         static_cast<uint32_t>(OCar1.indices.size()), 1, 0, 0, 0);
				break;
            case 1:
                PCar2.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar2, 0, currentImage);
                MCar2.bind(commandBuffer);
                DSCar2.bind(commandBuffer, PCar2, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar2.indices.size()), 1, 0, 0, 0);

				POverlay.bind(commandBuffer);
		        OCar2.bind(commandBuffer);
		        DSOCar2.bind(commandBuffer, POverlay, 0, currentImage);
		        vkCmdDrawIndexed(commandBuffer,
		                         static_cast<uint32_t>(OCar2.indices.size()), 1, 0, 0, 0);
		        break;
            case 2:
                PCar.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar, 0, currentImage);
                MCar3.bind(commandBuffer);
                DSCar3.bind(commandBuffer, PCar, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar3.indices.size()), 1, 0, 0, 0);

				POverlay.bind(commandBuffer);
		        OCar3.bind(commandBuffer);
		        DSOCar3.bind(commandBuffer, POverlay, 0, currentImage);
		        vkCmdDrawIndexed(commandBuffer,
		                         static_cast<uint32_t>(OCar3.indices.size()), 1, 0, 0, 0);
				break;
            case 3:
                PCar.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar, 0, currentImage);
                MCar4.bind(commandBuffer);
                DSCar4.bind(commandBuffer, PCar, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar4.indices.size()), 1, 0, 0, 0);

				POverlay.bind(commandBuffer);
		        OCar4.bind(commandBuffer);
		        DSOCar4.bind(commandBuffer, POverlay, 0, currentImage);
		        vkCmdDrawIndexed(commandBuffer,
		                         static_cast<uint32_t>(OCar4.indices.size()), 1, 0, 0, 0);
				break;
            case 4:
                PCar.bind(commandBuffer);
                DSGubo.bind(commandBuffer, PCar, 0, currentImage);
                MCar5.bind(commandBuffer);
                DSCar5.bind(commandBuffer, PCar, 1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                                 static_cast<uint32_t>(MCar5.indices.size()), 1, 0, 0, 0);

				POverlay.bind(commandBuffer);
		        OCar5.bind(commandBuffer);
		        DSOCar5.bind(commandBuffer, POverlay, 0, currentImage);
		        vkCmdDrawIndexed(commandBuffer,
		                         static_cast<uint32_t>(OCar5.indices.size()), 1, 0, 0, 0);
				break;
        }
    }

	// Definition of camera movement
    void GameLogic() {
        // Parameters
        // Camera FOV-y, Near Plane and Far Plane
        const float FOVy = glm::radians(60.0f);
        const float nearPlane = 0.1f;
        const float farPlane = 100.f;
        // Camera target height and distance
        const float camHeight = 1.5;
        const float camDist = 1;
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

        PrevPos = Pos;

        // Rotation
        Yaw = Yaw - ROT_SPEED * deltaT * r.y;

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

	// Helper function for printing the current shader for Car1
	static void PrintShader(int value) {
		switch (value){
			case 0:
				std::cout << "Shader: Blinn \n";
				break;
			case 1:
				std::cout << "Shader: Phong \n";
				break;
			case 2:
				std::cout << "Shader: GGX \n";
				break;
			default:
				break;
		}
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
		auto targetPoint = glm::vec3(6.0f, 1.0f, 6.0f); // Target point in the center (for the lights)

        // Key press parameters
        static bool debounce = false;
        static int curDebounce = 0;

        // Switch currCarModel on specific key press
        if(glfwGetKey(window, GLFW_KEY_N)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_N;
                currCarModel = (currCarModel+1 == NumCars) ? 0 : currCarModel+1;
                std::cout << "Scene : " << currCarModel << "\n";
                OCarVisible = 0;
	            uboOverCar.visible = OCarVisible;
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
                OCarVisible = 0;
	            uboOverCar.visible = OCarVisible;
                RebuildPipeline();
            }
        } else {
            if((curDebounce == GLFW_KEY_P) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

		// Switch shader version for car 1 on specific key press
		if (currCarModel == 0) {
		    if(glfwGetKey(window, GLFW_KEY_X)) {
			    if(!debounce) {
				    debounce = true;
				    curDebounce = GLFW_KEY_X;
				    currPipeShader = (currPipeShader+1 == NumShaders) ? 0 : currPipeShader+1;
				    PrintShader(currPipeShader);
				    RebuildPipeline();
			    }
		    } else {
			    if((curDebounce == GLFW_KEY_X) && debounce) {
				    debounce = false;
				    curDebounce = 0;
			    }
		    }
		    if(glfwGetKey(window, GLFW_KEY_Z)) {
			    if(!debounce) {
				    debounce = true;
				    curDebounce = GLFW_KEY_Z;
				    currPipeShader = (currPipeShader-1 < 0) ? NumShaders-1 : currPipeShader-1;
				    PrintShader(currPipeShader);
				    RebuildPipeline();
			    }
		    } else {
			    if((curDebounce == GLFW_KEY_Z) && debounce) {
				    debounce = false;
				    curDebounce = 0;
			    }
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

        // Use IJKL to change the spotlight position, hold ALT to change it's color parameters
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

	    /* Environment */
        // Car Spotlight
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

        // Mapping of the room
        mubEnv.amb = .0f; mubEnv.gamma = 100.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::scale(glm::mat4(1), glm::vec3(3));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));
        DSEnv.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

        // Mapping of the Showcase platform
        mubEnv.amb = .0f; mubEnv.gamma = 180.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::rotate(glm::translate(glm::mat4(1.0f),
                                   glm::vec3(6.0f, 0.0f, 6.0f)),ShowRot,
                                   glm::vec3(0,1,0));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));
        DSShow.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

        // Mapping of the Door
        mubEnv.amb = 0.01f; mubEnv.gamma = 100.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::rotate(glm::rotate(glm::scale(
            glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 1.65f, 12.0f)),
            glm::vec3(1.5)), glm::radians(-90.0f), glm::vec3(1, 0, 0)),
            glm::radians(-90.0f), glm::vec3(0, 0, 1));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));
        DSDoor.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

        // Mapping of the sphere
        mubEnv.amb = 1.0f; mubEnv.gamma = 180.0f; mubEnv.sColor = glm::vec3(1.0f);
        mubEnv.mMat = glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.2));
        mubEnv.mvpMat = ViewPrj * mubEnv.mMat;
        mubEnv.nMat = glm::inverse(glm::transpose(mubEnv.mMat));
        DSSphere.map((int)currentImage, &mubEnv, sizeof(mubEnv), 0);

		/* Spotlights */
	    mubSpotlight.amb = 1.0f;
	    mubSpotlight.gamma = 1000.0f;
	    mubSpotlight.sColor = 0.9f * glm::vec3(1.0f);
		// positioning spotlights
		const float radius = 4.0f; // Radius of the circle 
		auto circleCenter = glm::vec3(6.0f, 5.5f, 6.0f); // Center position of the circle
		// Location
		for (int i = 0; i < MAX_LIGHTS; i++) {
			float angle = (float)i * (2 * glm::pi<float>()) / MAX_LIGHTS; // Calculate the angle for each light
			gub.pointLights[i].lightPos = circleCenter + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
		}
		// Rotation
		for (int i = 0; i < MAX_LIGHTS; i++) {
			glm::vec3 direction = -glm::normalize(targetPoint - gub.pointLights[i].lightPos); // Calculate the direction towards the target point
			glm::vec3 up = glm::vec3(0, -1, -1.1f); // Define the up direction
            glm::mat4 rotationMat = glm::lookAt(glm::vec3(0.0f), direction, up); // Create a rotation matrix based on the direction and up vectors

			// Mapping of the spotlights
			mubSpotlight.mMat = glm::scale(glm::translate(glm::mat4(1.0f), gub.pointLights[i].lightPos) * rotationMat, glm::vec3(2));
			mubSpotlight.mvpMat = ViewPrj * mubSpotlight.mMat;
			mubSpotlight.nMat = glm::inverse(glm::transpose(mubSpotlight.mMat));
			DSSpotlight[i].map((int)currentImage, &mubSpotlight, sizeof(mubSpotlight), 0);
		}
		// Spotlight parameters setting  (color, gamma, Ms_factor)
		static float gamma = 500.0f;
		static float Ms_factor = 5.0f;
		// Press B to go to the next spotlight scene setting
		if (glfwGetKey(window, GLFW_KEY_B)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_B;
				currLightSetup = (currLightSetup + 1 == NumLightSetup) ? 0 : currLightSetup + 1;
				switch (currLightSetup) {
				case 0:
					gamma = 500.0f;
					Ms_factor = 5.0f;
					std::cout << "Light setup: BICOLOR, gamma = 500.0, Ms_factor = 5.0\n";
					break;
				case 1:
					gamma = 150.0f;
					Ms_factor = 190.0f;
					std::cout << "Light setup: MULTICOLOR, gamma = 150.0, Ms_factor = 190.0\n";
					break;
				case 2:
					gamma = 1000.0f;
					Ms_factor = 20.0f;
					std::cout << "Light setup: MONOCOLOR, gamma = 1000.0, Ms_factor = 20.0\n";
					break;
				default:
					break;
				}
				RebuildPipeline();
			}
		} else {
			if ((curDebounce == GLFW_KEY_B) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		// Press V to go to the previous spotlight scene setting
		if (glfwGetKey(window, GLFW_KEY_V)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;
				currLightSetup = (currLightSetup - 1 < 0) ? NumLightSetup - 1 : currLightSetup - 1;
				switch (currLightSetup) {
				case 0:
					gamma = 500.0f;
					Ms_factor = 5.0f;
					std::cout << "Light setup: BICOLOR, gamma = 500.0, Ms_factor = 5.0\n";
					break;
				case 1:
					gamma = 150.0f;
					Ms_factor = 190.0f;
					std::cout << "Light setup: MULTICOLOR, gamma = 150.0, Ms_factor = 190.0\n";
					break;
				case 2:
					gamma = 1000.0f;
					Ms_factor = 20.0f;
					std::cout << "Light setup: MONOCOLOR, gamma = 1000.0, Ms_factor = 20.0\n";
					break;
				default:
					break;
				}
				RebuildPipeline();
			}
		} else {
			if ((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		// Spotlights colors setting
		switch (currLightSetup) {
			case 0:
				gub.pointLights[0].lightColor = glm::vec4(glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
				gub.pointLights[1].lightColor = glm::vec4(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
				gub.pointLights[2].lightColor = glm::vec4(glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
				gub.pointLights[3].lightColor = glm::vec4(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
				break;
			case 1:
				gub.pointLights[0].lightColor = glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
				gub.pointLights[1].lightColor = glm::vec4(glm::vec3(0.0f, 1.0f, 1.0f), 1.0f);
				gub.pointLights[2].lightColor = glm::vec4(glm::vec3(1.0f, 0.0f, 1.0f), 1.0f);
				gub.pointLights[3].lightColor = glm::vec4(glm::vec3(1.0f, 0.5f, 0.0f), 1.0f);
				break;
			case 2:
				gub.pointLights[0].lightColor = glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
				gub.pointLights[1].lightColor = glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
				gub.pointLights[2].lightColor = glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
				gub.pointLights[3].lightColor = glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
				break;
			default:
				break;
		}
		// Manual tuning of gamma parameter (press 2 to increase, press 1 to decrease)
		if (glfwGetKey(window, GLFW_KEY_2)) {
			gamma = gamma + 1.0f;
			std::cout << "gamma= "<< gamma << std::endl;
		}
		if (glfwGetKey(window, GLFW_KEY_1)) {
			gamma = glm::max(gamma - 1.0f, 0.1f);
			std::cout << "gamma= " << gamma << std::endl;
		}
		// Manual tuning of specular factor parameter (press 4 to increase, press 3 to decrease)
		if (glfwGetKey(window, GLFW_KEY_4)) {
			Ms_factor = Ms_factor + 1.0f;
			std::cout << "Ms_factor= "<< Ms_factor << std::endl;
		}
		if (glfwGetKey(window, GLFW_KEY_3)) {
			Ms_factor = glm::max(Ms_factor - 1.0f, 0.1f);
			std::cout << "Ms_factor= " << Ms_factor << std::endl;
		}

		/* Overlays */
	    uboCommands.visible = 1.0f;
	    DSCommands.map((int)currentImage, &uboCommands, sizeof(uboCommands), 0);
		// Change visibility of overlay by pressing F
	    if(glfwGetKey(window, GLFW_KEY_F)) {
		    if (!debounce) {
			    debounce = true;
			    curDebounce = GLFW_KEY_F;
			    OCarVisible = (OCarVisible == 0.0f) ? 1.0f : 0.0f;
			    uboOverCar.visible = OCarVisible;
		    }
	    } else {
		    if ((curDebounce == GLFW_KEY_F) && debounce) {
			    debounce = false;
			    curDebounce = 0;
		    }
	    }

		/* Car Models */
		// Mapping of the car models
        switch(currCarModel) {
            case 0:
                mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(
							glm::scale(
							glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.1f, 6.0f)),
							glm::vec3(0.012, 0.012, 0.012)),
						ShowRot, glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar1.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
		        DSOCar1.map((int)currentImage, &uboOverCar, sizeof(uboOverCar), 0);
                break;
            case 1:
				mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(
						glm::scale(
                        glm::translate(glm::mat4(1.0f),glm::vec3(6.0f, 0.1f, 6.0f)),
                         glm::vec3(0.0115, 0.0115, 0.0115)),
					 ShowRot, glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar2.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
		        DSOCar2.map((int)currentImage, &uboOverCar, sizeof(uboOverCar), 0);
                break;
            case 2:
				mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(
						glm::scale(
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
		        DSOCar3.map((int)currentImage, &uboOverCar, sizeof(uboOverCar), 0);
                break;
            case 3:
                mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(
                        glm::scale(
                        glm::rotate(
                        glm::translate(glm::mat4(1.0f),glm::vec3(6.0f, 1.2f, 6.0f)),
                      glm::radians(90.0f),glm::vec3(0, 1, 0)),
                         glm::vec3(0.035, 0.035, 0.035)),
                      ShowRot, glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar4.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
		        DSOCar4.map((int)currentImage, &uboOverCar, sizeof(uboOverCar), 0);
                break;
            case 4:
				mubCar.amb = 1.0f; mubCar.gamma = gamma; mubCar.sColor = Ms_factor * glm::vec3(1.0f);
                mubCar.mMat = glm::rotate(
						glm::scale(
                        glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.1f, 6.0f)),
                         glm::vec3(1.3, 1.3, 1.3)),
					  ShowRot, glm::vec3(0,1,0));
                mubCar.mvpMat = ViewPrj * mubCar.mMat;
                mubCar.nMat = glm::inverse(glm::transpose(mubCar.mMat));
                DSCar5.map((int)currentImage, &mubCar, sizeof(mubCar), 0);
		        DSOCar5.map((int)currentImage, &uboOverCar, sizeof(uboOverCar), 0);
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
