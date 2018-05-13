#include "stdafx.h"
#include "Model.h"


Model::Model()
{
	vertCount = 0;
	vertArr = 0;
}


Model::~Model()
{
}

bool Model::buffer(string objFile)
{
	//declare collections
	vector<vec3> locations;
	vector<vec2> uvs;
	vector<vec3> normals;
	vector<VertInd> vertInds;

	//create input stream
	ifstream inFile;
	inFile.open(objFile);

	//check if stream is open
	string line;
	stringstream stream;
	float data1, data2, data3;
	string label;
	if (inFile.is_open())
	{
		while (!inFile.eof())
		{
			//reset data
			data1, data2, data3 = 0;

			//get line from file and setup stream
			getline(inFile, line);
			stream = stringstream(line);

			//check if we ignore the line
			if (line.length() != 0 && line[0] == 'v') //locations uvs and normals
			{
				//check subparts
				if (line[1] == 't') //uv
				{
					//pull data
					stream >> label >> data1 >> data2;

					//add to uvs
					uvs.push_back(vec2(data1, data2));
				}
				else if (line[1] == 'n') //normal
				{
					//pull data
					stream >> label >> data1 >> data2 >> data3;

					//add to normals
					normals.push_back(vec3(data1, data2, data3));
				}
				else //location
				{
					//pull data
					stream >> label >> data1 >> data2 >> data3;

					//add to locations
					locations.push_back(vec3(data1, data2, data3));
				}
			}
			else if (line.length() != 0 && line[0] == 'f') //indicies
			{
				//loop three times!
				for (int i = 0; i < 3; i++)
				{
					char slash; //to hold slash char... self explanitory honestly

					//pull data
					if (i == 0)
					{
						stream >>  label >> data1 >> slash >> data2 >> slash >> data3;
					}
					else
					{
						stream >> data1 >> slash >> data2 >> slash >> data3;
					}
					

					//decrement data
					data1--;
					data2--;
					data3--;

					//add to vertInds
					VertInd indicies =
					{
						data1, data2, data3
					};
					vertInds.push_back(indicies);
				}
			}
		}

		//close stream
		inFile.close();
	}

	//get vertcount based on indicies
	vertCount = vertInds.size();

	//duplicate verticies into a single buffer
	//vector<Vertex> vertBufData(vertCount);
	vector<Vertex> vertBufData;
	for (unsigned int i = 0; i < vertCount; i++)
	{
		/*vertBufData[i].location = locations[vertInds[i].locInd];
		vertBufData[i].uv = uvs[vertInds[i].uvInd];
		vertBufData[i].normal = normals[vertInds[i].normInd];*/

		Vertex vert = 
		{
			locations[vertInds[i].locInd],
			uvs[vertInds[i].uvInd],
			normals[vertInds[i].normInd]
		};

		vertBufData.push_back(vert);
	} 

	//setup variables for buffer
	GLuint vertBuf;

	//generate buffers
	glGenVertexArrays(1, &vertArr);
	glGenBuffers(1, &vertBuf);

	//bind buffers
	glBindVertexArray(vertArr);
	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);

	//store data in buffers
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertCount, &vertBufData[0], GL_STATIC_DRAW);

	//describe buffer layout
	glEnableVertexAttribArray(0); //location
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1); //UV
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(vec3));

	glEnableVertexAttribArray(1); //normal
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(vec3) * sizeof(vec3)));

	//return true for good run
	return true;
}

void Model::render()
{
	glBindVertexArray(vertArr);
		glDrawArrays(GL_TRIANGLES, 0, vertCount);
}
