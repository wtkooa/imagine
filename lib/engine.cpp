#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.

#include <iostream>
#include <map>
#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "ie_const.h"
#include "ie_camera.h"
#include "ie_wavefront.h"
#include "ie_time.h"

Engine::Engine(void)
{
  WINDOW_TITLE = "Imagine Engine";
  WINDOW_WIDTH = 800;
  WINDOW_HEIGHT = 600;
  ASPECT_RATIO = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
  FIELD_OF_VIEW = glm::radians(60.0f);
  Z_NEAR = 0.01;
  Z_FAR = 30.0;
  REQUIRED_SDL_MODULES = SDL_INIT_EVERYTHING;
  SDL_MODE = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL;
  ACTIVEBUFFERS = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
  WIREFRAME = false;
  DEPTHTEST = true;
  CULLFACE = true;
  vertexShaderFile = "lib/glsl/vshader.glsl";
  fragmentShaderFile = "lib/glsl/fshader.glsl";
  DEFAULT_CLEAR_COLOR = glm::vec4(0.0, 0.0, 0.0, 1.0);
  eye.setMoveSpeed(3.0f); //Meters per Second
  eye.setLookSpeed(glm::radians(0.05f)); //Degrees per rel movment
  init();
  run();
  cleanup();
}

bool Engine::cleanup(void)
{
  //rm.releaseMem();
  //vram.releaseMem();
  //compiler.cleanUpShaders();
  SDL_Quit();
  return true;
}

bool Engine::init(void)
{
  SDL_Init(REQUIRED_SDL_MODULES);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  p_window = SDL_CreateWindow(WINDOW_TITLE.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                WINDOW_WIDTH, WINDOW_HEIGHT,
                SDL_MODE);
  glContext = SDL_GL_CreateContext(p_window);
  SDL_GL_SetSwapInterval(1);
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  if (WIREFRAME) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
  if (DEPTHTEST) {glEnable(GL_DEPTH_TEST);}
  if (CULLFACE) {glEnable(GL_CULL_FACE);}
  glClearColor(DEFAULT_CLEAR_COLOR.r,
         DEFAULT_CLEAR_COLOR.g,
         DEFAULT_CLEAR_COLOR.b,
         DEFAULT_CLEAR_COLOR.a);
  glClear(ACTIVEBUFFERS);
  eye.setProjectionMatrix(glm::perspective(FIELD_OF_VIEW, ASPECT_RATIO, Z_NEAR, Z_FAR));
  SDL_SetWindowGrab(p_window, SDL_TRUE);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  initLighting();
  initShaders();
  loadAssets();
  return true;
}

bool Engine::initLighting(void)
{
  light.setPosVector(glm::vec3(0.0f, 3.0f, 0.0f));
  light.setGlobalAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
  light.setLightAmbient(glm::vec3(0.2f, 0.2f, 0.2f));
  light.setLightSpecular(glm::vec3(0.4f, 0.4f, 0.4f));
  light.setLightDiffuse(glm::vec3(0.8f, 0.8f, 0.8f));
  light.setConstantFalloff(1.0f);
  light.setLinearFalloff(0.1f);
  light.setQuadraticFalloff(0.0f);
}

