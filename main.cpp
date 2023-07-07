// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"

#define M_PI 3.14159265358979323846
#define HalfM_PI 1.57079632679489661923

// The uniform buffer objects data structures
// Remember to use the correct alignas(...) value
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)

/*struct MeshUniformBlock {
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
};*/

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

//TODO: riscrivere lo shader rimuovendo la necessità per il selector
struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 selector;
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec4 lightColor;
    alignas(16) glm::vec3 eyePos;
};

struct VertexMesh {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 norm;
    alignas(8) glm::vec2 UV;
};

// MAIN ! 
class Dealership : public BaseProject {
	protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	//DescriptorSetLayout DSLGubo;
	DescriptorSetLayout DSLMesh, DSLShow;

	// Vertex formats
	VertexDescriptor VMesh, VShow;

	// Pipelines [Shader couples]
	Pipeline PMesh, PShow;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model<VertexMesh> MEnv, MShow;
    //TODO: define car models array

	//DescriptorSet DSGubo;
	DescriptorSet DSEnv, DSShow;

	Texture TEnv, TShow;
	
	// C++ storage for uniform variables
    UniformBufferObject uboEnv, uboShow;
    GlobalUniformBufferObject gubo;

    // Other application parameters
    int currCarModel = 0;
    int NumCars = 0; //TODO: set to car models array length
    glm::mat4 ViewPrj;
    glm::vec3 Pos = glm::vec3(6.0f,1.0f,10.0f); // Initial spawn location
    glm::vec3 PrevPos = Pos;
    glm::vec3 CarSpawnPos = glm::vec3(6.0f,0.0f,6.0f);
    glm::vec3 cameraPos;
    // Initial camera angles
    float Yaw = glm::radians(0.0f);
    //float PrevYaw = Yaw;
    float Pitch = glm::radians(0.0f); //22.5f
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
		uniformBlocksInPool = 9;
		texturesInPool = 9;
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

