/*
    Copyright (C) 2014 David Fries <David@Fries.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDebug>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

#include "GUIMain.h"

#include "CubeControl.h"

GUIMain::GUIMain() :
	Pattern(0)
{
	rootContext()->setContextProperty("GUIMain", this);
	Data = SetCubePattern(Pattern);
	DataChanged();
}

void GUIMain::StepPattern()
{
	Data = SetCubePattern(++Pattern);
	DataChanged();
}

void GUIMain::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_QuoteLeft &&
		e->modifiers() == Qt::ControlModifier)
	{
		// required to force a reload
		engine()->clearComponentCache();
		qDebug() << "Reloading QML";
		setSource(source());
	}
	else if(e->key() == Qt::Key_Q &&
		e->modifiers() == Qt::ControlModifier)
	{
		engine()->quit();
	}
	else
	{
		QQuickView::keyPressEvent(e);
	}
}

int main(int argc, char **argv)
{
	QGuiApplication app(argc, argv);
	QString path;
	QByteArray match;

	GUIMain view;
	view.connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
	view.setResizeMode(QQuickView::SizeRootObjectToView);
	QString source("qml/main.qml");
	if(!QFile::exists(source) && QFile::exists("../" + source))
		source = "../" + source;
	view.setSource(QUrl(source));
	// view.showFullScreen();
	view.show();
	return app.exec();
}
