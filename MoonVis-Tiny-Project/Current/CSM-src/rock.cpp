
///////////////////////////////////////////////////

struct RockModel
{
	int _ID;
	CString _name;
	CString _path;
	nv::Model *_pNvModel;
	nv::Image *_pNvTex;
	CBitmap *_pPreviewBmp;

	RockModel() : _ID(-1), _pNvModel(NULL), _pNvTex(NULL) {};
	RockModel(int ID, const char* path)
		: _ID(ID), _pNvModel(NULL), _pPreviewBmp(NULL)
	{
		_path = (CString)path;
		int nStart = 0;
		int nFound = 0;
		while (-1 != nStart)
		{
			nFound = nStart + 1;
			nStart = _path.Find(_T("\\"), nStart + 1);
		}
		_name = _path.Mid(nFound);

		load(path);
	}
	~RockModel()
	{
		if (NULL != _pPreviewBmp)
		{
			_pPreviewBmp->Detach();
			delete _pPreviewBmp;
		}
	}

	void setID(int ID)
	{
		_ID = ID;
	}

	bool load(const char* path)
	{
		if (NULL != _pNvModel)
		{
			return true;
		}

		char* pathL = new char[strlen(path) * 2]; // 乘以求保守

		// 载入obj文件
		strcpy(pathL, path);
		strcat(pathL, "\\model.obj");
		_pNvModel = new nv::Model;
		if (!_pNvModel->loadModelFromFile(&pathL[0]))
		{
			if (!_pNvModel->loadModelFromFile(&pathL[3]))
			{
				AfxMessageBox(_T("载入石块obj文件失败！"));
				return false;
			}
		}
		_pNvModel->rescale(1.0f);
		_pNvModel->compileModel();

		// 载入纹理贴图
		strcpy(pathL, path);
		strcat(pathL, "\\texture.png");
		_pNvTex = new nv::Image;
		if (!_pNvTex->loadImageFromFile(&pathL[0]))
		{
			if (!_pNvTex->loadImageFromFile(&pathL[3]))
			{
				AfxMessageBox(_T("载入石块纹理失败！"));
				return false;
			}
		}

		CString strPath = _path + _T("\\screenshot.jpg");
		_pPreviewBmp = new CBitmap;
		CImage img;
		if (S_OK != img.Load(strPath))
		{
			strPath = (CString)&path[3];
			if (S_OK != img.Load(strPath))
			{
				AfxMessageBox(_T("载入石块预览图失败！"));
				return false;
			}
		}

		_pPreviewBmp->Attach(img.Detach());

		return true;
	}
};

// used for openGL rendering
struct RockModelGL
{
	int _ID;
	GLuint _vboID;
	GLuint _eboID;
	GLuint _texID;

	int _stride;
	int _normalOffset;
	int _texCoordOffset;
	int _positionSize;
	int _texCoordSize;
	int _indexCount;

	RockModelGL() : _ID(-1), _vboID(0), _eboID(0), _texID(0) {};
	RockModelGL(int ID, RockModel *pModel)
		: _ID(ID), _vboID(0), _eboID(0), _texID(0)
	{
		load(pModel);
	}
	~RockModelGL()
	{
		if (glIsBuffer(_vboID))
		{
			glDeleteBuffers(1, &_vboID);
		}
		if (glIsBuffer(_eboID))
		{
			glDeleteBuffers(1, &_eboID);
		}
		if (glIsTexture(_texID))
		{
			glDeleteTextures(1, &_texID);
		}
	}

	void setID(int ID)
	{
		_ID = ID;
	}

	bool load(RockModel *pModel)
	{
		nv::Model *pNvModel = pModel->_pNvModel;
		nv::Image *pNvTex = pModel->_pNvTex;

		int totalVertexSize = pNvModel->getCompiledVertexCount() * pNvModel->getCompiledVertexSize()*sizeof(GLfloat);
		int totalIndexSize = pNvModel->getCompiledIndexCount() * sizeof(GLuint);

		glGenBuffers(1, &_vboID);
		glBindBuffer(GL_ARRAY_BUFFER, _vboID);
		glBufferData(GL_ARRAY_BUFFER, totalVertexSize, pNvModel->getCompiledVertices(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &_eboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndexSize, pNvModel->getCompiledIndices(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		_stride = pNvModel->getCompiledVertexSize() * sizeof(GL_FLOAT);
		_normalOffset = pNvModel->getCompiledNormalOffset() * sizeof(GL_FLOAT);
		_texCoordOffset = pNvModel->getCompiledTexCoordOffset() * sizeof(GL_FLOAT);
		_positionSize = pNvModel->getPositionSize();
		_texCoordSize = pNvModel->getTexCoordSize();
		_indexCount = pNvModel->getIndexCount();

		glGenTextures(1, &_texID);
		glBindTexture(GL_TEXTURE_2D, _texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, pNvTex->getInternalFormat(), pNvTex->getWidth(), pNvTex->getHeight(), 0, pNvTex->getFormat(), pNvTex->getType(), pNvTex->getLevel(0));
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	void draw()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eboID);

		glVertexPointer(_positionSize, GL_FLOAT, _stride, NULL);
		glNormalPointer(GL_FLOAT, _stride, (GLubyte*)NULL + _normalOffset);
		glTexCoordPointer(_texCoordSize, GL_FLOAT, _stride, (GLubyte*)NULL + _texCoordOffset);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _texID);

		glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};