        DSLShow.init(this, {
                // this array contains the binding:
                // first  element : the binding number
                // second element : the type of element (buffer or texture)
                // third  element : the pipeline stage where it will be used
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
                {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
        });

		// Vertex descriptors
		VMesh.init(this, {
				  {0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
				         sizeof(glm::vec2), UV}
				});

        VShow.init(this, {
                {0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
                           {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
                                   sizeof(glm::vec3), POSITION},
                           {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
                                   sizeof(glm::vec3), NORMAL},
                           {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
                                   sizeof(glm::vec2), UV}
                   });

        // Pipelines [Shader couples]
		PMesh.init(this, &VMesh, "shaders/BlinnVert.spv", "shaders/BlinnFrag.spv", {&DSLMesh});
		PMesh.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

        PShow.init(this, &VShow, "shaders/BlinnVert.spv", "shaders/BlinnFrag.spv", {&DSLShow});
        PShow.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		createEnvMesh(MEnv.vertices, MEnv.indices);
		MEnv.initMesh(this, &VMesh);

        createShowcaseMesh(MShow.vertices, MShow.indices);
        MShow.initMesh(this, &VShow);

		// Create the textures
		// The second parameter is the file name
		TEnv.init(this, "textures/TBs_20140623_1_02.png");
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() override {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
        PShow.create();

		// Here you define the data set
		DSEnv.init(this, &DSLMesh, {
				{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
				{1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
				{2, TEXTURE, 0, &TEnv}
		});

        DSShow.init(this, &DSLShow, {
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                {2, TEXTURE, 0, &TEnv}
        });

		/*DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
				});*/
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() override {
		// Cleanup pipelines
		PMesh.cleanup();
        PShow.cleanup();

		DSEnv.cleanup();
        DSShow.cleanup();
		//DSGubo.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() override {
		// Cleanup textures
		TEnv.cleanup();
        TShow.cleanup();
		
		// Cleanup models
		MEnv.cleanup();
        MShow.cleanup();
		
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
        DSLShow.cleanup();
		//DSLGubo.cleanup();
		
		// Destroys the pipelines
		PMesh.destroy();
        PShow.destroy();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) override {
		//TODO: add currCarModel switch to switch between car models

		// binds the pipeline
		PMesh.bind(commandBuffer);
		// For a pipeline object, this command binds the corresponding pipeline to the command buffer passed in its parameter

		// For a Model object, this command binds the corresponding index and vertex buffer
		// to the command buffer passed in its parameter
		MEnv.bind(commandBuffer);
		DSEnv.bind(commandBuffer, PMesh, 0, currentImage); //TODO: figure how what the setID does
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MEnv.indices.size()), 1, 0, 0, 0);

        PShow.bind(commandBuffer);
        MShow.bind(commandBuffer);
        DSShow.bind(commandBuffer, PShow, 0, currentImage); //TODO: figure how what the setID does
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MShow.indices.size()), 1, 0, 0, 0);
    }

    void GameLogic() {
        // Parameters
        // Camera FOV-y, Near Plane and Far Plane
        const float FOVy = glm::radians(60.0f);
        const float nearPlane = 0.1f;
        const float farPlane = 100.f;
        // Camera target height and distance
        const float camHeight = 1.25;
        const float camDist = 1; /*1.5;*/
        // Camera Pitch limits
        const float minPitch = glm::radians(-60.0f);
        const float maxPitch = glm::radians(60.0f);
        const float minRoll = glm::radians(-30.0f);
        const float maxRoll = glm::radians(30.0f);
        // Rotation and motion speed
        const float ROT_SPEED = glm::radians(120.0f);
        // Car showcase radius
        const float RADIUS = 3.0f;

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
        Pos.y = 0.0f;
        Pos = Pos + MOVE_SPEED * m.z * uz * deltaT;
        if (Pos.z < 1.0f) Pos.z = 1.0f;
        else if (Pos.z > 11.0f) Pos.z = 11.0f; // was 12.0f

        // Collision check with the car showcase
        if (glm::distance(CarSpawnPos, Pos) < RADIUS) Pos = PrevPos;

        //if (Pos != PrevPos) std::cout << "Pos = (" << Pos.x << ", " << Pos.y << ", " << Pos.z << ")" << std::endl;
        PrevPos = Pos;

        // Rotation
        Yaw = Yaw - ROT_SPEED * deltaT * r.y;
        /*if (Yaw != PrevYaw) std::cout << "Yaw = " << Yaw << std::endl;
        PrevYaw = Yaw;*/

        /*if (Pos.x < 1.0f) {
            if ((Yaw < 0.0f && Yaw > -HalfM_PI) || Yaw > M_PI+HalfM_PI) Yaw = 0.0f;
            else if (Yaw > M_PI || Yaw < -HalfM_PI) Yaw = M_PI;
        }
        if (Pos.x > 11.0f){
            if ((Yaw > 0.0f && Yaw < HalfM_PI) || Yaw < -M_PI-HalfM_PI) Yaw = 0.0f;
            else if (Yaw < -M_PI || Yaw > HalfM_PI) Yaw = -M_PI;
        }
        if (Pos.z < 1.0f) {
            if ((Yaw < -HalfM_PI && Yaw > -M_PI) || Yaw > M_PI) Yaw = -HalfM_PI;
            else if (Yaw > HalfM_PI || Yaw < -M_PI) Yaw = HalfM_PI;
        }
        if (Pos.z > 11.0f) {
            if (Yaw < 0.0f && Yaw > -HalfM_PI) Yaw = -HalfM_PI;
            else if (Yaw > 0.0f && Yaw < HalfM_PI) Yaw = HalfM_PI;
        }*/

        Pitch = Pitch + ROT_SPEED * deltaT * r.x;
        Pitch  =  Pitch < minPitch ? minPitch :
                  (Pitch > maxPitch ? maxPitch : Pitch);
        Roll   = Roll - ROT_SPEED * deltaT * r.z;
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

        static bool debounce = false;
        static int curDebounce = 0;
        // Switch currCarModel on specific key press
        /*if(glfwGetKey(window, GLFW_KEY_N)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_SPACE;
                currCarModel = (currCarModel+1) % NumCars;
                std::cout << "Scene : " << currCarModel << "\n";
                RebuildPipeline();
            }
        } else {
            if((curDebounce == GLFW_KEY_SPACE) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_P)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_SPACE;
                currCarModel = (currCarModel-1) % NumCars;
                std::cout << "Scene : " << currCarModel << "\n";
                RebuildPipeline();
            }
        } else {
            if((curDebounce == GLFW_KEY_SPACE) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }*/

        //TODO: riscrivere lo shader rimuovendo la necessità per questo codice
        static bool showNormal = false;
        static bool showUV = false;

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

        GameLogic();

        //TODO: riscrivere lo shader rimuovendo la necessità del selector
        gubo.selector = glm::vec3(showNormal || showUV ? 0 : 1, showNormal ? 1 : 0, showUV ? 1 : 0);
		gubo.lightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = cameraPos;

		uboEnv.mMat = glm::scale(glm::mat4(1), glm::vec3(3));
		uboEnv.mvpMat = ViewPrj * uboEnv.mMat;
		uboEnv.nMat = glm::inverse(glm::transpose(uboEnv.mMat));

        uboShow.mMat = glm::rotate(glm::translate(glm::mat4(1.0f),
                                   glm::vec3(6.0f, 0.0f, 6.0f)),ShowRot,
                                   glm::vec3(0,1,0));
        uboShow.mvpMat = ViewPrj * uboShow.mMat;
        uboShow.nMat = glm::inverse(glm::transpose(uboShow.mMat));

        // Mapping of the room
		DSEnv.map((int)currentImage, &uboEnv, sizeof(uboEnv), 0);
        DSEnv.map((int)currentImage, &gubo, sizeof(gubo), 1);
        // Mapping of the Showcase platform
        DSShow.map((int)currentImage, &uboShow, sizeof(uboShow), 0);
        DSShow.map((int)currentImage, &gubo, sizeof(gubo), 1);

        //TODO: add switch between different car models
        // the room needs to stay the same!
        // Maybe through an array of DSs we can retrieve the correct model right await
        // substituting the switch.
        /*switch(currCarModel) {
            case 0:
                DS1.map(currentImage, &ubo, sizeof(ubo), 0);
                DS1.map(currentImage, &gubo, sizeof(gubo), 1);
                break;
            case 1:
                DS2.map(currentImage, &ubo, sizeof(ubo), 0);
                DS2.map(currentImage, &gubo, sizeof(gubo), 1);
                break;
        }*/
	}

	static void createEnvMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
    static void createShowcaseMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
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