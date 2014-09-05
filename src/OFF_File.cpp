#include "OFF_File.h"

OFF_File::OFF_File()
{
	m_vertices = NULL;
	m_faces = NULL;
	m_vertex_array = NULL;
	m_normal_array = NULL;
	m_faces_array = NULL;
	m_center = NULL;
}

OFF_File::~OFF_File()
{
	if (m_vertices != NULL)
		delete[] m_vertices;
	if (m_faces != NULL)
		delete[] m_faces;
	if (m_vertex_array != NULL)
		delete[] m_vertex_array;
	if (m_normal_array != NULL)
		delete[] m_normal_array;
	if (m_faces_array != NULL)
		delete[] m_faces_array;
	if (m_center != NULL)
		delete m_center;
}

void OFF_File::load_OFF(std::string filename)
{
	std::cout << "Loading OFF: " << filename << "..." << std::endl;
	std::ifstream fs(filename.c_str());
	std::string current_element;

	// Read the header
	fs >> current_element;
	if (current_element != "OFF")
		return;

	fs >> current_element;
	m_number_of_vertices = atoi(current_element.c_str());
	fs >> current_element;
	m_number_of_faces = atoi(current_element.c_str());
	fs >> current_element;
	std::cout << "Vertices: " << m_number_of_vertices << std::endl;
	std::cout << "Faces: " << m_number_of_faces << std::endl;

	m_vertices = new Vertex[m_number_of_vertices];
	m_faces = new Face[m_number_of_faces];
	
	// Read the vertices
	std::string x,y,z;
	std::string intensity,confidence;

	float max_x,max_y,max_z,
			 min_x,min_y,min_z;
	float current_x,current_y,current_z;
	for (unsigned int i = 0;i < m_number_of_vertices;i++)
	{
		fs >> x >> y >> z;
		current_x = (float)atof(x.c_str());
		current_y = (float)atof(y.c_str());
		current_z = (float)atof(z.c_str());

		if (i == 0)
		{
			max_x = min_x = current_x;
			max_y = min_y = current_y;
			max_z = min_z = current_z;
		}
		else
		{
			if (current_x > max_x)
				max_x = current_x;
			if (current_x < min_x)
				min_x = current_x;

			if (current_y > max_y)
				max_y = current_y;
			if (current_y < min_y)
				min_y = current_y;

			if (current_z > max_z)
				max_z = current_z;
			if (current_z < min_z)
				min_z = current_z;
		}

		m_vertices[i].m_arrPosition[0] = current_x;
		m_vertices[i].m_arrPosition[1] = current_y;
		m_vertices[i].m_arrPosition[2] = current_z;
			
		m_vertices[i].m_arrNormal[0] = 0.0f;
		m_vertices[i].m_arrNormal[1] = 0.0f;
		m_vertices[i].m_arrNormal[2] = 0.0f;
	}
	
	// Set the center
	m_center = new Vertex();
	m_center->m_arrPosition[0] = (max_x + min_x) / 2.0f;
	m_center->m_arrPosition[1] = (max_y + min_y) / 2.0f;
	m_center->m_arrPosition[2] = (max_z + min_z) / 2.0f;
		
	// Set the min max corners
	m_minCorner = new Vertex();
	m_minCorner->m_arrPosition[0] = min_x;
	m_minCorner->m_arrPosition[1] = min_y;
	m_minCorner->m_arrPosition[2] = min_z;
	
	m_maxCorner = new Vertex();
	m_maxCorner->m_arrPosition[0] = max_x;
	m_maxCorner->m_arrPosition[1] = max_y;
	m_maxCorner->m_arrPosition[2] = max_z;

	// Read the faces	
	std::string n,a,b,c;
	

	for (unsigned int i = 0;i < m_number_of_faces;i++)
	{
		fs >> n >> a >> b >> c;
		m_faces[i].a = atoi(a.c_str());
		m_faces[i].b = atoi(b.c_str());
		m_faces[i].c = atoi(c.c_str());
		normalize_Face(&m_faces[i]);
	}

	// Normalize the normals on all vertices
	for (unsigned int i = 0;i < m_number_of_vertices;i++)
 		normalize_Vertex(&m_vertices[i]);
	
	// Create arrays
	m_vertex_array = new float[m_number_of_vertices*3];
	m_normal_array = new float[m_number_of_vertices*3];
	m_faces_array = new unsigned int[m_number_of_faces*3];

	for (unsigned int i = 0;i < m_number_of_vertices;i++)
	{
		m_vertex_array[i*3] = m_vertices[i].m_arrPosition[0];
		m_vertex_array[i*3+1] = m_vertices[i].m_arrPosition[1];
		m_vertex_array[i*3+2] = m_vertices[i].m_arrPosition[2];

		m_normal_array[i*3] = m_vertices[i].m_arrNormal[0];
		m_normal_array[i*3+1] = m_vertices[i].m_arrNormal[1];
		m_normal_array[i*3+2] = m_vertices[i].m_arrNormal[2];
	}

	for (unsigned int i = 0;i < m_number_of_faces;i++)
	{
		m_faces_array[i*3] = m_faces[i].a;
		m_faces_array[i*3 + 1] = m_faces[i].b;
		m_faces_array[i*3 + 2] = m_faces[i].c;
	}

	fs.close();
	std::cout << filename << " loading complete." << std::endl << std::endl;
}

