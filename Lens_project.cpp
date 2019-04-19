#include "Lens_project.h"
#include "Lens.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDockWidget>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QTime>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QPainter>
#include <QPaintEvent>
#include <QPixMap>
#include <QFileDialog>
#include <map>
#include <iostream>
#include <fstream> 


Draw_widget::Draw_widget(Field field, QWidget *parent)
	: QWidget(parent),
	field(field)
{
	setPalette(QPalette(QColor(255, 255, 255)));
	setAutoFillBackground(true);
}

void Draw_widget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	draw(&painter);
}

void Draw_widget::draw(QPainter *painter)
{
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->drawLine(0, 400, 2000, 400);

	int j = 0;
	for (int i = 50; i < 2000; i += 300)
	{
		QString j_string = QString::number(j);
		j+=5;
		painter->drawLine(i, 395, i, 405);
		painter->drawText(i-3, 420, j_string);
	}

	// Drawing the object
	int object_x = int(50 + (field.original.x * 60)), object_height = int(field.original.height * 60);
	painter->drawLine(object_x, 400, object_x, 400 - object_height);
	
	if (object_height < 0)
	{
		painter->drawLine(object_x, 400 - object_height, object_x - 5, 400 - object_height - 10);
		painter->drawLine(object_x, 400 - object_height, object_x + 5, 400 - object_height - 10);
	}
	else
	{
		painter->drawLine(object_x, 400 - object_height, object_x - 5, 400 - object_height + 10);
		painter->drawLine(object_x, 400 - object_height, object_x + 5, 400 - object_height + 10);
	}

	// Drawing the lenses
	j = 0;
	for (std::vector<Lens>::iterator it = field.lenses.begin(); it != field.lenses.end(); ++it)
	{
		int lens_x = int(50 + ((it->x) * 60)), lens_foc_pos = int(50 + ((it->x + it->focus) * 60)), lens_foc_neg = int(50 + ((it->x - it->focus) * 60));
		QString lens_num = QString::number(j + 1);
		QString foc = "F";
		foc.append(lens_num);
		++j;

		painter->drawLine(lens_x, 700, lens_x, 100);

		if (it->type == Convex)
		{
			painter->drawLine(lens_x, 700, lens_x - 5, 690);
			painter->drawLine(lens_x, 700, lens_x + 5, 690);
			painter->drawLine(lens_x, 100, lens_x - 5, 110);
			painter->drawLine(lens_x, 100, lens_x + 5, 110);
		}
		else
		{
			painter->drawLine(lens_x, 700, lens_x - 5, 710);
			painter->drawLine(lens_x, 700, lens_x + 5, 710);
			painter->drawLine(lens_x, 100, lens_x - 5, 90);
			painter->drawLine(lens_x, 100, lens_x + 5, 90);
		}
		
		painter->drawLine(lens_foc_pos, 395, lens_foc_pos, 405);
		painter->drawText(lens_foc_pos - 3, 380, foc);

		painter->drawLine(lens_foc_neg, 395, lens_foc_neg, 405);
		painter->drawText(lens_foc_neg - 3, 380, foc);
	}

	// Drawing images
	// Real images
	painter->setPen(QColor(255, 0, 0));

	for (std::vector<Object>::iterator it = field.re_image.begin(); it != field.re_image.end(); ++it)
	{
		int image_x = int(50 + (it->x) * 60), image_height = int((it->height) * 60);
		painter->drawLine(image_x, 400, image_x, 400 - image_height);

		if (image_height < 0)
		{
			painter->drawLine(image_x, 400 - image_height, image_x - 5, 400 - image_height - 10);
			painter->drawLine(image_x, 400 - image_height, image_x + 5, 400 - image_height - 10);
		}
		else
		{
			painter->drawLine(image_x, 400 - image_height, image_x - 5, 400 - image_height + 10);
			painter->drawLine(image_x, 400 - image_height, image_x + 5, 400 - image_height + 10);
		}
	}

	// Imaginary images
	painter->setPen(QColor(0, 0, 255));

	for (std::vector<Object>::iterator it = field.im_image.begin(); it != field.im_image.end(); ++it)
	{
		int image_x = int(50 + (it->x) * 60), image_height = int((it->height) * 60);
		painter->drawLine(image_x, 400, image_x, 400 - image_height);

		if (image_height < 0)
		{
			painter->drawLine(image_x, 400 - image_height, image_x - 5, 400 - image_height - 10);
			painter->drawLine(image_x, 400 - image_height, image_x + 5, 400 - image_height - 10);
		}
		else
		{
			painter->drawLine(image_x, 400 - image_height, image_x - 5, 400 - image_height + 10);
			painter->drawLine(image_x, 400 - image_height, image_x + 5, 400 - image_height + 10);
		}
	}

	draw_rays(painter, field.original, field.lenses);
}

