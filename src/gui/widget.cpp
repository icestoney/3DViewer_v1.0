#include "widget.h"

#include <iostream>

#include "./gif.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      periodicTimer(new QTimer(this)),
      countdownTimer(new QTimer(this)),
      gifCount(0),
      gifFps(10),
      gifLength(5),
      dir(QFileInfo(QString(__FILE__)).absoluteDir()),
      screenDir(".images"),
      absScreenDir(dir.absolutePath() + QString("/") + screenDir +
                   QString("/")),
      recording(false),
      saveImg(QFileDialog(this, "Save image as", QDir::homePath())),
      saveGif(QFileDialog(this, "Save gif as", QDir::homePath(),
                          "GIF Files (*.gif)")),
      ui(new Ui::Widget) {
  setAcceptDrops(true);
  ui->setupUi(this);

  saveGif.setAcceptMode(QFileDialog::AcceptSave);
  saveImg.setAcceptMode(QFileDialog::AcceptSave);
  saveImg.setNameFilters(QStringList("BMP Files (*.bmp)")
                         << "JPEG Files (*.jpeg)");
  connect(ui->widget, &MyOglWidget::mousePress, this, &Widget::slotMousePress);
  connect(ui->widget, &MyOglWidget::mouseMove, this, &Widget::slotMouseMove);
  connect(periodicTimer, &QTimer::timeout, this, &Widget::createSnapshot);
  connect(countdownTimer, &QTimer::timeout, this, &Widget::countDown);
  ui->no_vertexes->setChecked(1);
  ui->strip_facets->setChecked(1);
  ui->central_projection->setChecked(1);
  loadSettings();
}

Widget::~Widget() { delete ui; }

void Widget::dragEnterEvent(QDragEnterEvent *event) {
  // Check if the event contains URLs (files)
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();  // Accept the drag event
  }
}

void Widget::dropEvent(QDropEvent *event) {
  // Get the list of dropped files
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QList<QUrl> urlList = mimeData->urls();
    // QStringList filePaths;
    QString filePath = ui->filePathEdit->text();

    for (const QUrl &url : urlList) {
      filePath = url.toLocalFile();  // Convert URL to local file path
      ui->filePathEdit->setText(filePath);
    }
    on_filePathEdit_textEdited(filePath);
    event->acceptProposedAction();  // Accept the drop event
  }
}

void Widget::closeEvent(QCloseEvent *event) {
  saveSettings();
  event->accept();
}

void Widget::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  on_filePathEdit_textEdited(ui->filePathEdit->text());
}

void Widget::slotMousePress(QMouseEvent *event) {
  startPos = event->pos();
  if (event->button() == Qt::LeftButton) {
    leftMouse = true;
    rightMouse = false;
  } else if (event->button() == Qt::RightButton) {
    leftMouse = false;
    rightMouse = true;
  } else {
    leftMouse = false;
    rightMouse = false;
  }
}

void Widget::slotMouseMove(QMouseEvent *event) {
  bool shiftPressed = (event->modifiers() & Qt::ShiftModifier) != 0;
  int temp_X = ui->rotation_slider_X->value();
  int temp_Y = ui->rotation_slider_Y->value();
  int temp_Z = ui->rotation_slider_Z->value();
  if (leftMouse) {
    QPoint offset = event->pos() - startPos;
    if (shiftPressed) {
      ui->move_slider_X->setValue(ui->move_slider_X->value() +
                                  (offset.x() / 2));
      ui->move_slider_Y->setValue(ui->move_slider_Y->value() -
                                  (offset.y() / 2));
    } else {
      if (ui->rotation_slider_X->value() == 360)
        ui->rotation_slider_X->setValue(0);
      if (ui->rotation_slider_Y->value() == 360)
        ui->rotation_slider_Y->setValue(0);
      if (temp_X == -1) ui->rotation_slider_X->setValue(359);
      if (temp_Y == -1) ui->rotation_slider_Y->setValue(359);
      ui->rotation_slider_X->setValue(ui->rotation_slider_X->value() +
                                      (offset.y() / 2));
      ui->rotation_slider_Y->setValue(ui->rotation_slider_Y->value() +
                                      (offset.x() / 2));
      ui->widget->rotationX = ui->rotation_slider_X->value();
      ui->widget->rotationY = ui->rotation_slider_Y->value();
    }
    startPos = event->pos();
  } else if (rightMouse) {
    QPoint offset = event->pos() - startPos;
    if (shiftPressed) {
      ui->move_slider_X->setValue(ui->move_slider_X->value() +
                                  (offset.x() / 2));
      ui->move_slider_Z->setValue(ui->move_slider_Z->value() +
                                  (offset.y() / 2));
    } else {
      if (ui->rotation_slider_Z->value() == 360)
        ui->rotation_slider_Z->setValue(0);
      if (ui->rotation_slider_Y->value() == 360)
        ui->rotation_slider_Y->setValue(0);
      if (temp_Z == -1) ui->rotation_slider_Z->setValue(359);
      if (temp_Y == -1) ui->rotation_slider_Y->setValue(359);
      ui->rotation_slider_Y->setValue(ui->rotation_slider_Y->value() -
                                      (offset.x() / 2));
      ui->rotation_slider_Z->setValue(ui->rotation_slider_Z->value() -
                                      (offset.y() / 2));
      ui->widget->rotationY = ui->rotation_slider_Y->value();
      ui->widget->rotationZ = ui->rotation_slider_Z->value();
    }
    startPos = event->pos();
  }
}

