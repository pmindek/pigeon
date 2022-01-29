#include "nnshader.h"
#include <QFile>
#include <QMessageBox>

NNShader::NNShader(QObject *parent) :
    QObject(parent)
{
	this->vsSource = "";
	this->gsSource = "";
	this->fsSource = "";

	this->vsFileName = "";
	this->gsFileName = "";
	this->fsFileName = "";

    this->shader = 0;
	this->watcher = 0;

    //Todo: do not repeat yourself
    //Todo: do not repeat yourself
    this->version = "430";
}

NNShader::NNShader(QString computeFileName, QObject *parent, bool loadCompute, bool buildImmediately) : QObject(parent)
{
	if (loadCompute)
	{
		this->computeFileName = computeFileName;
		this->computeSource = "";
	}
	else
	{
		this->computeFileName = "";
		this->computeSource = computeFileName;
	}

	this->shader = 0;
	this->watcher = 0;
    this->version = "430";
	this->loadShader(buildImmediately);
}

NNShader::NNShader(QString vsFileName, QString fsFileName, QObject *parent, bool loadVS, bool loadFS, bool buildImmediately) :
	QObject(parent)
{
	if (loadVS)
	{
		this->vsFileName = vsFileName;
		this->vsSource = "";
	}
	else
	{
		this->vsFileName = "";
		this->vsSource = vsFileName;
	}

	this->gsFileName = "";
	this->gsSource = "";

	if (loadFS)
	{
		this->fsFileName = fsFileName;
		this->fsSource = "";
	}
	else
	{
		this->fsFileName = "";
		this->fsSource = fsFileName;
	}

	this->shader = 0;
	this->watcher = 0;
    this->version = "430";
	this->loadShader(buildImmediately);
}

NNShader::NNShader(QString vsFileName, QString gsFileName, QString fsFileName, QObject *parent, bool loadVS, bool loadGS, bool loadFS, bool buildImmediately) :
	QObject(parent)
{
	if (loadVS)
	{
		this->vsFileName = vsFileName;
		this->vsSource = "";
	}
	else
	{
		this->vsFileName = "";
		this->vsSource = vsFileName;
	}

	if (loadGS)
	{
		this->gsFileName = gsFileName;
		this->gsSource = "";
	}
	else
	{
		this->gsFileName = "";
		this->gsSource = gsFileName;
	}

	if (loadFS)
	{
		this->fsFileName = fsFileName;
		this->fsSource = "";
	}
	else
	{
		this->fsFileName = "";
		this->fsSource = fsFileName;
	}

	this->shader = 0;
	this->watcher = 0;
    this->version = "430";
	this->loadShader(buildImmediately);
}

NNShader::NNShader(QString vsFileName, QString tcsFileName, QString tesFileName, QString gsFileName, QString fsFileName, QObject *parent,
	bool loadVS, bool loadTCS, bool loadTES, bool loadGS, bool loadFS, bool buildImmediately) : QObject(parent)
{
	if (loadVS)
	{
		this->vsFileName = vsFileName;
		this->vsSource = "";
	}
	else
	{
		this->vsFileName = "";
		this->vsSource = vsFileName;
	}

	if (loadGS)
	{
		this->gsFileName = gsFileName;
		this->gsSource = "";
	}
	else
	{
		this->gsFileName = "";
		this->gsSource = gsFileName;
	}

	if (loadFS)
	{
		this->fsFileName = fsFileName;
		this->fsSource = "";
	}
	else
	{
		this->fsFileName = "";
		this->fsSource = fsFileName;
	}

	if (loadTCS)
	{
		this->tcsFileName = tcsFileName;
		this->tcsSource = "";
	}
	else
	{
		this->tcsFileName = "";
		this->tcsSource = tcsFileName;
	}

	if (loadTES)
	{
		this->tesFileName = tesFileName;
		this->tesSource = "";
	}
	else
	{
		this->tesFileName = "";
		this->tesSource = tesFileName;
	}

	this->shader = 0;
    this->watcher = 0;
    this->version = "430";
	this->loadShader(buildImmediately);
}