void Draw_widget::draw_rays(QPainter *painter, Object original, std::vector<Lens> lenses)
{
	painter->setPen(QColor(150, 150, 150));

	std::vector<Lens>::iterator it = lenses.begin();

	Object last_image = original, current_image;
	
	while (it != lenses.end())
	{
		int last_im_x = int(50 + (last_image.x * 60)), last_im_height = int(last_image.height * 60);

		Lens current_lens = *it;
		int lens_x = int(50 + ((current_lens.x) * 60));

		current_image = current_lens.distortion(last_image);
		int cur_im_x = int(50 + (current_image.x * 60)), cur_im_height = int(current_image.height * 60);

		if (current_image.height / last_image.height != 0)
		{
			painter->drawLine(last_im_x, 400 - last_im_height, lens_x, 400 - last_im_height);
			painter->drawLine(lens_x, 400 - last_im_height, cur_im_x, 400 - cur_im_height);
			painter->drawLine(last_im_x, 400 - last_im_height, cur_im_x, 400 - cur_im_height);
		}
		else
		{
			int infinite_x = int(last_im_x + 40 * (current_lens.focus * 60)), infinite_height = int(last_im_height - 40 * last_im_height);

			painter->drawLine(last_im_x, 400 - last_im_height, lens_x, 400 - last_im_height);
			painter->drawLine(lens_x, 400 - last_im_height, infinite_x, 400 - infinite_height);
			painter->drawLine(last_im_x, 400 - last_im_height, infinite_x, 400 - infinite_height);
			return;
		}

		last_image = current_image;
		++it;
	}
}




Lens_project::Lens_project(QWidget *parent)
	: QMainWindow(parent)
{
	for (int i = 0; i < 3; ++i)
	{
		lens_params.insert(std::make_pair(i, std::make_tuple(Convex, 0.0, 0.0, false)));
	}

	std::vector<std::tuple<lens_type, float, float>> empty_lens;

	field = Field(object_param, empty_lens);
	QDockWidget *my_dock_widget = new QDockWidget(tr("&Field &parameters"), this);

	QWidget *my_widget = new QWidget(this);

	QGridLayout *grid = new QGridLayout;
	grid->addWidget(create_object_param_group(), 0, 0);
	grid->addWidget(create_first_lens_param_group(), 0, 1);
	grid->addWidget(create_second_lens_param_group(), 0, 2);
	grid->addWidget(create_third_lens_param_group(), 0, 3);

	my_widget->setLayout(grid);

	my_dock_widget->setWidget(my_widget);

	addDockWidget(Qt::BottomDockWidgetArea, my_dock_widget);

	create_tool_bar();

	Draw_widget *my_draw_widget = new Draw_widget(field);
	my_draw_widget->update();
	setCentralWidget(my_draw_widget);
}


QGroupBox* Lens_project::create_object_param_group()
{
	QGroupBox *group_box = new QGroupBox(tr("Object"));

	QLabel *x_line = new QLabel(tr("x:"));
	QLabel *height_line = new QLabel(tr("height:"));

	QLineEdit *x_edit = new QLineEdit("0.0");
	QLineEdit *height_edit = new QLineEdit("1.0");

	connect(x_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_ob_x()));
	connect(height_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_ob_h()));


	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(x_line, 2, 0);
	vbox->addWidget(x_edit, 2, 0);
	vbox->addWidget(height_line, 3, 0);
	vbox->addWidget(height_edit, 3, 0);

	group_box->setLayout(vbox);

	return group_box;
}

