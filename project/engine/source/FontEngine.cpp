#include "FontEngine.h"

#include "debug_macros.h"
#include <cstdlib>
#include <iostream>

#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"

#include "director.hpp"
#include "scene.hpp"
#include "camera.hpp"

const char *FontEngine::VERTEX_SHADER_FILE = "flood-fill/shaders/vertex-text.glsl";
const char *FontEngine::FRAGMENT_SHADER_FILE = "flood-fill/shaders/fragment-text.glsl";

const char *FontEngine::VERTEX_SHADER_FILE_BILLBOARD = "flood-fill/shaders/vertex-billboardtext.glsl";
const char *FontEngine::FRAGMENT_SHADER_FILE_BILLBOARD = "flood-fill/shaders/fragment-billboardtext.glsl";


FontEngine::FontEngine() {
    initialized = 0;
    curAtlas = 0;
    currentHandle = "";
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 0.0;
    color[3] = 1.0;
    zPos = 0.0;
}

FontEngine::~FontEngine() {
    // clean up fonts and atlas's
    for (auto handle_font : fonts) {
        for (auto size_atlas : handle_font.second->sizes) {
            delete size_atlas.second;
            size_atlas.second = 0;
        }
        FT_Done_Face(handle_font.second->face);
        delete handle_font.second;
        handle_font.second = 0;
    }

    if (initialized) {
        // clean up the library
        FT_Done_FreeType(library);
    }

    // clean up shader
    glDeleteProgram(program);
}

bool FontEngine::init() {
    if (FT_Init_FreeType(&library)) {
        INFO("FT_Init_FreeType");
        return 0;
    }

    program = create_program(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
    if (!program) {
        INFO("Didn't Create Program");
        return 0;
    }

    coord = get_attrib(program, "coord");
    uniform_tex = get_uniform(program, "tex");
    uniform_color = get_uniform(program, "color");
    if (coord < 0 || uniform_tex < 0 || uniform_color < 0) {
        INFO("attribs not properly allocated");
        return 0;
    }

    billboard_program = create_program(VERTEX_SHADER_FILE_BILLBOARD, FRAGMENT_SHADER_FILE_BILLBOARD);
    if (!billboard_program) {
        INFO("Didn't Create BillBoard Program");
        return 0;
    }    

    billboard_pos = get_attrib(billboard_program, "position");
    billboard_coord = get_attrib(billboard_program, "texcoord");
    billboard_uniform_tex = get_uniform(billboard_program, "tex");
    billboard_uniform_color = get_uniform(billboard_program, "color");
    billboard_uniform_projection = get_uniform(billboard_program, "uProjection");
    billboard_uniform_view = get_uniform(billboard_program, "uView");
    
    
    if (billboard_pos < 0 || billboard_coord < 0|| billboard_uniform_tex < 0|| billboard_uniform_color< 0 || billboard_uniform_projection < 0 || billboard_uniform_view < 0) {
        INFO("billboard attribs not properly allocated");
        return 0;
        }

    glGenBuffers(1, &vbo);

    initialized = 1;
    return 1;
}

bool FontEngine::addFont(std::string handle, std::string fontFile) {
    if (!initialized) {
        fprintf(stderr, "Font engine error: addFont: engine uninitalized");
        return 0;
    }
    if (fonts.find(handle) != fonts.end()) {
        // trying to add a handle that already exists
        fprintf(stderr, "Font engine warning: addFont: handle %s already exists",
                handle.c_str());

        if (fonts[handle]->fontFile != fontFile) {
            // the handle that exists is not coupled with the requested fontFile
            fprintf(stderr, "Font engine warning: addFont: existing handle %s not \
                    bound to requested font %s", handle.c_str(), fontFile.c_str());
            return 0;   // don't add anything or change existing handle
        }
        return 1;
    }

    Font *newFont = new Font;
    if (FT_New_Face(library, fontFile.c_str(), 0, &(newFont->face))) {
        delete newFont;
        fprintf(stderr, "Font engine error: addFont: could not open font %s",
                fontFile.c_str());
        return 0;
    }
    newFont->fontFile = fontFile;
    fonts[handle] = newFont;
    return 1;
}

bool FontEngine::useFont(std::string handle, int size) {
    if (!initialized) {
        fprintf(stderr, "Font engine error: useFont: engine uninitialized");
        return 0;
    }
    if (fonts.find(handle) == fonts.end()) {
        fprintf(stderr, "Font engine error: useFont: handle %s does not exist",
                 handle.c_str());
        return 0;
    }

    if (fonts[handle]->sizes.find(size) == fonts[handle]->sizes.end()) {
        // new size for this font
        fonts[handle]->sizes[size] = new FontAtlas(fonts[handle]->face, size);
    }
    curAtlas = fonts[handle]->sizes[size];
    currentHandle = handle;
    currentSize = size;
    return 1;
}

std::string FontEngine::getCurrentHandle() {
    return currentHandle;
}

int FontEngine::getCurrentSize() {
    return currentSize;
}

void FontEngine::setColor(float r, float g, float b, float a) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
}

