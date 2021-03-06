/*
 * line2d.h
 *
 *  Created on: 3Feb.,2017
 *      Author: bcub3d-desktop
 */

#ifndef P_LINE2D_H_
#define P_LINE2D_H_

// GLEW (OpenGL Extension Wrangler Library)
#define GLEW_STATIC
#include <GL/glew.h>

// Project Includes
#include "../src/lineColours.h"

// Standard Includes
#include <vector>
using std::vector;

namespace GLPL {

	float maxFloat = std::numeric_limits<float>::max();

	struct pt2D {
		GLfloat x;
		GLfloat y;
	};

	/* ----------------------- */
	/* Functions for all lines */
	/* ----------------------- */
	void createAndSetupBuffers(GLuint* VAOPt,GLuint* VBOPt,int dataSizeBytes,const void* dataAddress,int strideBytes) {
		/* Create Buffers */
		glGenVertexArrays(1,VAOPt);
		glGenBuffers(1,VBOPt);

		/* Setup Buffers */
		glBindVertexArray(*VAOPt);
		glBindBuffer(GL_ARRAY_BUFFER,*VBOPt);

		glBufferData(GL_ARRAY_BUFFER, dataSizeBytes,dataAddress,GL_DYNAMIC_DRAW);

		/* Position Attributes */
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideBytes,(GLvoid*)0);

