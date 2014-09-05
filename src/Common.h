
// a safe delete for single
#define SafeDelete(x) { if (x) delete x; x=NULL; };

// a safe delete for an array
#define SafeDeleteArray(x) {if (x!=NULL) delete[] x; x=NULL;}

struct Vertex
{
	float m_arrPosition[3];
	float m_arrNormal[3];
};

typedef struct Face
{
	unsigned int a;
	unsigned int b;
	unsigned int c;
}Face;