QOpenGLShaderProgram *NNShader::compileShader(QByteArray computeShaderSource)
{
	QOpenGLShader *shader = new QOpenGLShader(QOpenGLShader::Compute);
	shader->compileSourceCode(computeShaderSource);

	QOpenGLShaderProgram *program = new QOpenGLShaderProgram;
	program->addShader(shader);
	program->link();
	program->bind();

	return program;
}

QOpenGLShaderProgram *NNShader::compileShader(QByteArray vertexShaderSource, QByteArray fragmentShaderSource)
{
	return NNShader::compileShader(vertexShaderSource, QByteArray(), fragmentShaderSource);
}

QOpenGLShaderProgram *NNShader::compileShader(QByteArray vertexShaderSource, QByteArray geometryShaderSource, QByteArray fragmentShaderSource)
{
	return NNShader::compileShader(vertexShaderSource, QByteArray(), QByteArray(), geometryShaderSource, fragmentShaderSource);
}

QOpenGLShaderProgram *NNShader::compileShader(QByteArray vertexShaderSource , QByteArray tcsShaderSource, QByteArray tesShaderSource, QByteArray geometryShaderSource, QByteArray fragmentShaderSource)
{
	bool geometry = geometryShaderSource.length() > 0;
	bool tcs = tcsShaderSource.length() > 0;
	bool tes = tesShaderSource.length() > 0;
	QOpenGLShader *vShader = new QOpenGLShader(QOpenGLShader::Vertex);
	QOpenGLShader *tcShader = tcs ? new QOpenGLShader(QOpenGLShader::TessellationControl) : 0;
	QOpenGLShader *teShader = tes ? new QOpenGLShader(QOpenGLShader::TessellationEvaluation) : 0;
	QOpenGLShader *gShader = geometry ? new QOpenGLShader(QOpenGLShader::Geometry) : 0;
	QOpenGLShader *fShader = new QOpenGLShader(QOpenGLShader::Fragment);

	/*QGLShader *vShader = new QGLShader(QGLShader::Vertex);
	QGLShader *gShader = geometry ? new QGLShader(QGLShader::Geometry) : 0;
	QGLShader *fShader = new QGLShader(QGLShader::Fragment);*/

	qDebug() << "COMPILING...";

	qDebug() << "Vertex Shader...";
	vShader->compileSourceCode(vertexShaderSource);

	if (tcs)
	{
		qDebug() << "Tesselation Control...";
		tcShader->compileSourceCode(tcsShaderSource);
	}
	if (tes)
	{
		qDebug() << "Tesselation...";
		teShader->compileSourceCode(tesShaderSource);
	}
	if (geometry)
	{
		qDebug() << "Geometry...";
		gShader->compileSourceCode(geometryShaderSource);
	}

	qDebug() << "Fragment...";
	fShader->compileSourceCode(fragmentShaderSource);

	QOpenGLShaderProgram *shader;
	shader = new QOpenGLShaderProgram();
	shader->addShader(vShader);
	if (tcs)
		shader->addShader(tcShader);
	if (tes)
		shader->addShader(teShader);
	if (geometry)
		shader->addShader(gShader);
	shader->addShader(fShader);

	bool ok = shader->link();

	if (ok)
	{
		qDebug() << "DONE.";
	}
	else
	{
		//QMessageBox::critical(0, "Shader Error", "Shader not linked. " + QString(fragmentShaderSource));
		qDebug() << "Shader not linked:";
		qDebug() << QString(fragmentShaderSource);
	}
	shader->bind();

	delete vShader;
	if (geometry)
		delete gShader;
	if (tcs)
		delete tcShader;
	if (tes)
		delete teShader;
	delete fShader;

	if (!shader->hasOpenGLShaderPrograms())
	{
		return 0;
	}

	return shader;
}

QByteArray NNShader::loadSource(QString fileNameOrSource, bool loadFromDisk)
{
	QByteArray sourceArray;

	if (loadFromDisk)
	{
		QFile *sourceFile = new QFile(fileNameOrSource);
		//sourceFile->open(QIODevice::ReadOnly);
		NNU::persistentFileOpen(sourceFile, QIODevice::ReadOnly);

        if(!sourceFile->isOpen())
        {
			/*QMessageBox messageBox;
            messageBox.critical(0,"Error","Cannot open shader file: " + fileNameOrSource);
			messageBox.setFixedSize(500,200);*/
			qWarning() << "Cannot open shader file" << fileNameOrSource;
        }

		sourceArray = sourceFile->readAll();
		sourceFile->close();
		delete sourceFile;
	}
	else
	{
		sourceArray.append(fileNameOrSource);
	}

	return sourceArray;
}

