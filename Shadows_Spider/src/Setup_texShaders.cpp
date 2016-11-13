/*

Copyright 2016 Aleks Berg-Jones

This file is part of Shadows Spider.

Shadows Spider is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Shadows Spider is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Shadows Spider.  If not, see <http://www.gnu.org/licenses/>.

*/

//#include "Core.h"
#include "Setup_texShaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CubemapDirections cubeDirs[6] =
{
	{ glm::vec3(1.f,  0.f,  0.f), glm::vec3(0.f, -1.f,  0.f) },
	{ glm::vec3(-1.f,  0.f,  0.f), glm::vec3(0.f, -1.f,  0.f) },
	{ glm::vec3(0.f,  1.f,  0.f), glm::vec3(0.f,  0.f,  1.f) },
	{ glm::vec3(0.f, -1.f,  0.f), glm::vec3(0.f,  0.f, -1.f) },
	{ glm::vec3(0.f,  0.f,  1.f), glm::vec3(0.f, -1.f,  0.f) },
	{ glm::vec3(0.f,  0.f, -1.f), glm::vec3(0.f, -1.f,  0.f) }
};

void uboInit(Abj &myAbj)
{
	////////////////
	////////////////
	glCreateBuffers(1, &myAbj.uboLight);
	glNamedBufferData(myAbj.uboLight, uboAligned(sizeof(LightData)) * 50, 0, GL_DYNAMIC_DRAW);
	//glNamedBufferData(myAbj.uboLight, uboAligned(sizeof(LightData)), 0, GL_DYNAMIC_DRAW);
	glGetNamedBufferParameterui64vNV(myAbj.uboLight, GL_BUFFER_GPU_ADDRESS_NV, &myAbj.uboLight_64);
	glMakeNamedBufferResidentNV(myAbj.uboLight, GL_READ_ONLY);
	glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, 0, myAbj.uboLight_64, sizeof(LightData));

	////////////////
	////////////////
	glCreateBuffers(1, &myAbj.uboScene);
	glNamedBufferData(myAbj.uboScene, sizeof(SceneData), 0, GL_DYNAMIC_DRAW);
	glGetNamedBufferParameterui64vNV(myAbj.uboScene, GL_BUFFER_GPU_ADDRESS_NV, &myAbj.uboScene_64);
	glMakeNamedBufferResidentNV(myAbj.uboScene, GL_READ_ONLY);
	glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, 1, myAbj.uboScene_64, sizeof(SceneData));

	////////////////
	////////////////
	glCreateBuffers(1, &myAbj.uboDebug);
	glNamedBufferData(myAbj.uboDebug, sizeof(DebugData), 0, GL_DYNAMIC_DRAW);
	//glNamedBufferData(myAbj.uboDebug, uboAligned(sizeof(DebugData)), 0, GL_DYNAMIC_DRAW);
	glGetNamedBufferParameterui64vNV(myAbj.uboDebug, GL_BUFFER_GPU_ADDRESS_NV, &myAbj.uboDebug_64);
	glMakeNamedBufferResidentNV(myAbj.uboDebug, GL_READ_ONLY);
	glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, 2, myAbj.uboDebug_64, sizeof(DebugData));
}

void uboUp(Abj &myAbj)
{
	////////////////
	/* LIGHT */
	///////////////
	auto lightIter = 0;

	for (auto &i : myAbj.allCamLi)
	{
		if (i->v->val_b && i->camLiTypeGet("light"))
		{
			LightData lightD;

			float type;
			if (i->camLiType->val_s == "AREA") type = 0.f;
			else if (i->camLiType->val_s == "DIR") type = 1.f;
			else if (i->camLiType->val_s == "POINT") type = 2.f;
			else if (i->camLiType->val_s == "SPOT") type = 3.f;

			lightD.Cl_type = glm::vec4(i->Cl->val_3, type);
			lightD.falloff = glm::vec4(i->lInten->val_f, cos(glm::radians(i->lSpotI->val_f)), cos(glm::radians(i->lSpotO->val_f)), 0.f);
			lightD.DirRot = i->MM * glm::vec4(0.f, 0.f, -1.f, 0.f);
			lightD.P_WS = glm::vec4(i->t->val_3, 0.f);

			glNamedBufferSubData(myAbj.uboLight, sizeof(lightD) * lightIter, sizeof(LightData), &lightD);

			++lightIter;
		}
	}

	myAbj.UBO_light_needsUp = false;

	////////////////
	/* SCENE */
	///////////////
	SceneData sceneD;
	sceneD.PM = myAbj.selCamLi->PM;
	sceneD.PMinv = glm::inverse(myAbj.selCamLi->PM);
	sceneD.VM = myAbj.selCamLi->VM;
	sceneD.VMinv = glm::inverse(myAbj.selCamLi->VM);
	sceneD.VPM = myAbj.selCamLi->PM * myAbj.selCamLi->VM;
	sceneD.comboU0 = glm::vec4(myAbj.myFSQ->Kgi->val_f, lightIter, 0.f, 0.f); //vec4(Kgi, NUM_LIGHTS, debug0, 0.f)

	glNamedBufferSubData(myAbj.uboScene, 0, sizeof(SceneData), &sceneD);

	////////////////
	/* DEBUG */
	///////////////
	DebugData debugD;
	debugD.comboU0 = glm::vec4(0.f, 1.f, 0.f, 1.f); //vec4(Kgi, NUM_LIGHTS, debug0, 0.f)
	debugD.comboU1 = glm::vec4(0.f, 0.f, 1.f, 1.f); //vec4(Kgi, NUM_LIGHTS, debug0, 0.f)
	glNamedBufferSubData(myAbj.uboDebug, 0, sizeof(DebugData), &debugD);
}