bool Engine::initShaders(void)
{
  compiler.compileVertexShader(vertexShaderFile);
  compiler.compileFragmentShader(fragmentShaderFile);
  compiler.linkShaderProgram();
  programID = compiler.getProgramID();
  glUseProgram(programID);

  transformationMatShaderUniLoc = glGetUniformLocation(programID, "transformationMatrix");
  mtwMatShaderUniLoc = glGetUniformLocation(programID, "mtwMatrix");
  light0posShaderUniLoc = glGetUniformLocation(programID, "pointLightPos");
  eyePosShaderUniLoc = glGetUniformLocation(programID, "cameraPos");
  ambientLightShaderUniLoc = glGetUniformLocation(programID, "lightAmbient");
  ambientGlobalShaderUniLoc = glGetUniformLocation(programID, "globalAmbient");
  specularLightShaderUniLoc = glGetUniformLocation(programID, "lightSpecular");
  diffuseLightShaderUniLoc = glGetUniformLocation(programID, "lightDiffuse");
  KsShaderUniLoc = glGetUniformLocation(programID, "materialSpecular");
  NsShaderUniLoc = glGetUniformLocation(programID, "materialShininess");
  KdShaderUniLoc = glGetUniformLocation(programID, "materialDiffuse");
  KaShaderUniLoc = glGetUniformLocation(programID, "materialAmbient");
  KeShaderUniLoc = glGetUniformLocation(programID, "materialEmission");
  KcShaderUniLoc = glGetUniformLocation(programID, "lightConstantFalloff");
  KlShaderUniLoc = glGetUniformLocation(programID, "lightLinearFalloff");
  KqShaderUniLoc = glGetUniformLocation(programID, "lightQuadraticFalloff");
  textureShaderUniLoc = glGetUniformLocation(programID, "textureID");
  hasTextureShaderUniLoc = glGetUniformLocation(programID, "usingTexture");

  glm::vec3 globalAmbient = light.getGlobalAmbient();
  glUniform3fv(ambientGlobalShaderUniLoc, 1, &globalAmbient[0]); 
  glm::vec3 lightPosVec = light.getPosVector();
  glUniform3fv(light0posShaderUniLoc, 1, &lightPosVec[0]);
  glm::vec3 lightAmbient = light.getLightAmbient();
  glUniform3fv(ambientLightShaderUniLoc, 1, &lightAmbient[0]);
  glm::vec3 lightSpecular = light.getLightSpecular();
  glUniform3fv(specularLightShaderUniLoc, 1, &lightSpecular[0]);
  glm::vec3 lightDiffuse = light.getLightDiffuse();
  glUniform3fv(diffuseLightShaderUniLoc, 1, &lightDiffuse[0]);
  glUniform1f(KcShaderUniLoc, light.getConstantFalloff());
  glUniform1f(KlShaderUniLoc, light.getLinearFalloff());
  glUniform1f(KqShaderUniLoc, light.getQuadraticFalloff()); 
  glUniform1i(textureShaderUniLoc, ie::DIFFUSE_TEXTURE_BINDING_POINT);
  glActiveTexture(GL_TEXTURE0);
  return true;
}

bool Engine::loadAssets(void)
{
  //vram.genVBO(rm.modelArr, rm.modelAmount);
  return true;
}

bool Engine::run(void)
{
  engine_on = true;
  while (engine_on) {
    frameClock.measure();
    handleEvents();
    handleLogic();
    render();
  } 
  return true;
}

void Engine::handleLogic(void)
{
  //modelResource* model = rm.modelArr;
  //std::map<std::string, int> modelMap = rm.modelMap;
  //int Cube = modelMap["Cube"];
  //model[Cube].translationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
  //model[Cube].rotationMatrix *= glm::rotate(glm::mat4(), glm::radians(0.5f), glm::vec3(1.0f, 1.0f, 0.0f));
}