QOpenGLShaderProgram *NNShader::loadAndCompileShader(QString computeFileName)
{
	QByteArray computeSourceArray;
	computeSourceArray = NNShader::loadSource(computeFileName, true);

	qDebug() << "compiling" << computeFileName << "...";

    QOpenGLShaderProgram *shader = 0;

    shader = NNShader::compileShader(computeSourceArray);

    return shader;
}

QOpenGLShaderProgram *NNShader::loadAndCompileShader(QString vsFileName, QString gsFileName, QString fsFileName,
	bool loadVS, bool loadGS, bool loadFS)
{
	return NNShader::loadAndCompileShader(vsFileName, "", "", gsFileName, fsFileName, loadVS, false, false, loadGS, loadFS);
}

QOpenGLShaderProgram *NNShader::loadAndCompileShader(QString vsFileName, QString fsFileName, bool loadVS, bool loadFS)
{
	return NNShader::loadAndCompileShader(vsFileName, "", fsFileName, loadVS, false, loadFS);
}

QOpenGLShaderProgram *NNShader::loadAndCompileShader(QString vsFileName, QString tcsFileName, QString tesFileName, QString gsFileName, QString fsFileName,
	bool loadVS, bool loadTCS, bool loadTES, bool loadGS, bool loadFS)
{
	bool vertex = vsFileName.length() > 0;
	bool tcs = vsFileName.length() > 0;
	bool tes = vsFileName.length() > 0;
	bool geometry = gsFileName.length() > 0;
	bool fragment = fsFileName.length() > 0;

	QByteArray vsSourceArray;
	QByteArray tcsSourceArray;
	QByteArray tesSourceArray;
	QByteArray gsSourceArray;
	QByteArray fsSourceArray;

	if (vertex)
	{
		vsSourceArray = NNShader::loadSource(vsFileName, loadVS);
	}
	if (tcs)
	{
		tcsSourceArray = NNShader::loadSource(tcsFileName, loadTCS);
	}
	if (tes)
	{
		tesSourceArray = NNShader::loadSource(tesFileName, loadTES);
	}
	if (geometry)
	{
		gsSourceArray = NNShader::loadSource(gsFileName, loadGS);
	}
	if (fragment)
	{
		fsSourceArray = NNShader::loadSource(fsFileName, loadFS);
	}


	/*

	if (loadVS)
	{
		QFile *vsSource = new QFile(vsFileName);
		vsSource->open(QIODevice::ReadOnly);
		vsSourceArray = vsSource->readAll();
		vsSource->close();
		delete vsSource;
	}
	else
	{
		vsSourceArray.append(vsFileName);
	}

	if (geometry)
	{
		if (loadGS)
		{
			QFile *gsSource = new QFile(gsFileName);
			gsSource->open(QIODevice::ReadOnly);
			gsSourceArray = gsSource->readAll();
			gsSource->close();
			delete gsSource;
		}
		else
		{
			gsSourceArray.append(gsFileName);
		}
	}

	if (loadFS)
	{
		QFile *fsSource = new QFile(fsFileName);
		fsSource->open(QIODevice::ReadOnly);
		fsSourceArray = fsSource->readAll();
		fsSource->close();
		delete fsSource;
	}
	else
	{
		fsSourceArray.append(fsFileName);
	}*/

	qDebug() << "compiling" << vsFileName << fsFileName << "...";

	QOpenGLShaderProgram *shader = 0;
	if (tcs && tes && geometry)
		shader = NNShader::compileShader(vsSourceArray, tcsSourceArray, tesSourceArray, gsSourceArray, fsSourceArray);
	else if (geometry && !tcs && !tes)
		shader = NNShader::compileShader(vsSourceArray, gsSourceArray, fsSourceArray);
	else
		shader = NNShader::compileShader(vsSourceArray, fsSourceArray);

	return shader;
}






void NNShader::setVertexSource(QString vsSource)
{
	this->vsSource = vsSource;
}

void NNShader::setGeometrySource(QString gsSource)
{
	this->gsSource = gsSource;
}

