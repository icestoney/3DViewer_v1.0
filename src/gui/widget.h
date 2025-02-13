#ifndef WIDGET_H
#define WIDGET_H

#include <QCloseEvent>
#include <QColorDialog>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QSettings>
#include <QThread>
#include <QTimer>
#include <QWidget>

/// @brief 1 degree in radians
#define RAD 0.0174532925199433

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
  Q_OBJECT

 public:
  Widget(QWidget *parent = nullptr);
  ~Widget();
  unsigned int check_sliders();

 protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void showEvent(QShowEvent *event) override;

 private slots:

  void slotMousePress(QMouseEvent *event);
  void slotMouseMove(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event) override;
  void saveSettings();
  void loadSettings();
  void setFrontSettings();
  void setSettings();
  void on_Load_file_clicked();
  void on_filePathEdit_textEdited(const QString &arg1);
  void updateVertex();

  void on_rotation_slider_X_valueChanged(int value);
  void on_rotation_slider_Y_valueChanged(int value);
  void on_rotation_slider_Z_valueChanged(int value);
  void on_rotation_X_valueChanged(int arg1);
  void on_rotation_Y_valueChanged(int arg1);
  void on_rotation_Z_valueChanged(int arg1);

  void on_scale_slider_valueChanged(int value);
  void on_scale_valueChanged(int arg1);

  void on_move_slider_X_valueChanged(int value);
  void on_move_slider_Y_valueChanged(int value);
  void on_move_slider_Z_valueChanged(int value);
  void on_move_X_valueChanged(int arg1);
  void on_move_Y_valueChanged(int arg1);
  void on_move_Z_valueChanged(int arg1);

  void on_central_projection_toggled(bool checked);
  void on_parallel_projection_toggled(bool checked);

  void on_bkg_color_but_clicked();
  void on_facet_color_but_clicked();
  void on_vertex_color_but_clicked();
  void set_background_color(QColor color);
  void set_vertex_color(QColor color);
  void set_facet_color(QColor color);

  void on_square_vertexes_toggled(bool checked);
  void on_circle_vertexes_toggled(bool checked);
  void on_no_vertexes_toggled(bool checked);
  void on_vertex_size_valueChanged(int arg1);

  void on_strip_facets_toggled(bool checked);
  void on_flat_facets_toggled(bool checked);
  void on_no_facets_toggled(bool checked);
  void on_facet_size_valueChanged(int arg1);

  void on_save_image_clicked();
  void on_save_gif_clicked();
  void createSnapshot();
  void createGif(QString path_to_gif);
  void countDown();

 private:
  Ui::Widget *ui;
  QPoint startPos;
  bool leftMouse;
  bool rightMouse;
  int gifCount;
  int gifFps;
  int gifLength;
  int timer;
  bool recording;
  QTimer *periodicTimer;
  QTimer *countdownTimer;
  QDir dir;
  QString screenDir;
  QString absScreenDir;
  QFileDialog saveImg;
  QFileDialog saveGif;
};
#endif  // WIDGET_H