void Widget::wheelEvent(QWheelEvent *event) {
  int y = event->angleDelta().y();
  if (ui->scale->value() + y < 100 || ui->scale->value() < 200) {
    y = int(event->angleDelta().y() / 10);
  }
  ui->scale->setValue(ui->scale->value() + y);
}

void Widget::saveSettings() {
  QSettings settings("S21_3D_Viewer", "S21_3D_ViewerApp");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("back_color", ui->widget->clr_back);
  settings.setValue("vertex_color", ui->widget->clr_vert);
  settings.setValue("line_color", ui->widget->clr_line);
  settings.setValue("points", ui->widget->points);
  settings.setValue("point_size", ui->widget->pointsSize);
  settings.setValue("lines", ui->widget->dottedLine);
  settings.setValue("line_size", ui->widget->lineSize);
  settings.setValue("projection", ui->widget->projection);
  settings.setValue("filename", ui->filePathEdit->text());
}

void Widget::loadSettings() {
  QSettings settings("S21_3D_Viewer", "S21_3D_ViewerApp");

  if (settings.contains("geometry")) {
    restoreGeometry(settings.value("geometry").toByteArray());
    ui->widget->clr_back = settings.value("back_color").value<QColor>();
    ui->widget->clr_vert = settings.value("vertex_color").value<QColor>();
    ui->widget->clr_line = settings.value("line_color").value<QColor>();
    ui->widget->points = settings.value("points").value<int>();
    ui->widget->pointsSize = settings.value("point_size").value<int>();
    ui->widget->dottedLine = settings.value("lines").value<int>();
    ui->widget->lineSize = settings.value("line_size").value<int>();
    ui->widget->projection = settings.value("projection").value<int>();
    ui->filePathEdit->setText(settings.value("filename").value<QString>());
  }

  setFrontSettings();
  ui->widget->update();
}

void Widget::setFrontSettings() {
  set_background_color(ui->widget->clr_back);
  set_vertex_color(ui->widget->clr_vert);
  set_facet_color(ui->widget->clr_line);

  if (ui->widget->points == 0) {
    ui->no_vertexes->setChecked(1);
  } else if (ui->widget->points == 1) {
    ui->square_vertexes->setChecked(1);
  } else {
    ui->circle_vertexes->setChecked(1);
  }

  if (ui->widget->dottedLine == 0) {
    ui->no_facets->setChecked(1);
  } else if (ui->widget->dottedLine == 1) {
    ui->flat_facets->setChecked(1);
  } else {
    ui->strip_facets->setChecked(1);
  }

  if (ui->widget->projection == 0) {
    ui->central_projection->setChecked(1);
  } else {
    ui->parallel_projection->setChecked(1);
  }

  ui->vertex_size->setValue(ui->widget->pointsSize);
  ui->facet_size->setValue(ui->widget->lineSize);
}

