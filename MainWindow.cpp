#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    setupUi(this);

/*

  the structure of the main window's central widget

 *---------------------------------------------------*
 |                 central widget                    |
 |                                                   |
 |  *---------------------------*-----------------*  |
 |  |     rendering context     |   scroll area   |  |
 |  |       OpenGL widget       | *-------------* |  |
 |  |                           | | side widget | |  |
 |  |                           | |             | |  |
 |  |                           | |             | |  |
 |  |                           | *-------------* |  |
 |  *---------------------------*-----------------*  |
 |                                                   |
 *---------------------------------------------------*

*/
    _side_widget = new SideWidget(this);

    _scroll_area = new QScrollArea(this);
    _scroll_area->setWidget(_side_widget);
    _scroll_area->setSizePolicy(_side_widget->sizePolicy());
    _scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    _gl_widget = new GLWidget(this);

    _layout = new QHBoxLayout();
    _layout->addWidget(_gl_widget);
    _layout->addWidget(_scroll_area);

    _central_widget = new QWidget(this);
    _central_widget->setLayout(_layout);

    setCentralWidget(_central_widget);

    // creating a signal slot mechanism between the rendering context and the side widget
    connect(_side_widget->rotate_x_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_x(int)));
    connect(_side_widget->rotate_y_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_y(int)));
    connect(_side_widget->rotate_z_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_z(int)));

    connect(_side_widget->zoom_factor_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_zoom_factor(double)));

    connect(_side_widget->trans_x_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_x(double)));
    connect(_side_widget->trans_y_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_y(double)));
    connect(_side_widget->trans_z_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_z(double)));

    connect(_side_widget->loadButton, SIGNAL(clicked()), _gl_widget, SLOT(loadPointCloud()));
    connect(_side_widget->slowButton, SIGNAL(clicked()), _gl_widget, SLOT(slowConvexHull()));

    connect(_side_widget->quick_hull_button, SIGNAL(clicked()), _gl_widget, SLOT(quick_hull()));
    connect(_side_widget->fast_hull_button, SIGNAL(clicked()), _gl_widget,
            SLOT(fast_hull()));

    connect(_side_widget->load_model_button, SIGNAL(clicked()), _gl_widget,
            SLOT(load_model()));

    connect(_gl_widget, SIGNAL(show_volume(double)), this, SLOT(show_volume(double)));

    connect(_side_widget->load_polygon, SIGNAL(clicked()), _gl_widget, SLOT(load_polygon()));

    connect(_side_widget->load_3d_button, SIGNAL(clicked()), _gl_widget, SLOT(load_3d()));
    connect(_side_widget->convex_3d_button, SIGNAL(clicked()), _gl_widget, SLOT(convex_3d()));

    connect(_side_widget->step_button, SIGNAL(clicked()), _gl_widget, SLOT(step_convex()));
}

//--------------------------------
// implementation of private slots
//--------------------------------
void MainWindow::on_action_Quit_triggered()
{
    qApp->exit(0);
}

void MainWindow::show_volume(double value)
{
    statusBar()->showMessage("Model volume: " + QString::number(value));
}