QGroupBox* Lens_project::create_first_lens_param_group()
{
	QGroupBox *group_box = new QGroupBox(tr("First lens"));
	group_box->setCheckable(true);
	group_box->setChecked(false);

	QRadioButton *convex_radio_button = new QRadioButton(tr("Convex"));
	QRadioButton *concave_radio_button = new QRadioButton(tr("Concave"));

	convex_radio_button->setChecked(true);

	connect(convex_radio_button, SIGNAL(clicked()), this, SLOT(set_fst_lens_type()));
	connect(concave_radio_button, SIGNAL(clicked()), this, SLOT(set_fst_lens_type()));

	QLabel *x_line = new QLabel(tr("x:"));
	QLabel *focus_line = new QLabel(tr("focus:"));

	QLineEdit *x_edit = new QLineEdit("0.0");
	QLineEdit *focus_edit = new QLineEdit("0.0");

	connect(x_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_fst_lens_x(const QString&)));
	connect(focus_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_fst_lens_foc(const QString&)));


	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(convex_radio_button);
	vbox->addWidget(concave_radio_button);
	vbox->addWidget(x_line, 2, 0);
	vbox->addWidget(x_edit, 2, 0);
	vbox->addWidget(focus_line, 3, 0);
	vbox->addWidget(focus_edit, 3, 0);

	group_box->setLayout(vbox);

	connect(group_box, SIGNAL(toggled(bool)), this, SLOT(add_delete_fst_lens(bool)));

	return group_box;
}

QGroupBox* Lens_project::create_second_lens_param_group()
{

	QGroupBox *group_box = new QGroupBox(tr("Second lens"));
	group_box->setCheckable(true);
	group_box->setChecked(false);

	QRadioButton *convex_radio_button = new QRadioButton(tr("Convex"));
	QRadioButton *concave_radio_button = new QRadioButton(tr("Concave"));

	convex_radio_button->setChecked(true);

	connect(convex_radio_button, SIGNAL(clicked()), this, SLOT(set_snd_lens_type()));
	connect(concave_radio_button, SIGNAL(clicked()), this, SLOT(set_snd_lens_type()));

	QLabel *x_line = new QLabel(tr("x:"));
	QLabel *focus_line = new QLabel(tr("focus:"));

	QLineEdit *x_edit = new QLineEdit("0.0");
	QLineEdit *focus_edit = new QLineEdit("0.0");

	connect(x_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_snd_lens_x(const QString&)));
	connect(focus_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_snd_lens_foc(const QString&)));


	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(convex_radio_button, 0, 0);
	vbox->addWidget(concave_radio_button, 1, 0);
	vbox->addWidget(x_line, 2, 0);
	vbox->addWidget(x_edit, 2, 0);
	vbox->addWidget(focus_line, 3, 0);
	vbox->addWidget(focus_edit, 3, 0);


	group_box->setLayout(vbox);

	connect(group_box, SIGNAL(toggled(bool)), this, SLOT(add_delete_snd_lens(bool)));

	return group_box;
}