void NNShader::setFragmentSource(QString fsSource)
{
	this->fsSource = fsSource;
}

void NNShader::setVertexFragmentSources(QString vsSource, QString fsSource)
{
	this->vsSource = vsSource;
	this->fsSource = fsSource;
}

void NNShader::setSources(QString vsSource, QString gsSource, QString fsSource)
{
	this->vsSource = vsSource;
	this->gsSource = fsSource;
	this->fsSource = fsSource;
}


void NNShader::setVertexFileName(QString vsFileName)
{
	this->vsFileName = vsFileName;
}

void NNShader::setGeometryFileName(QString gsFileName)
{
	this->gsFileName = gsFileName;
}

void NNShader::setFragmentFileName(QString fsFileName)
{
	this->fsFileName = fsFileName;
}

void NNShader::setVertexFragmentFileNames(QString vsFileName, QString fsFileName)
{
	this->vsFileName = vsFileName;
	this->fsFileName = fsFileName;
}

void NNShader::setFileNames(QString vsFileName, QString gsFileName, QString fsFileName)
{
	this->vsFileName = vsFileName;
	this->gsFileName = fsFileName;
	this->fsFileName = fsFileName;
}

void NNShader::buildShader()
{
	QOpenGLShaderProgram *newShader = 0;

	// when program = VS + TCS + TES + GS + FS
	if (this->tesControl && this->tesEval && geometry) {
		newShader = NNShader::loadAndCompileShader(this->vsString, this->tcsString, this->tesString, this->gsString, this->fsString,
			this->loadVS, this->loadTCS, this->loadTES, this->loadGS, this->loadFS);
	}
	// when program = VS + GS + FS
	else if (this->geometry && !this->tesControl && !this->tesEval)
	{
		newShader = NNShader::loadAndCompileShader(this->vsString, this->gsString, this->fsString, this->loadVS, this->loadGS, this->loadFS);
	}
	// when program = COMPUTE
	else if (this->compute)
	{
        if (!prefixes.empty())
        {
            QByteArray computeSourceArray;
            computeSourceArray = NNShader::loadSource(computeFileName, true);

            foreach (auto prefix, prefixes)
            {
                computeSourceArray.push_front(prefix.toUtf8() + QString("\n").toUtf8());
            }
            QString versionPrefix("#version ");
            computeSourceArray.push_front(versionPrefix.toUtf8() + version.toUtf8() + QString("\n").toUtf8());

            qDebug() << "compiling" << computeFileName << "...";

            newShader = NNShader::compileShader(computeSourceArray);
        }
        else
        {
            newShader = NNShader::loadAndCompileShader(this->computeString);
        }
    }
	//when program = VS + FS
	else
	{
		newShader = NNShader::loadAndCompileShader(this->vsString, this->fsString, this->loadVS, this->loadFS);
	}

	if (newShader->isLinked())
	{
		if (this->shader != 0)
		{
			delete this->shader;
		}
		this->shader = newShader;
	}
	else
	{
		delete newShader;
	}
}

void NNShader::notifyFileChanges()
{
	if (this->watcher != 0)
		delete this->watcher;
	this->watcher = new QFileSystemWatcher(this->parent());

	connect(this->watcher, SIGNAL(fileChanged(QString)), this, SLOT(notifyChangedFile(QString)));

	if (this->vsFileName.length() > 0)
	{
		this->watcher->addPath(this->vsFileName);
	}
	if (this->gsFileName.length() > 0)
	{
		this->watcher->addPath(this->gsFileName);
	}
	if (this->fsFileName.length() > 0)
	{
		this->watcher->addPath(this->fsFileName);
	}
}