		glBindVertexArray(0); // Unbind VAO

	}


	void drawData(Shader shader, glm::mat4 axesLimitViewportTrans,GLuint* VAOPt, glm::vec3 colour, int nPts, GLenum mode) {
		// Draws the data currently stored in the line corresponding to the given VAO
		shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"transformViewport"), 1, GL_FALSE, glm::value_ptr(axesLimitViewportTrans));
		glm::vec4 inColor = glm::vec4(colour, 1.0);
		glUniform4fv(glGetUniformLocation(shader.Program,"inColor"),1,glm::value_ptr(inColor));
		glBindVertexArray(*VAOPt);
		glDrawArrays(mode,0,nPts);
		glBindVertexArray(0);
	}

	/* ------------------------ */
	/* 	    Line Functions 	    */
	/* ------------------------ */
	/* ====================================================================== */
	/*						 Line of pt2D Structures						  */
	/* ====================================================================== */
	class Line2DPts {
	public:
		/* Buffers */
		GLuint VAO, VBO;
		int nPts = 0;

		/* Data */
		vector<pt2D>* dataPtPt;

		/* Attributes */
		glm::vec3 colour = LC_WHITE;
		GLenum mode; // Mode, line or points

		Line2DPts(vector<pt2D>* dataPtPt, GLenum mode = GL_LINE_STRIP) {
			this->dataPtPt = dataPtPt;
			this->mode = mode;

			/* Setup Buffers */
			int dataSizeBytes = dataPtPt->size()*2*sizeof(GLfloat);
			createAndSetupBuffers(&VAO, &VBO, dataSizeBytes, dataPtPt->data(), sizeof(pt2D)); // dataPtPt->data() gives the address of the first element of the vector

			/* Set Number of Points */
			nPts = dataPtPt->size();

		}

		void Draw(Shader shader, glm::mat4 axesLimitViewportTrans) {
			// Check if number of points changed
			int newPts = dataPtPt->size();
			if (newPts != nPts) {
				nPts = newPts;
				// Update buffer and attributes
				glBindBuffer(GL_ARRAY_BUFFER,VBO);
				glBufferData(GL_ARRAY_BUFFER, dataPtPt->size()*2*sizeof(GLfloat),dataPtPt->data(),GL_DYNAMIC_DRAW);
			}

			// Draw Plot
			drawData(shader, axesLimitViewportTrans, &VAO, colour, nPts, mode);
		}

		void appendPt(pt2D pt) {
			// Appends a point to the current data
			this->dataPtPt->push_back(pt);
		}

		vector<float> getMinMax() {
			// Gets the minimum and maximum values of both x and y for the data
			float xmin = 0;
			float xmax = 0;
			float ymin = 0;
			float ymax = 0;
			for (unsigned int i = 0; i<dataPtPt->size(); i++) {
				float xval = (*dataPtPt)[i].x;
				float yval = (*dataPtPt)[i].y;
				if (xval > xmax) {
					xmax = xval;
				}
				if (xval < xmin) {
					xmin = xval;
				}
				if (yval > ymax) {
					ymax = yval;
				}
				if (yval < ymin) {
					ymin = yval;
				}
			}

			return vector<float> {xmin,xmax,ymin,ymax};
		}

	};

	/* ====================================================================== */
	/*						 Line of 2-length Vectors						  */
	/* ====================================================================== */
	class Line2DVec {
	public:
		/* Buffers */
		GLuint VAO, VBO;
		int nPts = 0;

		/* Data */
		vector<float>* dataVecPt;

		/* Attributes */
		glm::vec3 colour = LC_WHITE;
		GLenum mode; // Mode, line or points

		Line2DVec(vector<float>* dataVecPt, GLenum mode = GL_LINE_STRIP) {
			this->dataVecPt = dataVecPt;
			this->mode = mode;

			/* Setup Buffers */
			int dataSizeBytes = dataVecPt->size()*sizeof((*dataVecPt)[0]);
			createAndSetupBuffers(&VAO, &VBO, dataSizeBytes, dataVecPt->data(), 2*sizeof((*dataVecPt)[0])); // dataVecPt->data() gives the address of the first element of the vector

			/* Set Number of Points */
			nPts = dataVecPt->size()/2;

		}

		void Draw(Shader shader, glm::mat4 axesLimitViewportTrans) {
			// Check if number of points changed
			int newPts = dataVecPt->size()/2;
			if (newPts != nPts) {
				nPts = newPts;
				// Update buffer and attributes
				glBindBuffer(GL_ARRAY_BUFFER,VBO);
				glBufferData(GL_ARRAY_BUFFER, dataVecPt->size()*sizeof((*dataVecPt)[0]),dataVecPt->data(),GL_DYNAMIC_DRAW);
			}

			// Draw Plot
			drawData(shader, axesLimitViewportTrans, &VAO, colour, nPts, mode);
		}

		void appendVec(float x, float y) {
			// Appends a point to the current data
			this->dataVecPt->push_back(x);
			this->dataVecPt->push_back(y);
		}

		vector<float> getMinMax() {
			// Gets the minimum and maximum values of both x and y for the data
			float xmin = 0;
			float xmax = 0;
			float ymin = 0;
			float ymax = 0;
			for (unsigned int i = 0; i<dataVecPt->size()/2.0; i++) {
				float xval = (*dataVecPt)[2*i];
				float yval = (*dataVecPt)[2*i+1];
				if (xval > xmax) {
					xmax = xval;
				}
				if (xval < xmin) {
					xmin = xval;
				}
				if (yval > ymax) {
					ymax = yval;
				}
				if (yval < ymin) {
					ymin = yval;
				}
			}

			return vector<float> {xmin,xmax,ymin,ymax};
		}
	};

	/* ====================================================================== */
	/*						 Line of vector of vectors						  */
	/* ====================================================================== */
	class Line2DVecVec {
	public:
		/* Buffers */
		GLuint VAO, VBO;
		int nPts = 0;

		/* Data */
		vector<vector<float>>* dataVecPt;
		vector<float> internalData;

		/* Selection */
		int indexX;
		int indexY;

		/* Attributes */
		glm::vec3 colour = LC_WHITE;
		GLenum mode; // Mode, line or points

		Line2DVecVec(vector<vector<float>>* dataVecPt, int indexX = 0, int indexY = 1, GLenum mode = GL_LINE_STRIP) {
			this->dataVecPt = dataVecPt;
			this->mode = mode;
			this->indexX = indexX;
			this->indexY = indexY;

			/* Setup Buffers */
			updateInternalData();
			int dataSizeBytes = internalData.size()*sizeof(internalData[0]);
			createAndSetupBuffers(&VAO, &VBO, dataSizeBytes, &internalData[0], 2*sizeof(internalData[0]));

			/* Set Number of Points */
			nPts = dataVecPt->size()/2.0;

		}

		void updateInternalData() {
			/* Creates an internal data store from the current dataVecPt */
			// Clear Previous Data
			internalData.clear();
			// Update With New Data
			for(unsigned int i=0; i<dataVecPt->size(); i++) {
				internalData.push_back((*dataVecPt)[i][indexX]);
				internalData.push_back((*dataVecPt)[i][indexY]);
			}
		}

		void Draw(Shader shader, glm::mat4 axesLimitViewportTrans) {
			// Check if number of points changed
			int newPts = (internalData).size()/2;
			if (newPts != nPts) {
				nPts = newPts;
				// Update buffer and attributes
				glBindBuffer(GL_ARRAY_BUFFER,VBO);
				glBufferData(GL_ARRAY_BUFFER, internalData.size()*sizeof(internalData[0]),&internalData[0],GL_DYNAMIC_DRAW);
			}

			// Draw Plot
			drawData(shader, axesLimitViewportTrans, &VAO, colour, nPts, mode);
		}

		vector<float> getMinMax() {
			// Gets the minimum and maximum values of both x and y for the data
			float xmin = 0;
			float xmax = 0;
			float ymin = 0;
			float ymax = 0;
			for (unsigned int i = 0; i<internalData.size()/2.0; i++) {
				float xval = (internalData)[2*i];
				float yval = (internalData)[2*i+1];
				if (xval > xmax) {
					xmax = xval;
				}
				if (xval < xmin) {
					xmin = xval;
				}
				if (yval > ymax) {
					ymax = yval;
				}
				if (yval < ymin) {
					ymin = yval;
				}
			}

			return vector<float> {xmin,xmax,ymin,ymax};
		}
	};

	/* ====================================================================== */
	/*						 Line of vector of glm::dvec3s					  */
	/* ====================================================================== */
	class Line2DVecGLMV3 {
	public:
		/* Buffers */
		GLuint VAO, VBO;
		int nPts = 0;

		/* Data */
		vector<glm::dvec3>* dataVecPt;
		vector<float> internalData;

		/* Selection */
		int indexX;
		int indexY;

		/* Attributes */
		glm::vec3 colour = LC_WHITE;
		GLenum mode; // Mode, line or points

		Line2DVecGLMV3(vector<glm::dvec3>* dataVecPt, int indexX = 0, int indexY = 1, GLenum mode = GL_LINE_STRIP) {
			this->dataVecPt = dataVecPt;
			this->mode = mode;
			this->indexX = indexX;
			this->indexY = indexY;

			/* Setup Buffers */
			updateInternalData();
			int dataSizeBytes = internalData.size()*sizeof(internalData[0]);
			createAndSetupBuffers(&VAO, &VBO, dataSizeBytes, &internalData[0], 2*sizeof(internalData[0]));

			/* Set Number of Points */
			nPts = dataVecPt->size()/2.0;

		}

		void updateInternalData() {
			/* Creates an internal data store from the current dataVecPt */
			// Clear Previous Data
			internalData.clear();
			// Update With New Data
			for(unsigned int i=0; i<dataVecPt->size(); i++) {
				internalData.push_back((*dataVecPt)[i][indexX]);
				internalData.push_back((*dataVecPt)[i][indexY]);
			}
		}

		void Draw(Shader shader, glm::mat4 axesLimitViewportTrans) {
			// Check if number of points changed
			int newPts = (internalData).size()/2;
			if (newPts != nPts) {
				nPts = newPts;
				// Update buffer and attributes
				glBindBuffer(GL_ARRAY_BUFFER,VBO);
				glBufferData(GL_ARRAY_BUFFER, internalData.size()*sizeof(internalData[0]),&internalData[0],GL_DYNAMIC_DRAW);
			}

			// Draw Plot
			drawData(shader, axesLimitViewportTrans, &VAO, colour, nPts, mode);
		}

		vector<float> getMinMax() {
			// Gets the minimum and maximum values of both x and y for the data
			float xmin = maxFloat;
			float xmax = -maxFloat;
			float ymin = maxFloat;
			float ymax = -maxFloat;
			for (unsigned int i = 0; i<internalData.size()/2.0; i++) {
				float xval = (internalData)[2*i];
				float yval = (internalData)[2*i+1];
				if (xval > xmax) {
					xmax = xval;
				}
				if (xval < xmin) {
					xmin = xval;
				}
				if (yval > ymax) {
					ymax = yval;
				}
				if (yval < ymin) {
					ymin = yval;
				}
			}

			return vector<float> {xmin,xmax,ymin,ymax};
		}
	};

	/* ====================================================================== */
	/*			   Line of vector<float> vs vector of glm::dvec3s		  	  */
	/* ====================================================================== */
	class Line2DVecfVecGLMV3 {
	// For plotting a vector of floats against an index selected form a vector of glm::dvec3s
	// For example plotting a vector of time data against a vector of glm::dvec3.
	public:
		/* Buffers */
		GLuint VAO, VBO;
		int nPts = 0;

		/* Data */
		vector<float>*	dataVecfPt;
		vector<glm::dvec3>* dataVecGLMV3Pt;
		vector<float> internalData;

		/* Selection */
		int indexGLM;

		/* Attributes */
		glm::vec3 colour = LC_WHITE;
		GLenum mode; // Mode, line or points

		Line2DVecfVecGLMV3(vector<float>* dataVecfPt, vector<glm::dvec3>* dataVecGLMV3Pt, int indexGLM = 0, GLenum mode = GL_LINE_STRIP) {
			this->dataVecfPt = dataVecfPt;
			this->mode = mode;
			this->dataVecGLMV3Pt = dataVecGLMV3Pt;
			this->indexGLM = indexGLM;

			/* Setup Buffers */
			updateInternalData();
			int dataSizeBytes = internalData.size()*sizeof(internalData[0]);
			createAndSetupBuffers(&VAO, &VBO, dataSizeBytes, &internalData[0], 2*sizeof(internalData[0]));

			/* Set Number of Points */
			nPts = internalData.size()/2.0;

		}

		void updateInternalData() {
			/* Creates an internal data store from the current dataVecPt */
			// Clear Previous Data
			internalData.clear();
			// Get maximum length of both vectors
			int len = std::min(dataVecfPt->size(),dataVecGLMV3Pt->size());
			// Update With New Data
			for(int i=0; i<len; i++) {
				internalData.push_back((*dataVecfPt)[i]);
				internalData.push_back((*dataVecGLMV3Pt)[i][indexGLM]);
			}
		}

		void Draw(Shader shader, glm::mat4 axesLimitViewportTrans) {
			// Check if number of points changed
			int newPts = (internalData).size()/2;
			if (newPts != nPts) {
				nPts = newPts;
				// Update buffer and attributes
				glBindBuffer(GL_ARRAY_BUFFER,VBO);
				glBufferData(GL_ARRAY_BUFFER, internalData.size()*sizeof(internalData[0]),&internalData[0],GL_DYNAMIC_DRAW);
			}

			// Draw Plot
			drawData(shader, axesLimitViewportTrans, &VAO, colour, nPts, mode);
		}

		vector<float> getMinMax() {
			// Gets the minimum and maximum values of both x and y for the data
			float xmin = maxFloat;
			float xmax = -maxFloat;
			float ymin = maxFloat;
			float ymax = -maxFloat;
			for (unsigned int i = 0; i<internalData.size()/2.0; i++) {
				float xval = (internalData)[2*i];
				float yval = (internalData)[2*i+1];
				if (xval > xmax) {
					xmax = xval;
				}
				if (xval < xmin) {
					xmin = xval;
				}
				if (yval > ymax) {
					ymax = yval;
				}
				if (yval < ymin) {
					ymin = yval;
				}
			}

			return vector<float> {xmin,xmax,ymin,ymax};
		}
	};
}

#endif /* P_LINE2D_H_ */