QGroupBox* Lens_project::create_third_lens_param_group()
{
	lens_type *l_type = new lens_type(Convex);
	float *l_x = new float(0.0), *l_f = new float(0.0);

	QGroupBox *group_box = new QGroupBox(tr("Third lens"));
	group_box->setCheckable(true);
	group_box->setChecked(false);

	QRadioButton *convex_radio_button = new QRadioButton(tr("Convex"));
	QRadioButton *concave_radio_button = new QRadioButton(tr("Concave"));

	convex_radio_button->setChecked(true);

	connect(convex_radio_button, SIGNAL(clicked()), this, SLOT(set_trd_lens_type()));
	connect(concave_radio_button, SIGNAL(clicked()), this, SLOT(set_trd_lens_type()));

	QLabel *x_line = new QLabel(tr("x:"));
	QLabel *focus_line = new QLabel(tr("focus:"));

	QLineEdit *x_edit = new QLineEdit("0.0");
	QLineEdit *focus_edit = new QLineEdit("0.0");

	connect(x_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_trd_lens_x(const QString&)));
	connect(focus_edit, SIGNAL(textChanged(const QString&)), this, SLOT(set_value_trd_lens_foc(const QString&)));


	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(convex_radio_button);
	vbox->addWidget(concave_radio_button);
	vbox->addWidget(x_line, 2, 0);
	vbox->addWidget(x_edit, 2, 0);
	vbox->addWidget(focus_line, 3, 0);
	vbox->addWidget(focus_edit, 3, 0);

	group_box->setLayout(vbox);

	connect(group_box, SIGNAL(toggled(bool)), this, SLOT(add_delete_trd_lens(bool)));

	return group_box;
}


 
void Lens_project::on_draw_clicked()
{
	std::vector<std::tuple<lens_type, float, float>> lens_params_vector;
	for (std::map<int, std::tuple<lens_type, float, float, bool>>::iterator it = lens_params.begin(); it != lens_params.end(); ++it)
	{
		if (std::get<3>(it->second))
		{
			std::tuple<lens_type, float, float> insert = std::make_tuple(std::get<0>(it->second), std::get<1>(it->second), std::get<2>(it->second));
			lens_params_vector.push_back(insert);
		}
	}

	field = Field(object_param, lens_params_vector);

	Draw_widget *new_pic = new Draw_widget(field);
	new_pic->update();
	setCentralWidget(new_pic);
}
 
void Lens_project::on_save_clicked()
{
	QString file_name;

	QTime time = QTime::currentTime();
	QDate date = QDate::currentDate();
	
	if (date.day() < 10)
	{
		file_name += "0";
	}
	file_name += QString::number(date.day()) + ".";
	if (date.month() < 10)
	{
		file_name += "0";
	}
	file_name += QString::number(date.month()) + ".";
	file_name += QString::number(date.year()) + "_";
	
	if (time.hour() < 10)
	{
		file_name += "0";
	}
	file_name += QString::number(time.hour()) + "-";
	if (time.minute() < 10)
	{
		file_name += "0";
	}
	file_name += QString::number(time.minute()) + "-";
	if (time.second() < 10)
	{
		file_name += "0";
	}
	file_name += QString::number(time.second());
	
	QFile saved_file(file_name+".ls");
	saved_file.open(QIODevice::WriteOnly);
	QMessageBox msg_box;
	msg_box.setStandardButtons(QMessageBox::Ok);

	std::string saved_field = field.save_field();
	QString s_f_qs = QString::fromStdString(saved_field);
	QTextStream stream(&saved_file);
	stream << s_f_qs << endl;

	msg_box.setText("Saved to program folder with name: " + file_name + ".ls");
	msg_box.setWindowTitle("Saved!");
	msg_box.exec();
}

void Lens_project::on_open_clicked()
{
	QFileDialog file_dialog(0, tr("Load lens file"), QDir::home().absolutePath(), tr("Lens file (*.ls)"));
	file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
	file_dialog.setFileMode(QFileDialog::ExistingFiles);
	if (QDialog::Accepted != file_dialog.exec())
	{
		return;
	}
	QStringList file_names = file_dialog.selectedFiles() ;
	QString file_name = file_names.at(0);

	if (file_name.isEmpty())
	{
		return;
	}
	else
	{
		QFile file(file_name);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QMessageBox::information(this, tr("Unable to open file"), file.errorString());
			return;
		}

		QDataStream in(&file);
		in.setVersion(QDataStream::Qt_4_5);

		QByteArray buffer(10000, Qt::Uninitialized);

		in.readRawData(buffer.data(), 10000);
		QString file_text(buffer);

		std::string string_text = file_text.toLocal8Bit().constData();

		field = Field(string_text);

		Draw_widget *new_pic = new Draw_widget(field);
		new_pic->update();
		setCentralWidget(new_pic);
	}
}


//
// FIELD SLOTS
//