void OFF_File::normalize_Face(Face* face)
{
	float V1[] = {0,0,0};
	float V2[] = {0,0,0};
	float Normal[] = {0,0,0};

	Vertex* P1 = &m_vertices[face->a];
	Vertex* P2 = &m_vertices[face->b];
	Vertex* P3 = &m_vertices[face->c];

	V1[0] = P2->m_arrPosition[0] - P1->m_arrPosition[0];
	V1[1] = P2->m_arrPosition[1] - P1->m_arrPosition[1];
	V1[2] = P2->m_arrPosition[2] - P1->m_arrPosition[2];

	V2[0] = P3->m_arrPosition[0] - P1->m_arrPosition[0];
	V2[1] = P3->m_arrPosition[1] - P1->m_arrPosition[1];
	V2[2] = P3->m_arrPosition[2] - P1->m_arrPosition[2];

	Normal[0] = (V1[1]*V2[2]) - (V1[2]*V2[1]);
	Normal[1] = (V1[2]*V2[0]) - (V1[0]*V2[2]);
	Normal[2] = (V1[0]*V2[1]) - (V1[1]*V2[0]);

	// Normalize the normal
	float vlen;
	vlen = sqrt( (Normal[0] * Normal[0]) + (Normal[1] * Normal[1]) + (Normal[2] * Normal[2]) );
	
	Normal[0] = Normal[0] / vlen;
	Normal[1] = Normal[1] / vlen;
	Normal[2] = Normal[2] / vlen;
	
	// Add the normal to the vertices
	P1->m_arrNormal[0] = P1->m_arrNormal[0] + Normal[0];
	P2->m_arrNormal[0] = P2->m_arrNormal[0] + Normal[0];
	P3->m_arrNormal[0] = P3->m_arrNormal[0] + Normal[0];
	
	P1->m_arrNormal[1] = P1->m_arrNormal[1] + Normal[1];
	P2->m_arrNormal[1] = P2->m_arrNormal[1] + Normal[1];
	P3->m_arrNormal[1] = P3->m_arrNormal[1] + Normal[1];
	
	P1->m_arrNormal[2] = P1->m_arrNormal[2] + Normal[2];
	P2->m_arrNormal[2] = P2->m_arrNormal[2] + Normal[2];
	P3->m_arrNormal[2] = P3->m_arrNormal[2] + Normal[2];
}

void OFF_File::normalize_Vertex(Vertex* vertex)
{
	float vlen = (float)sqrt( (vertex->m_arrNormal[0] * vertex->m_arrNormal[0]) + (vertex->m_arrNormal[1] * vertex->m_arrNormal[1]) + (vertex->m_arrNormal[2] * vertex->m_arrNormal[2]) );
	if (vlen != 0)
	{
		vertex->m_arrNormal[0] = vertex->m_arrNormal[0] / vlen;
		vertex->m_arrNormal[1] = vertex->m_arrNormal[1] / vlen;
		vertex->m_arrNormal[2] = vertex->m_arrNormal[2] / vlen;
	}
	
	
}