void Widget::setSettings() {
  QString vertexesC =
      "Вершины: " + QString::number(ui->widget->data.vertex_count);
  ui->vertexes_count->setText(vertexesC);
  QString facetsC =
      "       Рёбра: " + QString::number(ui->widget->data.facet_count);
  ui->facets_count->setText(facetsC);
  ui->rotation_X->setValue(0);
  ui->rotation_slider_X->setValue(0);
  ui->rotation_Y->setValue(0);
  ui->rotation_slider_Y->setValue(0);
  ui->rotation_Z->setValue(0);
  ui->rotation_slider_Z->setValue(0);
  ui->scale->setValue(100);
  ui->scale_slider->setValue(0);
  ui->move_X->setValue(0);
  ui->move_slider_X->setValue(0);
  ui->move_Y->setValue(0);
  ui->move_slider_Y->setValue(0);
  ui->move_Z->setValue(0);
  ui->move_slider_Z->setValue(0);
  ui->widget->clr_back = ui->bkg_color->palette().color(QPalette::Window);
  ui->widget->clr_vert = ui->vertex_color->palette().color(QPalette::Window);
  ui->widget->clr_line = ui->facet_color->palette().color(QPalette::Window);
}

void Widget::on_Load_file_clicked() {
  QString file_name = QFileDialog::getOpenFileName(this, "Select file",
                                                   QDir::homePath(), "*.obj");
  if (!file_name.isEmpty()) {
    ui->filePathEdit->setText(file_name);
    on_filePathEdit_textEdited(file_name);
  }
}

void Widget::on_filePathEdit_textEdited(const QString &arg1) {
  if (arg1 != QString("")) {
    int err = ui->widget->initModel(arg1);
    if (!err) {
      setSettings();
    } else {
      QString vertexesC = "Ошибка чтения файла";
      ui->vertexes_count->setText(vertexesC);
      QString facetsC = "Файл пуст или испорчен";
      ui->facets_count->setText(facetsC);
      ui->widget->data = copy_data(&(ui->widget->object));
    }
  }
  updateVertex();
}

void Widget::updateVertex() {
  transform_mx(&ui->widget->mx, check_sliders());
  mx_mult_vector(ui->widget->data.vertexes.matrix,
                 ui->widget->object.vertexes.matrix, ui->widget->mx.current,
                 ui->widget->data.vertex_count);
  ui->widget->update();
}

unsigned int Widget::check_sliders() {
  unsigned int res = 0;

  if (ui->rotation_slider_X->value() != 0) {
    res |= ROTATE_X;
  }

  if (ui->rotation_slider_Y->value() != 0) {
    res |= ROTATE_Y;
  }

  if (ui->rotation_slider_Z->value() != 0) {
    res |= ROTATE_Z;
  }

  if (ui->move_slider_X->value() != 0 || ui->move_slider_Y->value() != 0 ||
      ui->move_slider_Z->value() != 0) {
    res |= MOVE;
  }
  res |= SCALE;

  return res;
}

void Widget::on_rotation_slider_X_valueChanged(int value) {
  float rt = (float)value * RAD;
  ui->widget->mx.rotate_x[5] = ui->widget->mx.rotate_x[10] = std::cos(rt);
  ui->widget->mx.rotate_x[9] = std::sin(rt);
  ui->widget->mx.rotate_x[6] = -ui->widget->mx.rotate_x[9];
  ui->rotation_X->setValue(value);

  updateVertex();
}

void Widget::on_rotation_slider_Y_valueChanged(int value) {
  float rt = (float)value * RAD;
  ui->widget->mx.rotate_y[0] = ui->widget->mx.rotate_y[10] = std::cos(rt);
  ui->widget->mx.rotate_y[2] = std::sin(rt);
  ui->widget->mx.rotate_y[8] = -ui->widget->mx.rotate_y[2];
  ui->rotation_Y->setValue(value);

  updateVertex();
}

void Widget::on_rotation_slider_Z_valueChanged(int value) {
  float rt = (float)value * RAD;
  ui->widget->mx.rotate_z[0] = ui->widget->mx.rotate_z[5] = std::cos(rt);
  ui->widget->mx.rotate_z[4] = std::sin(rt);
  ui->widget->mx.rotate_z[1] = -ui->widget->mx.rotate_z[4];
  ui->rotation_Z->setValue(value);

  updateVertex();
}

void Widget::on_rotation_X_valueChanged(int arg1) {
  ui->rotation_slider_X->setValue(arg1);
}