void Lens_project::set_fst_lens_type()
{
	QObject *object = QObject::sender();
	QRadioButton* pbtn = qobject_cast<QRadioButton*>(object);
	QString name = pbtn->text();

	QString convex_name(tr("Convex"));

	if (!QString::compare(name, convex_name))
	{
		std::get<0>(lens_params[0]) = Convex;
	}
	else
	{
		std::get<0>(lens_params[0]) = Concave;
	}
}

void Lens_project::set_snd_lens_type()
{
	QObject *object = QObject::sender();
	QRadioButton* pbtn = qobject_cast<QRadioButton*>(object);
	QString name = pbtn->text();

	QString convex_name(tr("Convex"));

	if (!QString::compare(name, convex_name))
	{
		std::get<0>(lens_params[1]) = Convex;
	}
	else
	{
		std::get<0>(lens_params[1]) = Concave;
	}
}

void Lens_project::set_trd_lens_type()
{
	QObject *object = QObject::sender();
	QRadioButton* pbtn = qobject_cast<QRadioButton*>(object);
	QString name = pbtn->text();

	QString convex_name(tr("Convex"));

	if (!QString::compare(name, convex_name))
	{
		std::get<0>(lens_params[2]) = Convex;
	}
	else
	{
		std::get<0>(lens_params[2]) = Concave;
	}
}

void Lens_project::set_value_ob_x()
{
	QObject *object = QObject::sender();
	QLineEdit* edit_line = qobject_cast<QLineEdit*>(object);
	QString line = edit_line->text();

	std::get<0>(object_param) = line.toFloat();
}

void Lens_project::set_value_ob_h()
{
	QObject *object = QObject::sender();
	QLineEdit* edit_line = qobject_cast<QLineEdit*>(object);
	QString line = edit_line->text();

	std::get<1>(object_param) = line.toFloat();
}

void Lens_project::set_value_fst_lens_x(const QString& received_change)
{
	std::get<1>(lens_params[0]) = received_change.toFloat();
}

void Lens_project::set_value_fst_lens_foc(const QString& received_change)
{
	std::get<2>(lens_params[0]) = received_change.toFloat();
}

void Lens_project::set_value_snd_lens_x(const QString& received_change)
{
	std::get<1>(lens_params[1]) = received_change.toFloat();
}

void Lens_project::set_value_snd_lens_foc(const QString& received_change)
{
	std::get<2>(lens_params[1]) = received_change.toFloat();
}

void Lens_project::set_value_trd_lens_x(const QString& received_change)
{
	std::get<1>(lens_params[2]) = received_change.toFloat();
}

void Lens_project::set_value_trd_lens_foc(const QString& received_change)
{
	std::get<2>(lens_params[2]) = received_change.toFloat();
}

void Lens_project::add_delete_fst_lens(bool checked)
{
	std::get<3>(lens_params[0]) = checked;
}

void Lens_project::add_delete_snd_lens(bool checked)
{
	std::get<3>(lens_params[1]) = checked;
}

void Lens_project::add_delete_trd_lens(bool checked)
{
	std::get<3>(lens_params[2]) = checked;
}

void Lens_project::create_tool_bar()
{
	file_tool_bar = new QToolBar();
	file_tool_bar = addToolBar(tr("File"));

	QAction *save_action = file_tool_bar->addAction(tr("Save"));
	save_action->setShortcut(QKeySequence::Save);
	save_action->setStatusTip(tr("Save current lens file"));
	connect(save_action, SIGNAL(triggered()), this, SLOT(on_save_clicked()));

	QAction *draw_action = file_tool_bar->addAction(tr("Draw"));
	draw_action->setStatusTip(tr("Draw field with current settings"));
	connect(draw_action, SIGNAL(triggered()), this, SLOT(on_draw_clicked()));

	QAction *open_action = file_tool_bar->addAction(tr("Open"));
	open_action->setShortcut(QKeySequence::Open);
	open_action->setStatusTip(tr("Open a lens file"));
	connect(open_action, SIGNAL(triggered()), this, SLOT(on_open_clicked()));
}