void NNShader::processFileChanges()
{
	if (this->watcher != 0)
		delete this->watcher;
	this->watcher = new QFileSystemWatcher(this);
	connect(this->watcher, SIGNAL(fileChanged(QString)), this, SLOT(processChangedFile(QString)));

	if (this->computeFileName.length() > 0)
	{
		this->watcher->addPath(this->computeFileName);
		this->computeString = this->computeFileName;
		this->loadCompute = true;
	}

	if (this->tcsFileName.length() > 0)
	{
		this->watcher->addPath(this->tcsFileName);
		this->tcsString = this->tcsFileName;
		this->loadTCS = true;
	}

	if (this->tesFileName.length() > 0)
	{
		this->watcher->addPath(this->tesFileName);
		this->tesString = this->tesFileName;
		this->loadTES = true;
	}

	if (this->vsFileName.length() > 0)
	{
		this->watcher->addPath(this->vsFileName);
		this->vsString = this->vsFileName;
		this->loadVS = true;
	}
	if (this->gsFileName.length() > 0)
	{
		this->watcher->addPath(this->gsFileName);
		this->gsString = this->gsFileName;
		this->loadGS = true;
	}
	if (this->fsFileName.length() > 0)
	{
		this->watcher->addPath(this->fsFileName);
		this->fsString = this->fsFileName;
		this->loadFS = true;
	}
}

bool NNShader::loadShader(bool buildImmediately)
{
	this->vertex = this->vsSource.length() > 0 || this->vsFileName.length() > 0;
	this->tesControl = this->tcsSource.length() > 0 || this->tcsFileName.length() > 0;
	this->tesEval = this->tesSource.length() > 0 || this->tesFileName.length() > 0;
	this->geometry = this->gsSource.length() > 0 || this->gsFileName.length() > 0;
	this->fragment = this->fsSource.length() > 0 || this->fsFileName.length() > 0;
	this->compute = this->computeSource.length() > 0 || this->computeFileName.length() > 0;

	if (!this->vertex || !this->fragment)
	{
		if (!this->compute)
		{
			return false;
		}

		if (this->computeSource.length() > 0)
		{
			this->computeString = this->computeSource;
			this->loadCompute = false;
		}
		else
		{
			this->computeString = this->computeFileName;
			this->loadCompute = true;
		}
	}

	if (this->vsSource.length() > 0)
	{
		this->vsString = this->vsSource;
		this->loadVS = false;
	}
	else
	{
		this->vsString = this->vsFileName;
		this->loadVS = true;
	}

	if (this->tcsSource.length() > 0)
	{
		this->tcsString = this->tcsSource;
		this->loadTCS = false;
	}
	else
	{
		this->tcsString = this->tcsFileName;
		this->loadTCS = true;
	}

	if (this->tesSource.length() > 0)
	{
		this->tesString = this->tesSource;
		this->loadTES = false;
	}
	else
	{
		this->tesString = this->tesFileName;
		this->loadTES = true;
	}

	if (this->gsSource.length() > 0)
	{
		this->gsString = this->gsSource;
		this->loadGS = false;
	}
	else
	{
		this->gsString = this->gsFileName;
		this->loadGS = true;
	}

	if (this->fsSource.length() > 0)
	{
		this->fsString = this->fsSource;
		this->loadFS = false;
	}
	else
	{
		this->fsString = this->fsFileName;
		this->loadFS = true;
	}

	if (buildImmediately)
	{
		this->buildShader();
	}

	return this->shader != 0;
}

void NNShader::processChangedFile(QString fileName)
{
	this->buildShader();

	this->notifyChangedFile(fileName);
}

void NNShader::notifyChangedFile(QString fileName)
{
	if (fileName == this->computeFileName)
		emit this->computeShaderChanged();
	if (fileName == this->vsFileName)
		emit this->vertexShaderChanged();
	if (fileName == this->tcsFileName)
		emit this->tesControlShaderChanged();
	if (fileName == this->tesFileName)
		emit this->tesEvaluationShaderChanged();
	if (fileName == this->gsFileName)
		emit this->geometryShaderChanged();
	if (fileName == this->fsFileName)
		emit this->fragmentShaderChanged();

	emit this->shaderReloaded(fileName);
}

void NNShader::bind()
{
   if (this->shader != 0)
       if(this->shader->isLinked())
            this->shader->bind();

   if (!this->shader->isLinked())
   {
       qDebug() << "Shader" << this->computeFileName << "is not linked";
   }
}

void NNShader::release()
{
	if (this->shader != 0)
		this->shader->release();
}

QOpenGLShaderProgram *NNShader::getShader()
{
	return this->shader;
}

void NNShader::addPrefixes(std::vector<QString> prefixes)
{
    this->prefixes.insert(this->prefixes.end(), prefixes.begin(), prefixes.end());
}

void NNShader::addPrefix(QString p)
{
    prefixes.push_back(p);
}
