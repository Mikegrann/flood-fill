#ifndef OBJECT_H
#define OBJECT_H

#include "glm/glm.hpp"

#include "mesh.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include "pointer_macros.h"
DEF_PTR(Object, ObjectPtr);

class Object {

public:
    Object();
    Object(MeshPtr _mesh, MaterialPtr _material);

    MeshPtr getMesh();
    MaterialPtr getMaterial();
    MaterialPtr getAlternativeMaterial();
    glm::mat4 getModelMatrix();
    TexturePtr getTexture();
    TexturePtr getTexture(unsigned int i);
    TexturePtr getNormalMap();
    TexturePtr getNormalMap(unsigned int i);
    float getAlpha();
    float getNormalMapScale();
    float getNormalMapBias();
    GLuint getWaterData();
    GLuint getWaterColor();
    GLuint getWaterBlock();
    glm::vec2 getDTime();
    glm::vec3 getVelocity();
    glm::vec2 getGridScale();
    int getShearX();
    int getShearZ();
    std::vector<TexturePtr> getTextures();
    std::vector<TexturePtr> getNormalMaps();
    unsigned int getTexturePack();

    void applyTexture(TexturePtr _texture);
    void applyTextureIndex(TexturePtr _texture, int index);
    void enableTexture();
    void disableTexture();
    bool isTextureEnabled();
    bool hasTexture();

    void applyNormalMap(TexturePtr _texture);
    void applyNormalMapIndex(TexturePtr _texture, int index);
    bool hasNormalMap();

    bool isWater();
    void enableWater();
    void applyWaterData(GLuint id);
    void applyWaterColor(GLuint id);
    void applyWaterBlock(GLuint id);
    void setDTime(glm::vec2 _dTime);
    void setVelocity(glm::vec3 _velocity);
    void setGridScale(glm::vec2 _gridScale);
    void setShear(int x, int z);
    void setTextures(std::vector<TexturePtr> tex);
    void setNormalMaps(std::vector<TexturePtr> norms);
    void setTexturePack(unsigned int texturePck);

    // Sets wich pair or texture and normal map it should use
    void setTextureAndNormalMapPack(unsigned int i);

    void setAlpha(float _alpha);
    void scale(glm::vec3 scalingVector);
    void rotate(float angle, glm::vec3 direction);
    void translate(glm::vec3 translationVector);
    void loadIdentity();
    void setMatrix(glm::mat4 matrix);
    void setMaterial(MaterialPtr _material);
    void setAlternativeMaterial(MaterialPtr _material);

protected:
    MeshPtr mesh;
    MaterialPtr material;
    MaterialPtr alternativeMaterial;

    std::vector<TexturePtr> texture;
    std::vector<TexturePtr> normalMap;

    GLuint waterData;
    GLuint waterColor;
    GLuint waterBlock;

    glm::mat4 modelMatrix;
    bool _hasTexture, _hasNormalMap, textureEnabled;
    bool water;
    float alpha;
    float normalMapScale;
    float normalMapBias;
    glm::vec2 dTime;
    glm::vec3 velocity;
    glm::vec2 gridScale;
    int shearX, shearZ;

    unsigned int texturePack;
};

#endif
