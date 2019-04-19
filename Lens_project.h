#pragma once

#include <QtWidgets/QMainWindow>
#include <QGroupBox>
#include "ui_Lens_project.h"
#include <Field.h>
#include <map>


class Draw_widget : public QWidget
{
	Q_OBJECT

private:
	Field field;

public:
	Draw_widget(Field field, QWidget *parent = 0);
	void draw(QPainter *painter);
	void draw_rays(QPainter *painter, Object original, std::vector<Lens> lenses);

protected:
	void paintEvent(QPaintEvent *event);
};


class Lens_project : public QMainWindow
{
	Q_OBJECT

public:
	Lens_project(QWidget *parent = Q_NULLPTR);

private:
	std::map<int, std::tuple<lens_type, float, float, bool>> lens_params;

	std::tuple<float, float> object_param = std::make_tuple(0.0, 1.0);

	QGroupBox *create_object_param_group();
	QGroupBox *create_first_lens_param_group();
	QGroupBox *create_second_lens_param_group();
	QGroupBox *create_third_lens_param_group();

	void create_tool_bar();

	Field field;

	QToolBar *file_tool_bar;

private slots:
	void on_open_clicked();
	void on_draw_clicked();
	void on_save_clicked();
	void set_fst_lens_type();
	void set_snd_lens_type();
	void set_trd_lens_type();
	void set_value_ob_x();
	void set_value_ob_h();
	void set_value_fst_lens_x(const QString& received_change);
	void set_value_fst_lens_foc(const QString& received_change);
	void set_value_snd_lens_x(const QString& received_change);
	void set_value_snd_lens_foc(const QString& received_change);
	void set_value_trd_lens_x(const QString& received_change);
	void set_value_trd_lens_foc(const QString& received_change);
	void add_delete_fst_lens(bool checked);
	void add_delete_snd_lens(bool checked);
	void add_delete_trd_lens(bool checked);
};