void Widget::on_rotation_Y_valueChanged(int arg1) {
  ui->rotation_slider_Y->setValue(arg1);
}

void Widget::on_rotation_Z_valueChanged(int arg1) {
  ui->rotation_slider_Z->setValue(arg1);
}

void Widget::on_scale_slider_valueChanged(int value) {
  if (value <= 0) {
    if (ui->scale->value() - 100 != value) ui->scale->setValue(value + 100);
  } else {
    if (int(ui->scale->value() / 100) != value)
      ui->scale->setValue(value * 100);
  }
}

void Widget::on_scale_valueChanged(int arg1) {
  if (arg1 <= 100) {
    if (arg1 - 100 != ui->scale_slider->value())
      ui->scale_slider->setValue(arg1 - 100);
  } else {
    if (int(arg1 / 100) != ui->scale_slider->value())
      ui->scale_slider->setValue(int(arg1 / 100));
  }
  ui->widget->mx.scale[0] = ui->widget->mx.scale[5] = ui->widget->mx.scale[10] =
      (float)(arg1) / 100.0f;

  updateVertex();
}

void Widget::on_move_slider_X_valueChanged(int value) {
  ui->widget->mx.move[3] = (float)value / 60.0f;
  ui->move_X->setValue(value);
  updateVertex();
}

void Widget::on_move_slider_Y_valueChanged(int value) {
  ui->widget->mx.move[7] = (float)value / 60.0f;
  ui->move_Y->setValue(value);
  updateVertex();
}

void Widget::on_move_slider_Z_valueChanged(int value) {
  ui->widget->mx.move[11] = (float)value / 60.0f;
  ui->move_Z->setValue(value);
  updateVertex();
}

void Widget::on_move_X_valueChanged(int arg1) {
  ui->move_slider_X->setValue(arg1);
}

void Widget::on_move_Y_valueChanged(int arg1) {
  ui->move_slider_Y->setValue(arg1);
}

void Widget::on_move_Z_valueChanged(int arg1) {
  ui->move_slider_Z->setValue(arg1);
}

void Widget::on_central_projection_toggled(bool checked) {
  if (checked) {
    ui->widget->projection = 0;
    ui->widget->setupProjection(0, 0);
    ui->widget->update();
  }
}

void Widget::on_parallel_projection_toggled(bool checked) {
  if (checked) {
    ui->widget->projection = 1;
    ui->widget->setupProjection(0, 0);
    ui->widget->update();
  }
}

void Widget::on_bkg_color_but_clicked() {
  QColor clr = QColorDialog::getColor(QColor(255, 255, 255, 255));
  if (clr.isValid()) set_background_color(clr);
}

void Widget::on_vertex_color_but_clicked() {
  QColor clr = QColorDialog::getColor(QColor(255, 255, 255, 255));
  if (clr.isValid()) set_vertex_color(clr);
}

void Widget::on_facet_color_but_clicked() {
  QColor clr = QColorDialog::getColor(QColor(255, 255, 255, 255));
  if (clr.isValid()) set_facet_color(clr);
}

void Widget::set_background_color(QColor color) {
  ui->widget->clr_back = color;
  updateVertex();
  QPalette bcgcolor = ui->bkg_color->palette();
  bcgcolor.setColor(ui->bkg_color->backgroundRole(), color);
  ui->bkg_color->setPalette(bcgcolor);
  ui->bkg_color->setAutoFillBackground(true);
}

void Widget::set_vertex_color(QColor color) {
  ui->widget->clr_vert = color;
  updateVertex();
  QPalette bcgcolor = ui->vertex_color->palette();
  bcgcolor.setColor(ui->vertex_color->backgroundRole(), color);
  ui->vertex_color->setPalette(bcgcolor);
  ui->vertex_color->setAutoFillBackground(true);
}

void Widget::set_facet_color(QColor color) {
  ui->widget->clr_line = color;
  updateVertex();
  QPalette bcgcolor = ui->facet_color->palette();
  bcgcolor.setColor(ui->facet_color->backgroundRole(), color);
  ui->facet_color->setPalette(bcgcolor);
  ui->facet_color->setAutoFillBackground(true);
}

void Widget::on_square_vertexes_toggled(bool checked) {
  if (checked == true) {
    ui->widget->points = 1;
  }
  updateVertex();
}

