#ifndef CLASS_DEFINE_OFF_File
#define CLASS_DEFINE_OFF_File

#include "Common.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

class OFF_File
{
private:
	Vertex* m_vertices;
	Face* m_faces;

	float* m_vertex_array;
	float* m_normal_array;
	unsigned int* m_faces_array;
	unsigned int m_number_of_vertices;
	unsigned int m_number_of_faces;

	Vertex* m_center;
	Vertex* m_minCorner;
	Vertex* m_maxCorner;

	bool isAttrified(int a,int b,int c);
	void normalize_Face(Face* face);
	void normalize_Vertex(Vertex* vertex);
	
public:
	OFF_File();
	~OFF_File();

	inline float* get_Vertex_Array() { return m_vertex_array; }
	inline float* get_Normal_Array() { return m_normal_array; }
	inline unsigned int* get_Faces_Array() { return m_faces_array; }
	inline unsigned int get_Number_Of_Vertices() { return m_number_of_vertices; }
	inline unsigned int get_Number_Of_Faces() { return m_number_of_faces; }
	inline const Vertex* get_center() const { return m_center; }
	inline const Vertex* get_minCorner() const { return m_minCorner; }
	inline const Vertex* get_maxCorner() const { return m_maxCorner; }

	void load_OFF(std::string filename);
};

#endif
