#ifndef MYOGLWIDGET_H
#define MYOGLWIDGET_H

#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>
// #include <GLFW/glfw3.h>
#include "widget.h"

extern "C" {
#include "./../modules/include/common.h"
}

class MyOglWidget : public QOpenGLWidget, public QOpenGLFunctions {
  Q_OBJECT
 private:
  int coeffMatrixLoc = 0;
  QOpenGLShaderProgram *m_program;
  QOpenGLBuffer vbo;
  QOpenGLBuffer ebo;
  QOpenGLVertexArrayObject vao;
  QMatrix4x4 projectionMatrix;
  QMatrix4x4 cameraMatrix;

 public:
  explicit MyOglWidget(QWidget *parent = 0);
  ~MyOglWidget();
  GLfloat rotationX;
  GLfloat rotationY;
  GLfloat rotationZ;
  void initBuffers();
  void updateBuffer(float *update_to);
  void destroyBuffers();

  int initModel(QString filepath);
  void setupProjection(int w, int h);
  int createImage(QString filePath, QString format);
  data_t data;
  data_t object;
  afinne_t mx;
  QColor clr_back;
  QColor clr_vert;
  QColor clr_line;
  int points;
  int pointsSize;
  int dottedLine;
  int lineSize;
  int projection;

 signals:
  void mousePress(QMouseEvent *event);
  void mouseMove(QMouseEvent *event);

 protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
};

#endif  // MYOGLWIDGET_H
