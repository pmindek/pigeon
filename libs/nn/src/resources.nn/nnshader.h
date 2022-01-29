#ifndef NNSHADER_H
#define NNSHADER_H

#include "nn_global.h"
#include <QObject>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QFileSystemWatcher>
#include "nnu.h"

//TODO: subclass QOpenGLShaderProgram
class NN_API NNShader : public QObject
{
	Q_OBJECT
public:
	explicit NNShader(QObject *parent = 0);
	NNShader(QString computeFileName, QObject *parent = 0, bool loadCompute = true, bool buildImmediately = true);
	NNShader(QString vsFileName, QString fsFileName,
		QObject *parent = 0, bool loadVS = true, bool loadFS = true, bool buildImmediately = true);
	NNShader(QString vsFileName, QString gsFileName, QString fsFileName,
		QObject *parent = 0, bool loadVS = true, bool loadGS = true, bool loadFS = true, bool buildImmediately = true);
	NNShader(QString vsFileName, QString tcsFileName, QString tesFileName, QString gsFileName, QString fsFileName,
		QObject *parent = 0, bool loadVS = true, bool loadTCS = true, bool loadTES = true, bool loadGS = true, bool loadFS = true, bool buildImmediately = true);


	static QOpenGLShaderProgram *compileShader(QByteArray computeShaderSource);
	static QOpenGLShaderProgram *compileShader(QByteArray vertexShaderSource, QByteArray fragmentShaderSource);
	static QOpenGLShaderProgram *compileShader(QByteArray vertexShaderSource, QByteArray geometryShaderSource, QByteArray fragmentShaderSource);
	static QOpenGLShaderProgram *compileShader(QByteArray vertexShaderSource, QByteArray tcsShaderSource, QByteArray tesShaderSource, QByteArray geometryShaderSource, QByteArray fragmentShaderSource);

	static QByteArray loadSource(QString fileNameOrSource, bool loadFromDisk);
	static QOpenGLShaderProgram *loadAndCompileShader(QString computeFileName);
	static QOpenGLShaderProgram *loadAndCompileShader(QString vsFileName, QString fsFileName, bool loadVS = true, bool loadFS = true);
	static QOpenGLShaderProgram *loadAndCompileShader(QString vsFileName, QString gsFileName, QString fsFileName, bool loadVS = true, bool loadGS = true, bool loadFS = true);
	static QOpenGLShaderProgram *loadAndCompileShader(QString vsFileName, QString tcsFileName, QString tesFileName, QString gsFileName, QString fsFileName,
		bool loadVS = true, bool loadTCS = true, bool loadTES = true, bool loadGS = true, bool loadFS = true);


	void setVertexSource(QString vsSource);
	void setGeometrySource(QString gsSource);
	void setFragmentSource(QString fsSource);
	void setVertexFragmentSources(QString vsSource, QString fsSource);
	void setSources(QString vsSource, QString gsSource, QString fsSource);

	void setVertexFileName(QString vsFileName);
	void setGeometryFileName(QString gsFileName);
	void setFragmentFileName(QString fsFileName);
	void setVertexFragmentFileNames(QString vsFileName, QString fsFileName);
	void setFileNames(QString vsFileName, QString gsFileName, QString fsFileName);

	void notifyFileChanges();
	void processFileChanges();

	bool loadShader(bool buildImmediately = true);
	void buildShader();

	void bind();
	void release();
	QOpenGLShaderProgram *getShader();

    void addPrefixes(std::vector<QString> prefixes);
    void addPrefix(QString p);

private:

    std::vector<QString> prefixes;

	QString vsFileName;
	QString tcsFileName;
	QString tesFileName;
	QString gsFileName;
	QString fsFileName;
	QString computeFileName;

	QString vsSource;
	QString tcsSource;
	QString tesSource;
	QString gsSource;
	QString fsSource;
	QString computeSource;
	QOpenGLShaderProgram *shader;

	bool vertex;
	bool tesControl;
	bool tesEval;
	bool geometry;
	bool fragment;
	bool compute;

	bool loadVS;
	bool loadTCS;
	bool loadTES;
	bool loadGS;
	bool loadFS;
	bool loadCompute;

	QString vsString;
	QString tcsString;
	QString tesString;
	QString gsString;
	QString fsString;
	QString computeString;

    QFileSystemWatcher *watcher;
    QString version;

signals:
	void vertexShaderChanged();
	void geometryShaderChanged();
	void fragmentShaderChanged();
	void computeShaderChanged();
	void tesControlShaderChanged();
	void tesEvaluationShaderChanged();
	void shaderReloaded(QString);
	
public slots:
	void processChangedFile(QString);
	void notifyChangedFile(QString);
};

#endif // NNSHADER_H