/*void Engine::render(void)
{
  modelResource* model = rm.modelArr;
  int modelAmount = rm.modelAmount;
  if (SDL_GetWindowGrab(p_window) == SDL_TRUE)
  {
    eye.frameUpdate(frameClock.getFrameDelta());
  }
  glClear(ACTIVEBUFFERS);
  glBindBuffer(GL_ARRAY_BUFFER, vram.vboID);
   
  glm::vec3 pos = eye.getPosVector();
  glUniform3fv(eyePosShaderUniLoc, 1, &pos[0]);
  for (int n = 0; n < modelAmount; n++)
  {
    glm::mat4 mtwMatrix = model[n].translationMatrix *
                model[n].rotationMatrix;
    glm::mat4 transformationMatrix = eye.getProjectionMatrix() *
                     eye.getViewMatrix() *
                     mtwMatrix;
    glUniformMatrix4fv(mtwMatShaderUniLoc, 1, GL_FALSE, &mtwMatrix[0][0]);
    glUniformMatrix4fv(transformationMatShaderUniLoc, 1, GL_FALSE, &transformationMatrix[0][0]);
    if (model[n].vboloaded == true && model[n].hidden == false)
    {
      for (int j = 0; j < model[n].faceGroupAmount; j++)
      {
        GLuint textureID = model[n].materialArr[model[n].groupArr[j].mtlID].textureID;
        if (textureID != 0)
        {
          glBindTexture(GL_TEXTURE_2D, textureID);
          glUniform1i(hasTextureShaderUniLoc, 1);
        }
        else
        {
          glUniform1i(hasTextureShaderUniLoc, 0);
        }
        glm::vec3 Ks = model[n].materialArr[model[n].groupArr[j].mtlID].Ks;
        glUniform3fv(KsShaderUniLoc, 1, &Ks[0]);
        float Ns = model[n].materialArr[model[n].groupArr[j].mtlID].Ns;
        glUniform1f(NsShaderUniLoc, Ns);
        glm::vec3 Kd = model[n].materialArr[model[n].groupArr[j].mtlID].Kd;
        glUniform3fv(KdShaderUniLoc, 1, &Kd[0]);
        glm::vec3 Ka = model[n].materialArr[model[n].groupArr[j].mtlID].Ka;
        glUniform3fv(KaShaderUniLoc, 1, &Ka[0]);
        glm::vec3 Ke = model[n].materialArr[model[n].groupArr[j].mtlID].Ke;
        glUniform3fv(KeShaderUniLoc, 1, &Ke[0]);
        glDrawArrays(GL_TRIANGLES,
               model[n].vboOffsetIndex + model[n].groupArr[j].offsetIndex,
               model[n].groupArr[j].vertexAmount);
        if (textureID != 0) {glBindTexture(GL_TEXTURE_2D, 0);}
      }
    }
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  SDL_GL_SwapWindow(p_window);
}*/

void Engine::handleEvents(void)
{
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    switch (evnt.type)
    {
      case SDL_QUIT:
        engine_on = false;
        break;
      case SDL_WINDOWEVENT:
        switch (evnt.window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
          handleResize(evnt.window.data1, evnt.window.data2);
          break;
        }
        break;
      case SDL_MOUSEMOTION:
        if (SDL_GetRelativeMouseMode() == SDL_TRUE)
        {
          eye.rotateEventVec.x += evnt.motion.xrel;  
          eye.rotateEventVec.y += evnt.motion.yrel;  
        }
        break;
      case SDL_KEYDOWN:
        switch (evnt.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            engine_on = false;
            break;
          case SDLK_e:
            if (SDL_GetWindowGrab(p_window) == SDL_FALSE) 
            {
              std::cout << "GrabMode On" << std::endl;
              SDL_SetWindowGrab(p_window, SDL_TRUE);
              SDL_SetRelativeMouseMode(SDL_TRUE);
            }
            else
            {
              std::cout << "GrabMode Off" << std::endl;
              SDL_SetWindowGrab(p_window, SDL_FALSE);
              SDL_SetRelativeMouseMode(SDL_FALSE);
            } 
            break;
          case SDLK_w:
            if (!evnt.key.repeat)
            { 
              eye.translEventVec.z += 1.0;
            }
            break;
          case SDLK_s:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.z -= 1.0;
            }
            break;
          case SDLK_d:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.x += 1.0;
            }
            break;
          case SDLK_a:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.x -= 1.0;
            }
            break;
          case SDLK_SPACE:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.y += 1.0;
            }
            break;
          case SDLK_LSHIFT:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.y -= 1.0;
            }
            break;
        }
        break;        
      case SDL_KEYUP:
        switch(evnt.key.keysym.sym)
        {
          case SDLK_w:
              eye.translEventVec.z -= 1.0;
            break;
          case SDLK_s:
              eye.translEventVec.z += 1.0;
            break;
          case SDLK_d:
              eye.translEventVec.x -= 1.0;
            break;
          case SDLK_a:
              eye.translEventVec.x += 1.0;
            break;
          case SDLK_SPACE:
              eye.translEventVec.y -= 1.0;
            break;
          case SDLK_LSHIFT:
              eye.translEventVec.y += 1.0;
            break;
        }
        break;
    }
  }
}

void Engine::handleResize(int width, int height)
{
  float aspect_ratio = float(width) / float(height);
  SDL_SetWindowSize(p_window, width, height);
  glViewport(0, 0, width, height);
  glm::mat4 newProjection = glm::perspective(FIELD_OF_VIEW,
                                             aspect_ratio,
                                             Z_NEAR, Z_FAR);
  eye.setProjectionMatrix(newProjection);  
}
