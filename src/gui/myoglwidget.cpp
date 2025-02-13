#include "myoglwidget.h"

#include <QDebug>

MyOglWidget::MyOglWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(nullptr),
      clr_back(0, 0, 0),
      clr_vert(255, 255, 255),
      clr_line(0, 200, 0),
      points(0),
      pointsSize(1),
      dottedLine(2),
      lineSize(1),
      projection(0) {
  setlocale(LC_NUMERIC, "C");
  init_afinne(&mx);
  parse_file(nullptr, &data);
  object = copy_data(&data);
}

MyOglWidget::~MyOglWidget() {
  remove_data(&data);
  remove_data(&object);
  destroy_affine(&mx);
  destroyBuffers();
}

void MyOglWidget::initializeGL() {
  initializeOpenGLFunctions();
  const char *sh_vertex = R"(
        #version 330

        layout (location = 0) in vec3 inPos;

        flat out vec3 startPos;
        out vec3 vertPos;

        uniform mat4 coeffMatrix;

        void main()
        {
            vec4 pos    = coeffMatrix * vec4(inPos, 1.0);
            gl_Position = pos;
            vertPos     = pos.xyz / pos.w;
            startPos    = vertPos;
        }
      )";
  const char *sh_fragment = R"(
        #version 330

        flat in vec3 startPos;
        in vec3 vertPos;

        out vec4 fragColor;

        uniform vec2  u_resolution;
        uniform float u_dashSize;
        uniform float u_gapSize;
        uniform vec4 color;

        void main()
        {
            vec2  dir  = (vertPos.xy-startPos.xy) * u_resolution/2.0;
            float dist = length(dir);

            if (fract(dist / (u_dashSize + u_gapSize)) > u_dashSize/(u_dashSize + u_gapSize))
                discard; 
            fragColor = color;
        }
      )";

  glEnable(GL_DEPTH_TEST);

  m_program = new QOpenGLShaderProgram;
  m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, sh_vertex);
  m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, sh_fragment);

  m_program->bindAttributeLocation("vertex", 0);
  m_program->link();
  m_program->bind();
  coeffMatrixLoc = m_program->uniformLocation("coeffMatrix");
}

void MyOglWidget::resizeGL(int w, int h) { setupProjection(w, h); }

void MyOglWidget::paintGL() {
  glClearColor(clr_back.redF(), clr_back.greenF(), clr_back.blueF(), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  updateBuffer(data.vertexes.matrix);

  m_program->setUniformValue(coeffMatrixLoc, projectionMatrix * cameraMatrix);
  if (vao.isCreated() && vbo.isCreated() && ebo.isCreated()) {
    vao.bind();

    if (dottedLine != 0) {
      m_program->setUniformValue(
          "color", QVector4D(clr_line.redF(), clr_line.greenF(),
                             clr_line.blueF(), clr_line.alphaF()));
      glLineWidth(lineSize);
      int w, h;
      w = width();
      h = height();
      if (dottedLine == 2) {
        m_program->setUniformValue("u_resolution",
                                   QVector2D((float)w, (float)h));
        m_program->setUniformValue("u_dashSize", 10.0f);
        m_program->setUniformValue("u_gapSize", 10.0f);
      } else {
        m_program->setUniformValue("u_resolution",
                                   QVector2D((float)w, (float)h));
        m_program->setUniformValue("u_dashSize", 10.0f);
        m_program->setUniformValue("u_gapSize", 0.0f);
      }

      glDrawElements(GL_LINES, data.full_cnt, GL_UNSIGNED_INT, nullptr);
    }

    if (points != 0) {
      m_program->setUniformValue(
          "color", QVector4D(clr_vert.redF(), clr_vert.greenF(),
                             clr_vert.blueF(), clr_vert.alphaF()));
      glPointSize(pointsSize);

      if (points == 2) {
        glEnable(GL_POINT_SMOOTH);
      } else {
        glDisable(GL_POINT_SMOOTH);
      }

      glDrawArrays(GL_POINTS, 0, data.vertex_count);
    }

    vao.release();
  }
}

void MyOglWidget::mousePressEvent(QMouseEvent *event) {
  emit mousePress(event);
}

void MyOglWidget::mouseMoveEvent(QMouseEvent *event) { emit mouseMove(event); }

void MyOglWidget::initBuffers() {
  makeCurrent();

  if (vao.isCreated() == false) {
    vao.create();
    vao.bind();

    if (vbo.isCreated() == false) {
      vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
      vbo.create();
      vbo.bind();
      vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
      if (data.vertex_count != 0) {
        vbo.allocate(data.vertexes.matrix,
                     data.vertex_count * 3 * sizeof(GLfloat));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                              nullptr);
        glEnableVertexAttribArray(0);
      }
    }

    if (ebo.isCreated() == false) {
      ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
      ebo.create();
      ebo.bind();
      ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
      if (data.facet_count != 0) {
        ebo.allocate(data.facets, data.full_cnt * sizeof(GLuint));
      }
    }

    vao.release();
  }
}

void MyOglWidget::updateBuffer(float *update_to) {
  vbo.bind();
  vbo.write(0, std::move(update_to), data.vertex_count * 3 * sizeof(GLfloat));
  vbo.release();
}

void MyOglWidget::destroyBuffers() {
  makeCurrent();
  if (vao.isCreated()) {
    vao.destroy();
  }
  if (vbo.isCreated()) {
    vbo.destroy();
  }
  if (ebo.isCreated()) {
    ebo.destroy();
  }
}

int MyOglWidget::initModel(QString filepath) {
  std::string str = filepath.toStdString();
  remove_data(&data);
  remove_data(&object);

  int err = parse_file(reinterpret_cast<const char *>(str.c_str()), &data);

  if (!err) {
    normalize_vertex(&data);
    object = copy_data(&data);
    destroyBuffers();
    initBuffers();
  }
  return err;
}

void MyOglWidget::setupProjection(int w, int h) {
  if (w == 0 || h == 0) {
    w = width();
    h = height();
  }

  cameraMatrix.setToIdentity();
  projectionMatrix.setToIdentity();

  if (projection == 0) {
    cameraMatrix.translate(0.0f, 0.0f, -3.0f);
    projectionMatrix.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
  } else {
    float top, bottom, right, left;
    float aspect = (GLfloat)w / h;
    float coeff = 1.3f;

    if (w > h) {
      top = coeff;
      bottom = -coeff;
      right = coeff * aspect;
      left = -right;
    } else {
      right = coeff;
      left = -coeff;
      top = coeff / aspect;
      bottom = -top;
    }

    cameraMatrix.ortho(left, right, bottom, top, -100.0f, 100.0f);
  }
}

int MyOglWidget::createImage(QString filePath, QString format) {
  makeCurrent();
  QImage image = grabFramebuffer();
  int err = image.save(filePath, format.toStdString().c_str());
  doneCurrent();

  return err;
}