void FontEngine::setZ(float z) {
    zPos = z;
}

void FontEngine::renderText(std::string text, float x, float y) {
    if (!initialized) {
        // commented so you dont get spammed for making a mistake
        // uncomment for more debugging
        fprintf(stderr, "Font engine error: renderText: engine uninitialized");
        return;
    }
    if (!curAtlas) {
        // commented so you dont get spammed for making a mistake
        // uncomment if want more debugging
        fprintf(stderr, "Font engine error: renderText: no current atlas");
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(program);
    float sx = 1.0, sy = 1.0;
    adjustScaling(sx, sy);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, curAtlas->getAtlasTexture());
    glUniform1i(uniform_tex, 0);
    glUniform4fv(uniform_color, 1, color);


    // rendering characters on 2 triangles
    const int VERT_COUNT = 6;

    float xCur = x;
    float yCur = y;
    unsigned int c = 0;
    std::vector<Point> coords(VERT_COUNT * text.length());
    for (unsigned int i = 0; i < text.length(); i++) {
        if (text[i] == '\n') {
            xCur = x;
            yCur = yCur - getLineHeight();
            continue;
        }
        FontAtlas::CharInfo charInfo = curAtlas->getCharInfo(text[i]);

        // calculate vertex and texture coordinates
        float x2 = xCur + charInfo.bitLeft * sx;
        float y2 = -yCur - charInfo.bitTop * sy;
        float w = charInfo.bitWidth * sx;
        float h = charInfo.bitHeight * sy;

        // advance cursor to start of next character
        xCur += charInfo.advanceX * sx;
        yCur += charInfo.advanceY * sy;

        if (!w || !h) {
            // skip glyphs with no pixels
            continue;
        }

        // calculate vertices
        for (int i = 0; i < VERT_COUNT; i++) {
            if (i % 2 == 0) {
                coords[c + i].x = x2;
                coords[c + i].s = charInfo.tex_offX;
            } else {
                coords[c + i].x = x2 + w;
                coords[c + i].s = charInfo.tex_offX +
                                  charInfo.bitWidth / curAtlas->getAtlasWidth();
            }

            if (i < 2 || i == 3) {
                coords[c + i].y = -y2;
                coords[c + i].t = charInfo.tex_offY;
            } else {
                coords[c + i].y = -y2 - h;
                coords[c + i].t = charInfo.tex_offY +
                                  charInfo.bitHeight / curAtlas->getAtlasHeight();
            }
        }
        c += VERT_COUNT;
    }

    glEnableVertexAttribArray(coord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
    

    glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(Point), &coords[0], GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, c);

    glDisableVertexAttribArray(coord);
  }