void Widget::on_circle_vertexes_toggled(bool checked) {
  if (checked == true) {
    ui->widget->points = 2;
  }
  updateVertex();
}

void Widget::on_no_vertexes_toggled(bool checked) {
  if (checked == true) {
    ui->widget->points = 0;
  }
  updateVertex();
}

void Widget::on_vertex_size_valueChanged(int arg1) {
  ui->widget->pointsSize = arg1;
  updateVertex();
}

void Widget::on_strip_facets_toggled(bool checked) {
  if (checked == true) {
    ui->widget->dottedLine = 2;
  }
  updateVertex();
}

void Widget::on_flat_facets_toggled(bool checked) {
  if (checked == true) {
    ui->widget->dottedLine = 1;
  }
  updateVertex();
}

void Widget::on_no_facets_toggled(bool checked) {
  if (checked == true) {
    ui->widget->dottedLine = 0;
  }
  updateVertex();
}

void Widget::on_facet_size_valueChanged(int arg1) {
  ui->widget->lineSize = arg1;
  updateVertex();
}

void Widget::on_save_image_clicked() {
  if (saveImg.exec() == QDialog::Accepted) {
    QString filePath = saveImg.selectedFiles().first();
    QString selectedFilter = saveImg.selectedNameFilter();

    QString format;
    if (selectedFilter == "BMP Files (*.bmp)") {
      format = "BMP";
      if (!filePath.endsWith(".bmp", Qt::CaseInsensitive)) {
        filePath += ".bmp";
      }
    } else {
      format = "JPEG";
      if (!filePath.endsWith(".jpeg", Qt::CaseInsensitive)) {
        filePath += ".jpeg";
      }
    }

    if (!filePath.isEmpty() && !ui->widget->createImage(filePath, format)) {
      QMessageBox::warning(this, "Error",
                           "Error when creating a snapshot of the model");
    }
  }
}

void Widget::on_save_gif_clicked() {
  if (recording == false) {
    recording = true;
    gifCount = 0;
    dir.mkdir(screenDir);
    timer = 3;
    countdownTimer->start(1000);
    ui->save_gif->setText(QString::number(timer));
  }
}

void Widget::createSnapshot() {
  gifCount++;

  QString format = QString(".bmp");

  ui->widget->createImage(absScreenDir + QString::number(gifCount) + format,
                          "BMP");

  if (gifCount == gifFps * gifLength) {
    periodicTimer->stop();

    if (saveGif.exec() == QFileDialog::Accepted) {
      QString filePath = saveGif.selectedFiles().first();
      if (!filePath.endsWith(".gif", Qt::CaseInsensitive)) {
        filePath += ".gif";
      }

      createGif(filePath);
    }

    QDir imagesDir(dir.absoluteFilePath(screenDir));
    if (imagesDir.exists()) {
      imagesDir.removeRecursively();
    }

    recording = false;
    ui->save_gif->setText("Экспорт в GIF");
  }
}

void Widget::createGif(QString path_to_gif) {
  bool correct = true;

  QString format = QString(".bmp");

  GifWriter writer = {};
  if (!GifBegin(&writer, path_to_gif.toStdString().c_str(), 640, 480, gifFps)) {
    correct = false;
  }

  for (int count = 1; count <= gifFps * gifLength && correct; count++) {
    QString framePath = absScreenDir + QString::number(count) + format;
    QImage frame(framePath);

    if (frame.isNull()) {
      correct = false;
      break;
    }

    QImage convertedFrame = frame.convertToFormat(QImage::Format_RGBA8888);
    if (convertedFrame.width() != 640 || convertedFrame.height() != 480) {
      convertedFrame = convertedFrame.scaled(640, 480);
    }

    if (!GifWriteFrame(&writer, convertedFrame.bits(), 640, 480, gifFps)) {
      correct = false;
      break;
    }
  }

  GifEnd(&writer);

  if (correct == false) {
    QMessageBox::warning(this, "Error",
                         "An error occurred while recording a GIF image");
  }
}

void Widget::countDown() {
  timer--;
  ui->save_gif->setText(QString::number(timer));

  if (timer == 0) {
    countdownTimer->stop();
    ui->save_gif->setText("Идёт запись");
    periodicTimer->start(1000 / gifFps);
  }
}