AbjNode texN_create(string name, string type, string pathIn, int idx)
{
	//this is basically myWin.myGLWidgetSH->singleN_create() / w a texture uploaded into slot 1

	GLuint fboNew;
	glCreateFramebuffers(1, &fboNew);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

	//upload tex into slot 0...consider additional slots (2 total) for brush's outline silhouette
	auto pathTex = "C:/users/bergj/desktop/tex/";
	auto pathTemp = pathTex + pathIn;
	const auto *path = pathTemp.c_str();

	GLuint fboNew2, tex, tex2;
	int imgW, imgH, chan;
	imgW = 999;
	imgH = 999;

	if (type == "CUBE_IRRAD" || type == "CUBE_SPEC")
	{
		auto *myFile = fopen(path, "rb");

		if (!myFile)
		{
			fprintf(stderr, "Error opening file '%s'\n", path);

			return { name, imgW, imgH, 0, 0, 0, 0, 0, 0, 0, 0, idx };
		}

		char filecode[4];
		fread(filecode, 1, 4, myFile);

		ddsHeader myHeader;
		fread(&myHeader, sizeof(ddsHeader), 1, myFile);

		vector<uint8_t*> _data;

		for (int i = 0; i < 6; ++i)
		{
			int w = myHeader.dwWidth;
			int h = myHeader.dwHeight;

			for (unsigned int j = 0; j < myHeader.dwMipMapCount; ++j)
			{
				int size = w * h * 8;
				uint8_t *pixels = new uint8_t[size];

				fread(pixels, size, 1, myFile);
				_data.push_back(pixels);

				w = (w > 1) ? w >> 1 : 1;
				h = (h > 1) ? h >> 1 : 1;
			}
		}

		fclose(myFile);

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &tex);

		for (int i = 0; i < 6; ++i)
		{
			auto w = myHeader.dwWidth;
			auto h = myHeader.dwHeight;

			for (unsigned int j = 0; j < myHeader.dwMipMapCount; ++j)
			{
				void* texData = _data[i * myHeader.dwMipMapCount + j];

				glTextureImage2DEXT(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGBA16F, w, h, 0, GL_RGBA, GL_HALF_FLOAT, texData);

				w >>= 1;
				h >>= 1;
				w = w ? w : 1;
				h = h ? h : 1;
			}
		}

		glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}

	else
	{
		auto *img = stbi_load(path, &imgW, &imgH, &chan, 0);

		for (int h = 0; h < imgH / 2; ++h) //vFlip
		{
			auto top = h * imgW * chan;
			auto btm = (imgH - 1 - h) * imgW * chan;

			for (int w = 0; w < imgW * chan; ++w)
			{
				GLubyte temp = img[top];
				img[top] = img[btm];
				img[btm] = temp;
				++top;
				++btm;
			}
		}

		GLint formatI, formatP;
		if (chan == 3) { formatI = GL_RGB8; formatP = GL_RGB; }
		else if (chan == 4) { formatI = GL_RGBA16; formatP = GL_RGBA; }

		int numMip;
		if (pathIn == "txt/verasansmono.png") numMip = 1;
		else numMip = 1 + int(floor(log2(glm::max(imgW, imgH))));

		glCreateTextures(GL_TEXTURE_2D, 1, &tex);
		glTextureStorage2D(tex, numMip, formatI, imgW, imgH);
		glTextureSubImage2D(tex, 0, 0, 0, imgW, imgH, formatP, GL_UNSIGNED_BYTE, img);

		glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//ANISOTROPIC FILTERING
		GLint maxAniso = 0;
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(tex, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
		glGenerateTextureMipmap(tex);

		stbi_image_free(img);
	}

	glNamedFramebufferTexture(fboNew, DrawBuffers[0], tex, 0);
	glNamedFramebufferDrawBuffers(fboNew, 1, DrawBuffers);

	if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "error with texN_create" << endl;

	if (type == "BRUSH")
	{
		glCreateFramebuffers(1, &fboNew2);
		glCreateTextures(GL_TEXTURE_2D, 1, &tex2);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

		glTextureStorage2D(tex2, 1, GL_RGBA16F, imgW, imgH);
		glTextureParameteri(tex2, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(tex2, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(tex2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(tex2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glNamedFramebufferTexture(fboNew2, DrawBuffers[0], tex2, 0);
		glNamedFramebufferDrawBuffers(fboNew2, 1, DrawBuffers);

		if (glCheckNamedFramebufferStatus(fboNew2, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "error with BRUSH fbo1 create" << endl;
	}

	else if (type != "CUBE_IRRAD" && type != "CUBE_SPEC")
	{
		//create the "copyTex" FBO
		glCreateFramebuffers(1, &fboNew2);
		glCreateTextures(GL_TEXTURE_2D, 1, &tex2);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

		glTextureStorage2D(tex2, 1, GL_RGBA16F, imgW, imgH);
		glTextureParameteri(tex2, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(tex2, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//glTextureParameteri(tex2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTextureParameteri(tex2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(tex2, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(tex2, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glNamedFramebufferTexture(fboNew2, DrawBuffers[0], tex2, 0);
		glNamedFramebufferDrawBuffers(fboNew2, 1, DrawBuffers);

		if (glCheckNamedFramebufferStatus(fboNew2, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "error with TEX AKA COPYTEX fbo1 create" << endl;
	}

	return{ name, imgW, imgH, fboNew, tex, 0, fboNew2, tex2, 0, 0, 0, idx };
}

void up64N(AbjNode &node, bool up)
{
	glMakeTextureHandleNonResidentARB(node.DS_64);
	glMakeTextureHandleNonResidentARB(node.tex0_64);
	glMakeTextureHandleNonResidentARB(node.tex1_64);

	if (up)
	{
		node.tex0_64 = glGetTextureHandleARB(node.tex0_32);
		glMakeTextureHandleResidentARB(node.tex0_64);

		node.tex1_64 = glGetTextureHandleARB(node.tex1_32);
		glMakeTextureHandleResidentARB(node.tex1_64);

		node.DS_64 = glGetTextureHandleARB(node.DS_32);
		glMakeTextureHandleResidentARB(node.DS_64);
	}

	else
	{
		glDeleteTextures(1, &node.DS_32);

		glDeleteTextures(1, &node.tex0_32);
		glDeleteFramebuffers(1, &node.fbo0);

		glDeleteTextures(1, &node.tex1_32);
		glDeleteFramebuffers(1, &node.fbo1);
	}
}

void up64T(GLuint &tex32, GLuint64 &tex64, bool up)
{
	glMakeTextureHandleNonResidentARB(tex64);

	if (up)
	{
		tex64 = glGetTextureHandleARB(tex32);
		glMakeTextureHandleResidentARB(tex64);
	}

	else
		glDeleteTextures(1, &tex32);
}

void mapInit(Abj &myAbj) //NSIGHT
{
	//DEBUG
	//myAbj.allMaps.push_back( { "debugBG", "DEBUG", "debug/nvidia-3d-vision.jpg" } );
	//myAbj.allMaps.push_back( { "debugBG", "DEBUG", "debug/oldPaper.jpg" } );
	//myAbj.allMaps.push_back({ "debugBG", "DEBUG", "debug/oldPaper.png" });
	//myAbj.allMaps.push_back({ "freiChen", "DEBUG", "debug/freiChen.tga" });

	//ALBEDO
	myAbj.allMaps.push_back({ "BLANK", "ALBEDO", "single/BLANK_white_256.tga" });
	//myAbj.allMaps.push_back({ "checker", "ALBEDO", "albedo/checker.png" });
	//myAbj.allMaps.push_back({ "uv", "ALBEDO", "albedo/uv.jpg" });
	//myAbj.allMaps.push_back({ "gold", "ALBEDO", "single/gold_256.tga" });
	//myAbj.allMaps.push_back({ "abj", "ALBEDO", "albedo/ABJ.png" });

	myAbj.allMaps.push_back({ "red", "ALBEDO", "single/colors/BLANK_red_256.png" });
	myAbj.allMaps.push_back({ "green", "ALBEDO", "single/colors/BLANK_green_256.png" });
	myAbj.allMaps.push_back({ "blue", "ALBEDO", "single/colors/BLANK_blue_256.png" });
	myAbj.allMaps.push_back({ "cyan", "ALBEDO", "single/colors/BLANK_cyan_256.png" });
	myAbj.allMaps.push_back({ "yellow", "ALBEDO", "single/colors/BLANK_yellow_256.png" });
	myAbj.allMaps.push_back({ "magenta", "ALBEDO", "single/colors/BLANK_magenta_256.png" });

	//ALPHA
	myAbj.allMaps.push_back({ "BLANK", "ALPHA", "single/BLANK_white_1024.png" });
	//myAbj.allMaps.push_back( { "BLANK", "ALPHA", "single/BLANK_white_1024.tga" } );
	//myAbj.allMaps.push_back( { "BLANK", "ALPHA", "albedo/checker.png" } );

	//ANISO
	//myAbj.allMaps.push_back({ "BLANK", "ANISO", "single/BLANK_white_256.tga" });
	//myAbj.allMaps.push_back({ "VIEW", "ANISO", "single/BLANK_white_256.tga" });
	//myAbj.allMaps.push_back({ "rot1", "ANISO", "aniso/rot1.jpg" });
	//myAbj.allMaps.push_back({ "rot2", "ANISO", "aniso/rot2.jpg" });
	//myAbj.allMaps.push_back({ "rot3", "ANISO", "aniso/rot3.jpg" });

	//BRUSH
	//myAbj.allMaps.push_back( { "round", "BRUSH", "brush/superSoft.tga" } );
	myAbj.allMaps.push_back({ "round", "BRUSH", "brush/round_soft.tga" });
	//myAbj.allMaps.push_back( { "round", "BRUSH", "brush/round_hard.tga" } );
	//myAbj.allMaps.push_back( { "round", "BRUSH", "brush/wavy.tga" } );
	myAbj.allMaps.push_back({ "square", "BRUSH", "brush/square.tga" });

	////CUBE_IRRAD
	myAbj.allMaps.push_back({ "ennis", "CUBE_IRRAD", "cube/ennis_cube_irradiance.dds" });
	//myAbj.allMaps.push_back({ "glacier", "CUBE_IRRAD", "cube/glacier_cube_irradiance.dds" });
	//myAbj.allMaps.push_back({ "grace", "CUBE_IRRAD", "cube/grace_cube_irradiance.dds" });
	//myAbj.allMaps.push_back({ "pisa", "CUBE_IRRAD", "cube/pisa_cube_irradiance.dds" });
	//myAbj.allMaps.push_back({ "uffizi", "CUBE_IRRAD", "cube/uffizi_cube_irradiance.dds" });

	////CUBE_SPEC
	myAbj.allMaps.push_back({ "ennis", "CUBE_SPEC", "cube/ennis_cube_specular.dds" });
	//myAbj.allMaps.push_back({ "glacier", "CUBE_SPEC", "cube/glacier_cube_specular.dds" });
	//myAbj.allMaps.push_back({ "grace", "CUBE_SPEC", "cube/grace_cube_specular.dds" });
	//myAbj.allMaps.push_back({ "pisa", "CUBE_SPEC", "cube/pisa_cube_specular.dds" });
	//myAbj.allMaps.push_back({ "uffizi", "CUBE_SPEC", "cube/uffizi_cube_specular.dds" });

	////LENS
	//myAbj.allMaps.push_back({ "BLANK", "LENS", "lens/BLANK_black_256.png" });
	//myAbj.allMaps.push_back({ "abjLens1", "LENS", "lens/abjLens1.png" });

	////METALLIC
	//myAbj.allMaps.push_back({ "BLANK", "METALLIC", "single/BLANK_black_256.tga" });
	//myAbj.allMaps.push_back({ "WHITE", "METALLIC", "single/BLANK_white_256.tga" });

	//NORMAL
	myAbj.allMaps.push_back({ "BLANK", "NORMAL", "normal/BLANK.png" });
	//myAbj.allMaps.push_back({ "squares", "NORMAL", "normal/squares.jpg" });
	//myAbj.allMaps.push_back({ "voronoi", "NORMAL", "normal/voronoi.jpg" });
	//myAbj.allMaps.push_back({ "brushed_H", "NORMAL", "normal/brushed_H.png" });
	//myAbj.allMaps.push_back({ "brushed_V", "NORMAL", "normal/brushed_V.png" });
	//myAbj.allMaps.push_back({ "rand", "NORMAL", "normal/rand.png" }); // for ssao only

	////RUFF
	myAbj.allMaps.push_back({ "BLANK", "RUFF", "single/BLANK_black_256.tga" });
	myAbj.allMaps.push_back({ "gray", "RUFF", "single/BLANK_gray_256.tga" });
	myAbj.allMaps.push_back({ "white", "RUFF", "single/BLANK_white_256.tga" });
	//myAbj.allMaps.push_back({ "squiggle0", "RUFF", "ruff/squiggle0.tga" });
	//myAbj.allMaps.push_back({ "squiggle1", "RUFF", "ruff/squiggle1.tga" });

	////SSS
	myAbj.allMaps.push_back({ "BLANK", "SSS", "single/BLANK_black_256.tga" });
	myAbj.allMaps.push_back({ "WHITE", "SSS", "single/BLANK_white_256.tga" });
	//myAbj.allMaps.push_back({ "sssLookup", "SSS_ref", "sss/lookUp.png" });

	//TXT
	myAbj.allMaps.push_back({ "atlas", "TXT", "txt/verasansmono.png" });

	unsigned int IDct = 0;

	//push LAYERS back into maps
	for (auto &i : myAbj.allMaps)
	{
		i.layer.push_back({ texN_create("layer0", i.type, i.flat, 0) });

		//drag u/d and change idx debug
		if (i.name == "BLANK" && i.type == "ALBEDO")
		{
			i.layer.push_back({ texN_create("layer1", i.type, i.flat, 1) });
			i.layer.push_back({ texN_create("layer2", i.type, i.flat, 2) });
		}

		i.ID = IDct;
		++IDct;
	}

	//upload 64 bit bindless handles for all LAYERS
	for (auto &i : myAbj.allMaps)
	{
		for (auto &j : i.layer)
		{
			up64N(j, 1);
			j.ID = IDct;
			++IDct;
		}
	}

	//GEN BRUSH OUTLINES FOR ALL BRUSHES AND PUSH BACK OUTLINE INTO MAP LAYER[1]
	//for brushes render the sobel outline into FBO2 / tex2
	//use 128x128 size
	//brushOutlineUpB = true;
}

//void mapInit(Abj &myAbj) //FULL
//{
//	//DEBUG
//	//myAbj.allMaps.push_back( { "debugBG", "DEBUG", "debug/nvidia-3d-vision.jpg" } );
//	//myAbj.allMaps.push_back( { "debugBG", "DEBUG", "debug/oldPaper.jpg" } );
//	myAbj.allMaps.push_back({ "debugBG", "DEBUG", "debug/oldPaper.png" });
//	myAbj.allMaps.push_back({ "freiChen", "DEBUG", "debug/freiChen.tga" });
//
//	//ALBEDO
//	myAbj.allMaps.push_back({ "BLANK", "ALBEDO", "single/BLANK_white_256.tga" });
//	myAbj.allMaps.push_back({ "checker", "ALBEDO", "albedo/checker.png" });
//	myAbj.allMaps.push_back({ "uv", "ALBEDO", "albedo/uv.jpg" });
//	myAbj.allMaps.push_back({ "gold", "ALBEDO", "single/gold_256.tga" });
//	myAbj.allMaps.push_back({ "abj", "ALBEDO", "albedo/ABJ.png" });
//
//	myAbj.allMaps.push_back({ "red", "ALBEDO", "single/colors/BLANK_red_256.png" });
//	myAbj.allMaps.push_back({ "green", "ALBEDO", "single/colors/BLANK_green_256.png" });
//	myAbj.allMaps.push_back({ "blue", "ALBEDO", "single/colors/BLANK_blue_256.png" });
//	myAbj.allMaps.push_back({ "cyan", "ALBEDO", "single/colors/BLANK_cyan_256.png" });
//	myAbj.allMaps.push_back({ "yellow", "ALBEDO", "single/colors/BLANK_yellow_256.png" });
//	myAbj.allMaps.push_back({ "magenta", "ALBEDO", "single/colors/BLANK_magenta_256.png" });
//
//	//ALPHA
//	myAbj.allMaps.push_back({ "BLANK", "ALPHA", "single/BLANK_white_1024.png" });
//	//myAbj.allMaps.push_back( { "BLANK", "ALPHA", "single/BLANK_white_1024.tga" } );
//	//myAbj.allMaps.push_back( { "BLANK", "ALPHA", "albedo/checker.png" } );
//
//	//ANISO
//	myAbj.allMaps.push_back({ "BLANK", "ANISO", "single/BLANK_white_256.tga" });
//	myAbj.allMaps.push_back({ "VIEW", "ANISO", "single/BLANK_white_256.tga" });
//	myAbj.allMaps.push_back({ "rot1", "ANISO", "aniso/rot1.jpg" });
//	myAbj.allMaps.push_back({ "rot2", "ANISO", "aniso/rot2.jpg" });
//	myAbj.allMaps.push_back({ "rot3", "ANISO", "aniso/rot3.jpg" });
//
//	//BRUSH
//	//myAbj.allMaps.push_back( { "round", "BRUSH", "brush/superSoft.tga" } );
//	myAbj.allMaps.push_back({ "round", "BRUSH", "brush/round_soft.tga" });
//	//myAbj.allMaps.push_back( { "round", "BRUSH", "brush/round_hard.tga" } );
//	//myAbj.allMaps.push_back( { "round", "BRUSH", "brush/wavy.tga" } );
//	myAbj.allMaps.push_back({ "square", "BRUSH", "brush/square.tga" });
//
//	//CUBE_IRRAD
//	myAbj.allMaps.push_back({ "ennis", "CUBE_IRRAD", "cube/ennis_cube_irradiance.dds" });
//	myAbj.allMaps.push_back({ "glacier", "CUBE_IRRAD", "cube/glacier_cube_irradiance.dds" });
//	myAbj.allMaps.push_back({ "grace", "CUBE_IRRAD", "cube/grace_cube_irradiance.dds" });
//	myAbj.allMaps.push_back({ "pisa", "CUBE_IRRAD", "cube/pisa_cube_irradiance.dds" });
//	myAbj.allMaps.push_back({ "uffizi", "CUBE_IRRAD", "cube/uffizi_cube_irradiance.dds" });
//
//	//CUBE_SPEC
//	myAbj.allMaps.push_back({ "ennis", "CUBE_SPEC", "cube/ennis_cube_specular.dds" });
//	myAbj.allMaps.push_back({ "glacier", "CUBE_SPEC", "cube/glacier_cube_specular.dds" });
//	myAbj.allMaps.push_back({ "grace", "CUBE_SPEC", "cube/grace_cube_specular.dds" });
//	myAbj.allMaps.push_back({ "pisa", "CUBE_SPEC", "cube/pisa_cube_specular.dds" });
//	myAbj.allMaps.push_back({ "uffizi", "CUBE_SPEC", "cube/uffizi_cube_specular.dds" });
//
//	//LENS
//	myAbj.allMaps.push_back({ "BLANK", "LENS", "lens/BLANK_black_256.png" });
//	myAbj.allMaps.push_back({ "abjLens1", "LENS", "lens/abjLens1.png" });
//
//	//METALLIC
//	myAbj.allMaps.push_back({ "BLANK", "METALLIC", "single/BLANK_black_256.tga" });
//	myAbj.allMaps.push_back({ "WHITE", "METALLIC", "single/BLANK_white_256.tga" });
//
//	//NORMAL
//	myAbj.allMaps.push_back({ "BLANK", "NORMAL", "normal/BLANK.png" });
//	myAbj.allMaps.push_back({ "squares", "NORMAL", "normal/squares.jpg" });
//	myAbj.allMaps.push_back({ "voronoi", "NORMAL", "normal/voronoi.jpg" });
//	myAbj.allMaps.push_back({ "brushed_H", "NORMAL", "normal/brushed_H.png" });
//	myAbj.allMaps.push_back({ "brushed_V", "NORMAL", "normal/brushed_V.png" });
//	myAbj.allMaps.push_back({ "rand", "NORMAL", "normal/rand.png" }); // for ssao only
//
//	//RUFF
//	myAbj.allMaps.push_back({ "BLANK", "RUFF", "single/BLANK_black_256.tga" });
//	myAbj.allMaps.push_back({ "gray", "RUFF", "single/BLANK_gray_256.tga" });
//	myAbj.allMaps.push_back({ "white", "RUFF", "single/BLANK_white_256.tga" });
//	myAbj.allMaps.push_back({ "squiggle0", "RUFF", "ruff/squiggle0.tga" });
//	myAbj.allMaps.push_back({ "squiggle1", "RUFF", "ruff/squiggle1.tga" });
//
//	//SSS
//	myAbj.allMaps.push_back({ "BLANK", "SSS", "single/BLANK_black_256.tga" });
//	myAbj.allMaps.push_back({ "WHITE", "SSS", "single/BLANK_white_256.tga" });
//	myAbj.allMaps.push_back({ "sssLookup", "SSS_ref", "sss/lookUp.png" });
//
//	//TXT
//	myAbj.allMaps.push_back({ "atlas", "TXT", "txt/verasansmono.png" });
//
//	unsigned int IDct = 0;
//
//	//push LAYERS back into maps
//	for (auto &i : myAbj.allMaps)
//	{
//		i.layer.push_back({ texN_create("layer0", i.type, i.flat, 0) });
//
//		//drag u/d and change idx debug
//		if (i.name == "BLANK" && i.type == "ALBEDO")
//		{
//			i.layer.push_back({ texN_create("layer1", i.type, i.flat, 1) });
//			i.layer.push_back({ texN_create("layer2", i.type, i.flat, 2) });
//		}
//
//		i.ID = IDct;
//		++IDct;
//	}
//
//	//upload 64 bit bindless handles for all LAYERS
//	for (auto &i : myAbj.allMaps)
//	{
//		for (auto &j : i.layer)
//		{
//			up64N(j, 1);
//			j.ID = IDct;
//			++IDct;
//		}
//	}
//
//	//GEN BRUSH OUTLINES FOR ALL BRUSHES AND PUSH BACK OUTLINE INTO MAP LAYER[1]
//	//for brushes render the sobel outline into FBO2 / tex2
//	//use 128x128 size
//	//brushOutlineUpB = true;
//}

void proInit(Abj &myAbj)
{

	/* #INCLUDE */
	{
		auto incTemp = myAbj.pathGLSL + string("Core_glsl.h");

		const auto *incFilePath = incTemp.c_str();
		auto incStr = shader_read(incFilePath);
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, GLint(strlen("/Core_glsl.h")), "/Core_glsl.h", GLint(strlen(incStr)), incStr);
	}

	myAbj.allPro.push_back( { "pAlphaAsRGBA", createProg("fboV.glsl", "", "alphaAsRGBAF.glsl") } );
	myAbj.allPro.push_back( { "pBB", createProg("bbV.glsl", "", "bbF.glsl") } );
	myAbj.allPro.push_back( { "pBlendMode", createProg("wsQuadV.glsl", "", "blendModeF.glsl") } );
	myAbj.allPro.push_back( { "pBloomC", createProg("fboV.glsl", "", "bloomCF.glsl") } );
	myAbj.allPro.push_back( { "pBloom", createProg("fboV.glsl", "", "bloomF.glsl") } );
	myAbj.allPro.push_back( { "pCopyTex", createProg("fboV.glsl", "", "copyTexF.glsl") } );
	myAbj.allPro.push_back( { "pDef", createProg("fboV.glsl", "", "defF.glsl") } );
	myAbj.allPro.push_back( { "pDepthRev", createProg("depthRevV.glsl", "", "depthRevF.glsl") } );
	myAbj.allPro.push_back( { "pDown", createProg("downV.glsl", "", "downF.glsl") } );
	myAbj.allPro.push_back( { "pFinal", createProg("fboV.glsl", "", "finalF.glsl") } );
	myAbj.allPro.push_back({ "pEdgeDetect", createProg("fboV.glsl", "", "edgeDetectF.glsl") });
	myAbj.allPro.push_back({ "pFxaa", createProg("fboV.glsl", "", "fxaaF.glsl") });
	myAbj.allPro.push_back({ "pGauss", createProg("fboV.glsl", "", "gaussF.glsl") });
	myAbj.allPro.push_back({ "pGBuffer", createProg("gBufferV.glsl", "", "gBufferF.glsl") });
	myAbj.allPro.push_back({ "pGiz", createProg("gizV.glsl", "", "gizF.glsl") });
	myAbj.allPro.push_back({ "pGiz_circ", createProg("giz_circV.glsl", "", "gizF.glsl") });
	myAbj.allPro.push_back({ "pGrid", createProg("gizV.glsl", "", "gridF.glsl") });
	myAbj.allPro.push_back({ "pLumaInit", createProg("fboV.glsl", "", "lumaInitF.glsl") });
	myAbj.allPro.push_back({ "pLumaAdapt", createProg("fboV.glsl", "", "lumaAdaptF.glsl") });
	myAbj.allPro.push_back({ "pEraseMix", createProg("fboV.glsl", "", "eraseMixF.glsl") });
	myAbj.allPro.push_back({ "pPaintProj", createProg("paintProjV.glsl", "", "paintProjF.glsl") });
	myAbj.allPro.push_back({ "pPaintStroke", createProg("wsQuadV.glsl", "", "paintStrokeF.glsl") });
	myAbj.allPro.push_back({ "pSelRect", createProg("selRectV.glsl", "", "selRectF.glsl") });
	myAbj.allPro.push_back({ "pShadow", createProg("shadowV.glsl", "", "shadowF.glsl") });
	myAbj.allPro.push_back({ "pSky", createProg("skyV.glsl", "", "skyF.glsl") });
	myAbj.allPro.push_back({ "pSSAO_32", createProg("fboV.glsl", "", "ssao_32F.glsl") });
	myAbj.allPro.push_back({ "pSSAO_64", createProg("fboV.glsl", "", "ssao_64F.glsl") });
	myAbj.allPro.push_back({ "pSSR", createProg("fboV.glsl", "", "ssrF.glsl") });
	myAbj.allPro.push_back({ "pStencilHi", createProg("stencilHiV.glsl", "", "stencilHiF.glsl") });
	myAbj.allPro.push_back({ "pStencilGeo", createProg("stencilGeoV.glsl", "", "stencilGeoF.glsl") });
	myAbj.allPro.push_back({ "pTonemap", createProg("fboV.glsl", "", "tonemapF.glsl") });
	myAbj.allPro.push_back({ "pTransp", createProg("transpV.glsl", "", "transpF.glsl") });
	myAbj.allPro.push_back({ "pTranspComp", createProg("fboV.glsl", "", "transpCompF.glsl") });
	myAbj.allPro.push_back({ "pTxt", createProg("txtV.glsl", "txtG.glsl", "txtF.glsl") });
	myAbj.allPro.push_back({ "pVolumeLight", createProg("volumeLightV.glsl", "", "volumeLightF.glsl") });
	myAbj.allPro.push_back({ "pWireframe", createProg("wireframeV.glsl", "", "wireframeF.glsl") });
}

GLuint createProg(string vIn, string gIn, string fIn)
{
	auto pathGLSL = "./shaders/";

	auto vTemp = pathGLSL + vIn;
	const auto *vFile = vTemp.c_str();

	auto gTemp = pathGLSL + gIn;
	const auto *gFile = gTemp.c_str();

	auto fTemp = pathGLSL + fIn;
	const auto *fFile = fTemp.c_str();

	//
	auto proC = glCreateProgram();
	GLuint shaderV, shaderF, shaderG;

	if (vFile)
	{
		shaderV = createIndy(vFile, GL_VERTEX_SHADER);

		if (!shaderV)
			return 0;

		glAttachShader(proC, shaderV);
	}

	if (gIn != "")
	{
		shaderG = createIndy(gFile, GL_GEOMETRY_SHADER);

		if (!shaderG)
			return 0;

		glAttachShader(proC, shaderG);
	}

	if (fFile)
	{
		shaderF = createIndy(fFile, GL_FRAGMENT_SHADER);

		if (!shaderF)
			return 0;

		glAttachShader(proC, shaderF);
	}

	glLinkProgram(proC);
	auto link_ok = GL_FALSE;
	glGetProgramiv(proC, GL_LINK_STATUS, &link_ok);

	if (!link_ok)
	{
		fprintf(stderr, "glLinkProgram:");
		shader_error(proC);
		glDeleteProgram(proC);
		return 0;
	}

	glDetachShader(proC, shaderV);
	glDeleteShader(shaderV);

	glDetachShader(proC, shaderF);
	glDeleteShader(shaderF);

	if (gIn != "")
	{
		glDetachShader(proC, shaderG);
		glDeleteShader(shaderG);
	}

	return proC;
}

GLuint createIndy(const char *file, GLenum type)
{
	const auto *source = shader_read(file);

	if (source == 0)
	{
		fprintf(stderr, "Error opening %s: ", file);
		perror("");

		return 0;
	}

	auto shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);

	free((void*)source);

	static char* incPaths[] = { "/" };
	glCompileShaderIncludeARB(shader, 1, incPaths, NULL);

	auto compile_ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);

	if (compile_ok == GL_FALSE) // error here
	{
		fprintf(stderr, "%s:", file);
		shader_error(shader);
		glDeleteShader(shader);

		return 0;
	}

	return shader;
}

char* shader_read(const char *file)
{
	auto *input = fopen(file, "rb");

	if (!input)
	{
		cout << "input prob in shader_read()" << endl;
		return 0;
	}

	if (fseek(input, 0, SEEK_END) == -1)
		return 0;

	auto size = ftell(input);

	if (size == -1)
		return 0;

	if (fseek(input, 0, SEEK_SET) == -1)
		return 0;

	auto *content = (char*)malloc((size_t)size + 1);

	if (content == 0)
		return 0;

	if (!(fread(content, 1, (size_t)size, input)) || ferror(input))
	{
		cout << "error reading shader" << endl;
		free(content);

		return 0;
	}

	fclose(input);
	content[size] = '\0';

	return content;
}

void shader_error(GLuint object)
{
	auto log_length = 0;

	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);

	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);

	else
	{
		fprintf(stderr, "printlog: Not a shader or a program\n");

		return;
	}

	auto *log = (char*)malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, 0, log);

	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, 0, log);

	fprintf(stderr, "%s", log);
	free(log);
}

void glUseProgram2(string name, Abj &myAbj)
{
	bool found = false;

	for (auto &i : myAbj.allPro)
	{
		if (i.name == name)
		{
			myAbj.pro = i.pro;
			found = true;
		}
	}

	if (found == false)
	{
		cout << "couldn't find pro for : " << name << endl;
	}

	glUseProgram(myAbj.pro);
	myAbj.proN = name;
}

void addDeleteShadows(string type, Abj &myAbj)
{
	if (type == "add")
	{
		for (auto &i : myAbj.allCamLi)
		{
			if (i->v->val_b && i->camLiTypeGet("light"))
			{
				int shadowSize = 1024;
				AbjNode shadowN = shadowN_create(i->name->val_s, shadowSize, shadowSize);

				glMakeTextureHandleNonResidentARB(shadowN.tex0_64);
				shadowN.tex0_64 = glGetTextureHandleARB(shadowN.tex0_32);
				glMakeTextureHandleResidentARB(shadowN.tex0_64);

				glMakeTextureHandleNonResidentARB(shadowN.tex1_64);
				shadowN.tex1_64 = glGetTextureHandleARB(shadowN.tex1_32);
				glMakeTextureHandleResidentARB(shadowN.tex1_64);

				myAbj.allShadow.push_back(shadowN);
			}
		}
	}

	else if (type == "delete")
	{
		//SHADOW FBO
		for (auto &i : myAbj.allShadow)
		{
			glMakeTextureHandleNonResidentARB(i.tex0_64);
			glDeleteTextures(1, &i.tex0_32);
			glDeleteFramebuffers(1, &i.fbo0);

			glMakeTextureHandleNonResidentARB(i.tex1_64);
			glDeleteTextures(1, &i.tex1_32);
			glDeleteFramebuffers(1, &i.fbo1);
		}

		myAbj.allShadow.clear();
	}

	else if (type == "refresh")
	{
		addDeleteShadows("delete", myAbj);
		addDeleteShadows("add", myAbj);
	}
}

AbjNode shadowN_create(string name, int widthIn, int heightIn)
{
	GLuint fboNew1; //spot / dir shadows
	glCreateFramebuffers(1, &fboNew1);

	GLfloat border[] = { 1.f, 1.f, 1.f, 1.f };

	GLuint texNew1;
	glCreateTextures(GL_TEXTURE_2D, 1, &texNew1);
	glTextureStorage2D(texNew1, 1, GL_DEPTH_COMPONENT32F, widthIn, heightIn);
	glTextureParameteri(texNew1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(texNew1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTextureParameteri(texNew1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texNew1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(texNew1, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTextureParameteri(texNew1, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTextureParameterfv(texNew1, GL_TEXTURE_BORDER_COLOR, border);

	glNamedFramebufferTexture(fboNew1, GL_DEPTH_ATTACHMENT, texNew1, 0);
	glNamedFramebufferDrawBuffer(fboNew1, GL_NONE);
	glNamedFramebufferReadBuffer(fboNew1, GL_NONE);

	if (glCheckNamedFramebufferStatus(fboNew1, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "error with shadowN_create FBO1" << endl;


	GLuint fboNew2; //point shadows
	glCreateFramebuffers(1, &fboNew2);

	GLuint texNew2;
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texNew2);
	glTextureParameteri(texNew2, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texNew2, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texNew2, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texNew2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texNew2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(texNew2, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTextureParameteri(texNew2, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	for (int i = 0; i < 6; ++i)
		glTextureImage2DEXT(texNew2, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, widthIn, heightIn, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glNamedFramebufferTexture(fboNew2, GL_DEPTH_ATTACHMENT, texNew2, 0);
	glNamedFramebufferDrawBuffer(fboNew2, GL_NONE);
	glNamedFramebufferReadBuffer(fboNew2, GL_NONE);

	if (glCheckNamedFramebufferStatus(fboNew2, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "error with shadowN_create FBO2" << endl;

	return{ name, widthIn, heightIn, fboNew1, texNew1, 0, fboNew2, texNew2 };
}

void writeShadow(shared_ptr<Object> obj, Abj &myAbj)
{
	//for each light in allShadow, render each object into the light's FBO1...except for point lights which render into fbo1 ?
	for (auto &i : myAbj.allShadow)
	{
		if (i.name == obj->name->val_s) // if it's a light
		{
			myAbj.shadowObj = obj; // found the light

			if (obj->camLiType->val_s == "DIR" || obj->camLiType->val_s == "SPOT")
			{
				glBindFramebuffer(GL_FRAMEBUFFER, i.fbo0);
				glViewport(0, 0, i.width, i.height);
				glClear(GL_DEPTH_BUFFER_BIT);

				for (auto &j : myAbj.allObj)
				{
					if (j->type == "OBJ" && j->shadowCast->val_b & searchUp(j, myAbj))
						j->render(); //
				}
			}

			else if (obj->camLiType->val_s == "POINT")
			{
				glBindFramebuffer(GL_FRAMEBUFFER, i.fbo1);
				glViewport(0, 0, i.width, i.height);

				for (int j = 0; j < 6; ++j)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i.tex1_32, 0);
					glDrawBuffer(GL_DEPTH_ATTACHMENT);
					glClear(GL_DEPTH_BUFFER_BIT);

					myAbj.MM_cube = glm::translate(glm::mat4(), -obj->t->val_3);
					myAbj.VM_cube = glm::lookAt(glm::vec3(0.f), cubeDirs[j].targ, cubeDirs[j].up);
					myAbj.PM_cube = glm::perspective(glm::radians(90.f), 1.f, obj->nearShadow->val_f, obj->farShadow->val_f);

					for (auto &k : myAbj.allObj)
					{
						if (k->type == "OBJ" && k->shadowCast->val_b && searchUp(k, myAbj))
							k->render(); //
					}
				}
			}

			obj->dirtyShadow = false;
		}
	}
}

AbjNode topLayer(Map mapIn)
{
	for (auto &i : mapIn.layer)
	{
		if (i.idx == 0)
			return i;
	}

	AbjNode myLayer;

	return myLayer;
}

/* ETC */
int countLights(Abj &myAbj)
{
	auto count = 0;

	for (auto &i : myAbj.allCamLi)
	{
		if (!i->ref && i->v->val_b && i->camLiTypeGet("light"))
			count += 1;
	}

	return count;
}

glm::vec2 toNDC(glm::vec2 pt, Abj &myAbj, string mode)
{
	if (mode == "SELRECT")
	{
		pt.x = (2.f * pt.x / myAbj.width) - 1.f;
		pt.y = (2.f * pt.y / myAbj.height) - 1.f;
		pt.y *= -1.f;
	}

	else
	{
		pt.x = pt.x / myAbj.width;
		pt.y = pt.y / myAbj.height;
	}

	return pt;
}

void getPtsBetweenRect(Abj &myAbj)
{
	if (!myAbj.selRectPts_color.empty())
		myAbj.selRectPts_color.clear();

	int bigX, smallX, bigY, smallY;

	bigX = int(glm::max(myAbj.rayP.x, myAbj.pMouseNew.x));
	smallX = int(glm::min(myAbj.rayP.x, myAbj.pMouseNew.x));
	bigY = int(glm::max(myAbj.rayP.y, myAbj.pMouseNew.y));
	smallY = int(glm::min(myAbj.rayP.y, myAbj.pMouseNew.y));

	if (bigY - smallY > bigX - smallX) //bigger y diff
	{
		for (int y = smallY; y < bigY; ++y)
		{
			for (int x = smallX; x < bigX; ++x)
			{
				myAbj.selRectPts_color.push_back({ x, y });

				continue;
			}
		}
	}

	else if (bigX - smallX > bigY - smallY) //bigger x diff
	{
		for (int x = smallX; x < bigX; ++x)
		{
			for (int y = smallY; y < bigY; ++y)
			{
				myAbj.selRectPts_color.push_back({ x, y });

				continue;
			}
		}
	}
}

//PAINTING
void brushOutlineUp(Abj &myAbj)
{
	//edge detect ALL brushes
	//upload sobel silh into brush.FBO2

	//take the alpha of a brush (always solid white) INTO ITS OWN TEXTURE !!!! - brushTemp1
	//gaussian blur THE ALPHA CHANNEL - alphaGauss2
	//sobel the blurred tex

	for (auto &i : myAbj.allMaps)
	{
		if (i.type == "BRUSH")
		{
			myAbj.sobelMap = i;

			//ALPHA INTO ITS OWN CHANNEL
			glBindFramebuffer(GL_FRAMEBUFFER, myAbj.brushTempN.fbo0);
			glViewport(0, 0, myAbj.brushTempN.width, myAbj.brushTempN.height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram2("pAlphaAsRGBA", myAbj);
			myAbj.myFSQ->render();

			//GAUSSIAN BLUR
			glMakeTextureHandleNonResidentARB(myAbj.alphaGaussN.tex1_64);
			glUseProgram2("pGauss", myAbj);
			//alphaGaussN.tex1_64 = glGetTextureHandleARB(gaussianBlur2(brushTempN.tex0_32, alphaGaussN, 0, myAbj));
			myAbj.alphaGaussN.tex1_64 = glGetTextureHandleARB(gaussianBlur2(myAbj.brushTempN.tex0_32, myAbj.alphaGaussN, 1, myAbj));
			glMakeTextureHandleResidentARB(myAbj.alphaGaussN.tex1_64);

			//EDGE DETECT
			glBindFramebuffer(GL_FRAMEBUFFER, i.layer[0].fbo1);
			glViewport(0, 0, i.layer[0].width, i.layer[0].height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram2("pEdgeDetect", myAbj);
			//edgeDetect_mode = 0; // sobel
			myAbj.edgeDetect_mode = 1; // freiChen
			myAbj.myFSQ->render();
		}
	}

	myAbj.brushOutlineUpB = false;
}

//PP
GLuint gaussianLinear(GLuint src, AbjNode dest, Abj &myAbj)
{
	for (int i = 0; i < 2; ++i)
	{
		if (i == 0)
			glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo0);

		else
			glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo1);

		glViewport(0, 0, dest.width, dest.height);
		glClear(GL_COLOR_BUFFER_BIT);

		myAbj.rttGaussIn32 = (i == 0) ? src : dest.tex0_32;
		up64T(myAbj.rttGaussIn32, myAbj.rttGaussIn64, 1);

		myAbj.myFSQ->gaussStage = i;
		myAbj.myFSQ->render();
	}

	return dest.tex1_32;
}

void gaussianRecursivePasses(GLuint src, AbjNode dest, int passes, int iter, Abj &myAbj)
{
	myAbj.tempGauss = gaussianLinear(src, dest, myAbj);

	if (iter + 1 < passes)
		gaussianRecursivePasses(myAbj.tempGauss, dest, passes, iter + 1, myAbj);
}

GLuint gaussianBlur(AbjNode src, AbjNode dest, int passes, Abj &myAbj)
{
	if (passes == 0)
		return src.tex0_32;

	gaussianRecursivePasses(src.tex0_32, dest, passes, 0, myAbj);

	return myAbj.tempGauss;
}

GLuint gaussianBlur2(GLuint src, AbjNode dest, int passes, Abj &myAbj)
{
	if (passes == 0)
		return src;

	gaussianRecursivePasses(src, dest, passes, 0, myAbj);

	return myAbj.tempGauss;
}

void downSampRender(AbjNode src, AbjNode dest, int downPixAmt, Abj &myAbj)
{
	glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo0);
	glViewport(0, 0, dest.width, dest.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	up64T(src.tex0_32, myAbj.downSamp_64, 1);

	myAbj.texelSize = glm::vec2(downPixAmt / src.width, downPixAmt / src.height);
	myAbj.myFSQ->render();
}

void bloomRender(Abj &myAbj)
{
	glUseProgram2("pDown", myAbj);

	for (int i = 0; i < 6; ++i) //DOWNSAMP
	{
		if (i == 0)
			downSampRender(myAbj.deferredN, myAbj.downN[i], 2, myAbj);

		else
			downSampRender(myAbj.downN[i - 1], myAbj.downN[i], 2, myAbj);
	}

	glUseProgram2("pGauss", myAbj);

	for (int i = 0; i < 6; ++i)
		gaussianBlur(myAbj.downN[i], myAbj.bloomGaussN[i], 4, myAbj);

	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.bloomN.fbo0);
	glViewport(0, 0, myAbj.bloomN.width, myAbj.bloomN.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram2("pBloom", myAbj);
	myAbj.myFSQ->render();
}