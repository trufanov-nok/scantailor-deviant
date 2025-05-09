/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

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

#include "ErrorWidget.h"
#include "ui/ui_ErrorWidget.h"

#include <QStyle>
#include <QIcon>

ErrorWidget::ErrorWidget(QString const& text, Qt::TextFormat fmt):
    ui(new Ui::ErrorWidget)
{
    ui->setupUi(this);
    ui->textLabel->setTextFormat(fmt);
    ui->textLabel->setText(text);
    QIcon icon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning));
    ui->imageLabel->setPixmap(icon.pixmap(48, 48));

    connect(ui->textLabel, SIGNAL(linkActivated(QString)), SLOT(linkActivated(QString)));
}

ErrorWidget::~ErrorWidget()
{
    delete ui;
}
