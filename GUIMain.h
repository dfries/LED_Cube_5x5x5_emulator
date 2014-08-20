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
#ifndef _GUI_MAIN_
#define _GUI_MAIN_ 1

#include <QList>
#include <QMap>
#include <QQuickView>
#include <QVariant>

// The top level window for QML and C++ part of the program.
class GUIMain : public QQuickView
{
	Q_OBJECT
	Q_PROPERTY(QVariantList data READ GetData NOTIFY DataChanged)
public:
	GUIMain();

	QVariantList GetData() const { return Data; }

	Q_INVOKABLE void StepPattern(int pattern);

protected:
	void keyPressEvent(QKeyEvent *e);

signals:
	void DataChanged();

private:
	QVariantList Data;
	int Sequence;
};

#endif // _GUI_MAIN_