void FontEngine::renderBillBoardText(std::string text, float x, float y, float z, glm::vec3 playerPos) {
    if (!initialized) {
        // commented so you dont get spammed for making a mistake
        // uncomment for more debugging
        fprintf(stderr, "Font engine error: renderText: engine uninitialized");
        return;
    }
    if (!curAtlas) {
        // commented so you dont get spammed for making a mistake
        // uncomment if want more debugging
        fprintf(stderr, "Font engine error: renderText: no current atlas");
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(billboard_program);
    float sx = 1.0, sy = 1.0;
    adjustScaling(sx, sy);

    CameraPtr cam = Director::getScene()->getCamera();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, curAtlas->getAtlasTexture());
    glUniform1i(billboard_uniform_tex, 0);
    glUniform4fv(billboard_uniform_color, 1, color);
    glUniformMatrix4fv(billboard_uniform_projection, 1 , GL_FALSE, glm::value_ptr(cam->getProjectionMatrix()));
    glUniformMatrix4fv(billboard_uniform_view, 1 , GL_FALSE, glm::value_ptr(cam->getViewMatrix()));
      

    // Obtain cross product to determine how to billboard text
    glm::vec3 x_product = glm::cross(glm::vec3(0,1,0), playerPos - glm::vec3(x,y,z));
    x_product = glm::normalize(glm::vec3(x_product.x, 0, x_product.z));

    // rendering characters on 2 triangles
    const int VERT_COUNT = 6;

    float xCur = x;
    float yCur = y;
    float zCur = z;
    unsigned int c = 0;
	std::vector<WorldPoint> b_positions(VERT_COUNT * text.length());
    std::vector<TextCoord> b_coords(VERT_COUNT * text.length());
    for (unsigned int i = 0; i < text.length(); i++) {
        if (text[i] == '\n') {
            xCur = x;
            yCur = yCur - getLineHeight();
            continue;
        }
        FontAtlas::CharInfo charInfo = curAtlas->getCharInfo(text[i]);

        // calculate vertex and texture coordinates
        //float x2 = xCur + charInfo.bitLeft * sx;
        float x2 = xCur + charInfo.bitLeft * sx * x_product.x;
        float y2 = -yCur - charInfo.bitTop * sy;
        float z2 = zCur + charInfo.bitLeft * sx * x_product.z;
        float w = charInfo.bitWidth * sx;
        float h = charInfo.bitHeight * sy;

        // advance cursor to start of next character
        xCur += charInfo.advanceX * sx * x_product.x;
        yCur += charInfo.advanceY * sy;
        zCur += charInfo.advanceX * sy * x_product.z;
        
        if (!w || !h) {
            // skip glyphs with no pixels
            continue;
        }

        // 01 3
        // 2 45
        // calculate vertices
        for (int i = 0; i < VERT_COUNT; i++) {
            if (i % 2 == 0) {
                b_positions[c + i].x = x2;
                b_positions[c + i].z = z2;
                //b_positions[c + i].z = -10;
                b_coords[c + i].x = charInfo.tex_offX;
            } else {
                b_positions[c + i].x = x2 + w * x_product.x;
                b_positions[c + i].z = z2 + w * x_product.z;
                //b_positions[c + i].z = -10;
                b_coords[c + i].x = charInfo.tex_offX +
                                  charInfo.bitWidth / curAtlas->getAtlasWidth();
            }

            if (i < 2 || i == 3) {
                b_positions[c + i].y = -y2;
                b_coords[c + i].y = charInfo.tex_offY;
            } else {
                b_positions[c + i].y = -y2 - h;
                b_coords[c + i].y = charInfo.tex_offY +
                                  charInfo.bitHeight / curAtlas->getAtlasHeight();
            }
        }
        c += VERT_COUNT;
    }
    
    // Vertex Positions
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(billboard_pos);
    glVertexAttribPointer(billboard_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);  
    glBufferData(GL_ARRAY_BUFFER, b_positions.size() * sizeof(WorldPoint), &b_positions[0], GL_DYNAMIC_DRAW);
    
    // Texture Coords
    glEnableVertexAttribArray(billboard_coord);
    glVertexAttribPointer(billboard_coord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, b_coords.size() * sizeof(TextCoord), &b_coords[0], GL_DYNAMIC_DRAW);
    
    // Draw
    glDrawArrays(GL_TRIANGLES, 0, c);
    
    // Disable
    glDisableVertexAttribArray(billboard_pos);
    glDisableVertexAttribArray(billboard_coord);
}


