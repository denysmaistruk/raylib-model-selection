#include "extern/raylib/src/external/glad.h"
#include "raylib.h"
#include "rlgl.h"

#define MODELS_NUM 4

Color ToColor(unsigned int modelId) 
{ 
    return *reinterpret_cast<Color*>(&++modelId);
}

unsigned int ToModelId(Color color)
{
    return --(*reinterpret_cast<unsigned int*>(&color));
}

unsigned int GetModelId(int mouseX, int mouseY, const RenderTexture& texture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, texture.id);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.texture.id, 0);

    unsigned char color[4] = { 0 };
    glReadPixels(mouseX, texture.texture.height - mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return ToModelId(Color{ color[0], color[1], color[2], color[3] });
}

int main(int argc, char const** argv)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib model selection");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = Vector3{ 3.0f, 3.0f, 3.0f }; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera mode type

    // Setting up models
    //--------------------------------------------------------------------------------------
    Model models[MODELS_NUM] = { 0 };
    Vector3 positions[MODELS_NUM] = { 0 };

    Image checked = GenImageChecked(2, 2, 1, 1, RED, BLUE);
    Texture2D texture = LoadTextureFromImage(checked);
    UnloadImage(checked);

    // Cube
    models[0] = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    models[0].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    positions[0] = Vector3{ 1, 0, 1 };

    // Sphere
    models[1] = LoadModelFromMesh(GenMeshSphere(0.5f, 10, 20));
    models[1].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    positions[1] = Vector3{ -1, 0, 1 };

    // Cylinder
    models[2] = LoadModelFromMesh(GenMeshCylinder(0.5f, 1, 20));
    models[2].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    positions[2] = Vector3{ 1, 0, -1 };

    // Cone
    models[3] = LoadModelFromMesh(GenMeshCone(0.5f, 1, 20));
    models[3].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    positions[3] = Vector3{ -1, 0, -1 };

    // Shaders and render texture
    //--------------------------------------------------------------------------------------
    Shader shader = LoadShader(0, TextFormat("%s\\shaders\\selection.fs", GetPrevDirectoryPath(GetWorkingDirectory())));
    Shader shaderDefault = LoadMaterialDefault().shader;
    RenderTexture renderTexture = LoadRenderTexture(screenWidth, screenHeight);
    //--------------------------------------------------------------------------------------

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_ORBITAL);          // Update camera

        // Get id for selected model (hovered by mouse)
        unsigned int selected = GetModelId((int)GetMousePosition().x, (int)GetMousePosition().y, renderTexture);
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            // Draw model ids as colors to texture
            //----------------------------------------------------------------------------------
            BeginTextureMode(renderTexture);

            ClearBackground(BLANK);

                BeginMode3D(camera);
                rlDisableColorBlend();

                    for (unsigned int modelId = 0; modelId < MODELS_NUM; ++modelId)
                    {
                        models[modelId].materials[0].shader = shader;
                        DrawModel(models[modelId], positions[modelId], 1, ToColor(modelId));
                    }

                rlEnableColorBlend();
                EndMode3D();

            EndTextureMode();
            //----------------------------------------------------------------------------------

            // Draw scene geometry
            //----------------------------------------------------------------------------------
            BeginMode3D(camera);

            ClearBackground(BLACK);

                DrawGrid(10, 1);

                for (unsigned int modelId = 0; modelId < MODELS_NUM; ++modelId)
                {
                    models[modelId].materials[0].shader = shaderDefault;
                    DrawModel(models[modelId], positions[modelId], 1, selected == modelId ? GREEN : WHITE);        
                }
            
            EndMode3D();
            //----------------------------------------------------------------------------------

            // Draw text
            //----------------------------------------------------------------------------------
            DrawText("Model selection", 10, 10, 20, WHITE);
            DrawText("Hover object by mouse cursor", 10, 40, 20, WHITE);
            DrawText(TextFormat("Hovered model id: %i", selected), 10, 70, 20, YELLOW);

            DrawFPS(710, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    for (int i = 0; i < MODELS_NUM; ++i)
    {
        UnloadModel(models[i]);
    }
    UnloadRenderTexture(renderTexture);
    UnloadShader(shader);
    UnloadShader(shaderDefault);
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}