void FontEngine::renderTextWrapped(std::string text, float x, float y, float width) {
    if (!curAtlas) {
        fprintf(stderr, "Font engine error: renderTextWrapped: no current atlas");
        return;
    }
    float sx = 1.0, sy = 1.0;
    adjustScaling(sx, sy);

    float curLineWidth = 0;
    unsigned int lastSpace = 0;
    for (unsigned int i = 0; i < text.length(); i++) {
        if (text[i] == '\n') {
            // respect already existing newlines
            curLineWidth = 0;
            continue;
        }
        if (text[i] == ' ') {
            // keep track of last space found
            // (it is a candidate to be replaced with next needed newline)
            lastSpace = i;
        }
        FontAtlas::CharInfo charInfo = curAtlas->getCharInfo(text[i]);
        curLineWidth += charInfo.advanceX * sx;
        if (curLineWidth >= width) {
            // need to enter a line break into the string
            if (!lastSpace) {
                // no space found in string yet
                // go forward in string until find next space or the end
                while (i < text.length() && text[i] != ' ') {
                    ++i;
                }
                if (i < text.length()) {
                    // found a space, replace it with newline
                    text[i] = '\n';
                    curLineWidth = 0;
                } // else no space anywhere, nothing to do
            } else {
                // replace the last space found with a newline
                text[lastSpace] = '\n';
                curLineWidth = 0;

                // go back to where the last space was found and start
                // measuring next line's width from there
                i = lastSpace;
            }
        }
    }

    // render the wrapped string
    renderText(text, x, y);
}

float FontEngine::getMaxCharHeight() {
    if (!curAtlas) {
        fprintf(stderr, "Font engine error: getMaxCharHeight: no current atlas");
        return 0.0;
    }
    float sx = 1.0, sy = 1.0;
    adjustScaling(sx, sy);
    return curAtlas->getMaxCharHeight() * sy;
}

float FontEngine::getLineHeight() {
    if (!curAtlas) {
        fprintf(stderr, "Font engine error: getLineHeight: no current atlas");
        return 0.0;
    }
    float sx = 1.0, sy = 1.0;
    adjustScaling(sx, sy);
    return curAtlas->getLineHeight() * sy;
}

void FontEngine::addLineHeight(float val) {
    curAtlas->addLineHeight(val);
}

float FontEngine::getTextWidth(std::string text) {
    if (!curAtlas) {
        fprintf(stderr, "Font engine error: getTextWidth: no current atlas");
        return 0.0;
    }
    float sx = 1.0, sy = 1.0;
    adjustScaling(sx, sy);

    float maxLineWidth = 0;
    float curLineWidth = 0;
    for (unsigned int i = 0; i < text.length(); i++) {
        if (text[i] == '\n') {
            maxLineWidth = std::max(maxLineWidth, curLineWidth);
            curLineWidth = 0;
            continue;
        }
        FontAtlas::CharInfo charInfo = curAtlas->getCharInfo(text[i]);
        curLineWidth += charInfo.advanceX * sx;
    }
    return std::max(maxLineWidth, curLineWidth);
}

float FontEngine::getTextHeight(std::string text) {
    if (!curAtlas) {
        fprintf(stderr, "Font engine error: getTextHeight: no current atlas");
        return 0.0;
    }
    float sx = 1.0, sy = 1.0;
    adjustScaling(sx, sy);

    float maxCharHeight = 0;
    int newLineHeight = 0;
    for (unsigned int i = 0; i < text.length(); i++) {
        if (text[i] == '\n') {
            newLineHeight += (int)getLineHeight();
            continue;
        }
        float h = curAtlas->getCharInfo(text[i]).bitHeight * sy;
        maxCharHeight = std::max(maxCharHeight, h);
    }
    return maxCharHeight + newLineHeight;
}

void FontEngine::adjustScaling(float &sx, float &sy) {
    //const float baseXRes = 800.0;
    //const float baseYRes = 600.0;

    // this makes it so fonts scale with window
    // (and have same aspect ratio as window)
    //sx = 2.0 / baseXRes;
    //sy = 2.0 / baseYRes;

    // or to make fonts not scale with window:
    sx = 2.0f / windowWidth;
    sy = 2.0f / windowHeight;
}

void FontEngine::updateWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}
